/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

#include "config.h"
#include "detail.hpp"

namespace GooseEYE {

/*
Collect kernels.
*/
namespace kernel {

/*
Return kernel with nearest neighbours.
@arg ndim : Number of dimensions (rank).
@ret The kernel.
*/
inline xt::xarray<int> nearest(size_t ndim);

} // namespace kernel

/*
Different methods to compute a pixel-path.
*/
enum class path_mode {
    Bresenham,
    actual,
    full
};

/*
Compute a path between two pixels.
@arg x0 : Pixel coordinate (e.g. {0, 0}).
@arg x1 : Pixel coordinate (e.g. {10, 5}).
@opt mode : Method to use (see "path_mode").
@ret The path: the coordinate of one pixel per row.
*/
inline xt::xtensor<int,2> path(
    const xt::xtensor<int,1>& x0,
    const xt::xtensor<int,1>& x1,
    path_mode mode = path_mode::Bresenham);

/*
Dummy image with circles. The positions and radii of the circles are randomly generated.
@arg shape : Shape of the output image.
@opt periodic : Switch to assume image periodic.
@ret The dummy image.
*/
inline xt::xarray<int> dummy_circles(const std::vector<size_t>& shape, bool periodic = true);

/*
Dummy image with circles.
@arg shape : Shape of the output image.
@arg row : The x-position of each circle.
@arg col : The y-position of each circle.
@arg r : The radius of each circle.
@opt periodic : Switch to assume image periodic.
@ret The dummy image.
*/
inline xt::xarray<int> dummy_circles(
    const std::vector<size_t>& shape,
    const xt::xtensor<int,1>& row,
    const xt::xtensor<int,1>& col,
    const xt::xtensor<int,1>& r,
    bool periodic = true);

/*
Dilate image. The input image can be binary (1), or have integer labels (>=1).
In each case the background are 0.
@arg f : The image (integer).
@opt kernel : The kernel with which to dilate (binary).
@opt iterations : Number of iterations per label.
@opt periodic : Switch to assume image periodic.
@ret The dilated image.
*/
template <
    class T,
    class S,
    std::enable_if_t<std::is_integral<typename T::value_type>::value &&
                     std::is_integral<typename S::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    const S& kernel,
    const xt::xtensor<size_t, 1>& iterations,
    bool periodic = true);

/*
Dilate image. Select "kernel::nearest" as kernel.
See above for parameters.
*/
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    const xt::xtensor<size_t,1>& iterations,
    bool periodic = true);

/*
Dilate image. Fixed number of iterations for all labels.
See above for parameters.
*/
template <
    class T,
    class S,
    std::enable_if_t<std::is_integral<typename T::value_type>::value &&
                     std::is_integral<typename S::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    const S& kernel,
    size_t iterations = 1,
    bool periodic = true);

/*
Dilate image. Fixed number of iterations for all labels. Select "kernel::nearest" as kernel.
See above for parameters.
*/
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    size_t iterations = 1,
    bool periodic = true);

// -------------------------------------------------------------------------------------------------
// Clusters - TODO
// -------------------------------------------------------------------------------------------------

class Clusters {
public:

    // Constructors
    Clusters() = default;

    template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
    Clusters(const T& f, bool periodic = true);

    template <
        class T,
        class S,
        std::enable_if_t<std::is_integral<typename T::value_type>::value &&
                         std::is_integral<typename S::value_type>::value, int> = 0>
    Clusters(const T& f, const S& kernel, bool periodic = true);

    // Return labels (1..n)
    xt::xarray<int> labels() const;

    // Return label only in the center of gravity
    xt::xarray<int> centers() const;

    // Return positions of the centers of gravity (in the original rank, or as 3-d)
    xt::xtensor<double, 2> center_positions(bool as3d = false) const;

    // Return size per cluster
    xt::xtensor<size_t, 1> sizes() const;

private:
    // Compute clusters
    void compute();

    // Compute position of the cluster centers
    template <class T> xt::xtensor<double, 2> average_position(const T& lab) const;
    xt::xtensor<double, 2> average_position_periodic() const;

    static const size_t MAX_DIM = 3;
    std::vector<size_t> m_shape; // shape of the input image
    std::vector<std::vector<size_t>> m_pad;
    xt::xtensor<int, 3> m_kernel;
    bool m_periodic;
    xt::xtensor<int, 3> m_l;    // labels (>= 1, 0 = background), 3-d
    xt::xtensor<int, 3> m_l_np; // labels before applying periodicity
};

// Wrapper function

template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
xt::xarray<int> clusters(const T& f, bool periodic = true);

// Find map to relabel

xt::xtensor<size_t, 1> relabel_map(const xt::xarray<int>& src, const xt::xarray<int>& dest);

// -------------------------------------------------------------------------------------------------


/*
Convert positions to an image.
@arg img : The image.
@arg positions : The position.
@arg labels : The label to apply for each image.
@ret The image, with labels inserted (overwritten) at the positions.
*/
template <typename T, typename U, typename V>
inline T pos2img(const T& img, const U& positions, const V& labels);

/*
Get the position of the center of each label.
@arg labels : An image with labels [0..N].
@opt periodic : Switch to assume image periodic.
@ret The position of the center of each label.
*/
template <class T>
xt::xtensor<double,2> center_of_mass(const T& labels, bool periodic = true);

/*
Compute ensemble averaged statistics, by repetitively calling the member-function of a certain
statistical measure with different data.

Note:
- One class instance can only be used to compute one statistical measure.
- Simple wrapper functions are provided to compute the statistical measure on one image.
- The normalized result can be read at any moment using 'result', without affecting the raw-data.
- The raw-data and normalization are separately stored as class member variables.
*/
class Ensemble {
public:

    /*
    Constructor.
    */
    Ensemble() = default;

    /*
    Constructor.
    @arg roi : Shape of the 'region-of-interest' on which the statistics are computed.
    @opt periodic : Switch to assume image periodic.
    @opt variance : Switch to compute the variance together with the mean.
    */
    Ensemble(const std::vector<size_t>& roi, bool periodic = true, bool variance = true);

    /*
    Get ensemble average.
    @ret The average along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> result() const;

    /*
    Get ensemble variance.
    @ret The variance along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> variance() const;

    /*
    Get raw-data: ensemble sum of the first moment: x_1 + x_2 + ...
    @ret The sum along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> data_first() const;

    /*
    Get raw-data: ensemble sum of the second moment: x_1^2 + x_2^2 + ...
    @ret The sum along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> data_second() const;

    /*
    Get raw-data: normalisation (number of measurements per pixel).
    @ret The norm along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> norm() const;

    /*
    Get the relative distance of each pixel in the 'region-of-interest' to its center.
    @ret The distances along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> distance() const;

    /*
    Get the relative distance of each pixel in the 'region-of-interest' to its center,
    along a specific axis (distance can now be positive and negative).
    @arg axis : Select axis.
    @ret The distances along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> distance(size_t axis) const;

    /*
    Get the relative distance of each pixel in the 'region-of-interest' to its center.
    Convert the distance to physical dimensions.
    @arg h : The physical dimensions of one pixel (in each direction).
    @ret The distances along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> distance(const std::vector<double>& h) const;

    /*
    Get the relative distance of each pixel in the 'region-of-interest' to its center,
    along a specific axis (distance can now be positive and negative).
    Convert the distance to physical dimensions.
    @arg h : The physical dimensions of one pixel (in each direction).
    @arg axis : Select axis.
    @ret The distances along the 'region-of-interest' set at construction.
    */
    xt::xarray<double> distance(const std::vector<double>& h, size_t axis) const;

    /*
    Add realization to arithmetic mean.
    @arg f : The image.
    @opt fmask : Mask certain pixels (binary, 1: masked, 0: not masked).
    */
    template <class T>
    void mean(const xt::xarray<T>& f);

    template <class T>
    void mean(const xt::xarray<T>& f, const xt::xarray<int>& fmask);

    /*
    Add realization to 2-point correlation: P(f(i) * g(i + di)).
    @arg f : The image.
    @arg g : The comparison image.
    @opt fmask : Mask certain pixels of f (binary, 1: masked, 0: not masked).
    @opt gmask : Mask certain pixels of g (binary, 1: masked, 0: not masked).
    */
    template <class T>
    void S2(
        const xt::xarray<T>& f,
        const xt::xarray<T>& g);

    template <class T>
    void S2(
        const xt::xarray<T>& f,
        const xt::xarray<T>& g,
        const xt::xarray<int>& fmask,
        const xt::xarray<int>& gmask);

    /*
    Add realization to 2-point cluster function: P(f(i) == g(i + di)).
    @arg f : The image.
    @arg g : The comparison image.
    @opt fmask : Mask certain pixels of f (binary, 1: masked, 0: not masked).
    @opt gmask : Mask certain pixels of g (binary, 1: masked, 0: not masked).
    */
    void C2(
        const xt::xarray<int>& f,
        const xt::xarray<int>& g);

    void C2(
        const xt::xarray<int>& f,
        const xt::xarray<int>& g,
        const xt::xarray<int>& fmask,
        const xt::xarray<int>& gmask);

    /*
    Add realization to weighted 2-point correlation.
    @arg w : The weights.
    @arg f : The image.
    @opt fmask : Mask certain pixels of f (binary, 1: masked, 0: not masked).
    */
    template <class T>
    void W2(const xt::xarray<T>& w, const xt::xarray<T>& f);

    template <class T>
    void W2(const xt::xarray<T>& w, const xt::xarray<T>& f, const xt::xarray<int>& fmask);

    /*
    Add realization to collapsed weighted 2-point correlation
    @arg clusters : The clusters.
    @arg centers : The cluster-centers: label only at the center.
    @arg f : The image.
    @opt mode : Method to use (see "path_mode").
    @opt fmask : Mask certain pixels of f (binary, 1: masked, 0: not masked).
    */
    template <class T>
    void W2c(
        const xt::xarray<int>& clusters,
        const xt::xarray<int>& centers,
        const xt::xarray<T>& f,
        path_mode mode = path_mode::Bresenham);

    template <class T>
    void W2c(
        const xt::xarray<int>& clusters,
        const xt::xarray<int>& centers,
        const xt::xarray<T>& f,
        const xt::xarray<int>& fmask,
        path_mode mode = path_mode::Bresenham);

    /*
    Add realization to height-height correlation.
    @arg f : The image.
    @opt fmask : Mask certain pixels of f (binary, 1: masked, 0: not masked).
    */
    template <class T>
    void heightheight(const xt::xarray<T>& f);

    template <class T>
    void heightheight(const xt::xarray<T>& f, const xt::xarray<int>& fmask);

    /*
    Add realization to lineal-path function
    @arg f : The image.
    @opt mode : Method to use (see "path_mode").
    */
    template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
    void L(const xt::xarray<T>& f, path_mode mode = path_mode::Bresenham);

private:

    // Type: used to lock the ensemble to a certain measure.
    enum class Type {
        Unset,
        mean,
        S2,
        C2,
        W2,
        W2c,
        L,
        heightheight
    };

    // Initialize class as unlocked.
    Type m_stat = Type::Unset;

    // Maximum number of dimensions.
    static const size_t MAX_DIM = 3;

    // Switch to assume periodicity (for the entire ensemble).
    bool m_periodic;

    // Switch to compute the variance (for the entire ensemble).
    bool m_variance;

    // Raw (not normalized) result, and normalization:
    // - sum of the first moment: x_1 + x_2 + ...
    xt::xtensor<double,3> m_first;
    // - sum of the second moment: x_1^2 + x_2^2 + ...
    xt::xtensor<double,3> m_second;
    // - number of measurements per pixel
    xt::xtensor<double,3> m_norm;

    // Shape of the region-of-interest, as specified.
    std::vector<size_t> m_shape_orig;

    // 3d equivalent of "m_shape_orig".
    std::vector<size_t> m_shape;

    // Pad size (3d).
    std::vector<std::vector<size_t>> m_pad;
};

// ---------------------------------------------------------
// Wrapper functions to compute the statistics for one image
// ---------------------------------------------------------

inline xt::xarray<double> distance(const std::vector<size_t>& roi);

inline xt::xarray<double> distance(const std::vector<size_t>& roi, size_t axis);

inline xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h);

inline xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t axis);

template <class T>
inline xt::xarray<double> S2(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& f,
    const xt::xarray<T>& g,
    bool periodic = true);

template <class T>
inline xt::xarray<double> S2(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& f,
    const xt::xarray<T>& g,
    const xt::xarray<int>& fmask,
    const xt::xarray<int>& gmask,
    bool periodic = true);

inline xt::xarray<double> C2(
    const std::vector<size_t>& roi,
    const xt::xarray<int>& f,
    const xt::xarray<int>& g,
    bool periodic = true);

inline xt::xarray<double> C2(
    const std::vector<size_t>& roi,
    const xt::xarray<int>& f,
    const xt::xarray<int>& g,
    const xt::xarray<int>& fmask,
    const xt::xarray<int>& gmask,
    bool periodic = true);

template <class T>
inline xt::xarray<double> W2(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& w,
    const xt::xarray<T>& f,
    bool periodic = true);

template <class T>
inline xt::xarray<double> W2(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& w,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask,
    bool periodic = true);

template <class T>
inline xt::xarray<double> W2c(
    const std::vector<size_t>& roi,
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

template <class T>
inline xt::xarray<double> W2c(
    const std::vector<size_t>& roi,
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

template <class T>
inline xt::xarray<double> heightheight(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& f,
    bool periodic = true);

template <class T>
inline xt::xarray<double> heightheight(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask,
    bool periodic = true);

template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline xt::xarray<double> L(
    const std::vector<size_t>& roi,
    const xt::xarray<T>& f,
    bool periodic = true,
    path_mode mode = path_mode::Bresenham);

} // namespace GooseEYE

#include "Ensemble.hpp"
#include "Ensemble_C2.hpp"
#include "Ensemble_L.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_W2c.hpp"
#include "Ensemble_heightheight.hpp"
#include "Ensemble_mean.hpp"
#include "GooseEYE.hpp"
#include "clusters.hpp"
#include "dilate.hpp"
#include "dummy_circles.hpp"
#include "kernel.hpp"

#endif

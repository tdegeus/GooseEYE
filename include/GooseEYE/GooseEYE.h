/**
 *  \file
 *  \copyright Copyright 2017. Tom de Geus. All rights reserved.
 *  \license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

#include "config.h"
#include "detail.hpp"
#include "version.h"

namespace GooseEYE {

/**
 *  Collect kernels.
 */
namespace kernel {

/**
 *  Return kernel with nearest neighbours.
 *  \param ndim Number of dimensions (rank).
 *  \return The kernel.
 */
inline xt::xarray<int> nearest(size_t ndim);

} // namespace kernel

/**
 *  Random number generator.
 */
namespace random {

/**
 *  Set the random seed.
 *  \param seed Random seed: e.g. `static_cast<size_t>(std::time(0))`
 */
inline void seed(size_t seed = 0);

/**
 *  Return random numbers.
 *  \param shape Shape of the output array.
 */
auto random(const std::vector<size_t>& shape);

/**
 *  Return random numbers.
 *  \param shape Shape of the output array.
 */
template <class T>
auto random(const T& shape);

/**
 *  Return random numbers.
 *  \param shape Shape of the output array.
 *  \param mean The mean of the distribution.
 *  \param std_dev The standard deviation of the distribution.
 */
auto normal(const std::vector<size_t>& shape, double mean = 0, double std_dev = 1);

/**
 *  Return random numbers.
 *  \param shape Shape of the output array.
 *  \param mean The mean of the distribution.
 *  \param std_dev The standard deviation of the distribution.
 */
template <class T>
auto normal(const T& shape, double mean = 0, double std_dev = 1);

} // namespace random

/**
 *  Different methods to compute a pixel-path.
 */
enum class path_mode
{
    Bresenham, ///< Bresenham algorithm.
    actual, ///< The actual path.
    full ///< Similar to actual selecting every voxel that is crossed.
};

/**
 *  Compute a path between two pixels.
 *  \param x0 Pixel coordinate (e.g. {0, 0}).
 *  \param x1 Pixel coordinate (e.g. {10, 5}).
 *  \param mode Method to use (see "path_mode").
 *  \return The path: the coordinate of one pixel per row.
 */
inline xt::xtensor<int, 2> path(
    const xt::xtensor<int, 1>& x0,
    const xt::xtensor<int, 1>& x1,
    path_mode mode = path_mode::Bresenham);

/**
 *  Dummy image with circles. The positions and radii of the circles are randomly generated.
 *  \param shape Shape of the output image.
 *  \param periodic Switch to assume image periodic.
 *  \return The dummy image.
 */
inline xt::xarray<int> dummy_circles(const std::vector<size_t>& shape, bool periodic = true);

/**
 *  Dummy image with circles.
 *  \param shape Shape of the output image.
 *  \param row The x-position of each circle.
 *  \param col The y-position of each circle.
 *  \param r The radius of each circle.
 *  \param periodic Switch to assume image periodic.
 *  \return The dummy image.
 */
inline xt::xarray<int> dummy_circles(
    const std::vector<size_t>& shape,
    const xt::xtensor<int, 1>& row,
    const xt::xtensor<int, 1>& col,
    const xt::xtensor<int, 1>& r,
    bool periodic = true);

/**
 *  Dilate image. The input image can be binary (1), or have integer labels (>=1).
 *  In each case the background are 0.
 *  \param f The image (integer).
 *  \param kernel The kernel with which to dilate (binary).
 *  \param iterations Number of iterations per label.
 *  \param periodic Switch to assume image periodic.
 *  \return The dilated image.
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

/**
 *  Dilate image. Select "kernel::nearest" as kernel.
 *  See above for parameters.
 */
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    const xt::xtensor<size_t, 1>& iterations,
    bool periodic = true);

/**
 *  Dilate image. Fixed number of iterations for all labels.
 *  See above for parameters.
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

/**
 *  Dilate image. Fixed number of iterations for all labels. Select "kernel::nearest" as kernel.
 *  See above for parameters.
 */
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(
    const T& f,
    size_t iterations = 1,
    bool periodic = true);

/**
 *  Compute clusters and obtain certain characteristic about them.
 */
class Clusters {
public:

    Clusters() = default;

    /**
     *  Constructor. Use default kernel::nearest().
     *
     *  \param f Image.
     *  \param periodic Interpret image as periodic.
     */
    template <class T>
    Clusters(const T& f, bool periodic = true);

    /**
     *  Constructor.
     *
     *  \param f Image.
     *  \param kernel Kernel.
     *  \param periodic Interpret image as periodic.
     */
    template <class T, class S>
    Clusters(const T& f, const S& kernel, bool periodic = true);

    /**
     *  Return labels (1..n).
     *  \return 'Image'.
     */
    xt::xarray<int> labels() const;

    /**
     *  Return label only in the center of gravity (1..n).
     *  \return 'Image'.
     */
    xt::xarray<int> centers() const;

    /**
     *  Return positions of the centers of gravity (in the original rank, or as 3-d).
     *  \return List of positions.
     */
    xt::xtensor<double, 2> center_positions(bool as3d = false) const;

    /**
     *  Return size per cluster
     *  \return List.
     */
    xt::xtensor<size_t, 1> sizes() const;

private:

    void compute();

    template <class T>
    xt::xtensor<double, 2> average_position(const T& lab) const;

    xt::xtensor<double, 2> average_position_periodic() const;

    static const size_t MAX_DIM = 3;
    std::vector<size_t> m_shape; // shape of the input image
    std::vector<std::vector<size_t>> m_pad;
    xt::xtensor<int, 3> m_kernel;
    bool m_periodic;
    xt::xtensor<int, 3> m_l;    // labels (>= 1, 0 = background), 3-d
    xt::xtensor<int, 3> m_l_np; // labels before applying periodicity
};

/**
 *  Compute clusters. Wraps GooseEYE::Clusters::labels().
 *  \param f Image.
 *  \param periodic Interpret image as periodic.
 */
template <class T>
xt::xarray<int> clusters(const T& f, bool periodic = true);

/**
 *  Find map to relabel from "src" to "dest".
 *  \param src Image.
 *  \param dest Image.
 */
template <class T, class S>
xt::xtensor<size_t, 1> relabel_map(const T& src, const S& dest);

/**
 *  Convert positions to an image.
 *  \param img The image.
 *  \param positions The position.
 *  \param labels The label to apply for each image.
 *  \return The image, with labels inserted (overwritten) at the positions.
 */
template <typename T, typename U, typename V>
inline T pos2img(const T& img, const U& positions, const V& labels);

/**
 *  Get the position of the center of each label.
 *  \param labels An image with labels [0..N].
 *  \param periodic Switch to assume image periodic.
 *  \return The position of the center of each label.
 */
template <class T>
xt::xtensor<double, 2> center_of_mass(const T& labels, bool periodic = true);

/**
 *  Compute ensemble averaged statistics, by repetitively calling the member-function of a certain
 *  statistical measure with different data.
 *
 *  Note:
 *  - One class instance can only be used to compute one statistical measure.
 *  - Simple wrapper functions are provided to compute the statistical measure on one image.
 *  - The normalized result can be read at any moment using 'result', without affecting the raw-data.
 *  - The raw-data and normalization are separately stored as class member variables.
 */
class Ensemble {
public:

    /**
     *  Constructor.
     */
    Ensemble() = default;

    /**
     *  Constructor.
     *  \param roi Shape of the 'region-of-interest' on which the statistics are computed.
     *  \param periodic Switch to assume image periodic.
     *  \param variance Switch to compute the variance together with the mean.
     */
    Ensemble(const std::vector<size_t>& roi, bool periodic = true, bool variance = true);

    /**
     *  Get ensemble average.
     *  \return The average along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> result() const;

    /**
     *  Get ensemble variance.
     *  \return The variance along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> variance() const;

    /**
     *  Get raw-data: ensemble sum of the first moment: x_1 + x_2 + ...
     *  \return The sum along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> data_first() const;

    /**
     *  Get raw-data: ensemble sum of the second moment: x_1^2 + x_2^2 + ...
     *  \return The sum along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> data_second() const;

    /**
     *  Get raw-data: normalisation (number of measurements per pixel).
     *  \return The norm along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> norm() const;

    /**
     *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
     *  \return The distances along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> distance() const;

    /**
     *  Get the relative distance of each pixel in the 'region-of-interest' to its center,
     *  along a specific axis (distance can now be positive and negative).
     *  \param axis Select axis.
     *  \return The distances along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> distance(size_t axis) const;

    /**
     *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
     *  Convert the distance to physical dimensions.
     *  \param h The physical dimensions of one pixel (in each direction).
     *  \return The distances along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> distance(const std::vector<double>& h) const;

    /**
     *  Get the relative distance of each pixel in the 'region-of-interest' to its center,
     *  along a specific axis (distance can now be positive and negative).
     *  Convert the distance to physical dimensions.
     *  \param h The physical dimensions of one pixel (in each direction).
     *  \param axis Select axis.
     *  \return The distances along the 'region-of-interest' set at construction.
     */
    xt::xarray<double> distance(const std::vector<double>& h, size_t axis) const;

    /**
     *  Add realization to arithmetic mean.
     *  \param f The image.
     */
    template <class T>
    void mean(const T& f);

    /**
     *  Add realization to arithmetic mean.
     *  \param f The image.
     *  \param fmask Mask certain pixels (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void mean(const T& f, const M& fmask);

    /**
     *  Add realization to 2-point correlation: P(f(i) * g(i + di)).
     *  \param f The image.
     *  \param g The comparison image.
     */
    template <class T>
    void S2(const T& f, const T& g);

    /**
     *  Add realization to 2-point correlation: P(f(i) * g(i + di)).
     *  \param f The image.
     *  \param g The comparison image.
     *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     *  \param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void S2(const T& f, const T& g, const M& fmask, const M& gmask);

    /**
     *  Add realization to 2-point cluster function: P(f(i) == g(i + di)).
     *  \param f The image.
     *  \param g The comparison image.
     */
    template <class T>
    void C2(const T& f, const T& g);

    /**
     *  Add realization to 2-point cluster function: P(f(i) == g(i + di)).
     *  \param f The image.
     *  \param g The comparison image.
     *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     *  \param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void C2(const T& f, const T& g, const M& fmask, const M& gmask);

    /**
     *  Add realization to weighted 2-point correlation.
     *  \param w The weights.
     *  \param f The image.
     */
    template <class T>
    void W2(const T& w, const T& f);

    /**
     *  Add realization to weighted 2-point correlation.
     *  \param w The weights.
     *  \param f The image.
     *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void W2(const T& w, const T& f, const M& fmask);

    /**
     *  Add realization to collapsed weighted 2-point correlation
     *  \param clusters The clusters.
     *  \param centers The cluster-centers: label only at the center.
     *  \param f The image.
     *  \param mode Method to use (see "path_mode").
     */
    template <class C, class T>
    void W2c(
        const C& clusters,
        const C& centers,
        const T& f,
        path_mode mode = path_mode::Bresenham);

    /**
     *  Add realization to collapsed weighted 2-point correlation
     *  \param clusters The clusters.
     *  \param centers The cluster-centers: label only at the center.
     *  \param f The image.
     *  \param mode Method to use (see "path_mode").
     *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     */
    template <class C, class T, class M>
    void W2c(
        const C& clusters,
        const C& centers,
        const T& f,
        const M& fmask,
        path_mode mode = path_mode::Bresenham);

    /**
     *  Add realization to height-height correlation.
     *  \param f The image.
     */
    template <class T>
    void heightheight(const T& f);

    /**
     *  Add realization to height-height correlation.
     *  \param f The image.
     *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void heightheight(const T& f, const M& fmask);

    /**
     *  Add realization to lineal-path function
     *  \param f The image.
     *  \param mode Method to use (see "path_mode").
     */
    template <class T>
    void L(const T& f, path_mode mode = path_mode::Bresenham);

private:

    // Type: used to lock the ensemble to a certain measure.
    enum class Type { Unset, mean, S2, C2, W2, W2c, L, heightheight };

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
    xt::xtensor<double, 3> m_first;
    // - sum of the second moment: x_1^2 + x_2^2 + ...
    xt::xtensor<double, 3> m_second;
    // - number of measurements per pixel
    xt::xtensor<double, 3> m_norm;

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

/**
 *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
 *  \param roi Region-of-interest.
 *  \return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi);

/**
 *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
 *  \param roi Region-of-interest.
 *  \param axis Select axis.
 *  \return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, size_t axis);

/**
 *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
 *  \param roi Region-of-interest.
 *  \param h The physical dimensions of one pixel (in each direction).
 *  \return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h);

/**
 *  Get the relative distance of each pixel in the 'region-of-interest' to its center.
 *  \param roi Region-of-interest.
 *  \param h The physical dimensions of one pixel (in each direction).
 *  \param axis Select axis.
 *  \return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t axis);

/**
 *  2-point correlation: P(f(i) * g(i + di)).
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param g The comparison image.
 *  \param periodic Switch to assume image periodic.
 */
template <class T>
inline auto S2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    bool periodic = true);

/**
 *  2-point correlation: P(f(i) * g(i + di)).
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param g The comparison image.
 *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 *  \param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
 *  \param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto S2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    const M& fmask,
    const M& gmask,
    bool periodic = true);

/**
 *  2-point cluster function: P(f(i) == g(i + di)).
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param g The comparison image.
 *  \param periodic Switch to assume image periodic.
 */
template <class T>
inline auto C2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    bool periodic = true);

/**
 *  2-point cluster function: P(f(i) == g(i + di)).
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param g The comparison image.
 *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 *  \param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
 *  \param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto C2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    const M& fmask,
    const M& gmask,
    bool periodic = true);

/**
 *  Weighted 2-point correlation.
 *  \param roi Region-of-interest.
 *  \param w Weights.
 *  \param f The image.
 *  \param periodic Switch to assume image periodic.
 */
template <class T>
inline auto W2(
    const std::vector<size_t>& roi,
    const T& w,
    const T& f,
    bool periodic = true);

/**
 *  Weighted 2-point correlation.
 *  \param roi Region-of-interest.
 *  \param w Weights.
 *  \param f The image.
 *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 *  \param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto W2(
    const std::vector<size_t>& roi,
    const T& w,
    const T& f,
    const M& fmask,
    bool periodic = true);

/**
 *  Collapsed weighted 2-point correlation
 *  \param roi Region-of-interest.
 *  \param clusters The clusters.
 *  \param centers The cluster-centers: label only at the center.
 *  \param f The image.
 *  \param mode Method to use (see path_mode()).
 *  \param periodic Switch to assume image periodic.
 */
template <class C, class T>
inline auto W2c(
    const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

/**
 *  Collapsed weighted 2-point correlation
 *  \param roi Region-of-interest.
 *  \param clusters The clusters.
 *  \param centers The cluster-centers: label only at the center.
 *  \param f The image.
 *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 *  \param mode Method to use (see path_mode()).
 *  \param periodic Switch to assume image periodic.
 */
template <class C, class T, class M>
inline auto W2c(
    const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    const M& fmask,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

/**
 *  Height-height correlation.
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param periodic Switch to assume image periodic.
 */
template <class T>
inline auto heightheight(
    const std::vector<size_t>& roi,
    const T& f,
    bool periodic = true);

/**
 *  Height-height correlation.
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 *  \param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto heightheight(
    const std::vector<size_t>& roi,
    const T& f,
    const M& fmask,
    bool periodic = true);

/**
 *  Lineal-path function
 *  \param roi Region-of-interest.
 *  \param f The image.
 *  \param periodic Switch to assume image periodic.
 *  \param mode Method to use (see path_mode()).
 */
template <class T>
inline auto L(
    const std::vector<size_t>& roi,
    const T& f,
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
#include "random.hpp"

#endif

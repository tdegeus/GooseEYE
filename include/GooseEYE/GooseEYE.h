/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

#include "config.h"
#include "detail.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------
// Return different standard kernels
// -------------------------------------------------------------------------------------------------

namespace kernel {

  // nearest neighbours
  xt::xarray<int> nearest(size_t ndim);

}

// -------------------------------------------------------------------------------------------------
// Pixel path
// -------------------------------------------------------------------------------------------------

enum class path_mode
{
  Bresenham,
  actual,
  full
};

// -------------------------------------------------------------------------------------------------

auto path(
  const xt::xtensor<int,1>& x0,
  const xt::xtensor<int,1>& x1,
  path_mode mode=path_mode::Bresenham);

// -------------------------------------------------------------------------------------------------
// Dummy image
// -------------------------------------------------------------------------------------------------

// Dummy image with circles at position ("row", "col") with radius "r" (can be randomly generated)

xt::xarray<int> dummy_circles(
  const std::vector<size_t>& shape,
  bool periodic=true);

xt::xarray<int> dummy_circles(
  const std::vector<size_t>& shape,
  const xt::xtensor<int,1>& row,
  const xt::xtensor<int,1>& col,
  const xt::xtensor<int,1>& r,
  bool periodic=true);

// -------------------------------------------------------------------------------------------------
// Dilate image
// -------------------------------------------------------------------------------------------------

// dilate image
template <
  class T,
  class S,
  std::enable_if_t<std::is_integral<T>::value, int> = 0,
  std::enable_if_t<std::is_integral<S>::value, int> = 0>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xarray<S>& kernel,
  const xt::xtensor<size_t,1>& iterations,
  bool periodic=true);

// dilate image, with "kernel::nearest"
template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xtensor<size_t,1>& iterations,
  bool periodic=true);

// dilate image
template <
  class T,
  class S,
  std::enable_if_t<std::is_integral<T>::value, int> = 0,
  std::enable_if_t<std::is_integral<S>::value, int> = 0>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xarray<S>& kernel,
  size_t iterations=1,
  bool periodic=true);

// dilate image, with "kernel::nearest"
template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  size_t iterations=1,
  bool periodic=true);

// -------------------------------------------------------------------------------------------------
// Clusters
// -------------------------------------------------------------------------------------------------

class Clusters
{
public:

  // Constructors
  Clusters() = default;

  Clusters(
    const xt::xarray<int>& f,
    bool periodic=true);

  Clusters(
    const xt::xarray<int>& f,
    const xt::xarray<int>& kernel,
    bool periodic=true);

  // Return labels (1..n)
  xt::xarray<int> labels() const;

  // Return label only in the center of gravity
  xt::xarray<int> centers() const;

  // Return positions of the centers of gravity (in the original rank, or as 3-d)
  xt::xtensor<double,2> center_positions(bool as3d=false) const;

  // Return size per cluster
  xt::xtensor<size_t,1> sizes() const;

private:

  // Compute clusters
  void compute();

  // Compute position of the cluster centers
  xt::xtensor<double,2> average_position(const xt::xarray<int>& lab) const;
  xt::xtensor<double,2> average_position_periodic() const;

  // Maximum number of dimensions
  static const size_t MAX_DIM=3;

  // Shape of the image
  std::vector<size_t> m_shape;
  std::vector<size_t> m_Shape; // pseudo 3-d equivalent

  // Padding size
  std::vector<std::vector<size_t>> m_pad;
  std::vector<std::vector<size_t>> m_Pad; // pseudo 3-d equivalent

  // Kernel
  xt::xarray<int> m_kernel;

  // Shape of the kernel
  std::vector<size_t> m_shape_kernel;
  std::vector<size_t> m_Shape_kernel; // pseudo 3-d equivalent

  // Labels
  bool m_periodic;
  xt::xarray<int> m_l; // labels (>= 1, 0 = background), 3-d
  xt::xarray<int> m_l_np; // labels before applying periodicity

};

// Wrapper function

xt::xarray<int> clusters(const xt::xarray<int>& f, bool periodic=true);

// Find map to relabel

xt::xtensor<size_t,1> relabel_map(const xt::xarray<int>& src, const xt::xarray<int>& dest);

// -------------------------------------------------------------------------------------------------
// Class to compute ensemble averaged statistics. Simple wrapper functions are provided to compute
// the statistics on one image.
//
// The raw-data and normalization are separately stored as class member variables. The normalized
// result can be read at any moment using 'result'.
// -------------------------------------------------------------------------------------------------

class Ensemble
{
public:

  // Constructors

  Ensemble() = default;

  Ensemble(const std::vector<size_t>& roi, bool periodic=true, bool variance=true);

  // Get ensemble averaged result

  xt::xarray<double> result() const;

  // Get ensemble variance of ensemble average

  xt::xarray<double> variance() const;

  // Get ensemble sum of the first moment: x_1 + x_2 + ...

  xt::xarray<double> data_first() const;

  // Get ensemble sum of the second moment: x_1^2 + x_2^2 + ...

  xt::xarray<double> data_second() const;

  // Get normalisation (number of measurements) per 'pixel' of the ROI

  xt::xarray<double> norm() const;

  // Get the relative distance of each pixel of the ROI

  xt::xarray<double> distance() const;

  // Get the relative distance of each pixel of the ROI:
  // - along a dimension -> can be positive and negative

  xt::xarray<double> distance(size_t dim) const;

  // Get the relative distance of each pixel of the ROI:
  // - normalised by the pixel-size along each axis

  xt::xarray<double> distance(const std::vector<double>& h) const;

  // Get the relative distance of each pixel of the ROI:
  // - along a dimension -> can be positive and negative
  // - normalised by the pixel-size along the relevant axis

  xt::xarray<double> distance(const std::vector<double>& h, size_t dim) const;

  // Mean

  template <class T>
  void mean(const xt::xarray<T>& f);

  template <class T>
  void mean(const xt::xarray<T>& f, const xt::xarray<int>& fmask);

  // 2-point correlation

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

  // 2-point cluster

  void C2(
    const xt::xarray<int>& f,
    const xt::xarray<int>& g);

  void C2(
    const xt::xarray<int>& f,
    const xt::xarray<int>& g,
    const xt::xarray<int>& fmask,
    const xt::xarray<int>& gmask);

  // Weighted 2-point correlation

  template <class T>
  void W2(
    const xt::xarray<T>& w,
    const xt::xarray<T>& f);

  template <class T>
  void W2(
    const xt::xarray<T>& w,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask);

  // Collapsed weighted 2-point correlation

  template <class T>
  void W2c(
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    path_mode mode=path_mode::Bresenham);

  template <class T>
  void W2c(
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask,
    path_mode mode=path_mode::Bresenham);

  // Height-Height Correlation function

  template <class T>
  void heightheight(
    const xt::xarray<T>& f);

  template <class T>
  void heightheight(
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask);

  // Lineal-path function

  template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
  void L(
    const xt::xarray<T>& f,
    path_mode mode=path_mode::Bresenham);

private:

  // Type: used to lock the ensemble to a certain measure
  struct Type {
    enum : unsigned {
      Unset = 0x00u,
      mean = 0x01u,
      S2 = 0x02u,
      C2 = 0x03u,
      W2 = 0x04u,
      W2c = 0x05u,
      L = 0x06u,
      heightheight = 0x07u,
  };};

  // Initialize class as unlocked
  unsigned m_stat=Type::Unset;

  // Maximum number of dimensions
  static const size_t MAX_DIM=3;

  // Periodicity settings used for the entire cluster
  bool m_periodic;

  // Signal to compute the variance
  bool m_variance;

  // Raw (not normalized) result, and normalization
  xt::xarray<double> m_first; // sum of the first moment: x_1 + x_2 + ...
  xt::xarray<double> m_second; // sum of the second moment: x_1^2 + x_2^2 + ...
  xt::xarray<double> m_norm; // number of measurements per pixel

  // Shape of the ROI (of "m_data" and "m_norm")
  std::vector<size_t> m_shape;
  std::vector<size_t> m_Shape; // pseudo 3-d equivalent

  // Padding size
  std::vector<std::vector<size_t>> m_pad;
  std::vector<std::vector<size_t>> m_Pad; // pseudo 3-d equivalent

};

// -------------------------------------------------------------------------------------------------
// Wrapper functions to compute the statistics for one image
// -------------------------------------------------------------------------------------------------

// Distance (see "Ensemble::distance")

xt::xarray<double> distance(const std::vector<size_t>& roi);

xt::xarray<double> distance(const std::vector<size_t>& roi, size_t dim);

xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h);

xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t dim);

// 2-point correlation

template <class T>
xt::xarray<double> S2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<T>& g,
  bool periodic=true);

template <class T>
xt::xarray<double> S2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<T>& g,
  const xt::xarray<int>& fmask,
  const xt::xarray<int>& gmask,
  bool periodic=true);

// 2-point cluster

xt::xarray<double> C2(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& f,
  const xt::xarray<int>& g,
  bool periodic=true);

xt::xarray<double> C2(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& f,
  const xt::xarray<int>& g,
  const xt::xarray<int>& fmask,
  const xt::xarray<int>& gmask,
  bool periodic=true);

// Weighted 2-point correlation

template <class T>
xt::xarray<double> W2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& w,
  const xt::xarray<T>& f,
  bool periodic=true);

template <class T>
xt::xarray<double> W2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& w,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic=true);

// Collapsed weighted 2-point correlation

template <class T>
xt::xarray<double> W2c(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& clusters,
  const xt::xarray<int>& centers,
  const xt::xarray<T>& f,
  path_mode mode=path_mode::Bresenham,
  bool periodic=true);

template <class T>
xt::xarray<double> W2c(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& clusters,
  const xt::xarray<int>& centers,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  path_mode mode=path_mode::Bresenham,
  bool periodic=true);

// Height-Height Correlation Function

template <class T>
xt::xarray<double> heightheight(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  bool periodic=true);

template <class T>
xt::xarray<double> heightheight(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic=true);

// Lineal-path function

template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
xt::xarray<double> L(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  bool periodic=true,
  path_mode mode=path_mode::Bresenham);

// -------------------------------------------------------------------------------------------------

} // namespace ...

// -------------------------------------------------------------------------------------------------

#include "detail.hpp"
#include "GooseEYE.hpp"
#include "dummy_circles.hpp"
#include "kernel.hpp"
#include "dilate.hpp"
#include "clusters.hpp"
#include "Ensemble.hpp"
#include "Ensemble_mean.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_C2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_W2c.hpp"
#include "Ensemble_heightheight.hpp"
#include "Ensemble_L.hpp"

#endif

/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

#include "config.h"
#include "detail.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------
// Kernel
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

xt::xarray<int> path(
  const std::vector<int> &xa,
  const std::vector<int> &xb,
  path_mode mode=path_mode::Bresenham);

// -------------------------------------------------------------------------------------------------
// Dummy image
// -------------------------------------------------------------------------------------------------

// Dummy image with circles at position "row","col" with radius "r" (may be automatic)

xt::xarray<int> dummy_circles(
  const std::vector<size_t>& shape,
  bool periodic=true);

xt::xarray<int> dummy_circles(
  const std::vector<size_t>& shape,
  const std::vector<int>& row,
  const std::vector<int>& col,
  const std::vector<int>& r,
  bool periodic=true);

// -------------------------------------------------------------------------------------------------
// Clusters
// -------------------------------------------------------------------------------------------------

class Clusters
{
public:

  // Constructors

  Clusters() = default;

  Clusters(const xt::xarray<int>& f, const xt::xarray<int>& kernel, bool periodic);

  // Return result

  xt::xarray<int> get() const;

private:

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
  xt::xarray<int> m_l;

};

// front-end
xt::xarray<int> clusters(const xt::xarray<int>& f, bool periodic=true);

// -------------------------------------------------------------------------------------------------
// Class to compute ensemble averaged statistics. Simple front-end functions are provided to compute
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

  Ensemble(const std::vector<size_t>& roi, bool periodic=true);

  // Get ensemble averaged result or raw data, and distance

  xt::xarray<double> result() const;
  xt::xarray<double> data() const;
  xt::xarray<double> norm() const;
  xt::xarray<double> distance() const;
  xt::xarray<double> distance(size_t dim) const;

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

  // roughness

  template <class T>
  void roughness(
    const xt::xarray<T>& f);

  template <class T>
  void roughness(
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask);

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
      roughness = 0x07u,
  };};

  // Initialize class as unlocked
  unsigned m_stat=Type::Unset;

  // Maximum number of dimensions
  static const size_t MAX_DIM=3;

  // Periodicity settings used for the entire cluster
  bool m_periodic;

  // Raw (not normalized) result, and normalization
  xt::xarray<double> m_data;
  xt::xarray<double> m_norm;

  // Shape of the ROI (of "m_data" and "m_norm")
  std::vector<size_t> m_shape;
  std::vector<size_t> m_Shape; // pseudo 3-d equivalent

  // Padding size
  std::vector<std::vector<size_t>> m_pad;
  std::vector<std::vector<size_t>> m_Pad;

};

// -------------------------------------------------------------------------------------------------
// font-end functions to compute the statistics for one image
// -------------------------------------------------------------------------------------------------

// Distance

xt::xarray<double> distance(const std::vector<size_t>& roi);

xt::xarray<double> distance(const std::vector<size_t>& roi, size_t dim);

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

// roughness

template <class T>
xt::xarray<double> roughness(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  bool periodic=true);

template <class T>
xt::xarray<double> roughness(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic=true);

// -------------------------------------------------------------------------------------------------

} // namespace ...

// -------------------------------------------------------------------------------------------------

#include "detail.hpp"
#include "GooseEYE.hpp"
#include "dummy_circles.hpp"
#include "path.hpp"
#include "kernel.hpp"
#include "clusters.hpp"
#include "Ensemble.hpp"
#include "Ensemble_mean.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_C2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_roughness.hpp"

#endif

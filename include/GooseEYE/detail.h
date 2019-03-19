/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DETAIL_H
#define GOOSEEYE_DETAIL_H

#include "config.h"

namespace GooseEYE {
namespace detail {

// Return shape as quasi-"ndim", prepend with a set constant

inline std::vector<size_t> as_dim(
  const size_t ndim, const std::vector<size_t>& shape, size_t prepend);

inline std::vector<std::vector<size_t>> as_dim(
  const size_t ndim, const std::vector<std::vector<size_t>>& shape, size_t prepend);

template <class T>
inline std::vector<size_t> shape_as_dim(
  const size_t ndim, const xt::xarray<T>& f, size_t prepend);

// Compute padding width for a certain kernel.
// This is the number of voxels by which a kernel will overlap along each axis when it is
// convoluted over an image.
// The output is {{begin_1, end_1}, {begin_2, end_2}, ... {begin_N,  end_N}}
inline std::vector<std::vector<size_t>> pad_width(const std::vector<size_t>& shape);

template <class T>
inline std::vector<std::vector<size_t>> pad_width(const xt::xarray<T>& f);

}}

#endif

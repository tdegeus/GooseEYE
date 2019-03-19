/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DETAIL_HPP
#define GOOSEEYE_DETAIL_HPP

#include "GooseEYE.h"

namespace GooseEYE {
namespace detail {

// -------------------------------------------------------------------------------------------------

inline std::vector<size_t> as_dim(
  const size_t ndim,
  const std::vector<size_t>& shape,
  size_t prepend)
{
  std::vector<size_t> out(ndim, prepend);

  size_t pad = ndim - shape.size();

  for (size_t i = 0; i < shape.size(); ++i)
    out[i+pad] = shape[i];

  return out;
}

// -------------------------------------------------------------------------------------------------

inline std::vector<std::vector<size_t>> as_dim(
  const size_t ndim,
  const std::vector<std::vector<size_t>>& shape,
  size_t prepend)
{
  #ifndef NDEBUG
    assert(shape.size() > 0);
    for (auto& i: shape)
      assert(i.size() == shape[0].size());
  #endif

  std::vector<std::vector<size_t>> out(ndim, std::vector<size_t>(shape[0].size(), prepend));

  size_t pad = ndim - shape.size();

  for (size_t i = 0; i < shape.size(); ++i)
    for (size_t j = 0; j < shape[i].size(); ++j)
      out[i+pad][j] = shape[i][j];

  return out;
}

// -------------------------------------------------------------------------------------------------

template <class T>
inline std::vector<size_t> shape_as_dim(
  const size_t ndim,
  const xt::xarray<T>& f,
  size_t prepend)
{
  std::vector<size_t> shape(f.shape().cbegin(), f.shape().cend());
  return as_dim(ndim, shape, prepend);
}

// -------------------------------------------------------------------------------------------------

inline std::vector<std::vector<size_t>> pad_width(const std::vector<size_t>& shape)
{
  std::vector<std::vector<size_t>> pad(shape.size(), std::vector<size_t>(2));

  for (size_t i = 0; i < shape.size(); ++i) {
    if (shape[i]%2 == 0) {
      pad[i][0] = shape[i]/2 - 1;
      pad[i][1] = shape[i]/2;
    }
    else {
      pad[i][0] = (shape[i]-1)/2;
      pad[i][1] = (shape[i]-1)/2;
    }
  }

  return pad;
}

// -------------------------------------------------------------------------------------------------

template <class T>
inline std::vector<std::vector<size_t>> pad_width(const xt::xarray<T>& f)
{
  std::vector<size_t> shape(f.shape().cbegin(), f.shape().cend());
  return pad_width(shape);
}

// -------------------------------------------------------------------------------------------------

}} // namespace ...

#endif

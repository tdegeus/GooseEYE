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

template <class T>
inline std::vector<size_t> shape(const xt::xarray<T>& f)
{
  return std::vector<size_t>(f.shape().cbegin(), f.shape().cend());
}

// -------------------------------------------------------------------------------------------------

inline std::vector<size_t> half_shape(const std::vector<size_t>& shape)
{
  std::vector<size_t> out = shape;

  for (size_t i = 0; i < out.size(); ++i) {
    if (out[i] % 2 == 0) {
      out[i] = out[i] / 2;
    }
    else {
      out[i] = (out[i] - 1) / 2;
    }
  }

  return out;
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

// https://www.geeksforgeeks.org/bresenhams-algorithm-for-3-d-line-drawing/
void bressenham(
  xt::xtensor<size_t,2>& points,
  const xt::xtensor_fixed<size_t, xt::xshape<3>>& x0,
  const xt::xtensor_fixed<size_t, xt::xshape<3>>& x1
)
{
  size_t axis, npoints, ipoint;
  xt::xtensor_fixed<size_t, xt::xshape<3>> x, dx;
  xt::xtensor_fixed<int, xt::xshape<3>> s, p;

  x = x0;

  // Slopes
  for( size_t i = 0; i < 3; ++i ) {
    dx[i] = std::abs( (int) x1[i] - (int) x0[i] );
    s[i] = x1[i] > x0[i] ? 1 : -1;
  };

  // Determine driving axis
  if( dx[0] >= dx[1] && dx[0] >= dx[2] ) {
    axis = 0;
    npoints = dx[0] + 1;
  }
  else if ( dx[1] >= dx[0] && dx[1] >= dx[2] ) {
    axis = 1;
    npoints = dx[1] + 1;
  }
  else {
    axis = 2;
    npoints = dx[2] + 1;
  }

  // Slope errors
  p = 2 * dx - dx[axis];

  // Allocate points
  points.resize({npoints, 3});
  xt::view(points, 0, xt::all()) = x;
  ipoint = 1;

  // Loop until end of line
  while( x[axis] != x1[axis] )
  {
    x[axis] += s[axis];

    for( size_t i = 0; i < 3; ++i )
    {
      if( i != axis && p[i] >= 0 )
      {
        x[i] += s[i];
        p[i] -= 2 * dx[axis];
      }

      p[i] += 2 * dx[i];
    }

    xt::view(points, ipoint++, xt::all()) = x;
  }
}

// -------------------------------------------------------------------------------------------------

}} // namespace ...

#endif

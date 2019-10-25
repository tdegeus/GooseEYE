/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DILATE_HPP
#define GOOSEEYE_DILATE_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xarray<int>& kernel,
  const xt::xtensor<size_t,1>& iterations,
  bool periodic)
{
  GOOSEEYE_ASSERT(xt::all(xt::equal(kernel,0) || xt::equal(kernel,1)));
  GOOSEEYE_ASSERT(xt::all(xt::greater_equal(f,0)));
  GOOSEEYE_ASSERT(f.dimension() == kernel.dimension());
  GOOSEEYE_ASSERT(iterations.size() > xt::amax(iterations)(0));

  // maximum number of dimensions
  static const size_t MAX_DIM=3;

  // read/convert input
  auto shape = detail::shape(f);
  auto shape_kernel = detail::shape(kernel);
  auto pad = detail::pad_width(kernel);
  auto Shape = detail::as_dim(MAX_DIM, shape, 1);
  auto Shape_kernel = detail::as_dim(MAX_DIM, shape_kernel, 1);
  auto Pad = detail::as_dim(MAX_DIM, pad, 0);

  // copy input, make pseudo 3-d
  auto F = f;
  auto K = kernel;
  F.reshape(Shape);
  K.reshape(Shape_kernel);

  // padding default not periodic: mask padded items
  xt::pad_mode pad_mode = xt::pad_mode::constant;
  int pad_value = 0;

  // padding optionally periodic
  if (periodic)
    pad_mode = xt::pad_mode::periodic;

  // apply padding
  F = xt::pad(F, Pad, pad_mode, pad_value);

  // apply iterations
  for (size_t iter = 0; iter < xt::amax(iterations)(0); ++iter) {
    // loop over the image
    for (size_t h = Pad[0][0]; h < Shape[0]-Pad[0][1]; ++h) {
      for (size_t i = Pad[1][0]; i < Shape[1]-Pad[1][1]; ++i) {
        for (size_t j = Pad[2][0]; j < Shape[2]-Pad[2][1]; ++j) {
          // - get label
          T l = F(h,i,j);
          // - skip if needed: do not dilate background or labels added in this iteration
          if (l <= 0)
            continue;
          // - skip if needed: maximum number of iterations per label
          if (iter >= iterations(l))
            continue;
          // - get sub-matrix around (h, i, j)
          auto Fi = xt::view(F,
            xt::range(h-Pad[0][0], h+Pad[0][1]+1),
            xt::range(i-Pad[1][0], i+Pad[1][1]+1),
            xt::range(j-Pad[2][0], j+Pad[2][1]+1));
          // - dilate where needed: dilated labels are added as negative numbers
          Fi = xt::where(xt::equal(Fi, 0) && xt::equal(kernel, 1), - l * K, Fi);
        }
      }
    }
    // flip added label
    F = xt::abs(F);
  }

  // TODO: apply periodicity ????

  // remove padding
  F = xt::view(F,
    xt::range(Pad[0][0], F.shape(0)-Pad[0][1]),
    xt::range(Pad[1][0], F.shape(1)-Pad[1][1]),
    xt::range(Pad[2][0], F.shape(2)-Pad[2][1]));

  // return to original rank
  F.reshape(shape);

  // return output
  return F;
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xarray<int>& kernel,
  size_t iterations,
  bool periodic)
{
  return dilate(f, kernel, iterations * xt::ones<size_t>({xt::amax(f)(0) + 1}), periodic);
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  const xt::xtensor<size_t,1>& iterations,
  bool periodic)
{
  return dilate(f, kernel::nearest(f.dimension()), iterations, periodic);
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
  const xt::xarray<T>& f,
  size_t iterations,
  bool periodic)
{
  return dilate(
    f,
    kernel::nearest(f.dimension()),
    iterations * xt::ones<size_t>({xt::amax(f)(0) + 1}),
    periodic);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

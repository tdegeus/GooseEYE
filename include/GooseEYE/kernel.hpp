/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_KERNEL_HPP
#define GOOSEEYE_KERNEL_HPP

#include "GooseEYE.h"

namespace GooseEYE {
namespace kernel {

// -------------------------------------------------------------------------------------------------

xt::xarray<int> nearest(size_t ndim)
{
  GOOSEEYE_ASSERT(ndim > 0 && ndim <= 3);

  std::vector<size_t> shape(ndim, 3);

  xt::xarray<int> kern = xt::zeros<int>(shape);

  if (ndim == 1) {
    xt::view(kern, xt::all()) = 1;
    return kern;
  }

  if (ndim == 2) {
    xt::view(kern, xt::keep(1), xt::all()) = 1;
    xt::view(kern, xt::all(), xt::keep(1)) = 1;
    return kern;
  }

  xt::view(kern, xt::keep(1), xt::all(), xt::all()) = 1;
  xt::view(kern, xt::all(), xt::keep(1), xt::all()) = 1;
  xt::view(kern, xt::all(), xt::all(), xt::keep(1)) = 1;
  return kern;
}

// -------------------------------------------------------------------------------------------------

}} // namespace ...

#endif

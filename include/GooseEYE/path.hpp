/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_PATH_HPP
#define GOOSEEYE_PATH_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

auto path(
  const xt::xtensor<int,1>& x0,
  const xt::xtensor<int,1>& x1,
  path_mode mode)
{
  if (mode == path_mode::Bresenham) {
    return detail::path::bressenham(x0, x1);
  }
  else if (mode == path_mode::actual) {
    return detail::path::actual(x0, x1);
  }

  return detail::path::full(x0, x1);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

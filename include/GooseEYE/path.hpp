/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_PATH_HPP
#define GOOSEEYE_PATH_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

xt::xtensor<size_t,2> path(
  const xt::xtensor_fixed<size_t, xt::xshape<3>>& x0,
  const xt::xtensor_fixed<size_t, xt::xshape<3>>& x1,
  path_mode mode)
{
  xt::xtensor<size_t,2> points;

  if (mode == path_mode::Bresenham) {
    detail::bressenham( points, x0, x1 );
  }
  else {
    throw std::runtime_error( "Path mode not available" );
  }

  return points;

}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

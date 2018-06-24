/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_KERNEL_HPP
#define GOOSEEYE_KERNEL_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================

ArrI kernel(size_t ndim, std::string mode)
{
  std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

  if ( mode=="default" )
  {
    VecS shape = VecS::Constant(ndim, 3);

    ArrI kern = ArrI::Zero(shape);

    if ( ndim == 1 )
    {
      kern(0) = 1; kern(1) = 1; kern(2) = 1;

      return kern;
    }

    if ( ndim == 2 )
    {
      kern(1,0) = 1; kern(1,1) = 1; kern(1,2) = 1;
      kern(0,1) = 1; kern(2,1) = 1;

      return kern;
    }

    if ( ndim == 3 )
    {
      kern(1,1,0) = 1; kern(1,1,1) = 1; kern(1,1,2) = 1;
      kern(1,0,1) = 1; kern(1,2,1) = 1;
      kern(0,1,1) = 1; kern(2,1,1) = 1;

      return kern;
    }
  }

  throw std::length_error("Unknown ndim/mode");
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

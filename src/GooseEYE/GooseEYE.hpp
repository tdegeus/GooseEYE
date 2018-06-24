/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_HPP
#define GOOSEEYE_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================

MatI stampPoints(const VecS &shape)
{
  return Ensemble(shape).stampPoints();
}

// =================================================================================================

ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.S2(f, g);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g, const ArrI &fmask, const ArrI &gmask,
  bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.S2(f, g, fmask, gmask);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.S2(f, g);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g, const ArrI &fmask, const ArrI &gmask,
  bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.S2(f, g, fmask, gmask);

  return ensemble.result();
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

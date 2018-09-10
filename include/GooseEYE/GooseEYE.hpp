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
// wrapper function: list of end-points of ROI-stamp used in path-based correlations
// =================================================================================================

MatI stampPoints(const VecS &shape)
{
  return Ensemble(shape).stampPoints();
}

// =================================================================================================
// wrapper functions: 2-point correlation
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
// wrapper functions: weighted 2-point correlation
// =================================================================================================

ArrD W2(const VecS &roi, const ArrI &w, const ArrI &f,                    bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrI &w, const ArrI &f, const ArrI &fmask, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f, fmask);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrI &w, const ArrD &f,                    bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrI &w, const ArrD &f, const ArrI &fmask, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f, fmask);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrD &w, const ArrI &f,                    bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrD &w, const ArrI &f, const ArrI &fmask, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f, fmask);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrD &w, const ArrD &f,                    bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2(const VecS &roi, const ArrD &w, const ArrD &f, const ArrI &fmask, bool periodic, bool pad)
{
  Ensemble ensemble(roi, periodic, pad);

  ensemble.W2(w, f, fmask);

  return ensemble.result();
}

// =================================================================================================
// wrapper functions: collapsed weighted 2-point correlation
// =================================================================================================

ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrI &f,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c(clus, cntr, f, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrI &f, const ArrI &fmask,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c(clus, cntr, f, fmask, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrD &f,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c(clus, cntr, f, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrD &f, const ArrI &fmask,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c(clus, cntr, f, fmask, mode);

  return ensemble.result();
}

// =================================================================================================
// wrapper functions: collapsed weighted 2-point correlation
// =================================================================================================

ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrI &f,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c_auto(w, f, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrI &f, const ArrI &fmask,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c_auto(w, f, fmask, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrD &f,
  bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c_auto(w, f, mode);

  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrD &f, const ArrI &fmask,
  bool periodic, std::string mode)

{
  Ensemble ensemble(roi, periodic);

  ensemble.W2c_auto(w, f, fmask, mode);

  return ensemble.result();
}

// =================================================================================================
// wrapper functions: lineal path function
// =================================================================================================

ArrD L(const VecS &roi, const ArrI &f, bool periodic, std::string mode)
{
  Ensemble ensemble(roi, periodic);

  ensemble.L(f, mode);

  return ensemble.result();
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

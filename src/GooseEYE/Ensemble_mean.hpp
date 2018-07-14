/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_MEAN_HPP
#define GOOSEEYE_ENSEMBLE_MEAN_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// mean
// =================================================================================================

void Ensemble::mean(const ArrD &f)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("GooseEYE::mean - rank inconsistent");

  // loop over image
  for ( size_t i = 0 ; i < f.size() ; ++i ) {
    mData[i] += f[i];
    mNorm[i] += 1.;
  }
}

// =================================================================================================
// mean
// =================================================================================================

void Ensemble::mean(const ArrD &f, const ArrI &fmask)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("GooseEYE::mean - rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::length_error("GooseEYE::mean - shape inconsistent");

  // loop over image
  for ( size_t i = 0 ; i < f.size() ; ++i ) {
    if ( ! fmask[i] ) {
      mData[i] += f[i];
      mNorm[i] += 1.;
    }
  }
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

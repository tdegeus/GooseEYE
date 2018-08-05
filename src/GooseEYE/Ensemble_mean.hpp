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
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::mean;

  // checks
  std::string name = "GooseEYE::Ensemble::mean - ";
  if ( f.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");

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
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::mean;

  // checks
  std::string name = "GooseEYE::Ensemble::mean - ";
  if ( f.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::runtime_error(name+"shape inconsistent");

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

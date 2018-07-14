/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_HPP
#define GOOSEEYE_ENSEMBLE_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// constructor
// =================================================================================================

inline
Ensemble::Ensemble(const VecS &roi, bool periodic, bool zero_pad) : mPeriodic(periodic)
{
  // check
  if ( roi.size() > MAX_DIM )
    throw std::domain_error("Rank 'roi' too large");

  // check
  for ( auto &i : roi )
    if ( i%2 == 0 )
      throw std::domain_error("'roi' must be odd shaped");

  // initialize
  mShape = roi;
  mMid   = roi;

  // midpoint of ROI
  for ( auto &i : mMid ) i = (i-1)/2;

  // pad with half the ROI size
  if ( zero_pad ) mPad = mMid;

  // pad to "MAX_DIM"
  mShape.resize(MAX_DIM, 1);
  mMid  .resize(MAX_DIM, 0);

  // set skip size
  // - initialize
  mSkip = mMid;
  // - switch-off
  if ( periodic or zero_pad ) mSkip *= 0;

  // allocate average
  mData = ArrD::Zero(roi);
  mNorm = ArrD::Zero(roi);
}

// =================================================================================================
// return normalised result
// =================================================================================================

inline
cppmat::array<double> Ensemble::result() const
{
  ArrD norm = cppmat::max( mNorm, ArrD::Ones(mNorm.shape()) );

  return mData / norm;
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

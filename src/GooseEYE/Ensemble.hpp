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

  // shape of the region-of-interest
  // - initialize
  for ( size_t i = 0 ; i < MAX_DIM    ; ++i ) mShape[i] = 1;
  // - copy from input
  for ( size_t i = 0 ; i < roi.size() ; ++i ) mShape[i] = roi[i];

  // midpoint of ROI
  for ( size_t i = 0 ; i < MAX_DIM ; ++i ) mMid[i] = (mShape[i]-1)/2;

  // pad with half the ROI size
  if ( zero_pad )
  {
    // - allocate
    mPad.resize(roi.size());
    // - fill
    for ( size_t i = 0 ; i < mPad.size() ; ++i ) mPad[i] = mMid[i];
  }

  // set skip size
  for ( size_t i = 0 ; i < MAX_DIM ; ++i )
  {
    if ( periodic or zero_pad ) mSkip[i] = 0;
    else                        mSkip[i] = mMid[i];
  }

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

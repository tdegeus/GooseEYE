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

inline
Ensemble::Ensemble(const std::vector<size_t> roi, bool periodic, bool pad) : mPeriodic(periodic)
{
  // check region-of-interest
  // ------------------------

  if ( roi.size() > MAX_DIM )
    throw std::domain_error("Rank 'roi' too large");

  for ( auto &i : roi )
    if ( i%2 == 0 )
      throw std::domain_error("'roi' must be odd shaped");

  // get midpoint of the ROI
  // -----------------------

  mMid.resize(roi.size());

  for ( size_t i = 0 ; i < roi.size() ; ++i )
    mMid[i] = static_cast<int>((roi[i]-1)/2);

  mMid.resize(static_cast<int>(MAX_DIM), 0);

  // get region to skip
  // ------------------

  if ( periodic or pad )
    mSkip.resize(static_cast<int>(MAX_DIM), 0);
  else
    mSkip = mMid;

  // pad size
  // --------

  if ( pad )
  {
    mPad.resize(roi.size());

    for ( size_t i = 0 ; i < roi.size() ; ++i )
      mPad[i] = static_cast<size_t>(mMid[i]);
  }

  // allocate average
  // ----------------

  mData.resize(roi);
  mNorm.resize(roi);

  mData.setZero();
  mNorm.setZero();
}

// =================================================================================================

inline
cppmat::array<double> Ensemble::result() const
{
  return mData / mNorm;
}

// =================================================================================================

template<class T>
inline
std::vector<int> Ensemble::shape(const cppmat::array<T> &A, const std::string &name) const
{
  if ( A.rank() > MAX_DIM )
    throw std::domain_error("Rank of '"+name+"' too large");

  std::vector<size_t> shape = A.shape();

  shape.resize(MAX_DIM, 1);

  std::vector<int> out(MAX_DIM);

  for ( size_t i = 0 ; i < MAX_DIM ; ++i )
    out[i] = static_cast<int>(shape[i]);

  return out;
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

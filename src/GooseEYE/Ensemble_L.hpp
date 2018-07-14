/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_L_HPP
#define GOOSEEYE_ENSEMBLE_L_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================

void Ensemble::L(ArrI f, std::string mode)
{
  // checks
  if ( f.rank() != mData.rank() ) throw std::length_error("GooseEYE::L - rank inconsistent");

  // switch off bound-checks based on periodicity settings
  f.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  f.chrank(3);

  // list of end-points of ROI-stamp used in path-based correlations (make 3-d to simply below)
  MatI stamp = stampPoints(3);

  // correlation
  for ( size_t ipnt = 0 ; ipnt < stamp.shape(0) ; ++ipnt )
  {
    // - voxel-path
    MatI pix = path({0,0,0}, {stamp(ipnt,0), stamp(ipnt,1), stamp(ipnt,2)}, mode);
    // - compute correlation
    for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h ) {
      for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i ) {
        for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j ) {
          for ( size_t ipix = 0 ; ipix < pix.shape(0) ; ++ipix ) {
            // -- get current voxel
            int dh = pix(ipix,0);
            int di = pix(ipix,1);
            int dj = pix(ipix,2);
            // -- check to terminate this path
            if ( ! f(h+dh,i+di,j+dj) ) break;
            // -- update result
            mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;
          }
        }
      }
    }
  }

  // number data-points
  double N = static_cast<double>((f.shape(0)-mSkip[0])*(f.shape(1)-mSkip[1])*(f.shape(2)-mSkip[2]));

  // normalization
  for ( size_t ipnt = 0 ; ipnt < stamp.shape(0) ; ++ipnt )
  {
    // - voxel-path
    MatI pix = path({0,0,0}, {stamp(ipnt,0), stamp(ipnt,1), stamp(ipnt,2)}, mode);
    // - loop over voxel-path
    for ( size_t ipix = 0 ; ipix < pix.shape(0) ; ++ipix )
      mNorm(pix(ipix,0)+mMid[0],pix(ipix,1)+mMid[1],pix(ipix,2)+mMid[2]) += N;
  }
}
// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

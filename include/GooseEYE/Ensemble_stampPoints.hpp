/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP
#define GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// list of end-points of ROI-stamp used in path-based correlations
// =================================================================================================

MatI Ensemble::stampPoints(size_t N) const
{
  int n,i,j;
  int idx = 0;
  int nd  = static_cast<int>(mData.rank());
  int H   = mShape[0];
  int I   = mShape[1];
  int J   = mShape[2];
  int dH  = mMid  [0];
  int dI  = mMid  [1];
  int dJ  = mMid  [2];

  // number of points
  if ( nd==1 ) n =           2;
  if ( nd==2 ) n =           2*H+2*POS(I-2);
  if ( nd==3 ) n = POS(J-2)*(2*H+2*POS(I-2))+2*H*I;

  // allocate
  MatI ret = MatI::Zero(static_cast<size_t>(n), std::max(N,static_cast<size_t>(nd)));

  // 1-D
  // ---

  if ( nd==1 )
  {
    ret(idx,0) = -dH; idx++;
    ret(idx,0) = +dH; idx++;
    return ret;
  }

  // 2-D
  // ---

  if ( nd==2 )
  {
    for ( i=0 ; i<H   ; i++ ) {
      ret(idx,0) = i-dH; ret(idx,1) =  -dI; idx++;
      ret(idx,0) = i-dH; ret(idx,1) =  +dI; idx++;
    }
    for ( i=1 ; i<I-1 ; i++ ) {
      ret(idx,0) =  -dH; ret(idx,1) = i-dI; idx++;
      ret(idx,0) =  +dH; ret(idx,1) = i-dI; idx++;
    }
    return ret;
  }

  // 3-D
  // ---

  for ( j=1 ; j<J-1 ; j++ ) {
    for ( i=0 ; i<H   ; i++ ) {
      ret(idx,0) = i-dH; ret(idx,1) =  -dI; ret(idx,2) = j-dJ; idx++;
      ret(idx,0) = i-dH; ret(idx,1) =  +dI; ret(idx,2) = j-dJ; idx++;
    }
    for ( i=1 ; i<I-1 ; i++ ) {
      ret(idx,0) =  -dH; ret(idx,1) = i-dI; ret(idx,2) = j-dJ; idx++;
      ret(idx,0) =  +dH; ret(idx,1) = i-dI; ret(idx,2) = j-dJ; idx++;
    }
  }
  for ( i=0 ; i<H   ; i++ ) {
    for ( j=0 ; j<I   ; j++ ) {
      ret(idx,0) = i-dH; ret(idx,1) = j-dI; ret(idx,2) =  -dJ; idx++;
      ret(idx,0) = i-dH; ret(idx,1) = j-dI; ret(idx,2) =  +dJ; idx++;
    }
  }
  return ret;
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

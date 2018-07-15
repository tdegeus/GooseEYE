/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DILATE_HPP
#define GOOSEEYE_DILATE_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// core functions: called by wrappers below
// =================================================================================================

ArrI dilate(ArrI f, ArrI kern, const VecS &iterations, bool periodic)
{
  // check input
  if ( static_cast<size_t>(f.max()+1) != iterations.size() )
    throw std::length_error("Iteration must be specified for each label");

  if ( f.min() < 0 )
    throw std::length_error("Iteration must be specified for each label");

  // apply periodicity
  f   .setPeriodic(periodic);
  kern.setPeriodic(periodic);

  // initialize output: copy of input
  ArrI out = f;

  // change rank to 3 (to simplify implementation)
  f   .chrank(3);
  kern.chrank(3);

  // mid-point of the kernel
  VecS mid = kern.midpoint();

  // region-of-interest
  int dH = mid[0];
  int dI = mid[1];
  int dJ = mid[2];

  // loop through iterations
  for ( size_t iter = 0 ; iter < iterations.max() ; ++iter )
  {
    // loop through all voxel
    for ( int h = 0 ; h < f.shape<int>(0) ; ++h ) {
      for ( int i = 0 ; i < f.shape<int>(1) ; ++i ) {
        for ( int j = 0 ; j < f.shape<int>(2) ; ++j ) {
          // label over the current voxel
          int ilab = out(h,i,j);
          // proceed:
          // - for non-zero label
          // - if the number of iterations for this label has not been exceeded
          if ( ilab>0 and iterations[ilab]>iter )
            // loop through the kernel
            for ( int dh = -dH ; dh <= dH ; ++dh )
              for ( int di = -dI ; di <= dI ; ++di )
                for ( int dj = -dJ ; dj <= dJ ; ++dj )
                  // check to dilate for non-zero kernel value
                  if ( kern(dh+dH,di+dI,dj+dJ) )
                    if ( out.inBounds(h+dh,i+di,j+dj) )
                      if ( !out(h+dh,i+di,j+dj) )
                        out(h+dh,i+di,j+dj) = -1*ilab;
        }
      }
    }
    // accept all new labels (which were stored as negative)
    out = out.abs();
  }

  return out;
}

// =================================================================================================
// wrapper functions
// =================================================================================================


ArrI dilate(const ArrI &f, size_t iterations, bool periodic)
{
  return dilate(f, GooseEYE::kernel(f.rank()), VecS::Constant(f.max()+1,iterations), periodic);
}

// -------------------------------------------------------------------------------------------------

ArrI dilate(const ArrI &f, const ArrI &kernel, size_t iterations, bool periodic)
{
  return dilate(f, kernel, VecS::Constant(f.max()+1,iterations), periodic);
}

// -------------------------------------------------------------------------------------------------

ArrI dilate(const ArrI &f, const VecS &iterations, bool periodic)
{
  return dilate(f, GooseEYE::kernel(f.rank()), iterations, periodic);
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_S2_HPP
#define GOOSEEYE_ENSEMBLE_S2_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// 2-point correlation -- "master"
// =================================================================================================

void Ensemble::S2(ArrD f, ArrD g, ArrI fmask, ArrI gmask)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("GooseEYE::S2 - rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");
  if ( f.shape() != gmask.shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");
  if ( f.shape() != g    .shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");

  // switch off bound-checks based on periodicity settings
  f    .setPeriodic(mPeriodic);
  g    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);
  gmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    f     = f    .pad(mPad, 0);
    g     = g    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
    gmask = gmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  f.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( f(h,i,j) and !fmask(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !gmask(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += f(h,i,j) * g(h+dh,i+di,j+dj);

  // normalisation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( !fmask(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !gmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;
}

// =================================================================================================
// 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::S2(ArrI f, ArrI g)
{
  // optionally use masked implementation
  if ( mPad.size() > 0 ) return S2(f, g, ArrI::Zero(f.shape()), ArrI::Zero(g.shape()));

  // checks
  if ( f.rank()  != mData.rank() ) throw std::length_error("GooseEYE::S2 - rank inconsistent");
  if ( f.shape() != g.shape()    ) throw std::length_error("GooseEYE::S2 - shape inconsistent");

  // switch off bound-checks based on periodicity settings
  f.setPeriodic(mPeriodic);
  g.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  f.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( f(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( g(h+dh,i+di,j+dj) == f(h,i,j) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // normalisation
  mNorm += static_cast<double>(f.size());
}

// =================================================================================================
// 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::S2(ArrI f, ArrI g, ArrI fmask, ArrI gmask)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("GooseEYE::S2 - rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");
  if ( f.shape() != gmask.shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");
  if ( f.shape() != g    .shape() ) throw std::length_error("GooseEYE::S2 - shape inconsistent");

  // switch off bound-checks based on periodicity settings
  f    .setPeriodic(mPeriodic);
  g    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);
  gmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    f     = f    .pad(mPad, 0);
    g     = g    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
    gmask = gmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  f.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( f(h,i,j) and !fmask(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( g(h+dh,i+di,j+dj) == f(h,i,j) and !gmask(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // normalisation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( !fmask(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !gmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;
}

// =================================================================================================
// 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::S2(ArrD f, ArrD g)
{
  // optionally use masked implementation
  if ( mPad.size() > 0 ) return S2(f, g, ArrI::Zero(f.shape()), ArrI::Zero(g.shape()));

  // checks
  if ( f.rank()  != mData.rank() ) throw std::length_error("GooseEYE::S2 - rank inconsistent");
  if ( f.shape() != g.shape()    ) throw std::length_error("GooseEYE::S2 - shape inconsistent");

  // switch off bound-checks based on periodicity settings
  f.setPeriodic(mPeriodic);
  g.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  f.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < f.shape(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < f.shape(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < f.shape(2)-mSkip[2] ; ++j )
        if ( f(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += f(h,i,j) * g(h+dh,i+di,j+dj);

  // normalisation
  mNorm += static_cast<double>(f.size());
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

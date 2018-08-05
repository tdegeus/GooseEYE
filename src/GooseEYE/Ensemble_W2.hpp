/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_W2_HPP
#define GOOSEEYE_ENSEMBLE_W2_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// weighted 2-point correlation -- "master"
// =================================================================================================

void Ensemble::W2(ArrD w, ArrD f, ArrI fmask)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != fmask.shape() ) throw std::runtime_error(name+"shape inconsistent");
  if ( w.shape() != f    .shape() ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w    .setPeriodic(mPeriodic);
  f    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    w     = w    .pad(mPad, 0);
    f     = f    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j) * f(h+dh,i+di,j+dj);

  // normalisation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j);
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrI w, ArrD f, ArrI fmask)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != fmask.shape() ) throw std::runtime_error(name+"shape inconsistent");
  if ( w.shape() != f    .shape() ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w    .setPeriodic(mPeriodic);
  f    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    w     = w    .pad(mPad, 0);
    f     = f    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += f(h+dh,i+di,j+dj);

  // normalisation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrD w, ArrI f, ArrI fmask)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != fmask.shape() ) throw std::runtime_error(name+"shape inconsistent");
  if ( w.shape() != f    .shape() ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w    .setPeriodic(mPeriodic);
  f    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    w     = w    .pad(mPad, 0);
    f     = f    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) and f(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j);

  // normalisation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j);
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrI w, ArrI f, ArrI fmask)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank()  ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != fmask.shape() ) throw std::runtime_error(name+"shape inconsistent");
  if ( w.shape() != f    .shape() ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w    .setPeriodic(mPeriodic);
  f    .setPeriodic(mPeriodic);
  fmask.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    w     = w    .pad(mPad, 0);
    f     = f    .pad(mPad, 0);
    fmask = fmask.pad(mPad, 1);
  }

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) and f(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // normalisation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !fmask(h+dh,i+di,j+dj) )
                  mNorm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrD w, ArrD f)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // optionally use masked implementation
  if ( mPad.size() > 0 ) return W2(w, f, ArrI::Zero(f.shape()));

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank() ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != f.shape()    ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w.setPeriodic(mPeriodic);
  f.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j) * f(h+dh,i+di,j+dj);

  // normalisation
  mNorm += w.sum();
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrI w, ArrD f)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // optionally use masked implementation
  if ( mPad.size() > 0 ) return W2(w, f, ArrI::Zero(f.shape()));

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank() ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != f.shape()    ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w.setPeriodic(mPeriodic);
  f.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += f(h+dh,i+di,j+dj);

  // normalisation
  for ( size_t i = 0 ; i < w.size() ; ++i )
    if ( w[i] )
      mNorm += 1.;
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrD w, ArrI f)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // optionally use masked implementation
  if ( mPad.size() > 0 ) return W2(w, f, ArrI::Zero(f.shape()));

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank() ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != f.shape()    ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w.setPeriodic(mPeriodic);
  f.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( f(h+dh,i+di,j+dj) )
                  mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += w(h,i,j);

  // normalisation
  mNorm += w.sum();
}

// =================================================================================================
// weighted 2-point correlation -- "slave": compare to "master"
// =================================================================================================

void Ensemble::W2(ArrI w, ArrI f)
{
  // lock measure
  if ( mStat == Stat::Unset) mStat = Stat::W2;

  // optionally use masked implementation
  if ( mPad.size() > 0 ) return W2(w, f, ArrI::Zero(f.shape()));

  // checks
  std::string name = "GooseEYE::Ensemble::W2 - ";
  if ( w.rank()  != mData.rank() ) throw std::runtime_error(name+"rank inconsistent");
  if ( w.shape() != f.shape()    ) throw std::runtime_error(name+"shape inconsistent");

  // switch off bound-checks based on periodicity settings
  w.setPeriodic(mPeriodic);
  f.setPeriodic(mPeriodic);

  // change rank (to avoid failing assertions)
  w.chrank(3);

  // correlation
  for ( int h = mSkip[0] ; h < w.shape<int>(0)-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < w.shape<int>(1)-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < w.shape<int>(2)-mSkip[2] ; ++j )
        if ( w(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( f(h+dh,i+di,j+dj) )
                mData(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // normalisation
  for ( size_t i = 0 ; i < w.size() ; ++i )
    if ( w[i] )
      mNorm += 1.;
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

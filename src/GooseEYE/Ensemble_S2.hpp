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

void Ensemble::S2(const ArrI &f, const ArrI &g)
{
  // optionally use masked implementation
  if ( mPad.size() > 0 ) return S2(f, g, ArrI::Zero(f.shape()), ArrI::Zero(g.shape()));

  // checks
  if ( f.rank()  != mData.rank() ) throw std::length_error("S2: rank inconsistent");
  if ( f.shape() != g.shape()    ) throw std::length_error("S2: shape images inconsistent");

  // copy input
  ArrI F = f;
  ArrI G = g;

  // switch of bound-checks based on periodicity settings
  F.setPeriodic(mPeriodic);
  G.setPeriodic(mPeriodic);

  // get shape of "f/g" in "MAX_DIM"
  VecI shape = F.shape(); shape.resize(MAX_DIM, 1);

  // allocate 'output'
  ArrD out = ArrD::Zero(mData.shape());

  // compute correlation
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( F(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( G(h+dh,i+di,j+dj) == F(h,i,j) )
                  out(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // add to ensemble average
  mData += out;
  mNorm += static_cast<double>(F.size());
}

// =================================================================================================

void Ensemble::S2(const ArrI &f, const ArrI &g, const ArrI &fmask, const ArrI &gmask)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("S2: rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::length_error("S2: shape images inconsistent");
  if ( f.shape() != gmask.shape() ) throw std::length_error("S2: shape images inconsistent");
  if ( f.shape() != g    .shape() ) throw std::length_error("S2: shape images inconsistent");

  // copy input
  ArrI F  = f;
  ArrI G  = g;
  ArrI Fm = fmask;
  ArrI Gm = gmask;

  // switch of bound-checks based on periodicity settings
  F .setPeriodic(mPeriodic);
  G .setPeriodic(mPeriodic);
  Fm.setPeriodic(mPeriodic);
  Gm.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    F  = F .pad(mPad, 0);
    G  = G .pad(mPad, 0);
    Fm = Fm.pad(mPad, 1);
    Gm = Gm.pad(mPad, 1);
  }

  // get shape of "f/g" in "MAX_DIM"
  VecI shape = F.shape(); shape.resize(MAX_DIM, 1);

  // allocate 'output'
  ArrD out  = ArrD::Zero(mData.shape());
  ArrD norm = ArrD::Zero(mData.shape());

  // compute correlation
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( F(h,i,j) and !Fm(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( G(h+dh,i+di,j+dj) == F(h,i,j) and !Gm(h+dh,i+di,j+dj) )
                  out(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // compute normalization
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( !Fm(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !Gm(h+dh,i+di,j+dj) )
                  norm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // add to ensemble average
  mData += out;
  mNorm += norm;
}

// =================================================================================================

void Ensemble::S2(const ArrD &f, const ArrD &g)
{
  // optionally use masked implementation
  if ( mPad.size() > 0 ) return S2(f, g, ArrI::Zero(f.shape()), ArrI::Zero(g.shape()));

  // checks
  if ( f.rank()  != mData.rank() ) throw std::length_error("S2: rank inconsistent");
  if ( f.shape() != g.shape()    ) throw std::length_error("S2: shape images inconsistent");

  // copy input
  ArrI F = f;
  ArrI G = g;

  // switch of bound-checks based on periodicity settings
  F.setPeriodic(mPeriodic);
  G.setPeriodic(mPeriodic);

  // get shape of "f/g" in "MAX_DIM"
  VecI shape = F.shape(); shape.resize(MAX_DIM, 1);

  // allocate 'output'
  ArrD out = ArrD::Zero(mData.shape());

  // compute correlation
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( F(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                out(dh+mMid[0], di+mMid[1], dj+mMid[2]) += F(h,i,j) * G(h+dh,i+di,j+dj);

  // add to ensemble average
  mData += out;
  mNorm += static_cast<double>(F.size());
}

// =================================================================================================

void Ensemble::S2(const ArrD &f, const ArrD &g, const ArrI &fmask, const ArrI &gmask)
{
  // checks
  if ( f.rank()  != mData.rank()  ) throw std::length_error("S2: rank inconsistent");
  if ( f.shape() != fmask.shape() ) throw std::length_error("S2: shape images inconsistent");
  if ( f.shape() != gmask.shape() ) throw std::length_error("S2: shape images inconsistent");
  if ( f.shape() != g    .shape() ) throw std::length_error("S2: shape images inconsistent");

  // copy input
  ArrI F  = f;
  ArrI G  = g;
  ArrI Fm = fmask;
  ArrI Gm = gmask;

  // switch of bound-checks based on periodicity settings
  F .setPeriodic(mPeriodic);
  G .setPeriodic(mPeriodic);
  Fm.setPeriodic(mPeriodic);
  Gm.setPeriodic(mPeriodic);

  // zero-pad images
  if ( mPad.size() > 0 ) {
    F  = F .pad(mPad, 0);
    G  = G .pad(mPad, 0);
    Fm = Fm.pad(mPad, 1);
    Gm = Gm.pad(mPad, 1);
  }

  // get shape of "f/g" in "MAX_DIM"
  VecI shape = F.shape(); shape.resize(MAX_DIM, 1);

  // allocate 'output'
  ArrD out  = ArrD::Zero(mData.shape());
  ArrD norm = ArrD::Zero(mData.shape());

  // compute correlation
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( F(h,i,j) and !Fm(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !Gm(h+dh,i+di,j+dj) )
                  out(dh+mMid[0], di+mMid[1], dj+mMid[2]) += F(h,i,j) * G(h+dh,i+di,j+dj);

  // compute normalization
  for ( int h = mSkip[0] ; h < shape[0]-mSkip[0] ; ++h )
    for ( int i = mSkip[1] ; i < shape[1]-mSkip[1] ; ++i )
      for ( int j = mSkip[2] ; j < shape[2]-mSkip[2] ; ++j )
        if ( !Fm(h,i,j) )
          for ( int dh = -mMid[0] ; dh <= mMid[0] ; ++dh )
            for ( int di = -mMid[1] ; di <= mMid[1] ; ++di )
              for ( int dj = -mMid[2] ; dj <= mMid[2] ; ++dj )
                if ( !Gm(h+dh,i+di,j+dj) )
                  norm(dh+mMid[0], di+mMid[1], dj+mMid[2]) += 1.;

  // add to ensemble average
  mData += out;
  mNorm += norm;
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

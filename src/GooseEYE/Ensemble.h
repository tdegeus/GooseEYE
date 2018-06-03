/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_H
#define GOOSEEYE_ENSEMBLE_H

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================

class Ensemble
{
private:

  static const size_t   MAX_DIM=3;
  cppmat::array<double> mData;
  cppmat::array<double> mNorm;
  std::vector<int>      mMid;
  std::vector<int>      mSkip;
  std::vector<size_t>   mPad;
  bool                  mPeriodic;

public:

  // default constructor
  Ensemble() = default;

  // constructor
  Ensemble(const std::vector<size_t> roi, bool periodic=true, bool zero_pad=false);

  // get result
  cppmat::array<double> result() const;

  // 2-point correlation
  void S2(const ArrS &f, const ArrS &g);
  void S2(const ArrS &f, const ArrS &g, const ArrS &fmask, const ArrS &gmask);

private:

  // get the shape of "A" in "MAX_DIM" dimensions (padded with ones)
  template<class T>
  std::vector<int> shape(const cppmat::array<T> &A, const std::string &name="A") const;
};

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

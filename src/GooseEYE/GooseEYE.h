/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

// =================================================================================================

#include <assert.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cppmat/cppmat.h>

// =================================================================================================

#define GOOSEEYE_WORLD_VERSION 0
#define GOOSEEYE_MAJOR_VERSION 2
#define GOOSEEYE_MINOR_VERSION 0

#define GOOSEEYE_VERSION_AT_LEAST(x,y,z) \
  (GOOSEEYE_WORLD_VERSION>x || (GOOSEEYE_WORLD_VERSION>=x && \
  (GOOSEEYE_MAJOR_VERSION>y || (GOOSEEYE_MAJOR_VERSION>=y && \
                                GOOSEEYE_MINOR_VERSION>=z))))

#define GOOSEEYE_VERSION(x,y,z) \
  (GOOSEEYE_WORLD_VERSION==x && \
   GOOSEEYE_MAJOR_VERSION==y && \
   GOOSEEYE_MINOR_VERSION==z)

// =================================================================================================

// dummy operation that can be use to suppress the "unused parameter" warnings
#define UNUSED(p) ( (void)(p) )

// =================================================================================================

// alias types
namespace GooseEYE
{
  typedef cppmat::array<double> ArrD;
  typedef cppmat::array<size_t> ArrS;
}

// =================================================================================================

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

} // namespace ...

// =================================================================================================

// #include "Ensemble.h"
#include "Ensemble.hpp"
#include "Ensemble_S2.hpp"

// =================================================================================================

#endif

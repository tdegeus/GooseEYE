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

#define SIGN(a)( (a<0) ? -1 : a>0 ? 1 : 0 )
#define POS(a) ( (a<0) ?  0 : a           )

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
  typedef cppmat::array <double> ArrD;
  typedef cppmat::array <int>    ArrI;
  typedef cppmat::matrix<double> MatD;
  typedef cppmat::matrix<int>    MatI;
  typedef cppmat::vector<size_t> VecS;
  typedef cppmat::vector<int>    VecI;
}

// =================================================================================================

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

class Ensemble
{
private:

  static const size_t MAX_DIM=3;
  ArrD mData;
  ArrD mNorm;
  VecI mShape;
  VecI mMid;
  VecI mSkip;
  VecS mPad;
  bool mPeriodic;

public:

  // default constructor
  Ensemble() = default;

  // constructor
  explicit Ensemble(const VecS &roi, bool periodic=true, bool zero_pad=false);

  // get ensemble averaged result
  ArrD result() const;

  // 2-point correlation
  void S2(const ArrI &f, const ArrI &g);
  void S2(const ArrI &f, const ArrI &g, const ArrI &fmask, const ArrI &gmask);
  void S2(const ArrD &f, const ArrD &g);
  void S2(const ArrD &f, const ArrD &g, const ArrI &fmask, const ArrI &gmask);

  // list of end-points of ROI-stamp used in path-based correlations
  MatI stampPoints() const;

};

// -------------------------------------------------------------------------------------------------

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
MatI path(const VecI &xa, const VecI &xb, const std::string &mode="Bresenham");

// list of end-points of ROI-stamp used in path-based correlations
MatI stampPoints(const VecS &shape);

// 2-point corr`elation
ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g,                                       bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g, const ArrI &fmask, const ArrI &gmask, bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g,                                       bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g, const ArrI &fmask, const ArrI &gmask, bool periodic=true, bool pad=false);

// -------------------------------------------------------------------------------------------------

} // namespace ...

// =================================================================================================

#include "GooseEYE.hpp"
#include "path.hpp"
#include "Ensemble.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_stampPoints.hpp"

// =================================================================================================

#endif

/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

// =================================================================================================

#include "include.h"

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
  void S2(ArrI f, ArrI g);
  void S2(ArrI f, ArrI g, ArrI fmask, ArrI gmask);
  void S2(ArrD f, ArrD g);
  void S2(ArrD f, ArrD g, ArrI fmask, ArrI gmask);

  // list of end-points of ROI-stamp used in path-based correlations
  MatI stampPoints() const;

};

// -------------------------------------------------------------------------------------------------

// create a dummy image with circles at position "row","col" with radius "r"
MatI dummy_circles(const VecS &shape);
MatI dummy_circles(const VecS &shape, const VecI &row, const VecI &col, const VecI &r);

// define kernel
// mode: "default"
ArrI kernel(size_t ndim, std::string mode="default");

// determine clusters in image (for "min_size=0" the minimum size is ignored)
ArrI clusters(const ArrI &f,                                   bool periodic=true);
ArrI clusters(const ArrI &f,                   int min_size  , bool periodic=true);
ArrI clusters(const ArrI &f, const ArrI &kern, int min_size=0, bool periodic=true);

// determine clusters and centers of gravity in image (for "min_size=0" the minimum size is ignored)
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f,                                   bool periodic=true);
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f,                   int min_size  , bool periodic=true);
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f, const ArrI &kern, int min_size=0, bool periodic=true);

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
MatI path(const VecI &xa, const VecI &xb, std::string mode="Bresenham");

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
#include "dummy_circles.hpp"
#include "path.hpp"
#include "kernel.hpp"
#include "clusters.hpp"
#include "Ensemble.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_stampPoints.hpp"

// =================================================================================================

#endif

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

  // 2-point probability (binary), 2-point cluster function (int), and 2-point correlation (double)
  void S2(ArrI f, ArrI g);
  void S2(ArrI f, ArrI g, ArrI fmask, ArrI gmask);
  void S2(ArrD f, ArrD g);
  void S2(ArrD f, ArrD g, ArrI fmask, ArrI gmask);

  // weighted 2-point correlation
  void W2(ArrD w, ArrD f);
  void W2(ArrD w, ArrD f, ArrI fmask);

  // // collapsed weighted 2-point correlation
  void W2c(ArrI clus, ArrI cent, ArrD w, ArrD f,             std::string mode="Bresenham");
  void W2c(ArrI clus, ArrI cent, ArrD w, ArrD f, ArrI fmask, std::string mode="Bresenham");

  // lineal path function (binary or int)
  void L(ArrI f, std::string mode="Bresenham");

  // list of end-points of ROI-stamp used in path-based correlations
  MatI stampPoints(size_t nd=0) const; // (nd == 0 -> number of columns is automatic)

};

// -------------------------------------------------------------------------------------------------

// create a dummy image with circles at position "row","col" with radius "r"
// (the image is always constructed periodic)
MatI dummy_circles(const VecS &shape, bool periodic=true);
MatI dummy_circles(const VecS &shape, const VecI &row, const VecI &col, const VecI &r, bool periodic=true);

// define kernel
// mode: "default"
ArrI kernel(size_t ndim, std::string mode="default");

// determine clusters of an image (for "min_size=0" the minimum size is ignored)
ArrI clusters(const ArrI &f,                                   bool periodic=true);
ArrI clusters(const ArrI &f,                   int min_size  , bool periodic=true);
ArrI clusters(const ArrI &f, const ArrI &kern, int min_size=0, bool periodic=true);

// dilate image (binary or int)
// for 'int' image the number of iterations can be specified per label
ArrI dilate(const ArrI &src                    , size_t      iterations=1, bool periodic=true);
ArrI dilate(const ArrI &src                    , const VecS &iterations  , bool periodic=true);
ArrI dilate(const ArrI &src, const ArrI &kernel, size_t      iterations=1, bool periodic=true);
ArrI dilate(      ArrI  src,       ArrI  kernel, const VecS &iterations  , bool periodic=true);

// determine clusters and centers of gravity of an image (for "min_size=0" the minimum size is ignored)
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
#include "dilate.hpp"
#include "Ensemble.hpp"
#include "Ensemble_stampPoints.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_L.hpp"

// =================================================================================================

#endif

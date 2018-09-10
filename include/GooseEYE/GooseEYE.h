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
// Class to compute ensemble averaged statistics. Simple front-end functions are provided to compute
// the statistics on one image.
//
// The raw-data and normalization are separately stored as class member variables. The normalized
// result can be read at any moment using 'result'.
// -------------------------------------------------------------------------------------------------

class Ensemble
{
private:

  static const size_t MAX_DIM=3;
  ArrD mData;             // raw-result, not normalized (mShape)
  ArrD mNorm;             // normalization (mShape)
  int  mShape[MAX_DIM];   // ROI shape along each axis
  int  mMid[MAX_DIM];     // ROI midpoint along each axis
  int  mSkip[MAX_DIM];    // number of voxels to skip along each axis
  VecS mPad;              // shape with with to pad along each axis
  bool mPeriodic;         // periodicity settings used for the entire cluster
  int  mStat=Stat::Unset; // used to lock this class to a certain statistic

public:

  // default constructor
  Ensemble() = default;

  // constructor
  explicit Ensemble(const VecS &roi, bool periodic=true, bool zero_pad=false);

  // get ensemble averaged result, or raw data
  ArrD result() const;
  ArrD data() const;
  ArrD norm() const;

  // mean
  void mean(const ArrD &f);
  void mean(const ArrD &f, const ArrI &fmask);

  // 2-point probability (binary), 2-point cluster function (int), and 2-point correlation (double)
  void S2(ArrI f, ArrI g);
  void S2(ArrI f, ArrI g, ArrI fmask, ArrI gmask);
  void S2(ArrD f, ArrD g);
  void S2(ArrD f, ArrD g, ArrI fmask, ArrI gmask);

  // weighted 2-point correlation
  void W2(ArrI w, ArrI f);
  void W2(ArrI w, ArrI f, ArrI fmask);
  void W2(ArrI w, ArrD f);
  void W2(ArrI w, ArrD f, ArrI fmask);
  void W2(ArrD w, ArrI f);
  void W2(ArrD w, ArrI f, ArrI fmask);
  void W2(ArrD w, ArrD f);
  void W2(ArrD w, ArrD f, ArrI fmask);

  // collapsed weighted 2-point correlation
  // mode: "Bresenham", "actual", or "full"
  void W2c(ArrI clus, ArrI cntr, ArrI f,             std::string mode="Bresenham");
  void W2c(ArrI clus, ArrI cntr, ArrI f, ArrI fmask, std::string mode="Bresenham");
  void W2c(ArrI clus, ArrI cntr, ArrD f,             std::string mode="Bresenham");
  void W2c(ArrI clus, ArrI cntr, ArrD f, ArrI fmask, std::string mode="Bresenham");

  // collapsed weighted 2-point correlation: automatically compute clusters and their centres
  // mode: "Bresenham", "actual", or "full"
  void W2c_auto(ArrI w, ArrI f,             std::string mode="Bresenham");
  void W2c_auto(ArrI w, ArrI f, ArrI fmask, std::string mode="Bresenham");
  void W2c_auto(ArrI w, ArrD f,             std::string mode="Bresenham");
  void W2c_auto(ArrI w, ArrD f, ArrI fmask, std::string mode="Bresenham");

  // lineal path function (binary or int)
  // mode: "Bresenham", "actual", or "full"
  void L(ArrI f, std::string mode="Bresenham");

  // list of end-points of ROI-stamp used in path-based correlations
  MatI stampPoints(size_t nd=0) const; // (nd == 0 -> number of columns is automatic)

};

// -------------------------------------------------------------------------------------------------
// font-end functions to compute the statistics for one image
// -------------------------------------------------------------------------------------------------

// 2-point correlation
ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g,                                       bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrI &f, const ArrI &g, const ArrI &fmask, const ArrI &gmask, bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g,                                       bool periodic=true, bool pad=false);
ArrD S2(const VecS &roi, const ArrD &f, const ArrD &g, const ArrI &fmask, const ArrI &gmask, bool periodic=true, bool pad=false);

// weighted 2-point correlation
ArrD W2(const VecS &roi, const ArrI &w, const ArrI &f,                    bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrI &w, const ArrI &f, const ArrI &fmask, bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrI &w, const ArrD &f,                    bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrI &w, const ArrD &f, const ArrI &fmask, bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrD &w, const ArrI &f,                    bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrD &w, const ArrI &f, const ArrI &fmask, bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrD &w, const ArrD &f,                    bool periodic=true, bool pad=false);
ArrD W2(const VecS &roi, const ArrD &w, const ArrD &f, const ArrI &fmask, bool periodic=true, bool pad=false);

// collapsed weighted 2-point correlation
// mode: "Bresenham", "actual", or "full"
ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrI &f,                    bool periodic=true, std::string mode="Bresenham");
ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrI &f, const ArrI &fmask, bool periodic=true, std::string mode="Bresenham");
ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrD &f,                    bool periodic=true, std::string mode="Bresenham");
ArrD W2c(const VecS &roi, const ArrI &clus, const ArrI &cntr, const ArrD &f, const ArrI &fmask, bool periodic=true, std::string mode="Bresenham");

// collapsed weighted 2-point correlation: automatically compute clusters and their centres
// mode: "Bresenham", "actual", or "full"
ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrI &f,                    bool periodic=true, std::string mode="Bresenham");
ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrI &f, const ArrI &fmask, bool periodic=true, std::string mode="Bresenham");
ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrD &f,                    bool periodic=true, std::string mode="Bresenham");
ArrD W2c_auto(const VecS &roi, const ArrI &w, const ArrD &f, const ArrI &fmask, bool periodic=true, std::string mode="Bresenham");

// lineal path function (binary or int)
// mode: "Bresenham", "actual", or "full"
ArrD L(const VecS &roi, const ArrI &f, bool periodic=true, std::string mode="Bresenham");

// -------------------------------------------------------------------------------------------------
// miscellaneous functions
// -------------------------------------------------------------------------------------------------

// clusters of a binary image ("min_size=0": minimum size is ignored)
ArrI clusters(const ArrI &f,                                   bool periodic=true);
ArrI clusters(const ArrI &f,                   int min_size  , bool periodic=true);
ArrI clusters(const ArrI &f, const ArrI &kern, int min_size=0, bool periodic=true);

// clusters and their centers of gravity of a binary image ("min_size=0": minimum size is ignored)
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f,                                   bool periodic=true);
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f,                   int min_size  , bool periodic=true);
std::tuple<ArrI,ArrI> clusterCenters(const ArrI &f, const ArrI &kern, int min_size=0, bool periodic=true);

// dilate image (binary or int)
// for 'int' image the number of iterations can be specified per label
ArrI dilate(const ArrI &f                    , size_t      iterations=1, bool periodic=true);
ArrI dilate(const ArrI &f                    , const VecS &iterations  , bool periodic=true);
ArrI dilate(const ArrI &f, const ArrI &kernel, size_t      iterations=1, bool periodic=true);
ArrI dilate(      ArrI  f,       ArrI  kernel, const VecS &iterations  , bool periodic=true);

// kernel
// mode: "default"
ArrI kernel(size_t ndim, std::string mode="default");

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
MatI path(const VecI &xa, const VecI &xb, std::string mode="Bresenham");

// list of end-points of ROI-stamp used in path-based correlations
MatI stampPoints(const VecS &shape);

// dummy image with circles at position "row","col" with radius "r" (may be automatic)
MatI dummy_circles(const VecS &shape,                                                  bool periodic=true);
MatI dummy_circles(const VecS &shape, const VecI &row, const VecI &col, const VecI &r, bool periodic=true);

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
#include "Ensemble_mean.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_W2c.hpp"
#include "Ensemble_L.hpp"

// =================================================================================================

#endif

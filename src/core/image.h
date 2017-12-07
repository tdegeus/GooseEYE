/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_IMAGE_H
#define GOOSEEYE_IMAGE_H

#include "macros.h"

// -------------------------------------------------------------------------------------------------

namespace GooseEYE {
namespace Image {

// -------------------------------------------------------------------------------------------------

// abbreviate data types to enhance readability below
template <class T> using Mat = cppmat::matrix<T>;
using MatD = cppmat::matrix<double>;
using MatI = cppmat::matrix<int>;
using VecS = std::vector<size_t>;
using VecI = std::vector<int>;
using str  = std::string;

// -------------------------------------------------------------------------------------------------

// toolbox
// -------

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
MatI path ( const VecI &xa, const VecI &xb, str mode="Bresenham" );

// list of end-points of ROI-stamp used in path-based correlations
MatI stamp_points ( const VecS &shape );

// (zero)pad "npad" entries on each side of "src" (with a certain "value")
template <class T> Mat<T> pad ( const Mat<T> &src, const VecS &npad, T value=0 );

// define kernel
// mode: "default"
MatI kernel ( int ndim , str mode="default" );

// image manipulation
// ------------------

// create a dummy image with circles
// - randomly distribute circles
MatI dummy_circles ( const VecS &shape={100,100}, bool periodic=true );
// - specify each circle : position "row","col" and radius "r"
MatI dummy_circles (
  const VecS &shape, const VecI &row, const VecI &col, const VecI &r, bool periodic=true
);

// determine clusters in image
// - custom kernel
std::tuple<MatI,MatI> clusters (
  const MatI &src, const MatI &kernel, int min_size=0, bool periodic=true
);
// - default kernel
std::tuple<MatI,MatI> clusters (
  const MatI &src, bool periodic=true
);
// - default kernel, remove pixels below a certain minimum size
std::tuple<MatI,MatI> clusters (
  const MatI &src, int min_size, bool periodic=true
);

// dilate image (binary or int)
// - custom kernel
MatI dilate (
  const MatI &src, const MatI &kernel, const VecI &iterations
);
// - custom kernel, periodic (generally slower)
MatI dilate (
  const MatI &src, const MatI &kernel, const VecI &iterations, bool periodic
);
// - custom kernel, fixed iterations
MatI dilate (
  const MatI &src, const MatI &kernel, int iterations=1, bool periodic=true
);
// - default kernel
MatI dilate (
  const MatI &src, const VecI &iterations, bool periodic=true
);
// - default kernel, fixed iterations
MatI dilate (
  const MatI &src, int iterations=1, bool periodic=true
);

// statistics
// ----------

// spatial average
// - default
template <class T> std::tuple<double,double> mean ( const Mat<T> &src );
// - masked pixels
template <class T> std::tuple<double,double> mean ( const Mat<T> &src , const MatI &mask );

// 2-point probability (binary), 2-point cluster function (int), and 2-point correlations (double)
// - periodic, no mask, no padding
template <class T> std::tuple<MatD,double> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi
);
// - padding + not-periodic
template <class T> std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, bool zeropad, bool periodic
);
// - padding + not-periodic + mask
template <class T> std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, const MatI &fmsk,
  bool pad=false, bool periodic=true
);
// - padding + not-periodic + mask
template <class T> std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, const MatI &fmsk, const MatI &gmsk,
  bool pad=false, bool periodic=true
);

// weighted 2-point probability (binary) or 2-point correlation (float)
// - not periodic, no padding, no mask
template <class T, class U> std::tuple<MatD,double> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi
);
// - padding + periodic
template <class T, class U> std::tuple<MatD,MatD> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi, bool pad, bool periodic
);
// - padding + periodic + mask
template <class T, class U> std::tuple<MatD,MatD> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi, MatI &msk, bool pad=false, bool periodic=true
);

// collapsed weighted 2-point correlation
// - not periodic, no mask
template <class T> std::tuple<MatD,MatD> W2c (
  const MatI &W, const Mat<T> &I, const VecS &roi, str mode="Bresenham"
);
// - periodic, no mask
template <class T> std::tuple<MatD,MatD> W2c (
  const MatI &W, const Mat<T> &I, const VecS &roi, bool periodic, str mode="Bresenham"
);
// - periodic + mask
template <class T> std::tuple<MatD,MatD> W2c (
  const MatI &W, const Mat<T> &I, const VecS &roi, const MatI &msk,
  bool periodic=true, str mode="Bresenham"
);
// - clusters + periodic
template <class T> std::tuple<MatD,MatD> W2c (
  const MatI &clus, const MatI &cntr, const Mat<T> &I, const VecS &roi,
  bool periodic=true, str mode="Bresenham"
);
// - clusters + periodic + mask
template <class T> std::tuple<MatD,MatD> W2c (
  const MatI &clus, const MatI &cntr, const Mat<T> &I, const VecS &roi, const MatI &msk,
  bool periodic=true, str mode="Bresenham"
);

// lineal path function (binary or int)
// - periodic
std::tuple<MatD,MatD> L (
  const MatI &f, const VecS &roi, str mode="Bresenham"
);
// - not periodic
std::tuple<MatD,MatD> L (
  const MatI &f, const VecS &roi, bool periodic, str mode="Bresenham"
);

} // namespace Image
} // namespace GooseEYE

#endif

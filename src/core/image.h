
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <tuple>
#include <ctime>

#include "cppmat/include/cppmat/matrix.h"

#define BND(i,N) ( (i<0) ?   0 : (i>=N) ?   0 : 1 )
#define P(i,N)   ( (i<0) ? i+N : (i>=N) ? i-N : i )
#define SIGN(a)  ( (a<0) ?  -1 :  a>0   ?   1 : 0 )
#define POS(a)   ( (a<0) ?   0                : a )

namespace GooseEYE {
namespace Image {

// abbreviate data types to enhance readability below
template <class T> using Mat = cppmat::matrix<T>;
using MatD = cppmat::matrix<double>;
using MatI = cppmat::matrix<int>;
using VecS = std::vector<size_t>;
using VecI = std::vector<int>;
using str  = std::string;

// implementation support
// ----------------------

// return vector as "(h,i,j)", using a default "value" if "shape.size()<3"
std::tuple<int,int,int> unpack3d ( VecS shape, int value=1 );

// compute midpoint from "shape"-vector
VecS midpoint ( VecS shape );

// core functions for "S2"/"W2" (the input function determines the normalization)
template <class T, class U> std::tuple<MatD, double> S2_core ( Mat<T> &f, Mat<U> &g, VecS roi,
  double (*func)(U)
);
template <class T, class U> std::tuple<MatD,MatD> S2_core ( Mat<T> &f, Mat<U> &g, VecS roi,
  MatI &fmsk, MatI &gmsk, bool zeropad, bool periodic, double (*func)(U)
);
// support function that allow overload
inline double compare ( int    f, int    g );
inline double compare ( int    f, double g );
inline double compare ( double f, int    g );
inline double compare ( double f, double g );
inline double compare ( int    f           );
inline double compare ( double f           );
inline double unity   ( int    f           );
inline double unity   ( double f           );

// toolbox
// -------

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
MatI path ( VecI xa, VecI xb, str mode="Bresenham" );

// list of end-points of ROI-stamp used in path-based correlations
MatI stamp_points ( VecS shape );

// (zero)pad "pad_shape" entries on each side of "src" (with a certain "value")
template <class T> Mat<T> pad ( Mat<T> &src, VecS pad_shape, T value=0 );

// define kernel
// mode: "default"
MatI kernel ( int ndim , str mode="default" );

// image manipulation
// ------------------

// create a dummy image with circles at position "row","col" with radius "r"
MatI dummy_circles (                                            bool periodic=true );
MatI dummy_circles ( VecS shape,                                bool periodic=true );
MatI dummy_circles ( VecS shape, VecI &row, VecI &col, VecI &r, bool periodic=true );

// determine clusters in image (for "min_size=0" the minimum size is ignored)
std::tuple<MatI,MatI> clusters ( MatI &src,                               bool periodic=true);
std::tuple<MatI,MatI> clusters ( MatI &src,               int min_size  , bool periodic=true);
std::tuple<MatI,MatI> clusters ( MatI &src, MatI &kernel, int min_size=0, bool periodic=true);

// dilate image (binary or int)
// for 'int' image the number of iterations can be specified per label
MatI dilate ( MatI &src              , int   iterations=1, bool periodic=true );
MatI dilate ( MatI &src              , VecI& iterations  , bool periodic=true );
MatI dilate ( MatI &src, MatI &kernel, int   iterations=1, bool periodic=true );
MatI dilate ( MatI &src, MatI &kernel, VecI& iterations  , bool periodic=true );

// statistics
// ----------

// spatial average
template <class T> std::tuple<double,double> mean (
  Mat<T> &src
);
template <class T> std::tuple<double,double> mean (
  Mat<T> &src , MatI &mask
);

// 2-point probability (binary), 2-point cluster function (int), and 2-point correlations (double)
template <class T> std::tuple<MatD,double> S2 (
  Mat<T> &f, Mat<T> &g, VecS roi
);
template <class T> std::tuple<MatD,MatD> S2 (
  Mat<T> &f, Mat<T> &g, VecS roi,                         bool pad      , bool periodic
);
template <class T> std::tuple<MatD,MatD> S2 (
  Mat<T> &f, Mat<T> &g, VecS roi, MatI &fmsk,             bool pad=false, bool periodic=true
);
template <class T> std::tuple<MatD,MatD> S2 (
  Mat<T> &f, Mat<T> &g, VecS roi, MatI &fmsk, MatI &gmsk, bool pad=false, bool periodic=true
);

// weighted 2-point probability (binary) or 2-point correlation (float)
template <class T, class U> std::tuple<MatD,double> W2 (
  Mat<T> &W, Mat<U> &I, VecS roi
);
template <class T, class U> std::tuple<MatD,MatD> W2 (
  Mat<T> &W, Mat<U> &I, VecS roi,            bool pad      , bool periodic
);
template <class T, class U> std::tuple<MatD,MatD> W2 (
  Mat<T> &W, Mat<U> &I, VecS roi, MatI &msk, bool pad=false, bool periodic=true
);

// collapsed weighted 2-point correlation
template <class T> std::tuple<MatD,MatD> W2c (
  MatI &clus, MatI &cntr, Mat<T> &I, VecS roi, MatI &msk, str mode="Bresenham", bool periodic=true
);
template <class T> std::tuple<MatD,MatD> W2c (
  MatI &clus, MatI &cntr, Mat<T> &I, VecS roi,            str mode="Bresenham", bool periodic=true
);
template <class T> std::tuple<MatD,MatD> W2c (
  MatI &W               , Mat<T> &I, VecS roi, MatI &msk, str mode="Bresenham", bool periodic=true
);
template <class T> std::tuple<MatD,MatD> W2c (
  MatI &W               , Mat<T> &I, VecS roi,            str mode="Bresenham", bool periodic=true
);

// lineal path function (binary or int)
std::tuple<MatD,MatD> L (
  MatI &f, VecS roi, str mode="Bresenham", bool periodic=true
);

}; // namespace Image
}; // namespace GooseEYE

#endif


#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>

#include "cppmat/cppmat.h"

#define BND(i,N) ( (i<0) ?   0 : (i>=N) ?   0 : 1 )
#define P(i,N)   ( (i<0) ? i+N : (i>=N) ? i-N : i )
#define SIGN(a)  ( (a<0) ?  -1 :  a>0   ?   1 : 0 )
#define POS(a)   ( (a<0) ?   0                : a )

// abbreviate data types to enhance readability -> templates fit on one line
template <class T> using M = mat::matrix<T>;
using Md = mat::matrix<double>;
using Mi = mat::matrix<int>;
using Vs = std::vector<size_t>;
using Vi = std::vector<int>;
using d  = double;
using i  = int;
using b  = bool;
using s  = std::string;

namespace Image {

// implementation support
// ----------------------

// return vector as "(h,i,j)", using a default "value" if "src.size()<3"
std::tuple<i,i,i> unpack3d ( Vs src, i value=1 );

// compute midpoint from "shape"-vector
Vs midpoint ( Vs shape );

// toolbox
// -------

// pixel/voxel path between two points "xa" and "xb"
// mode: "Bresenham", "actual", or "full"
Mi path ( Vi xa, Vi xb, s mode="Bresenham" );

// list of end-points of ROI-stamp used in path-based correlations
Mi stamp_points ( Vs shape );

// (zero)pad "pad_shape" entries on each side of "src" (with a certain "value")
template <class T> mat::matrix<T> pad ( mat::matrix<T> &src, Vs pad_shape, T value=0 );

// define kernel
// mode: "default"
Mi kernel ( i ndim , s mode="default" );

// image manipulation
// ------------------

// create a dummy image with circles at position "row","col" with radius "r"
Mi dummy_circles (                                    b periodic=true );
Mi dummy_circles ( Vs shape,                          b periodic=true );
Mi dummy_circles ( Vs shape, Vi &row, Vi &col, Vi &r, b periodic=true );

// determine clusters in image (for "min_size=0" the minimum size is ignored)
std::tuple<Mi,Mi> clusters ( Mi &src,             i min_size=0, b periodic=true);
std::tuple<Mi,Mi> clusters ( Mi &src, Mi &kernel, i min_size=0, b periodic=true);

// dilate image (binary or int)
// for 'int' image the number of iterations can be specified per label
Mi dilate ( Mi &src            , i   iterations=1, b periodic=true );
Mi dilate ( Mi &src            , Vi& iterations  , b periodic=true );
Mi dilate ( Mi &src, Mi &kernel, i   iterations=1, b periodic=true );
Mi dilate ( Mi &src, Mi &kernel, Vi& iterations  , b periodic=true );

// statistics
// ----------

// spatial average
template <class T> double mean ( M<T> &src            );
template <class T> double mean ( M<T> &src , Mi &mask );

// 2-point probability (binary), 2-point cluster function (int), and 2-point correlations (double)
template <class T> std::tuple<Md,d > S2 ( M<T> &f, M<T> &g, Vs roi                                                   );
template <class T> std::tuple<Md,Md> S2 ( M<T> &f, M<T> &g, Vs roi,                     b pad      , b periodic      );
template <class T> std::tuple<Md,Md> S2 ( M<T> &f, M<T> &g, Vs roi, Mi &fmsk,           b pad=false, b periodic=true );
template <class T> std::tuple<Md,Md> S2 ( M<T> &f, M<T> &g, Vs roi, Mi &fmsk, Mi &gmsk, b pad=false, b periodic=true );

// weighted 2-point probability (binary) or 2-point correlation (float)
template <class T, class U> std::tuple<Md,d > W2 ( M<T> &W, M<U> &I, Vs roi                                        );
template <class T, class U> std::tuple<Md,Md> W2 ( M<T> &W, M<U> &I, Vs roi,          b pad      , b periodic      );
template <class T, class U> std::tuple<Md,Md> W2 ( M<T> &W, M<U> &I, Vs roi, Mi &msk, b pad=false, b periodic=true );

// collapsed weighted 2-point correlation
std::tuple<Md,Mi> W2c ( Md &I, Mi &clus, Mi &cntr, Vs &roi, Mi &msk, s mode="Bresenham", b periodic=true );
std::tuple<Md,Mi> W2c ( Md &I, Mi &clus, Mi &cntr, Vs &roi,          s mode="Bresenham", b periodic=true );

// lineal path function (binary or int)
std::tuple<Md,Mi> L ( Mi &f, Vs &roi, s mode="Bresenham", b periodic=true );

}; // namespace Image

#endif

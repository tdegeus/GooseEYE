
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>

#define BND(i,N) ( (i<0) ?   0 : (i>=N) ?   0 : 1 )
#define P(i,N)   ( (i<0) ? i+N : (i>=N) ? i-N : i )
#define SIGN(a)  ( (a<0) ?  -1 :  a>0   ?   1 : 0 )
#define POS(a)   ( (a<0) ?   0                : a )

namespace Image {

// =============================================================================
// Image::Matrix class
// =============================================================================

template <class T> class Matrix
{

  private:

    std::vector<T>      _data;     // data array
    std::vector<size_t> _shape;    // number of entries in each dimensions
    std::vector<size_t> _strides;  // stride length for each index

  public:

    // (copy) constructor
    // ------------------

    Matrix               (const Matrix<T> &) = default;
    Matrix<T>& operator= (const Matrix<T> &) = default;

    Matrix<T>(){};

    // explicit constructor
    // --------------------

    Matrix ( std::vector<size_t> shape )
    {
      resize(shape);

      // TODO: remove zero initializer, when the code has been checked
      for ( auto &i : _data )
        i = (T)0;
    };

    Matrix ( std::vector<size_t> shape, T value )
    {
      resize(shape);

      for ( auto &i : _data )
        i = value;
    };

    Matrix ( std::vector<size_t> shape, const T *data )
    {
      resize(shape);

      for ( size_t i=0 ; i<size() ; ++i )
        _data[i] = data[i];
    };

    // constructor to copy + change data type
    // --------------------------------------

    template<typename U, typename V=T, typename=typename std::enable_if<std::is_convertible<T,U>::value>::type>
    operator Matrix<U> ()
    {
      Matrix<U> out(shape());
      for ( size_t i=0 ; i<size() ; ++i ) {
        out[i] = static_cast<T>(_data[i]);
      }
      return out;
    };

    // resize matrix + store shape
    // ---------------------------

    void resize ( std::vector<size_t> shape )
    {
      if ( shape.size()<1 || shape.size()>3 )
        throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

      _shape  .resize(3);
      _strides.resize(3);

      for ( auto &i : _shape ) { i = 1; }

      for ( size_t i=0 ; i<shape.size() ; ++i ) { _shape[i] = shape[i]; }

      _strides[0] = _shape[2]*_shape[1];
      _strides[1] = _shape[2];
      _strides[2] = 1;

      _data.resize(_shape[0]*_shape[1]*_shape[2]);
    };

    // index operators
    // ---------------

    T& operator[] ( size_t i )
    { return _data[i]; };

    T& operator() ( size_t h, size_t i=0, size_t j=0 )
    { return _data[h*_strides[0]+i*_strides[1]+j*_strides[2]]; };

    // arithmetic operators
    // --------------------

    Matrix<T>& operator*= (const Matrix<T> &rhs)
    {
      if ( this->shape()!=rhs.shape() )
        throw std::runtime_error("Matrices must have the same shape");

      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] *= rhs._data[i];

      return *this;
    }

    Matrix<T>& operator/= (const Matrix<T> &rhs)
    {
      if ( this->shape()!=rhs.shape() )
        throw std::runtime_error("Matrices must have the same shape");

      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] /= rhs._data[i];

      return *this;
    }

    Matrix<T>& operator+= (const Matrix<T> &rhs)
    {
      if ( this->shape()!=rhs.shape() )
        throw std::runtime_error("Matrices must have the same shape");

      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] += rhs._data[i];

      return *this;
    }

    Matrix<T>& operator-= (const Matrix<T> &rhs)
    {
      if ( this->shape()!=rhs.shape() )
        throw std::runtime_error("Matrices must have the same shape");

      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] -= rhs._data[i];

      return *this;
    }

    Matrix<T>& operator*= (T rhs)
    { for ( size_t i=0 ; i<this->size() ; i++ ) _data[i] *= rhs; return *this; }

    Matrix<T>& operator/= (T rhs)
    { for ( size_t i=0 ; i<this->size() ; i++ ) _data[i] /= rhs; return *this; }

    Matrix<T>& operator+= (T rhs)
    { for ( size_t i=0 ; i<this->size() ; i++ ) _data[i] += rhs; return *this; }

    Matrix<T>& operator-= (T rhs)
    { for ( size_t i=0 ; i<this->size() ; i++ ) _data[i] -= rhs; return *this; }

    // iterators
    // ---------

    T* ptr ( void )
    { return &_data[0]; };

    auto begin ( void )
    { return _data.begin(); }

    auto end ( void )
    { return _data.end  (); }

    // return pointer to data
    // ----------------------

    const T* data ( void ) const
    { return _data.data(); };

    // return shape array [ndim]
    // -------------------------

    std::vector<size_t> shape ( size_t ndim=0 ) const
    {
      if ( ndim==0 )
        ndim = this->ndim();

      std::vector<size_t> ret(ndim);

      for ( size_t i=0 ; i<ndim ; i++ )
        ret[i] = _shape[i];

      return ret;
    };

    // return strides array [ndim]
    // ---------------------------

    std::vector<size_t> strides ( bool bytes=false ) const
    {
      size_t ndim = this->ndim();
      std::vector<size_t> ret(ndim);

      for ( size_t i=0 ; i<ndim ; i++ )
        ret[i] = _strides[i];

      if ( bytes )
        for ( size_t i=0 ; i<ndim ; i++ )
          ret[i] *= sizeof(T);

      return ret;
    };

    // return size
    // -----------

    size_t size ( void ) const
    { return _data.size(); };

    // return number of dimensions
    // ---------------------------

    size_t ndim ( void ) const
    {
      size_t i;

      for ( i=2 ; i>0 ; i-- )
        if ( _shape[i]!=1 )
          break;

      return i+1;
    };

    // minimum / maximum / mean / sum
    // ------------------------------

    T min ( void )
    { return *std::min_element(_data.begin(),_data.end()); };

    T max ( void )
    { return *std::max_element(_data.begin(),_data.end()); };

    double mean ( void )
    {
      T out = static_cast<T>(0);
      for ( auto i : _data )
        out += i;

      return static_cast<double>(out)/static_cast<double>(this->size());
    }

    T sum ( void )
    {
      T out = static_cast<T>(0);
      for ( auto i : _data )
        out += i;

      return out;
    }

    // initialize to zero/one
    // ----------------------

    void zeros ( void )
    {
      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] = static_cast<T>(0);
    }

    void ones ( void )
    {
      for ( size_t i=0 ; i<this->size() ; i++ )
        _data[i] = static_cast<T>(1);
    }

    // convert type
    // ------------

    Matrix<double> as_double ( double norm=1. )
    {
      Matrix<double> out(this->shape());

      for ( size_t i=0 ; i<this->size() ; i++ )
        out[i] = static_cast<double>(_data[i])/norm;

      return out;
    }

    // print to screen
    // ---------------

    void print(std::string fmt)
    {
      std::string sep;
      std::string str;

      if ( ndim()==1 ) {
        sep = ",";
        str = fmt+sep;
        for ( size_t h=0 ; h<shape()[0]-1 ; ++h )
          printf(str.c_str(),_data[h]);
        sep = "\n";
        str = fmt+sep;
        printf(str.c_str(),_data[shape()[0]-1]);
      }
      else if ( ndim()==2 ) {
        for ( size_t h=0 ; h<shape()[0] ; ++h ) {
          sep = ",";
          str = fmt+sep;
          for ( size_t i=0 ; i<shape()[1]-1 ; ++i )
            printf(str.c_str(),_data[h*_strides[0]+i*_strides[1]]);
          sep = ";\n";
          str = fmt+sep;
          printf(str.c_str(),_data[h*_strides[0]+(shape()[1]-1)*_strides[1]]);
        }
      }
      else if ( ndim()==3 ) {
        for ( size_t h=0 ; h<shape()[0] ; ++h ) {
          for ( size_t i=0 ; i<shape()[1] ; ++i ) {
            sep = ",";
            str = fmt+sep;
            for ( size_t j=0 ; j<shape()[2]-1 ; ++j )
              printf(str.c_str(),_data[h*_strides[0]+i*_strides[1]+j*_strides[2]]);
            sep = ";\n";
            str = fmt+sep;
            printf(str.c_str(),_data[h*_strides[0]+i*_strides[1]+(shape()[2]-1)*_strides[2]]);
          }
          if ( h<shape()[0]-1 )
            printf("\n");
        }
      }
    }

}; // class Matrix

// arithmetic operators
// --------------------

template <class T>
Matrix<T> operator* (const Matrix<T> &A, const Matrix<T> &B)
{ Matrix<T> C = A; return C *= B; }

template <class T>
Matrix<T> operator* (const Matrix<T> &A, T B)
{ Matrix<T> C = A; return C *= B; }

template <class T>
Matrix<T> operator* (T A, const Matrix<T> &B)
{ Matrix<T> C = B; return C *= A; }

template <class T>
Matrix<T> operator/ (const Matrix<T> &A, const Matrix<T> &B)
{ Matrix<T> C = A; return C /= B; }

template <class T>
Matrix<T> operator/ (const Matrix<T> &A, T B)
{ Matrix<T> C = A; return C /= B; }

template <class T>
Matrix<T> operator/ (T A, const Matrix<T> &B)
{ Matrix<T> C = B; return C /= A; }

template <class T>
Matrix<T> operator+ (const Matrix<T> &A, const Matrix<T> &B)
{ Matrix<T> C = A; return C += B; }

template <class T>
Matrix<T> operator+ (const Matrix<T> &A, T B)
{ Matrix<T> C = A; return C += B; }

template <class T>
Matrix<T> operator+ (T A, const Matrix<T> &B)
{ Matrix<T> C = B; return C += A; }

template <class T>
Matrix<T> operator- (const Matrix<T> &A, const Matrix<T> &B)
{ Matrix<T> C = A; return C -= B; }

template <class T>
Matrix<T> operator- (const Matrix<T> &A, T B)
{ Matrix<T> C = A; return C -= B; }

template <class T>
Matrix<T> operator- (T A, const Matrix<T> &B)
{ Matrix<T> C = B; return C -= A; }

// print to "std::cout"
// --------------------

template <class T>
std::ostream& operator<<(std::ostream& out, Matrix<T>& src) {

  if ( src.ndim()==1 ) {
    for ( size_t i=0 ; i<src.shape()[0]-1 ; ++i )
      out << src(i) << " , ";
    out << src(src.shape()[0]-1) << std::endl;
  }
  else if ( src.ndim()==2 ) {
    for ( size_t i=0 ; i<src.shape()[0] ; ++i ) {
      for ( size_t j=0 ; j<src.shape()[1]-1 ; ++j ) {
        out << src(i,j) << ", ";
      }
      out << src(i,src.shape()[1]-1) << "; " << std::endl;
    }
  }
  else if ( src.ndim()==3 ) {
    for ( size_t h=0 ; h<src.shape()[0] ; ++h ) {
      for ( size_t i=0 ; i<src.shape()[1] ; ++i ) {
        for ( size_t j=0 ; j<src.shape()[2]-1 ; ++j ) {
          out << src(h,i,j) << ", ";
        }
        out << src(i,src.shape()[1]-1) << "; " << std::endl;
      }
      if ( h<src.shape()[0]-1 )
        out << std::endl;
    }
  }

  return out;
}

// =============================================================================
// Image::... (functions)
// =============================================================================

// abbreviate data types to enhance readability -> templates fit on one line
using Md = Matrix<double>;
using Mi = Matrix<int>;
using Vs = std::vector<size_t>;
using Vi = std::vector<int>;
using d  = double;
using i  = int;
using b  = bool;
using s  = std::string;

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
Mi stamp_points ( Vs &shape );

// (zero)pad "pad_shape" entries on each side of "src" (with a certain "value")
template <class T> Matrix<T> pad ( Matrix<T> &src, Vs pad_shape, T value=0 );

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
template <class T> double mean ( Matrix<T> &src                     );
template <class T> double mean ( Matrix<T> &src , Matrix<int> &mask );

// 2-point probability (binary), 2-point cluster function (int), and 2-point correlations (double)
std::tuple<Md,i > S2 ( Mi &f, Mi &g, Vs &roi                                                   );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi,                     b pad      , b periodic      );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi, Mi &fmsk,           b pad=false, b periodic=true );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi, Mi &fmsk, Mi &gmsk, b pad=false, b periodic=true );
std::tuple<Md,i > S2 ( Md &f, Md &g, Vs &roi                                                   );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi,                     b pad      , b periodic      );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi, Mi &fmsk,           b pad=false, b periodic=true );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi, Mi &fmsk, Mi &gmsk, b pad=false, b periodic=true );

// weighted 2-point probability (binary) or 2-point correlation (float)
std::tuple<Md,i > W2 ( Mi &W, Mi &I, Vs &roi                                        );
std::tuple<Md,Mi> W2 ( Mi &W, Mi &I, Vs &roi,          b pad      , b periodic      );
std::tuple<Md,Mi> W2 ( Mi &W, Mi &I, Vs &roi, Mi &msk, b pad=false, b periodic=true );
std::tuple<Md,i > W2 ( Mi &W, Md &I, Vs &roi                                        );
std::tuple<Md,Mi> W2 ( Mi &W, Md &I, Vs &roi,          b pad      , b periodic      );
std::tuple<Md,Mi> W2 ( Mi &W, Md &I, Vs &roi, Mi &msk, b pad=false, b periodic=true );
std::tuple<Md,d > W2 ( Md &W, Md &I, Vs &roi                                        );
std::tuple<Md,Md> W2 ( Md &W, Md &I, Vs &roi,          b pad      , b periodic      );
std::tuple<Md,Md> W2 ( Md &W, Md &I, Vs &roi, Mi &msk, b pad=false, b periodic=true );

// collapsed weighted 2-point correlation
std::tuple<Md,Mi> W2c ( Md &I, Mi &clus, Mi &cntr, Vs &roi, Mi &msk, s mode="Bresenham", b periodic=true );
std::tuple<Md,Mi> W2c ( Md &I, Mi &clus, Mi &cntr, Vs &roi,          s mode="Bresenham", b periodic=true );

// lineal path function (binary or int)
std::tuple<Md,Mi> L ( Mi &f, Vs &roi, s mode="Bresenham", b periodic=true );

}; // namespace Image

#endif

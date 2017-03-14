
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>

#define PER(i,N) ( (i<0) ? i+N : (i>=N) ? i-N : i )
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

    // default constructor
    // -------------------

    Matrix<T>(){};

    // constructor
    // -----------

    Matrix<T>( std::vector<size_t> shape, const T *data=NULL )
    {
      if ( shape.size()<1 || shape.size()>3 )
        throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

      // store '_strides' and '_shape' always in 3-D,
      // use unit-length for "extra" dimensions (> 'shape.size()')
      while ( _shape  .size()<3 ) { _shape  .push_back(1); }
      while ( _strides.size()<3 ) { _strides.push_back(1); }

      for ( int i=0 ; i<shape.size() ; i++ )
        _shape[i] = shape[i];

      _strides[0] = _shape[2]*_shape[1];
      _strides[1] = _shape[2];
      _strides[2] = 1;

      int size = _shape[0]*_shape[1]*_shape[2];

      for ( int i=0 ; i<_data.size() ; i++ )
        _data[i] = (T)0;

      while ( _data.size()<size )
        _data.push_back((T)0);

      if ( data!=NULL )
        for ( int i=0 ; i<size ; i++ )
          _data[i] = data[i];
    };

    // copy constructor
    // ----------------

    Matrix<T>( const Matrix<T> &src )
    {
      _data    = src._data;
      _shape   = src._shape;
      _strides = src._strides;
    };

    // index operators
    // ---------------

    T& operator[] ( size_t i )
    { return _data[i]; };

    T& operator() ( size_t h, size_t i=0, size_t j=0 )
    { return _data[h*_strides[0]+i*_strides[1]+j*_strides[2]]; };

    // iterators
    // ---------

    auto begin()
    { return _data.begin(); }

    auto end ()
    { return _data.end  (); }

    // return pointer to data
    // ----------------------

    const T* data ( void ) const
    { return _data.data(); };

    // return shape array [ndim]
    // -------------------------

    std::vector<size_t> shape ( int ndim=0 ) const
    {
      if ( ndim==0 )
        ndim = this->ndim();

      std::vector<size_t> ret(ndim);

      for ( int i=0 ; i<ndim ; i++ )
        ret[i] = _shape[i];

      return ret;
    };

    // return strides array [ndim]
    // ---------------------------

    std::vector<size_t> strides ( bool bytes=false ) const
    {
      int ndim = this->ndim();
      std::vector<size_t> ret(ndim);

      for ( int i=0 ; i<ndim ; i++ )
        ret[i] = _strides[i];

      if ( bytes )
        for ( int i=0 ; i<ndim ; i++ )
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

    // maximum
    // -------

    T max ( void )
    { return *std::max_element(_data.begin(),_data.end()); };

}; // class Matrix

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

std::tuple<i,i,i> unpack3d ( Vs src, i value=1 );

Vs midpoint ( Vs shape );

Mi path ( Vi &xa, Vi &xb, std::string mode="Bresenham" );

Mi stamp_points ( Vs &shape );

Mi pad ( Mi &src, Vs &pad_shape, i value=0  );
Md pad ( Md &src, Vs &pad_shape, d value=0. );

Mi dummy_circles ( Vs &shape,                          b periodic=true );
Mi dummy_circles ( Vs &shape, Vi &row, Vi &col, Vi &r, b periodic=true );

std::tuple<Mi,Mi> clusters ( Mi &src,             b periodic=true, i min_size=0);
std::tuple<Mi,Mi> clusters ( Mi &src, Mi &kernel, b periodic=true, i min_size=0);

std::tuple<Md,i > S2 ( Mi &f, Mi &g, Vs &roi                                                   );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi,                     b periodic=true, b pad=false );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi, Mi &fmsk,           b periodic=true, b pad=false );
std::tuple<Md,Mi> S2 ( Mi &f, Mi &g, Vs &roi, Mi &fmsk, Mi &gmsk, b periodic=true, b pad=false );
std::tuple<Md,i > S2 ( Md &f, Md &g, Vs &roi                                                   );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi,                     b periodic=true, b pad=false );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi, Mi &fmsk,           b periodic=true, b pad=false );
std::tuple<Md,Mi> S2 ( Md &f, Md &g, Vs &roi, Mi &fmsk, Mi &gmsk, b periodic=true, b pad=false );

std::tuple<Md,i > W2 ( Mi &W, Mi &I, Vs &roi                                        );
std::tuple<Md,Mi> W2 ( Mi &W, Mi &I, Vs &roi,          b periodic=true, b pad=false );
std::tuple<Md,Mi> W2 ( Mi &W, Mi &I, Vs &roi, Mi &msk, b periodic=true, b pad=false );
std::tuple<Md,i > W2 ( Mi &W, Md &I, Vs &roi                                        );
std::tuple<Md,Mi> W2 ( Mi &W, Md &I, Vs &roi,          b periodic=true, b pad=false );
std::tuple<Md,Mi> W2 ( Mi &W, Md &I, Vs &roi, Mi &msk, b periodic=true, b pad=false );
std::tuple<Md,d > W2 ( Md &W, Md &I, Vs &roi                                        );
std::tuple<Md,Md> W2 ( Md &W, Md &I, Vs &roi,          b periodic=true, b pad=false );
std::tuple<Md,Md> W2 ( Md &W, Md &I, Vs &roi, Mi &msk, b periodic=true, b pad=false );

}; // namespace image

#endif

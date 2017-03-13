
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

#define PER(i,N) ( (i<0) ? i+N : (i>=N) ? i-N : i )

namespace Image {

template <class T> class Matrix
{

  private:

    std::vector<T>      _data;    // data array
    std::vector<size_t> _shape;   // number of entries in each dimensions
    std::vector<size_t> _strides;  // stride length for each index

  public:

    Matrix<T>();
    Matrix<T>( std::vector<size_t>, const T *data=NULL );
    Matrix<T>( const Matrix<T>& );

    const T*            data    ( void             ) const;
    std::vector<size_t> shape   ( void             ) const;
    std::vector<size_t> strides ( bool bytes=false ) const;
    size_t              size    ( void             ) const;
    size_t              ndim    ( void             ) const;

    T&   operator[] ( size_t i                         );
    T&   operator() ( size_t h, size_t i=0, size_t j=0 );

}; // class Matrix

std::tuple<int,int,int> unpack3d ( std::vector<size_t> src, int value=1 );

std::vector<size_t> midpoint ( std::vector<size_t> shape );

template <typename T> Matrix<T> pad ( Matrix<T> src, std::vector<size_t> pad_shape, T value=(T)0 );

Matrix<int> dummy_circles ( std::vector<size_t> &shape,                                                                    bool periodic=true );
Matrix<int> dummy_circles ( std::vector<size_t> &shape, std::vector<int> &row, std::vector<int> &col, std::vector<int> &r, bool periodic=true );

std::tuple<Matrix<double>,       int > S2 ( Matrix<int   > &f, Matrix<int   > &g, std::vector<size_t> &roi                                                                                 );
std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int   > &f, Matrix<int   > &g, std::vector<size_t> &roi,                                         bool periodic=true, bool zeropad=false );
std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int   > &f, Matrix<int   > &g, std::vector<size_t> &roi, Matrix<int> &fmask,                     bool periodic=true, bool zeropad=false );
std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int   > &f, Matrix<int   > &g, std::vector<size_t> &roi, Matrix<int> &fmask, Matrix<int> &gmask, bool periodic=true, bool zeropad=false );
std::tuple<Matrix<double>,       int > S2 ( Matrix<double> &f, Matrix<double> &g, std::vector<size_t> &roi                                                                                 );

std::tuple<Matrix<double>,       int > W2 ( Matrix<int> &W, Matrix<int> &I, std::vector<size_t> &roi );
std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<int> &I, std::vector<size_t> &roi,                    bool periodic=true, bool zeropad=false );
std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<int> &I, std::vector<size_t> &roi, Matrix<int> &mask, bool periodic=true, bool zeropad=false );

}; // namespace image

#endif

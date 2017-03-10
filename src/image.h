
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

std::vector<size_t> midpoint ( std::vector<size_t> shape );

Matrix<int> dummy_circles ( std::vector<size_t> &shape , std::vector<int> &x , std::vector<int> &y , std::vector<int> &r , bool periodic=true );

std::tuple<Matrix<int   >,int> S2 ( Matrix<int   > &f , Matrix<int   > &g , std::vector<size_t> &roi );
std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f , Matrix<double> &g , std::vector<size_t> &roi );

}; // namespace image

#endif

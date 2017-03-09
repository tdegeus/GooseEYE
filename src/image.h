
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>


namespace Image {

template <class T> class Matrix
{

  public:

    std::vector<T>      data;     // data array
    std::vector<size_t> shape;    // number of entries in each dimensions
    std::vector<size_t> strides;  // stride length for each index
    size_t              size;     // total number of entries (==prod(shape))
    size_t              ndim;     // number of dimensions (1,2,3)

    Matrix<T>();
    Matrix<T>(std::vector<size_t>, const T *data=NULL );
    Matrix<T>(const Matrix<T>& );

    T&   operator[] ( size_t i                         );
    T&   operator() ( size_t h, size_t i=0, size_t j=0 );

}; // class Matrix

std::tuple<Matrix<int   >,int> S2 ( Matrix<int   > &f , Matrix<int   > &g , std::vector<size_t> roi=std::vector<size_t>() , bool periodic=true );
std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f , Matrix<double> &g , std::vector<size_t> roi=std::vector<size_t>() , bool periodic=true );

}; // namespace image

#endif

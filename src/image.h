
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

}; // class Matrix

Matrix<int> S2 ( Matrix<int> &f , Matrix<int> &g );

}; // namespace image

#endif

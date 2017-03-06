
#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>


namespace image {

template <class T> class Matrix
{

  public:

    std::vector<T>   data;     // data array
    std::vector<int> shape;    // number of entries in each dimensions
    int              size;     // total number of entries (==prod(shape))
    int              nd;       // number of dimensions (1,2,3)
    bool             periodic; // periodic: true/false

    Matrix<T>();
    Matrix<T>(int nh, int ni=1, int nj=1, bool periodic=false);

}; // class Matrix



std::vector<double> S2 ( std::vector<double>& pos );

}; // namespace image

#endif

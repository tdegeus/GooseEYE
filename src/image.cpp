
#include "image.h"

namespace Image {

// =============================================================================
// Image::Matrix class
// =============================================================================

// -----------------------------------------------------------------------------
// Image::Matrix: default class constructor
// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix()
{
}

// -----------------------------------------------------------------------------
// Image::Matrix: class constructor with arguments
// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix (std::vector<size_t> shape_, const T *data_ )
{

  int i;

  ndim = shape_.size();

  if ( ndim<1 || ndim>3 )
    throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

  while ( shape  .size()<3 ) { shape  .push_back(1); }
  while ( strides.size()<3 ) { strides.push_back(1); }

  for ( i=0             ; i<shape_.size() ; i++ ) { shape[i] = shape_[i]; }
  for ( i=shape_.size() ; i<3             ; i++ ) { shape[i] = 1;         }

  for ( i=2 ; i>0 ; i-- )
    if ( shape[i]!=1 )
      break;

  ndim = i+1;

  size = 1;
  for ( i=0 ; i<3 ; i++ )
    size *= shape[i];

  strides[0] = shape[2]*shape[1];
  strides[1] = shape[2];
  strides[2] = 1;

  while ( data.size()<size )
    data.push_back((T)0);

  if ( data_!=NULL )
    for ( int i=0 ; i<size ; i++ )
      data[i] = data_[i];

}

// -----------------------------------------------------------------------------
// Image::Matrix: copy operator
// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix (const Matrix<T>& src )
{
  data    = src.data;
  shape   = src.shape;
  strides = src.strides;
  size    = src.size;
  ndim    = src.ndim;

}

// -----------------------------------------------------------------------------
// Image::Matrix: operator[]
// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator[] (size_t i)
{
  return data[i];
}

// -----------------------------------------------------------------------------
// Image::Matrix: operator()
// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator() (size_t h, size_t i, size_t j)
{
  return data[h*strides[0]+i*strides[1]+j*strides[2]];
}

// -----------------------------------------------------------------------------
// Image::Matrix: initialize different types
// -----------------------------------------------------------------------------

template class Matrix<int>;
template class Matrix<double>;

// =============================================================================
// 2-point correlation: bool/int
// =============================================================================

std::tuple<Matrix<int>,int> S2 ( Matrix<int> &f , Matrix<int> &g , std::vector<size_t> roi , bool periodic )
{
  Matrix<int> out(f.shape);

  std::cout << "int" << std::endl;

  for ( int i=0 ; i<f.shape[0] ; i++ )
    for ( int j=0 ; j<f.shape[1] ; j++ )
      out(i,j) = f(i,j)*g(i,j);

  // for ( int i=0 ; i<f.size ; i++ )
  //   out.data[i] = f.data[i]*g.data[i];

  return std::make_tuple(out,10);

}

std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f , Matrix<double> &g , std::vector<size_t> roi , bool periodic )
{
  Matrix<double> out(f.shape);

  std::cout << "double" << std::endl;

  for ( int i=0 ; i<f.size ; i++ )
    out.data[i] = f.data[i]*g.data[i];

  return std::make_tuple(out,10);

}

}

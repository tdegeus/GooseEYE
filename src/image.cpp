
#include "image.h"

namespace Image {

// =============================================================================
// class constructor: default
// =============================================================================

template <class T>
Matrix<T>::Matrix()
{
}

// =============================================================================
// class constructor: with arguments
// =============================================================================

template <class T>
Matrix<T>::Matrix (std::vector<size_t> shape_, const T *data_ )
{

  int i;

  ndim = shape_.size();

  if ( ndim<1 || ndim>3 )
    throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

  while ( shape.size()<3 )
    shape.push_back(1);
  while ( strides.size()<3 )
    strides.push_back(1);

  for ( i=0 ; i<shape_.size() ; i++ )
    shape[i] = shape_[i];
  for ( i=shape_.size() ; i<3 ; i++ )
    shape[i] = 1;

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

// =============================================================================
// class constructor: copy
// =============================================================================

template <class T>
Matrix<T>::Matrix (const Matrix<T>& src )
{
  data    = src.data;
  shape   = src.shape;
  strides = src.strides;
  size    = src.size;
  ndim    = src.ndim;

}

// =============================================================================
// initialize different types of matrices
// =============================================================================

template class Matrix<int>;
template class Matrix<bool>;
template class Matrix<double>;

// =============================================================================
// 2-point correlation
// =============================================================================

Matrix<int> S2 ( Matrix<int> &f , Matrix<int> &g )
{
  Matrix<int> out(f.shape);

  for ( int i=0 ; i<f.size ; i++ )
    out.data[i] = f.data[i]*g.data[i];

  return out;

}

}


#include "image.h"

namespace Image {

// =============================================================================
// Image::Matrix class
// =============================================================================

// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix()
{
}

// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix (std::vector<size_t> shape, const T *data )
{

  int i,size;

  if ( shape.size()<1 || shape.size()>3 )
    throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

  // store '_strides' and '_shape' always in 3-D,
  // use unit-length for "extra" dimensions (> 'shape.size()')
  while ( _shape  .size()<3 ) { _shape  .push_back(1); }
  while ( _strides.size()<3 ) { _strides.push_back(1); }

  for ( i=0 ; i<shape.size() ; i++ )
    _shape[i] = shape[i];

  _strides[0] = _shape[2]*_shape[1];
  _strides[1] = _shape[2];
  _strides[2] = 1;

  size = _shape[0]*_shape[1]*_shape[2];

  for ( i=0 ; i<_data.size() ; i++ )
    _data[i] = (T)0;

  while ( _data.size()<size )
    _data.push_back((T)0);

  if ( data!=NULL )
    for ( i=0 ; i<size ; i++ )
      _data[i] = data[i];

}

// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix (const Matrix<T>& src )
{
  _data    = src._data;
  _shape   = src._shape;
  _strides = src._strides;
}

// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator[] (size_t i)
{
  return _data[i];
}

// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator() (size_t h, size_t i, size_t j)
{
  return _data[h*_strides[0]+i*_strides[1]+j*_strides[2]];
}

// -----------------------------------------------------------------------------

template <class T>
const T* Matrix<T>::data ( void ) const
{
  return _data.data();
}

// -----------------------------------------------------------------------------

template <class T>
size_t Matrix<T>::size ( void ) const
{
  return _data.size();
}

// -----------------------------------------------------------------------------

template <class T>
size_t Matrix<T>::ndim ( void ) const
{
  size_t i;

  for ( i=2 ; i>0 ; i-- )
    if ( _shape[i]!=1 )
      break;

  return i+1;
}

// -----------------------------------------------------------------------------

template <class T>
std::vector<size_t> Matrix<T>::shape ( int ndim ) const
{
  if ( ndim==0 )
    ndim = this->ndim();

  std::vector<size_t> out(ndim);

  for ( int i=0 ; i<ndim ; i++ )
    out[i] = _shape[i];

  return out;
}

// -----------------------------------------------------------------------------

template <class T>
std::vector<size_t> Matrix<T>::strides ( bool bytes ) const
{
  int ndim = this->ndim();
  std::vector<size_t> out(ndim);

  for ( int i=0 ; i<ndim ; i++ )
    out[i] = _strides[i];

  if ( bytes )
    for ( int i=0 ; i<ndim ; i++ )
      out[i] *= sizeof(T);

  return out;
}

// -----------------------------------------------------------------------------

template class Matrix<int>;
template class Matrix<double>;

// =============================================================================

std::vector<size_t> midpoint ( std::vector<size_t> shape )
{
  std::vector<size_t> out(shape.size());

  for ( int i=0 ; i<shape.size() ; i++ )
    if ( !(shape[i]%2) )
      throw std::domain_error("Only allowed for odd-shaped matrices");

  for ( int i=0 ; i<shape.size() ; i++ )
    out[i] = (shape[i]-1)/2;

  return out;

}

// =============================================================================

Matrix<int> dummy_circles ( std::vector<size_t> shape ,
  std::vector<int> x , std::vector<int> y , std::vector<int> r , bool periodic )
{

  if ( shape.size()!=2 )
    throw std::length_error("Only allowed in 2 dimensions");

  if ( x.size()!=y.size() || x.size()!=y.size() )
    throw std::length_error("'x', 'y', and 'r' are inconsistent");

  int i,di,dj,N,M;
  Matrix<int> out(shape);

  N = shape[0];
  M = shape[1];

  for ( i=0 ; i<x.size() ; i++ )
    for ( di=-r[i] ; di<=r[i] ; di++ )
      for ( dj=-r[i] ; dj<=r[i] ; dj++ )
        if ( periodic || ( x[i]+di>=0 && x[i]+di<N && y[i]+dj>=0 && y[i]+dj<M ) )
          if ( (int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i] )
            out(PER(x[i]+di,N),PER(y[i]+dj,M)) = 1;

  return out;
}

// =============================================================================

std::tuple<Matrix<int>,int> S2 ( Matrix<int> &f , Matrix<int> &g ,
  std::vector<size_t> roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,N,M,P,dN,dM,dP;
  Matrix<int> out(roi);

  while ( roi.size()<3 )
    roi.push_back(1);

  std::vector<size_t> mid = midpoint(roi);

  N  = f.shape(3)[0];
  M  = f.shape(3)[1];
  P  = f.shape(3)[2];
  dN = (int)mid[0];
  dM = (int)mid[1];
  dP = (int)mid[2];

  for ( h = 0 ; h<N ; h++ )
    for ( i = 0 ; i<M ; i++ )
      for ( j = 0 ; j<P ; j++ )
        if ( f(h,i,j) )
          for ( dh=-dN ; dh<=dN ; dh++ )
            for ( di=-dM ; di<=dM ; di++ )
              for ( dj=-dP ; dj<=dP ; dj++ )
                if ( f(h,i,j)==g(PER(h+dh,N),PER(i+di,M),PER(j+dj,P)) )
                  out(dh+dN,di+dM,dj+dP)++;

  return std::make_tuple(out,N*M*P);
}

// =============================================================================

std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f , Matrix<double> &g ,
  std::vector<size_t> roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,N,M,P,dN,dM,dP;
  Matrix<double> out(roi);

  while ( roi.size()<3 )
    roi.push_back(1);

  std::vector<size_t> mid = midpoint(roi);

  N  = f.shape(3)[0];
  M  = f.shape(3)[1];
  P  = f.shape(3)[2];
  dN = (int)mid[0];
  dM = (int)mid[1];
  dP = (int)mid[2];

  for ( h = 0 ; h<N ; h++ )
    for ( i = 0 ; i<M ; i++ )
      for ( j = 0 ; j<P ; j++ )
        for ( dh=-dN ; dh<=dN ; dh++ )
          for ( di=-dM ; di<=dM ; di++ )
            for ( dj=-dP ; dj<=dP ; dj++ )
              out(dh+dN,di+dM,dj+dP) += f(h,i,j)*g(PER(h+dh,N),PER(i+di,M),PER(j+dj,P));

  return std::make_tuple(out,N*M*P);
}

}

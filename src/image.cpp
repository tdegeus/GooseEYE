
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
Matrix<T>::Matrix ( std::vector<size_t> shape, const T *data )
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
Matrix<T>::Matrix ( const Matrix<T>& src )
{
  _data    = src._data;
  _shape   = src._shape;
  _strides = src._strides;
}

// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator[] ( size_t i )
{
  return _data[i];
}

// -----------------------------------------------------------------------------

template <class T>
T& Matrix<T>::operator() ( size_t h, size_t i, size_t j )
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
std::vector<size_t> Matrix<T>::shape ( void ) const
{
  int ndim = this->ndim();
  std::vector<size_t> ret(ndim);

  for ( int i=0 ; i<ndim ; i++ )
    ret[i] = _shape[i];

  return ret;
}

// -----------------------------------------------------------------------------

template <class T>
std::vector<size_t> Matrix<T>::strides ( bool bytes ) const
{
  int ndim = this->ndim();
  std::vector<size_t> ret(ndim);

  for ( int i=0 ; i<ndim ; i++ )
    ret[i] = _strides[i];

  if ( bytes )
    for ( int i=0 ; i<ndim ; i++ )
      ret[i] *= sizeof(T);

  return ret;
}

// -----------------------------------------------------------------------------

template class Matrix<int>;
template class Matrix<double>;

// ==============================================================================

std::tuple<int,int,int> shape3d ( std::vector<size_t> shape, int value )
{
  int h,i,j;

  h = value;
  i = value;
  j = value;

  if ( shape.size()>=1 ) h = (int)shape[0];
  if ( shape.size()>=2 ) i = (int)shape[1];
  if ( shape.size()>=3 ) j = (int)shape[2];

  return std::make_tuple(h,i,j);
}

// ==============================================================================

std::vector<size_t> midpoint ( std::vector<size_t> shape )
{
  std::vector<size_t> ret(shape.size());

  for ( int i=0 ; i<shape.size() ; i++ )
    if ( !(shape[i]%2) )
      throw std::domain_error("Only allowed for odd-shaped matrices");

  for ( int i=0 ; i<shape.size() ; i++ )
    ret[i] = (shape[i]-1)/2;

  return ret;

}

// =============================================================================

template <typename T>
Matrix<T> pad ( Matrix<T> src, std::vector<size_t> pad_shape, T value )
{
  std::vector<size_t> shape = src.shape();

  for ( int i=0 ; i<pad_shape.size() ; i++ )
    shape[i] += 2*pad_shape[i];

  Matrix<T> ret(shape);

  int h,i,j,H,I,J,dH,dI,dJ;

  std::tie( H, I, J) = shape3d(src.shape(),1);
  std::tie(dH,dI,dJ) = shape3d(pad_shape  ,0);

  if ( value!=(T)0 )
    for ( i=0 ; i<ret.size() ; i++ )
      ret[i] = value;

  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        ret(h+dH,i+dI,j+dJ) = src(h,i,j);

  return ret;
}

// =============================================================================

Matrix<int> dummy_circles ( std::vector<size_t> &shape,
  std::vector<int> &x, std::vector<int> &y, std::vector<int> &r, bool periodic )
{
  if ( shape.size()!=2 )
    throw std::length_error("Only allowed in 2 dimensions");

  if ( x.size()!=y.size() || x.size()!=r.size() )
    throw std::length_error("'x', 'y', and 'r' are inconsistent");

  int i,di,dj,I,J;
  Matrix<int> ret(shape);

  I = shape[0];
  J = shape[1];

  for ( i=0 ; i<x.size() ; i++ )
    for ( di=-r[i] ; di<=r[i] ; di++ )
      for ( dj=-r[i] ; dj<=r[i] ; dj++ )
        if ( periodic || ( x[i]+di>=0 && x[i]+di<I && y[i]+dj>=0 && y[i]+dj<J ) )
          if ( (int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i] )
            ret(PER(x[i]+di,I),PER(y[i]+dj,J)) = 1;

  return ret;
}

// =============================================================================

Matrix<int> dummy_circles ( std::vector<size_t> &shape, bool periodic )
{
  if ( shape.size()!=2 )
    throw std::length_error("Only allowed in 2 dimensions");

  std::srand(std::time(0));

  const double PI = std::atan(1.0)*4;

  int N = (int)(.05*(double)shape[0]);
  int M = (int)(.05*(double)shape[1]);
  int R = (int)(pow((.3*(double)(shape[0]*shape[1]))/(PI*(double)(N*M)),.5));

  std::vector<int> x(N*M),y(N*M),r(N*M);

  // define regular grid of circles
  for ( int i=0 ; i<N ; i++ ) {
    for ( int j=0 ; j<M ; j++ ) {
      x[i*M+j] = (int)((double)i*(double)shape[0]/(double)N);
      y[i*M+j] = (int)((double)j*(double)shape[1]/(double)M);
      r[i*M+j] = R;
    }
  }

  int dN = (int)(.5*(double)shape[0]/(double)N);
  int dM = (int)(.5*(double)shape[1]/(double)M);

  // randomly perturb circles (move in any direction, enlarge/shrink)
  for ( int i=0 ; i<N*M ; i++ ) {
    x[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dN;
    y[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dM;
    r[i]  = (int)(((double)(std::rand()%100)/100.*2.+.1)*(double)(r[i]));
  }

  return dummy_circles(shape,x,y,r,periodic);
}

// =============================================================================

std::tuple<Matrix<int>,int> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ;

  Matrix<int> ret(roi);

  std::vector<size_t> mid = midpoint(roi);

  std::tie( H, I, J) = shape3d(f.shape(),1);
  std::tie(dH,dI,dJ) = shape3d(mid      ,0);

  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        if ( f(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( f(h,i,j)==g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  ret(dh+dH,di+dI,dj+dJ)++;

  return std::make_tuple(ret,H*I*J);
}

// =============================================================================

std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f, Matrix<double> &g,
  std::vector<size_t> &roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ;

  Matrix<double> ret(roi);

  std::vector<size_t> mid = midpoint(roi);

  std::tie( H, I, J) = shape3d(f.shape(),1);
  std::tie(dH,dI,dJ) = shape3d(mid      ,0);

  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=-dH ; dh<=dH ; dh++ )
          for ( di=-dI ; di<=dI ; di++ )
            for ( dj=-dJ ; dj<=dJ ; dj++ )
              ret(dh+dH,di+dI,dj+dJ) += f(h,i,j)*g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

  return std::make_tuple(ret,H*I*J);
}

// =============================================================================

std::tuple<Matrix<int>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, Matrix<int> &gmask,
  bool periodic, bool zeropad )
{
  if ( f.shape()!=g.shape() || f.shape()!=fmask.shape() || f.shape()!=gmask.shape() )
    throw std::length_error("'f,g,fmask,gmask' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ,bH=0,bI=0,bJ=0;

  Matrix<int> ret (roi);
  Matrix<int> norm(roi);

  std::vector<size_t> mid = midpoint(roi);

  if ( zeropad ) {
    f     = pad(f    ,mid  );
    g     = pad(g    ,mid  );
    fmask = pad(fmask,mid,1);
    gmask = pad(gmask,mid,1);
  }

  std::tie( H, I, J) = shape3d(f.shape(),1);
  std::tie(dH,dI,dJ) = shape3d(mid      ,0);

  if ( !periodic )
    std::tie(bH,bI,bJ) = shape3d(mid,0);

  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        if ( f(h,i,j) && !fmask(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !gmask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  if ( f(h,i,j)==g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                    ret(dh+dH,di+dI,dj+dJ)++;

  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        if ( !fmask(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !gmask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  norm(dh+dH,di+dI,dj+dJ)++;

  return std::make_tuple(ret,norm);
}

// =============================================================================

std::tuple<Matrix<int>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, bool periodic, bool zeropad )
{
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,periodic,zeropad);
}

// =============================================================================

std::tuple<Matrix<int>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, bool periodic, bool zeropad )
{
  Matrix<int> fmask(f.shape());
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,periodic,zeropad);
}

// =============================================================================



} // namespace Image

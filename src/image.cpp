
#include "image.h"

namespace Image {

// =============================================================================
// Image::Matrix class
// =============================================================================

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix()
{
}

// -----------------------------------------------------------------------------
// constructor -> allocate full matrix, optionally copy from source "*data"
// -----------------------------------------------------------------------------

template <class T>
Matrix<T>::Matrix ( std::vector<size_t> shape, const T *data )
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

}

// -----------------------------------------------------------------------------
// copy constructor
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

// =============================================================================
// return vector as (h,i,j) with default value if "src.size()<3"
// =============================================================================

std::tuple<int,int,int> unpack3d ( std::vector<size_t> src, int value )
{
  int h,i,j;

  h = value;
  i = value;
  j = value;

  if ( src.size()>=1 ) h = (int)src[0];
  if ( src.size()>=2 ) i = (int)src[1];
  if ( src.size()>=3 ) j = (int)src[2];

  return std::make_tuple(h,i,j);
}

// =============================================================================
// compute midpoint from "shape"-vector
// =============================================================================

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
// pad "pad_shape" entries on each side with a certain "value"
// =============================================================================

template <typename T>
Matrix<T> pad ( Matrix<T> src, std::vector<size_t> pad_shape, T value )
{
  std::vector<size_t> shape = src.shape();

  for ( int i=0 ; i<pad_shape.size() ; i++ )
    shape[i] += 2*pad_shape[i];

  Matrix<T> ret(shape);

  int h,i,j,H,I,J,dH,dI,dJ;

  std::tie( H, I, J) = unpack3d(src.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(pad_shape  ,0);

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
// create a dummy image with circles at position "x","y" with radius "r"
// =============================================================================

Matrix<int> dummy_circles ( std::vector<size_t> &shape,
  std::vector<int> &row, std::vector<int> &col, std::vector<int> &r, bool periodic )
{
  if ( shape.size()!=2 )
    throw std::length_error("Only allowed in 2 dimensions");

  if ( row.size()!=col.size() || row.size()!=r.size() )
    throw std::length_error("'row', 'col', and 'r' are inconsistent");

  int i,di,dj,I,J;
  Matrix<int> ret(shape);

  I = shape[0];
  J = shape[1];

  for ( i=0 ; i<row.size() ; i++ )
    for ( di=-r[i] ; di<=r[i] ; di++ )
      for ( dj=-r[i] ; dj<=r[i] ; dj++ )
        if ( periodic || ( row[i]+di>=0 && row[i]+di<I && col[i]+dj>=0 && col[i]+dj<J ) )
          if ( (int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i] )
            ret(PER(row[i]+di,I),PER(col[i]+dj,J)) = 1;

  return ret;
}

// =============================================================================
// create a dummy image with a default number of circles
// at random positions and random radii
// =============================================================================

Matrix<int> dummy_circles ( std::vector<size_t> &shape, bool periodic )
{
  if ( shape.size()!=2 )
    throw std::length_error("Only allowed in 2 dimensions");

  std::srand(std::time(0));

  const double PI = std::atan(1.0)*4;

  // set default: number of circles in both directions and (constant) radius
  int N = (int)(.05*(double)shape[0]);
  int M = (int)(.05*(double)shape[1]);
  int R = (int)(pow((.3*(double)(shape[0]*shape[1]))/(PI*(double)(N*M)),.5));

  std::vector<int> row(N*M),col(N*M),r(N*M);

  // define regular grid of circles
  for ( int i=0 ; i<N ; i++ ) {
    for ( int j=0 ; j<M ; j++ ) {
      row[i*M+j] = (int)((double)i*(double)shape[0]/(double)N);
      col[i*M+j] = (int)((double)j*(double)shape[1]/(double)M);
      r[i*M+j] = R;
    }
  }

  // distance between circles
  int dN = (int)(.5*(double)shape[0]/(double)N);
  int dM = (int)(.5*(double)shape[1]/(double)M);

  // randomly perturb circles (move in any direction, enlarge/shrink)
  for ( int i=0 ; i<N*M ; i++ ) {
    row[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dN;
    col[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dM;
    r[i]  = (int)(((double)(std::rand()%100)/100.*2.+.1)*(double)(r[i]));
  }

  // convert to image
  return dummy_circles(shape,row,col,r,periodic);
}

// =============================================================================
// 2-point correlation (binary) / 2-point cluster function (int)
// =============================================================================

std::tuple<Matrix<double>,int> S2 ( Matrix<int> &f, Matrix<int> &g,
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

  std::tie( H, I, J) = unpack3d(f.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(mid      ,0);

  // compute correlation
  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        if ( f(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( f(h,i,j)==g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  ret(dh+dH,di+dI,dj+dJ) += 1.;

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= (double)f.size();

  return std::make_tuple(ret,f.size());
}

// =============================================================================
// 2-point correlation (floating-point)
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

  std::tie( H, I, J) = unpack3d(f.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(mid      ,0);

  // compute correlation
  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=-dH ; dh<=dH ; dh++ )
          for ( di=-dI ; di<=dI ; di++ )
            for ( dj=-dJ ; dj<=dJ ; dj++ )
              ret(dh+dH,di+dI,dj+dJ) += f(h,i,j)*g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= (double)f.size();

  return std::make_tuple(ret,f.size());
}

// =============================================================================
// masked 2-point correlation (binary) / 2-point cluster function (int)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, Matrix<int> &gmask,
  bool periodic, bool zeropad )
{
  if ( f.shape()!=g.shape() || f.shape()!=fmask.shape() || f.shape()!=gmask.shape() )
    throw std::length_error("'f,g,fmask,gmask' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ,bH=0,bI=0,bJ=0;

  Matrix<double> ret (roi);
  Matrix<int   > norm(roi);

  std::vector<size_t> mid = midpoint(roi);

  if ( zeropad ) {
    f     = pad(f    ,mid  );
    g     = pad(g    ,mid  );
    fmask = pad(fmask,mid,1);
    gmask = pad(gmask,mid,1);
  }

  std::tie( H, I, J) = unpack3d(f.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(mid      ,0);

  // define boundary region to skip
  if ( !periodic )
    std::tie(bH,bI,bJ) = unpack3d(mid,0);

  // compute correlation
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        if ( f(h,i,j) && !fmask(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !gmask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  if ( f(h,i,j)==g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                    ret(dh+dH,di+dI,dj+dJ) += 1.;

  // compute normalization (account for masked voxels)
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        if ( !fmask(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !gmask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  norm(dh+dH,di+dI,dj+dJ)++;

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= (double)norm[i];

  return std::make_tuple(ret,norm);
}

// =============================================================================
// masked 2-point correlation (binary) / 2-point cluster function (int)
// - default "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, bool periodic, bool zeropad )
{
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,periodic,zeropad);
}

// =============================================================================
// masked 2-point correlation (binary) / 2-point cluster function (int)
// - default "fmask" and "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, bool periodic, bool zeropad )
{
  Matrix<int> fmask(f.shape());
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,periodic,zeropad);
}

// =============================================================================
// conditional 2-point probability (binary image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,int> W2 ( Matrix<int> &W, Matrix<int> &src,
  std::vector<size_t> &roi )
{
  if ( W.shape()!=src.shape() )
    throw std::length_error("'W' and 'I' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ;

  Matrix<double> ret(roi);

  std::vector<size_t> mid = midpoint(roi);

  std::tie( H, I, J) = unpack3d(src.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(mid        ,0);

  // compute correlation
  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        if ( W(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  ret(dh+dH,di+dI,dj+dJ) += 1.;

  // compute normalization: sum of weight factors (binary)
  int norm = 0;
  for ( i=0 ; i<W.size() ; i++ )
    if ( W[i] )
      norm++;

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= (double)norm;

  return std::make_tuple(ret,norm);
}

// =============================================================================
// masked conditional 2-point probability (binary image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<int> &src,
  std::vector<size_t> &roi, Matrix<int> &mask, bool periodic, bool zeropad )
{
  if ( W.shape()!=src.shape() || W.shape()!=mask.shape() )
    throw std::length_error("'W', 'I', and 'mask' are inconsistent");

  for ( int i=0 ; i<roi.size() ; i++ )
    if ( roi[i]%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ,bH=0,bI=0,bJ=0;

  Matrix<double> ret (roi);
  Matrix<int   > norm(roi);

  std::vector<size_t> mid = midpoint(roi);

  if ( zeropad ) {
    src  = pad(src ,mid  );
    mask = pad(mask,mid,1);
  }

  std::tie( H, I, J) = unpack3d(src.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(mid        ,0);

  // define boundary region to skip
  if ( !periodic )
    std::tie(bH,bI,bJ) = unpack3d(mid,0);

  // compute correlation
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        if ( W(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !mask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  if ( src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                    ret(dh+dH,di+dI,dj+dJ) += 1.;

  // compute normalization: sum of weight factors (binary) of unmasked voxels
  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        if ( W(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !mask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  norm(dh+dH,di+dI,dj+dJ)++;

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= (double)norm[i];

  return std::make_tuple(ret,norm);
}

// =============================================================================
// conditional 2-point probability (binary image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<int> &src,
  std::vector<size_t> &roi, bool periodic, bool zeropad )
{
  Matrix<int> mask(src.shape());
  return W2(W,src,roi,mask,periodic,zeropad);
}

// =============================================================================

} // namespace Image

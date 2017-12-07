
#include "image.h"

template class cppmat::matrix<int>;
template class cppmat::matrix<double>;

namespace GooseEYE {
namespace Image {

// abbreviate data types to enhance readability below
template <class T> using Mat = cppmat::matrix<T>;
using MatD = cppmat::matrix<double>;
using MatI = cppmat::matrix<int>;
using VecS = std::vector<size_t>;
using VecI = std::vector<int>;
using str  = std::string;

// =================================================================================================
// check number of dimensions / shape
// =================================================================================================

namespace {
bool max3d( int ndim )
{
  if ( ndim < 1 or ndim > 3 )
    return false;

  return true;
}
}

// -------------------------------------------------------------------------------------------------

namespace {
bool max3d( const VecS &shape )
{
  if ( shape.size() < 1 or shape.size() > 3 )
    return false;

  return true;
}
}

// -------------------------------------------------------------------------------------------------

namespace {
bool isodd( const VecS &shape )
{
  for ( auto &i : shape )
    if ( i%2 == 0 )
      return false;

  return true;
}
}

// =================================================================================================
// pixel/voxel path between two points "xa" and "xb"
// =================================================================================================

MatI path ( const VecI &xa, const VecI &xb, str mode )
{
  assert( xa.size() == xb.size() );
  assert( max3d(xa.size()) );

  int ndim = static_cast<int>(xa.size());

  std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

  VecI ret;

  if ( mode == "bresenham" )
  {
    // see http://www.luberth.com/plotter/line3d.c.txt.html
    int a[3],s[3],x[3],d[3],in[2],j,i,iin,nnz=0;

    // set defaults
    for ( i=0; i<3; i++ ) {
      a[i] = 1;
      s[i] = 0;
      x[i] = 0;
      d[i] = 0;
    }

    // calculate:
    // absolute distance, set to "1" if the distance is zero
    // sign of the distance (can be -1/+1 or 0)
    // current position (temporary value)
    for ( i=0; i<ndim; i++) {
      a[i] = std::abs(xb[i]-xa[i]) << 1;
      s[i] = SIGN    (xb[i]-xa[i]);
      x[i] = xa[i];
    }

    // determine which direction is dominant
    for ( j=0; j<3; j++ ) {
      // set the remaining directions
      iin = 0;
      for ( i=0; i<3; i++ ) {
        if ( i!=j ) {
          in[iin] = i;
          iin    += 1;
        }
      }
      // determine if the current direction is dominant
      if ( a[j] >= std::max(a[in[0]],a[in[1]]) )
        break;
    }

    // set increment in non-dominant directions
    for ( i=0; i<2; i++)
      d[in[i]] = a[in[i]]-(a[j]>>1);
    // loop until "x" coincides with "xb"
    while ( 1 ) {
      // add current voxel to path
      for ( i=0; i<ndim; i++)
        ret.push_back(x[i]);
      // update voxel counter
      nnz += 1;
      // check convergence
      if ( x[j]==xb[j] ) {
        MatI retmat({(size_t)nnz,(size_t)ndim},&ret[0]);
        return retmat;
      }
      // check increment in other directions
      for ( i=0; i<2; i++ ) {
        if ( d[in[i]]>=0 ) {
          x[in[i]] += s[in[i]];
          d[in[i]] -= a[j];
        }
      }
      // increment
      x[j] += s[j];
      for ( i=0; i<2; i++ )
        d[in[i]] += a[in[i]];
    }
  }

  if ( mode == "actual" or mode == "full" )
  {
    // position, slope, (length to) next intersection
    double x[3],v[3],t[3],next[3],sign[3];
    int isign[3];
    // active dimensions (for in-plane paths dimension have to be skipped
    // to avoid dividing by zero)
    int in[3],iin,nin;
    // path of the current voxel
    int cindex[3];
    int nnz = 1;
    // counters
    int i,imin,n;

    // set the direction coefficient in all dimensions; if it is zero this
    // dimension is excluded from further analysis (i.e. in-plane growth)
    nin = 0;
    for ( i=0 ; i<ndim ; i++ ) {
      // set origin, store to output array; initiate the position
      cindex[i] = xa[i];
      ret.push_back(cindex[i]);
      // initiate position, set slope
      x[i] = (double)(xa[i]);
      v[i] = (double)(xb[i]-xa[i]);
      // non-zero slope: calculate the sign and the next intersection
      // with a voxel's edges, and update the list to include this dimension
      // in the further analysis
      if ( v[i] ) {
        sign[i]  = v[i]/fabs(v[i]);
        isign[i] = (int)sign[i];
        next[i]  = sign[i]*0.5;
        in[nin]  = i;
        nin++;
      }
    }

    // starting from "xa" loop to "xb"
    while ( 1 ) {

      // find translation coefficient "t" for each next intersection
      // (only include dimensions with non-zero slope)
      for ( iin=0 ; iin<nin ; iin++ ) {
        i      = in[iin];
        t[iin] = (next[i]-x[i])/v[i];
      }
      // find the minimum "t": the intersection which is closet along the line
      // from the current position -> proceed in this direction
      imin = 0;
      for ( iin=1 ; iin<nin ; iin++ )
        if ( t[iin]<t[imin] )
          imin = iin;

      // update path: proceed in dimension of minimum "t"
      // note: if dimensions have equal "t" -> proceed in each dimension
      for ( iin=0 ; iin<nin ; iin++ ) {
        if ( fabs(t[iin]-t[imin])<1.e-6 ) {
          cindex[in[iin]] += isign[in[iin]];
          next[in[iin]]   += sign[in[iin]];
          // store all the face crossings ("mode")
          if ( mode == "full" ) {
            for ( i=0 ; i<ndim ; i++ )
              ret.push_back(cindex[i]);
            nnz++;
          }
        }
      }
      // store only the next voxel ("actual")
      if ( mode == "actual" ) {
        for ( i=0 ; i<ndim ; i++ )
          ret.push_back(cindex[i]);
        nnz++;
      }
      // update position, and current path
      for ( i=0 ; i<ndim ; i++ )
        x[i] = xa[i]+v[i]*t[imin];

      // check convergence: stop when "xb" is reached
      n = 0;
      for ( i=0 ; i<ndim; i++ )
        if ( cindex[i]==xb[i] )
          n++;
      if ( n==ndim )
        break;

    }

    MatI retmat({(size_t)nnz,(size_t)ndim},&ret[0]);
    return retmat;
  }

  throw std::out_of_range("Unknown 'mode'");
}

// =================================================================================================
// list of end-points of ROI-stamp used in path-based correlations
// =================================================================================================

MatI stamp_points ( const VecS &N )
{
  assert( max3d(N) );
  assert( isodd(N) );

  int n,i,j;
  int idx = 0;
  int nd  = N.size();

  int H  = N.size() >= 1 ? (int)N[0] : 1;
  int I  = N.size() >= 2 ? (int)N[1] : 1;
  int J  = N.size() >= 3 ? (int)N[2] : 1;

  int dH = ( H - 1 ) / 2;
  int dI = ( I - 1 ) / 2;
  int dJ = ( J - 1 ) / 2;

  // number of points
  if      ( nd == 1 ) n =           2;
  else if ( nd == 2 ) n =           2*H+2*POS(I-2);
  else                n = POS(J-2)*(2*H+2*POS(I-2))+2*H*I;

  // allocate
  MatI ret({(size_t)n,(size_t)nd});

  // 1-D
  // ---

  if ( nd == 1 )
  {
    ret(idx,0) = -dH; idx++;
    ret(idx,0) = +dH; idx++;
    return ret;
  }

  // 2-D
  // ---

  if ( nd == 2 )
  {
    for ( i=0 ; i<H   ; i++ ) {
      ret(idx,0) = i-dH; ret(idx,1) =  -dI; idx++;
      ret(idx,0) = i-dH; ret(idx,1) =  +dI; idx++;
    }
    for ( i=1 ; i<I-1 ; i++ ) {
      ret(idx,0) =  -dH; ret(idx,1) = i-dI; idx++;
      ret(idx,0) =  +dH; ret(idx,1) = i-dI; idx++;
    }
    return ret;
  }

  // 3-D
  // ---

  for ( j=1 ; j<J-1 ; j++ ) {
    for ( i=0 ; i<H   ; i++ ) {
      ret(idx,0) = i-dH; ret(idx,1) =  -dI; ret(idx,2) = j-dJ; idx++;
      ret(idx,0) = i-dH; ret(idx,1) =  +dI; ret(idx,2) = j-dJ; idx++;
    }
    for ( i=1 ; i<I-1 ; i++ ) {
      ret(idx,0) =  -dH; ret(idx,1) = i-dI; ret(idx,2) = j-dJ; idx++;
      ret(idx,0) =  +dH; ret(idx,1) = i-dI; ret(idx,2) = j-dJ; idx++;
    }
  }
  for ( i=0 ; i<H   ; i++ ) {
    for ( j=0 ; j<I   ; j++ ) {
      ret(idx,0) = i-dH; ret(idx,1) = j-dI; ret(idx,2) =  -dJ; idx++;
      ret(idx,0) = i-dH; ret(idx,1) = j-dI; ret(idx,2) =  +dJ; idx++;
    }
  }
  return ret;
}

// =================================================================================================
// define kernel
// =================================================================================================

MatI kernel ( int ndim , str mode )
{
  std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

  if ( mode == "default" )
  {
    VecS shape(ndim);
    for ( int i=0 ; i<ndim ; i++ )
      shape[i] = 3;

    MatI kern(shape); kern.zeros();

    if      ( ndim==1 )
    {
      kern(0) = 1; kern(1) = 1; kern(2) = 1;
    }
    else if ( ndim==2 )
    {
      kern(1,0) = 1; kern(1,1) = 1; kern(1,2) = 1;
      kern(0,1) = 1; kern(2,1) = 1;
    }
    else if ( ndim==3 )
    {
      kern(1,1,0) = 1; kern(1,1,1) = 1; kern(1,1,2) = 1;
      kern(1,0,1) = 1; kern(1,2,1) = 1;
      kern(0,1,1) = 1; kern(2,1,1) = 1;
    }
    else
    {
      throw std::length_error("Only defined in 1-, 2-, or 3-D");
    }

    return kern;
  }

  throw std::length_error("Unknown mode");
}

// =================================================================================================
// pad "npad" entries on each side of "src" with a certain "value"
// =================================================================================================

template <class T>
Mat<T> pad ( const Mat<T> &src, const VecS &npad, T value )
{
  // get the current shape of "src"
  VecS shape = src.shape();

  // extend the shape with the padding
  for ( size_t i = 0 ; i < npad.size() ; ++i )
    shape[i] += 2*npad[i];

  // allocate the output, and initialize to supplied value (e.g. 0)
  Mat<T> out(shape); out.setValue(value);

  // convert to counters
  size_t dh = npad.size() >= 1 ? npad[0] : 0;
  size_t di = npad.size() >= 2 ? npad[1] : 0;
  size_t dj = npad.size() >= 3 ? npad[2] : 0;

  // copy input matrix to inner region
  for ( size_t h = 0 ; h < src.shape(0) ; ++h )
    for ( size_t i = 0 ; i < src.shape(1) ; ++i )
      for ( size_t j = 0 ; j < src.shape(2) ; ++j )
        out(h+dh,i+di,j+dj) = src(h,i,j);

  return out;
}

// -------------------------------------------------------------------------------------------------

template MatI pad<int>    (const MatI &, const VecS &, int   );
template MatD pad<double> (const MatD &, const VecS &, double);

// =================================================================================================
// dilate image, not periodic
// =================================================================================================

MatI dilate ( const MatI &src, const MatI &kern, VecI &iterations )
{
  assert( (int)iterations.size() == src.max()+1 );
  assert( isodd(kern.shape()) );

  // initialize output
  MatI out = src;

  // get mid-point of the kernel
  size_t mh = (kern.shape(0)-1)/2;
  size_t mi = (kern.shape(1)-1)/2;
  size_t mj = (kern.shape(2)-1)/2;

  // check input
  assert( src.shape(0) >= mh );
  assert( src.shape(1) >= mi );
  assert( src.shape(2) >= mj );

  // find maximum number of iterations
  int max_iter = *std::max_element(iterations.begin(),iterations.end());

  // loop over iterations
  for ( int iter = 0 ; iter < max_iter ; iter++ )
  {
    // loop over all voxels
    for ( size_t h = mh ; h < src.shape(0)-mh ; ++h )
    {
      for ( size_t i = mi ; i < src.shape(1)-mi ; ++i )
      {
        for ( size_t j = mj ; j < src.shape(2)-mj ; ++j )
        {
          // label over the current voxel
          int ilab = out(h,i,j);
          // proceed:
          // - for non-zero label
          // - if the number of iterations for this label has not been exceeded
          if ( ilab > 0 and iterations[ilab] > iter )
          {
            // loop over the kernel
            for ( size_t dh = 0 ; dh <= kern.shape(0) ; ++dh )
            {
              for ( size_t di = 0 ; di <= kern.shape(1) ; ++di )
              {
                for ( size_t dj = 0 ; dj <= kern.shape(2) ; ++dj )
                {
                  // check to dilate for non-zero kernel value
                  if ( kern(dh,di,dj) and !(dh==mh and di==mi and dj==mj) )
                  {
                    size_t hh = h+dh-mh;
                    size_t ii = i+di-mi;
                    size_t jj = j+dj-mj;
                    if ( !out(hh,ii,jj) ) out(hh,ii,jj) = -1*ilab;
                  }
                }
              }
            }
          }
        }
      }
    }
    // accept all new labels (which were stored as negative)
    for ( auto &i : out )
      i = std::abs(i);
  }

  return out;
}

// =================================================================================================
// dilate image, periodic
// =================================================================================================

MatI dilate ( const MatI &src, const MatI &kern, VecI &iterations, bool periodic )
{
  // not periodic -> use non-periodic function (faster)
  if ( !periodic ) return dilate(src,kern,iterations);

  assert( (int)iterations.size() == src.max()+1 );
  assert( isodd(kern.shape()) );

  // initialize output
  MatI out = src;

  // get mid-point of the kernel
  size_t mh = (kern.shape(0)-1)/2;
  size_t mi = (kern.shape(1)-1)/2;
  size_t mj = (kern.shape(2)-1)/2;

  // check input
  assert( src.shape(0) >= mh );
  assert( src.shape(1) >= mi );
  assert( src.shape(2) >= mj );

  // find maximum number of iterations
  int max_iter = *std::max_element(iterations.begin(),iterations.end());

  // loop over iterations
  for ( int iter = 0 ; iter < max_iter ; iter++ )
  {
    // loop over all voxels
    for ( int h = 0 ; h < static_cast<int>(src.shape(0)) ; ++h )
    {
      for ( int i = 0 ; i < static_cast<int>(src.shape(1)) ; ++i )
      {
        for ( int j = 0 ; j < static_cast<int>(src.shape(2)) ; ++j )
        {
          // label over the current voxel
          int ilab = out(h,i,j);
          // proceed:
          // - for non-zero label
          // - if the number of iterations for this label has not been exceeded
          if ( ilab > 0 and iterations[ilab] > iter )
          {
            // loop over the kernel
            for ( size_t dh = 0 ; dh <= kern.shape(0) ; ++dh )
            {
              for ( size_t di = 0 ; di <= kern.shape(1) ; ++di )
              {
                for ( size_t dj = 0 ; dj <= kern.shape(2) ; ++dj )
                {
                  // check to dilate for non-zero kernel value
                  if ( kern(dh,di,dj) and !(dh==mh and di==mi and dj==mj) )
                  {
                    size_t hh = h+dh-mh;
                    size_t ii = i+di-mi;
                    size_t jj = j+dj-mj;
                    if ( !out(hh,ii,jj) ) out(hh,ii,jj) = -1*ilab;
                  }
                }
              }
            }
          }
        }
      }
    }
    // accept all new labels (which were stored as negative)
    for ( auto &i : out )
      i = std::abs(i);
  }

  return out;
}

// =================================================================================================
// dilate image: interfaces
// =================================================================================================

MatI dilate ( const MatI &src, int iterations, bool periodic )
{
  MatI kern = kernel(src.ndim());

  VecI iter(src.max()+1);

  for ( auto &i : iter )
    i = iterations;

  return dilate(src,kern,iter,periodic);
}

// -------------------------------------------------------------------------------------------------

MatI dilate ( const MatI &src, const MatI &kernel, int iterations, bool periodic )
{
  VecI iter(src.max()+1);

  for ( auto &i : iter )
    i = iterations;

  return dilate(src,kernel,iter,periodic);
}

// -------------------------------------------------------------------------------------------------

MatI dilate ( const MatI &src, const VecI &iterations, bool periodic )
{
  MatI kern = kernel(src.ndim());

  VecI iter = iterations;

  return dilate(src,kern,iter,periodic);
}

// =================================================================================================
// create a dummy image with circles at position "row","col" with radius "r"
// =================================================================================================

MatI dummy_circles(
  const VecS &shape, const VecI &row, const VecI &col, const VecI &r, bool periodic
)
{
  assert( row  .size() == col.size() );
  assert( row  .size() == r  .size() );
  assert( shape.size() == 2          );

  int I = static_cast<int>(shape[0]);
  int J = static_cast<int>(shape[1]);

  // allocate / zero-initialize image
  MatI out(shape); out.zeros();

  // fill the circles
  for ( size_t i = 0 ; i < row.size() ; ++i )
    for ( int di = -r[i] ; di <= r[i] ; ++di )
      for ( int dj = -r[i] ; dj <= r[i] ; ++dj )
        if ( periodic or ( row[i]+di >= 0 and row[i]+di < I and col[i]+dj > 0 and col[i]+dj < J ) )
          if ( (int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i] )
            out(row[i]+di,col[i]+dj) = 1;

  return out;
}

// =================================================================================================
// create a dummy image with a default number of circles
// at random positions and random radii
// =================================================================================================

MatI dummy_circles ( const VecS &shape, bool periodic )
{
  assert( shape.size() == 2 );

  std::srand(std::time(0));

  // set default: number of circles in both directions and (constant) radius
  int N = (int)(.05*(double)shape[0]);
  int M = (int)(.05*(double)shape[1]);
  int R = (int)(pow((.3*(double)(shape[0]*shape[1]))/(M_PI*(double)(N*M)),.5));

  VecI row(N*M),col(N*M),r(N*M);

  // define regular grid of circles
  for ( int i=0 ; i<N ; i++ ) {
    for ( int j=0 ; j<M ; j++ ) {
      row[i*M+j] = (int)((double)i*(double)shape[0]/(double)N);
      col[i*M+j] = (int)((double)j*(double)shape[1]/(double)M);
      r  [i*M+j] = R;
    }
  }

  // distance between circles
  int dN = (int)(.5*(double)shape[0]/(double)N);
  int dM = (int)(.5*(double)shape[1]/(double)M);

  // randomly perturb circles (move in any direction, enlarge/shrink)
  for ( int i=0 ; i<N*M ; i++ ) {
    row[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dN;
    col[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dM;
    r  [i]  = (int)(((double)(std::rand()%100)/100.*2.+.1)*(double)(r[i]));
  }

  // convert to image
  return dummy_circles(shape,row,col,r,periodic);
}

// =================================================================================================
// determine clusters in image
// =================================================================================================

namespace {
void _link ( VecI &linked, int a, int b )
{

  if ( a==b )
    return;

  int i,c;

  if ( a>b ) {
    c = a;
    a = b;
    b = c;
  }

  // both unlinked
  // -------------

  if ( linked[a]==a and linked[b]==b ) {
    linked[a] = b;
    linked[b] = a;
    return;
  }

  // a linked / b unlinked
  // ---------------------

  if ( linked[a]!=a and linked[b]==b ) {
    linked[b] = linked[a];
    linked[a] = b;
    return;
  }

  // a unlinked / b linked
  // ---------------------

  if ( linked[a]==a and linked[b]!=b ) {
    linked[a] = linked[b];
    linked[b] = a;
    return;
  }

  // both linked -> to each other
  // ----------------------------

  i = a;
  while ( 1 ) {
    if ( linked[i]==b ) { return; }
    i = linked[i];
    if ( i==a ) { break; }
  }

  // both linked -> to different groups
  // ----------------------------------

  c = linked[a];
  linked[a] = b;

  i = a;
  while ( 1 ) {
    i = linked[i];
    if ( linked[i]==b ) { break; }
  }
  linked[i] = c;

  return;
}
}

// -------------------------------------------------------------------------------------------------

std::tuple<MatI,MatI> clusters ( const MatI &f, const MatI &kern, int min_size, bool periodic )
{
  int h,i,j,di,dj,dh,lH,lI,lJ,uH,uI,uJ,ii,jj,ilab,nlab;
  // cluster links (e.g. 4 coupled to 2: lnk[4]=2)
  VecI lnk(f.size());
  // saved clusters: 1=saved, 0=not-saved
  VecI inc(f.size());

  MatI l(f.shape()); l.zeros();
  MatI c(f.shape()); c.zeros();

  // convert shape to counters
  int H  = static_cast<int>(f.shape(0));
  int I  = static_cast<int>(f.shape(1));
  int J  = static_cast<int>(f.shape(2));

  // get mid-point of the kernel
  int dH = ( static_cast<int>(kern.shape(0)) - 1 ) / 2;
  int dI = ( static_cast<int>(kern.shape(1)) - 1 ) / 2;
  int dJ = ( static_cast<int>(kern.shape(2)) - 1 ) / 2;

  // --------------
  // basic labeling
  // --------------

  // current label
  ilab = 0;

  // initialize cluster-links (only coupled to self)
  for ( i=0 ; i<(int)f.size() ; i++ )
    lnk[i] = i;
  // initialize: no saved clusters, except background (inc[0]=1)
  inc[0] = 1;
  for ( size_t i=1 ; i<f.size() ; i++ )
    inc[i] = 0;

  // periodic: lower/upper bound of the kernel always == (shape[i]-1)/2
  if ( periodic )
  {
    lH = -dH; uH = +dH;
    lI = -dI; uI = +dI;
    lJ = -dJ; uJ = +dJ;
  }

  // loop over voxels (in all directions)
  for ( h = 0 ; h < H ; ++h )
  {
    for ( i = 0 ; i < I ; ++i )
    {
      for ( j = 0 ; j < J ; ++j )
      {
        // only continue for non-zero voxels
        if ( f(h,i,j) )
        {
          // set lower/upper bound of the kernel near edges
          // -> avoids reading out-of-bounds
          if ( !periodic )
          {
            if ( h <    dH ) lH=0; else lH=-dH;
            if ( i <    dI ) lI=0; else lI=-dI;
            if ( j <    dJ ) lJ=0; else lJ=-dJ;
            if ( h >= H-dH ) uH=0; else uH=+dH;
            if ( i >= I-dI ) uJ=0; else uJ=+dI;
            if ( j >= J-dJ ) uI=0; else uI=+dJ;
          }

          // cluster not yet labeled: try to couple to labeled neighbors
          if ( l(h,i,j) == 0 )
          {
            for ( dh = lH ; dh <= uH ; ++dh )
            {
              for ( di = lI ; di <= uJ ; ++di )
              {
                for ( dj = lJ ; dj <= uI ; ++dj )
                {
                  if ( kern(dh+dH, di+dI, dj+dJ) )
                  {
                    if ( l(h+dh, i+di, j+dj) )
                    {
                      l(h,i,j) = l(h+dh,i+di,j+dj);
                      goto end;
                    }
                  }
                }
              }
            }
          }
          end: ;

          // cluster not yet labeled: create new label
          if ( l(h,i,j)==0 )
          {
            ilab++;
            l(h,i,j)  = ilab;
            inc[ilab] = 1;
          }

          // try to couple neighbors to current label
          // - not yet labeled -> label neighbor
          // - labeled         -> link labels
          for ( dh = lH ; dh <= uH ; ++dh )
          {
            for ( di = lI ; di <= uJ ; ++di )
            {
              for ( dj = lJ ; dj <= uI ; ++dj )
              {
                if ( kern(dh+dH, di+dI, dj+dJ) )
                {
                  if ( f(h+dh, i+di, j+dj) )
                  {
                    if ( l(h+dh, i+di, j+dj)==0 )
                     l(h+dh, i+di, j+dj) = l(h,i,j);
                    else
                     _link(lnk, l(h,i,j), l(h+dh,i+di,j+dj));
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  // ---------------------------------------
  // renumber labels: all links to one label
  // ---------------------------------------

  nlab = 0;

  // loop over assigned labels -> loop over all coupled links
  // lnk[i] will contain the new label number
  for ( i=0 ; i<=ilab ; i++ ) {
    if ( inc[i] ) {
      ii = i;
      while ( 1 ) {
        jj      = lnk[ii];
        lnk[ii] = nlab;
        inc[ii] = 0;
        if ( jj==i ) { break; }
        ii      = jj;
      }
      nlab++;
    }
  }
  // apply renumbering
  for ( size_t i=0 ; i<f.size() ; i++ )
    l[i] = lnk[l[i]];

  // --------------------------
  // threshold for cluster size
  // --------------------------

  if ( min_size>0 ) {

    // find the size of all clusters
    for ( i=0 ; i<nlab ; i++ ) {
      lnk[i] = 0;    // now: size of the cluster with the label
      inc[i] = 0;    // now: included label (true/false)
    }
    for ( size_t i=0 ; i<l.size() ; i++ ) {
      lnk[l[i]]++;
      inc[l[i]] = 1;
    }
    // remove clusters with too small size
    for ( i=0 ; i<nlab ; i++ )
      if ( lnk[i]<min_size )
        inc[i] = 0;

    // remove clusters with too small size
    for ( size_t i=0 ; i<l.size() ; i++ )
      if ( !inc[l[i]] )
        l[i] = 0;

    // new numbering for the included labels
    // inc[i] will contain the new cluster numbers,
    // j-1 will be the number of clusters
    j = 0;
    for ( i=0 ; i<nlab ; i++ ) {
      if ( inc[i] ) {
        inc[i] = j;
        j++;
      }
    }
    nlab = j;

    // renumber the labels
    for ( size_t i=0 ; i<l.size() ; i++ )
      l[i] = inc[l[i]];

  }

  // cluster centers: not periodic
  // -----------------------------

  if ( !periodic )
  {
    // number of labels
    nlab = l.max()+1;

    // allocate matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    MatI x({(size_t)nlab,4}); x.zeros();

    // loop over the image to update the position and size of each label
    for ( h=0 ; h<H ; h++ ) {
      for ( i=0 ; i<I ; i++ ) {
        for ( j=0 ; j<J ; j++ ) {
          ilab = l(h,i,j);
          if ( ilab>0 ) {
            x(ilab,0) += h;
            x(ilab,1) += i;
            x(ilab,2) += j;
            x(ilab,3) += 1;
          }
        }
      }
    }

    // fill the centers of gravity
    for ( ilab=1 ; ilab<nlab ; ilab++ ) {
      if ( x(ilab,3)>0 ) {

        h = (int)round( (float)x(ilab,0) / (float)x(ilab,3) );
        i = (int)round( (float)x(ilab,1) / (float)x(ilab,3) );
        j = (int)round( (float)x(ilab,2) / (float)x(ilab,3) );

        if ( h <  0 ) { h = 0; }
        if ( i <  0 ) { i = 0; }
        if ( j <  0 ) { j = 0; }
        if ( h >= H ) { h = H; }
        if ( i >= I ) { i = I; }
        if ( j >= J ) { j = J; }

        c(h,i,j) = ilab;
      }
    }
  }

  // cluster centers: periodic
  // -------------------------

  if ( periodic )
  {
    int h,i,j,dh,di,dj,ilab,nlab,nlab_;

    // "l_": labels for the non-periodic version image "f"
    MatI l_,c_;
    std::tie(l_,c_) = clusters(f,kern,min_size,false);

    // delete clusters that are also not present in "l"
    for ( size_t i=0 ; i<f.size() ; i++ )
      if ( f[i] )
        if ( !l[i] )
          l_[i] = 0;

    // number of labels
    nlab  = l .max()+1;
    nlab_ = l_.max()+1;

    // matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    MatI  x({(size_t)nlab ,4});  x.zeros();
    // if dx(ilab,ix)==1 than N[ix] is subtracted from the position in averaging
    MatI dx({(size_t)nlab_,3}); dx.zeros();

    // label dependency: which labels in "l_" correspond to which labels in "l"
    VecI lnk(nlab_);

    for ( size_t i=0 ; i<l.size() ; i++ )
      lnk[l_[i]] = l[i];

    // check periodicity: to which sides the clusters are connected
    // ------------------------------------------------------------

    // i-j plane
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=1 ; dh<=dH ; dh++ )
          for ( di=0 ; di<=dI ; di++ )
            for ( dj=0 ; dj<=dJ ; dj++ )
              if ( i+di<I and j+dj<J )
                if ( l_(H-1,i,j) )
                  if ( l(H-1,i,j)==l(H-1+dh,i+di,j+dj) )
                    dx(l_(H-1,i,j),0) = 1;

    // h-j plane
    for ( h=0 ; h<H ; h++ )
      for ( j=0 ; j<J ; j++ )
        for ( dh=0 ; dh<=dH ; dh++ )
          for ( di=1 ; di<=dI ; di++ )
            for ( dj=0 ; dj<=dJ ; dj++ )
              if ( h+dh<H and j+dj<J )
                if ( l_(h,I-1,j) )
                  if ( l(h,I-1,j)==l(h+dh,I-1+di,j+dj) )
                    dx(l_(h,I-1,j),1) = 1;

    // h-i plane
    for ( h=0 ; h<H ; h++ )
      for ( i=0 ; j<I ; j++ )
        for ( dh=0 ; dh<=dH ; dh++ )
          for ( di=0 ; di<=dI ; di++ )
            for ( dj=1 ; dj<=dJ ; dj++ )
              if ( h+dh<H and i+di<I )
                if ( l_(h,i,J-1) )
                  if ( l(h,i,J-1)==l(h+dh,i+di,J-1+dj) )
                    dx(l_(h,i,J-1),2) = 1;

    // calculate centers
    // -----------------

    // loop over the image to update the position and size of each label
    for ( h=0 ; h<H ; h++ ) {
      for ( i=0 ; i<I ; i++ ) {
        for ( j=0 ; j<J ; j++ ) {
          ilab = l_(h,i,j);
          if ( ilab>0 ) {
            if ( dx(ilab,0) ) dh = -H; else dh = 0;
            if ( dx(ilab,1) ) di = -I; else di = 0;
            if ( dx(ilab,2) ) dj = -J; else dj = 0;

            x(lnk[ilab],0) += h+dh;
            x(lnk[ilab],1) += i+di;
            x(lnk[ilab],2) += j+dj;
            x(lnk[ilab],3) += 1;
          }
        }
      }
    }

    // fill the centers of gravity
    for ( ilab=1 ; ilab<nlab ; ilab++ ) {
      if ( x(ilab,3)>0 ) {

        h = (int)round( (float)x(ilab,0) / (float)x(ilab,3) );
        i = (int)round( (float)x(ilab,1) / (float)x(ilab,3) );
        j = (int)round( (float)x(ilab,2) / (float)x(ilab,3) );

        while ( h <  0 ) { h += H; }
        while ( i <  0 ) { i += I; }
        while ( j <  0 ) { j += J; }
        while ( h >= H ) { h -= H; }
        while ( i >= I ) { i -= I; }
        while ( j >= J ) { j -= J; }

        c(h,i,j) = ilab;
      }
    }
  }

  return std::make_tuple(l,c);
}

// -------------------------------------------------------------------------------------------------

std::tuple<MatI,MatI> clusters ( const MatI &f, int min_size, bool periodic )
{
  MatI kern = kernel(f.ndim());

  return clusters(f,kern,min_size,periodic);
}

// -------------------------------------------------------------------------------------------------

std::tuple<MatI,MatI> clusters ( const MatI &f, bool periodic )
{
  MatI kern = kernel(f.ndim());

  return clusters(f,kern,0,periodic);
}

// =================================================================================================
// mean
// =================================================================================================

template <class T>
std::tuple<double,double> mean ( const Mat<T> &src )
{
  return std::make_tuple(src.mean(),static_cast<double>(src.size()));
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<double,double> mean ( const Mat<T> &src , const MatI &mask )
{
  T      out = static_cast<T>(0);
  size_t n   = 0;

  for ( size_t i=0 ; i<src.size() ; i++ ) {
    if ( !mask[i] ) {
      out += src[i];
      n++;
    }
  }

  return std::make_tuple(static_cast<double>(out)/static_cast<double>(n),static_cast<double>(n));
}

// -------------------------------------------------------------------------------------------------

template std::tuple<double,double> mean<int   >(const MatI &);
template std::tuple<double,double> mean<double>(const MatD &);
template std::tuple<double,double> mean<int   >(const MatI &, const MatI &);
template std::tuple<double,double> mean<double>(const MatD &, const MatI &);

// =================================================================================================
// comparison functions to allow int/double overload in "S2_core"
// =================================================================================================

namespace {
inline double compare ( int    f, int    g ) { return (f==g)? 1. : 0.; }
inline double compare ( int    f, double g ) { return (f   )? g  : 0.; }
inline double compare ( double f, int    g ) { return (g   )? f  : 0.; }
inline double compare ( double f, double g ) { return f*g            ; }
inline double compare ( int    f           ) { return (f   )? 1. : 0.; }
inline double compare ( double f           ) { return f              ; }
inline double unity   ( int    f           ) { return 1.             ; }
inline double unity   ( double f           ) { return 1.             ; }
}

// =================================================================================================
// core functions for "S2" and "W2"
// the function "func" will determine the normalization
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// periodic, no mask
// -------------------------------------------------------------------------------------------------

namespace {
template <class T, class U>
std::tuple<MatD,double> S2_core (
  const Mat<T> &f, const Mat<U> &g, const VecS &roi, double (*func)(U)
)
{
  assert( f.shape() == g.shape() );
  assert( isodd(roi) );

  MatD ret(roi); ret.zeros();

  double norm = 0.;

  int mh = ( ret.shape(0)-1 ) / 2;
  int mi = ( ret.shape(1)-1 ) / 2;
  int mj = ( ret.shape(2)-1 ) / 2;

  // compute correlation
  for ( int h = 0 ; h < static_cast<int>(f.shape(0)) ; ++h )
    for ( int i = 0 ; i < static_cast<int>(f.shape(1)) ; ++i )
      for ( int j = 0 ; j < static_cast<int>(f.shape(2)) ; ++j )
        if ( f(h,i,j) )
          for ( int dh = -mh ; dh <= mh ; dh++ )
            for ( int di = -mi ; di <= mi ; di++ )
              for ( int dj = -mj ; dj <= mj ; dj++ )
                ret(dh+mh,di+mi,dj+mj) += compare( f(h,i,j) , g(h+dh,i+di,j+dj) );

  // compute normalization
  for ( size_t i = 0 ; i < f.size() ; ++i ) norm += func(f[i]);

  return std::make_tuple(ret/norm,norm);
}
}

// -------------------------------------------------------------------------------------------------
// periodic, mask
// -------------------------------------------------------------------------------------------------

namespace {
template <class T, class U>
std::tuple<MatD,MatD> S2_core (
  const Mat<T> &f, const Mat<U> &g, const VecS &roi, const MatI &fmsk, const MatI &gmsk,
  double (*func)(U)
)
{
  assert( f.shape() == g   .shape() );
  assert( f.shape() == fmsk.shape() );
  assert( f.shape() == gmsk.shape() );
  assert( isodd(roi) );

  MatD out (roi); out .zeros();
  MatD norm(roi); norm.zeros();

  int mh = ( out.shape(0)-1 ) / 2;
  int mi = ( out.shape(1)-1 ) / 2;
  int mj = ( out.shape(2)-1 ) / 2;

  // compute correlation
  for ( int h = 0 ; h < static_cast<int>(f.shape(0)) ; ++h )
    for ( int i = 0 ; i < static_cast<int>(f.shape(1)) ; ++i )
      for ( int j = 0 ; j < static_cast<int>(f.shape(2)) ; ++j )
        if ( f(h,i,j) and !fmsk(h,i,j) )
          for ( int dh = -mh ; dh <= mh ; dh++ )
            for ( int di = -mi ; di <= mi ; di++ )
              for ( int dj = -mj ; dj <= mj ; dj++ )
                if ( !gmsk(h+dh,i+di,j+dj) )
                  out(dh+mh,di+mi,dj+mj) += compare( f(h,i,j) , g(h+dh,i+di,j+dj) );

  // compute normalization
  for ( int h = 0 ; h < static_cast<int>(f.shape(0)) ; ++h )
    for ( int i = 0 ; i < static_cast<int>(f.shape(1)) ; ++i )
      for ( int j = 0 ; j < static_cast<int>(f.shape(2)) ; ++j )
        if ( func(f(h,i,j)) and !fmsk(h,i,j) )
          for ( int dh = -mh ; dh <= mh ; dh++ )
            for ( int di = -mi ; di <= mi ; di++ )
              for ( int dj = -mj ; dj <= mj ; dj++ )
                if ( !gmsk(h+dh,i+di,j+dj) )
                  norm(dh+mh,di+mi,dj+mj) += func(f(h,i,j));

  return std::make_tuple(out/norm,norm);
}
}

// -------------------------------------------------------------------------------------------------
// not periodic, no mask
// -------------------------------------------------------------------------------------------------

namespace {
template <class T, class U>
std::tuple<MatD,double> S2_core (
  const Mat<T> &f, const Mat<U> &g, const VecS &roi, bool periodic, double (*func)(U)
)
{
  if ( periodic ) return S2_core(f,g,roi,func);

  assert( f.shape() == g.shape() );
  assert( isodd(roi) );

  MatD ret(roi); ret.zeros();

  double norm = 0.;

  size_t mh = ( ret.shape(0)-1 ) / 2;
  size_t mi = ( ret.shape(1)-1 ) / 2;
  size_t mj = ( ret.shape(2)-1 ) / 2;

  // compute correlation
  for ( size_t h = mh ; h < f.shape(0) - mh ; ++h )
    for ( size_t i = mi ; i < f.shape(1) - mi ; ++i )
      for ( size_t j = mj ; j < f.shape(2) - mj ; ++j )
        if ( f(h,i,j) )
          for ( size_t dh = 0 ; dh <= ret.shape(0) ; dh++ )
            for ( size_t di = 0 ; di <= ret.shape(1) ; di++ )
              for ( size_t dj = 0 ; dj <= ret.shape(2) ; dj++ )
                ret(dh,di,dj) += compare( f(h,i,j) , g(h+dh-mh,i+di-mi,j+dj-mj) );

  // compute normalization
  for ( size_t i = 0 ; i < f.size() ; ++i ) norm += func(f[i]);

  return std::make_tuple(ret/norm,norm);
}
}

// -------------------------------------------------------------------------------------------------
// not periodic, mask
// -------------------------------------------------------------------------------------------------

namespace {
template <class T, class U>
std::tuple<MatD,MatD> S2_core (
  const Mat<T> &f, const Mat<U> &g, const VecS &roi, const MatI &fmsk, const MatI &gmsk,
  bool periodic, double (*func)(U)
)
{
  if ( periodic ) return S2_core(f,g,roi,fmsk,gmsk,func);

  assert( f.shape() == g   .shape() );
  assert( f.shape() == fmsk.shape() );
  assert( f.shape() == gmsk.shape() );
  assert( isodd(roi) );

  MatD ret (roi); ret .zeros();
  MatD norm(roi); norm.zeros();

  size_t mh = ( ret.shape(0)-1 ) / 2;
  size_t mi = ( ret.shape(1)-1 ) / 2;
  size_t mj = ( ret.shape(2)-1 ) / 2;

  // compute correlation
  for ( size_t h = mh ; h < f.shape(0) - mh ; ++h )
    for ( size_t i = mi ; i < f.shape(1) - mi ; ++i )
      for ( size_t j = mj ; j < f.shape(2) - mj ; ++j )
        if ( f(h,i,j) and !fmsk(h,i,j) )
          for ( size_t dh = 0 ; dh <= ret.shape(0) ; dh++ )
            for ( size_t di = 0 ; di <= ret.shape(1) ; di++ )
              for ( size_t dj = 0 ; dj <= ret.shape(2) ; dj++ )
                if ( !gmsk(h+dh-mh,i+di-mi,j+dj-mj) )
                  ret(dh,di,dj) += compare( f(h,i,j) , g(h+dh-mh,i+di-mi,j+dj-mj) );

  // compute normalization
  for ( size_t h = mh ; h < f.shape(0) - mh ; ++h )
    for ( size_t i = mi ; i < f.shape(1) - mi ; ++i )
      for ( size_t j = mj ; j < f.shape(2) - mj ; ++j )
        if ( func(f(h,i,j)) and !fmsk(h,i,j) )
          for ( size_t dh = 0 ; dh <= ret.shape(0) ; dh++ )
            for ( size_t di = 0 ; di <= ret.shape(1) ; di++ )
              for ( size_t dj = 0 ; dj <= ret.shape(2) ; dj++ )
                if ( !gmsk(h+dh-mh,i+di-mi,j+dj-mj) )
                  norm(dh,di,dj) += func(f(h,i,j));

  // compute normalization
  for ( size_t i = 0 ; i < f.size() ; ++i ) norm += func(f[i]);

  return std::make_tuple(ret/norm,norm);
}
}

// -------------------------------------------------------------------------------------------------
// interface: zeropad + mask
// -------------------------------------------------------------------------------------------------

namespace {
template <class T, class U>
std::tuple<MatD,MatD> S2_core (
  const Mat<T> &f, const Mat<U> &g, const VecS &roi, const MatI &fmsk, const MatI &gmsk,
  bool zeropad, bool periodic, double (*func)(U)
)
{
  if ( zeropad )
  {
    assert( isodd(roi) );

    VecS mid(roi.size());

    for ( size_t i = 0 ; i < roi.size() ; ++i )
      mid[i] = ( roi[i] - 1 ) / 2;

    return S2_core(pad(f,mid),pad(g,mid),roi,pad(fmsk,mid,1),pad(gmsk,mid,1),func);
  }
  else
  {
    return S2_core(f,g,roi,fmsk,gmsk,periodic,func);
  }
}
}

// =================================================================================================
// 2-point probability/cluster (binary/int) / correlation (double)
// =================================================================================================

template <class T>
std::tuple<MatD,double> S2 ( const Mat<T> &f, const Mat<T> &g, const VecS &roi )
{
  return S2_core(f,g,roi,&unity); // (norm == f.size())
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, const MatI &fmsk, const MatI &gmsk,
  bool zeropad, bool periodic
)
{
  return S2_core(f,g,roi,fmsk,gmsk,zeropad,periodic,&unity);
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, const MatI &fmsk, bool zeropad, bool periodic
)
{
  MatI gmsk(g.shape()); gmsk.zeros();
  return S2_core(f,g,roi,fmsk,gmsk,zeropad,periodic,&unity);
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> S2 (
  const Mat<T> &f, const Mat<T> &g, const VecS &roi, bool zeropad, bool periodic
)
{
  MatI fmsk(f.shape()); fmsk.zeros();
  MatI gmsk(g.shape()); gmsk.zeros();
  return S2_core(f,g,roi,fmsk,gmsk,zeropad,periodic,&unity);
}

// -------------------------------------------------------------------------------------------------

template std::tuple<MatD,double> S2<int> (
  const MatI &, const MatI &, const VecS&
);

template std::tuple<MatD,MatD> S2<int> (
  const MatI &, const MatI &, const VecS&, const MatI &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> S2<int> (
  const MatI &, const MatI &, const VecS&, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> S2<int> (
  const MatI &, const MatI &, const VecS&, bool, bool
);

template std::tuple<MatD,double> S2<double> (
  const MatD &, const MatD &, const VecS&
);

template std::tuple<MatD,MatD> S2<double> (
  const MatD &, const MatD &, const VecS&, const MatI &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> S2<double> (
  const MatD &, const MatD &, const VecS&, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> S2<double> (
  const MatD &, const MatD &, const VecS&, bool, bool
);

// =================================================================================================
// conditional 2-point probability
// =================================================================================================

template <class T, class U> std::tuple<MatD,double> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi
)
{
  return S2_core(W,I,roi,&compare);
}

// -------------------------------------------------------------------------------------------------

template <class T, class U> std::tuple<MatD,MatD> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi, const MatI &mask, bool zeropad, bool periodic
)
{
  MatI fmsk(I.shape()); fmsk.zeros();

  return S2_core(W,I,roi,fmsk,mask,zeropad,periodic,&compare);
}

// -------------------------------------------------------------------------------------------------

template <class T, class U> std::tuple<MatD,MatD> W2 (
  const Mat<T> &W, const Mat<U> &I, const VecS &roi, bool zeropad, bool periodic
)
{
  MatI fmsk(I.shape()); fmsk.zeros();
  MatI mask(I.shape()); mask.zeros();

  return S2_core(W,I,roi,fmsk,mask,zeropad,periodic,&compare);
}

// -------------------------------------------------------------------------------------------------

template std::tuple<MatD,double> W2<int,int> (
  const MatI &, const MatI &, const VecS &
);

template std::tuple<MatD,double> W2<int,double> (
  const MatI &, const MatD &, const VecS &
);

template std::tuple<MatD,double> W2<double,int> (
  const MatD &, const MatI &, const VecS &
);

template std::tuple<MatD,double> W2<double,double> (
  const MatD &, const MatD &, const VecS &
);

template std::tuple<MatD,MatD> W2<int,int> (
  const MatI &, const MatI &, const VecS &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> W2<int,double> (
  const MatI &, const MatD &, const VecS &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> W2<double,int> (
  const MatD &, const MatI &, const VecS &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> W2<double,double> (
  const MatD &, const MatD &, const VecS &, const MatI &, bool, bool
);

template std::tuple<MatD,MatD> W2<int,int> (
  const MatI &, const MatI &, const VecS &, bool, bool
);

template std::tuple<MatD,MatD> W2<int,double> (
  const MatI &, const MatD &, const VecS &, bool, bool
);

template std::tuple<MatD,MatD> W2<double,int> (
  const MatD &, const MatI &, const VecS &, bool, bool
);

template std::tuple<MatD,MatD> W2<double,double> (
  const MatD &, const MatD &, const VecS &, bool, bool
);

// =================================================================================================
// weighted 2-point correlation -> collapse to center
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// periodic
// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> W2c (
  const MatI &clus, const MatI &cntr, const Mat<T> &src, const VecS &roi, const MatI &mask, str mode
)
{
  assert( src.shape() == clus.shape() );
  assert( src.shape() == cntr.shape() );
  assert( src.shape() == mask.shape() );
  assert( isodd(roi) );

  MatD out (roi); out .zeros();
  MatD norm(roi); norm.zeros();

  VecI begin(3),end(3);
  for ( size_t i=0 ; i<3 ; i++ ) { begin[i] = 0; end[i] = 0; }

  int mh = ( out.shape(0)-1 ) / 2;
  int mi = ( out.shape(1)-1 ) / 2;
  int mj = ( out.shape(2)-1 ) / 2;

  // define the "end"-point of each voxel path
  MatI pnt = stamp_points(roi);

  // loop over ROI
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i = 0 ; i < pnt.shape(1) ; i++ ) end[i] = pnt(ipnt,i);

    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over image
    for ( int h = 0 ; h < static_cast<int>(src.shape(0)) ; ++h )
    {
      for ( int i = 0 ; i < static_cast<int>(src.shape(1)) ; ++i )
      {
        for ( int j = 0 ; j < static_cast<int>(src.shape(2)) ; ++j )
        {
          if ( cntr(h,i,j) )
          {
            // store the label
            int label = cntr(h,i,j);
            // only proceed when the center is inside the cluster
            if ( clus(h,i,j) == label )
            {
              // initialize
              int jp = -1;
              // loop over the voxel-path
              for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
              {
                int dh = pix(ip,0);
                int di = pix(ip,1);
                int dj = pix(ip,2);
                // loop through the voxel-path until the end of a cluster
                if ( clus(h+dh,i+di,j+dj) != label and jp<0 ) jp = 0;
                // store: loop from the beginning of the path and store there
                if ( jp >= 0 )
                {
                  if ( !mask(h+dh, i+di, j+dj) )
                  {
                    norm(mh+pix(jp,0),mi+pix(jp,1),mj+pix(jp,2)) += 1.;
                    out (mh+pix(jp,0),mi+pix(jp,1),mj+pix(jp,2)) += compare(src(h+dh,i+di,j+dj));
                  }
                }
                // update counter
                jp++;
              }
            }
          }
        }
      }
    }
  }

  // apply normalization
  for ( size_t i=0 ; i<out.size() ; i++ )
    out[i] /= std::max(norm[i],1.);

  return std::make_tuple(out,norm);
}

// -------------------------------------------------------------------------------------------------
// not periodic
// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> W2c (
  const MatI &clus, const MatI &cntr, const Mat<T> &src, const VecS &roi, const MatI &mask,
  bool periodic, str mode
)
{
  if ( periodic ) return W2c(clus,cntr,src,roi,mask,mode);

  assert( src.shape() == clus.shape() );
  assert( src.shape() == cntr.shape() );
  assert( src.shape() == mask.shape() );
  assert( isodd(roi) );

  MatD out (roi); out .zeros();
  MatD norm(roi); norm.zeros();

  VecI begin(3),end(3);
  for ( size_t i=0 ; i<3 ; i++ ) { begin[i] = 0; end[i] = 0; }

  int mh = ( out.shape(0)-1 ) / 2;
  int mi = ( out.shape(1)-1 ) / 2;
  int mj = ( out.shape(2)-1 ) / 2;

  // define the "end"-point of each voxel path
  MatI pnt = stamp_points(roi);

  // loop over ROI
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i = 0 ; i < pnt.shape(1) ; i++ ) end[i] = pnt(ipnt,i);

    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over image
    for ( size_t h = mh ; h < src.shape(0) - mh ; ++h )
    {
      for ( size_t i = mi ; i < src.shape(1) - mi ; ++i )
      {
        for ( size_t j = mj ; j < src.shape(2) - mj ; ++j )
        {
          if ( cntr(h,i,j) )
          {
            // store the label
            int label = cntr(h,i,j);
            // only proceed when the center is inside the cluster
            if ( clus(h,i,j) == label )
            {
              // initialize
              int jp = -1;
              // loop over the voxel-path
              for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
              {
                int dh = pix(ip,0);
                int di = pix(ip,1);
                int dj = pix(ip,2);
                // loop through the voxel-path until the end of a cluster
                if ( clus(h+dh,i+di,j+dj) != label and jp<0 ) jp = 0;
                // store: loop from the beginning of the path and store there
                if ( jp >= 0 )
                {
                  if ( !mask(h+dh, i+di, j+dj) )
                  {
                    norm(mh+pix(jp,0),mi+pix(jp,1),mj+pix(jp,2)) += 1.;
                    out (mh+pix(jp,0),mi+pix(jp,1),mj+pix(jp,2)) += compare(src(h+dh,i+di,j+dj));
                  }
                }
                // update counter
                jp++;
              }
            }
          }
        }
      }
    }
  }

  // apply normalization
  for ( size_t i=0 ; i<out.size() ; i++ )
    out[i] /= std::max(norm[i],1.);

  return std::make_tuple(out,norm);
}

// -------------------------------------------------------------------------------------------------
// interfaces
// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> W2c (
  const MatI &clus, const MatI &cntr, const Mat<T> &I, const VecS &roi, bool periodic, str mode
)
{
  MatI mask(I.shape()); mask.zeros();
  return W2c(clus,cntr,I,roi,mask,periodic,mode);
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> W2c (
  const MatI &W, const Mat<T> &I, const VecS &roi, const MatI &mask, bool periodic, str mode
)
{
  MatI clus,cntr;

  std::tie(clus,cntr) = clusters(W,periodic);

  return W2c(clus,cntr,I,roi,mask,periodic,mode);
}

// -------------------------------------------------------------------------------------------------

template <class T>
std::tuple<MatD,MatD> W2c (
  const MatI &W, const Mat<T> &I, const VecS &roi, bool periodic, str mode
)
{
  MatI clus,cntr;
  MatI mask(I.shape()); mask.zeros();

  std::tie(clus,cntr) = clusters(W,periodic);

  return W2c(clus,cntr,I,roi,mask,periodic,mode);
}

// -------------------------------------------------------------------------------------------------

template std::tuple<MatD,MatD> W2c<int> (
  const MatI &, const MatI &, const MatI &, const VecS &, const MatI &, str
);

template std::tuple<MatD,MatD> W2c<int> (
  const MatI &, const MatI &, const MatI &, const VecS &, const MatI &, bool, str
);

template std::tuple<MatD,MatD> W2c<int> (
  const MatI &, const MatI &, const MatI &, const VecS &, bool, str
);

template std::tuple<MatD,MatD> W2c<int> (
  const MatI &, const MatI &, const VecS &, const MatI &, bool, str
);

template std::tuple<MatD,MatD> W2c<int> (
  const MatI &, const MatI &, const VecS &, bool, str
);

// =================================================================================================
// lineal path function
// =================================================================================================

// -------------------------------------------------------------------------------------------------
// periodic
// -------------------------------------------------------------------------------------------------

std::tuple<MatD,MatD> L ( const MatI &src, const VecS &roi, str mode )
{
  assert( isodd(roi) );

  MatD out (roi); out .zeros();
  MatD norm(roi); norm.zeros();

  VecI begin(3),end(3);
  for ( size_t i=0 ; i<3 ; i++ ) { begin[i] = 0; end[i] = 0; }

  int mh = ( out.shape(0)-1 ) / 2;
  int mi = ( out.shape(1)-1 ) / 2;
  int mj = ( out.shape(2)-1 ) / 2;

  // define the "end"-point of each voxel path
  MatI pnt = stamp_points(roi);

  // correlation
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i=0 ; i<pnt.shape(1) ; i++ )
      end[i] = pnt(ipnt,i);
    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over image
    for ( int h = 0 ; h < static_cast<int>(src.shape(0)) ; ++h )
    {
      for ( int i = 0 ; i < static_cast<int>(src.shape(1)) ; ++i )
      {
        for ( int j = 0 ; j < static_cast<int>(src.shape(2)) ; ++j )
        {

          for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
          {
            int dh = pix(ip,0);
            int di = pix(ip,1);
            int dj = pix(ip,2);
            // new voxel in path not "true" in f: terminate this path
            if ( !src(h+dh,i+di,j+dj) )
              break;
            // positive correlation: add to result
            out(dh+mh,di+mi,dj+mj) += 1.;
          }
        }
      }
    }
  }

  // normalization
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i=0 ; i<pnt.shape(1) ; ++i )
      end[i] = pnt(ipnt,i);
    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over voxel-path
    double V = static_cast<double>(src.shape(0)*src.shape(1)*src.shape(2));

    for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
      norm(pix(ip,0)+mh,pix(ip,1)+mi,pix(ip,2)+mj) += V;
  }

  // apply normalization
  for ( size_t i = 0 ; i < out.size() ; ++i )
    out[i] /= std::max(norm[i],1.);

  return std::make_tuple(out,norm);
}

// -------------------------------------------------------------------------------------------------
// not periodic
// -------------------------------------------------------------------------------------------------

std::tuple<MatD,MatD> L ( const MatI &src, const VecS &roi, bool periodic, str mode )
{
  if ( periodic ) return L(src,roi,mode);

  assert( isodd(roi) );

  MatD out (roi); out .zeros();
  MatD norm(roi); norm.zeros();

  VecI begin(3),end(3);
  for ( size_t i=0 ; i<3 ; i++ ) { begin[i] = 0; end[i] = 0; }

  size_t mh = ( out.shape(0)-1 ) / 2;
  size_t mi = ( out.shape(1)-1 ) / 2;
  size_t mj = ( out.shape(2)-1 ) / 2;

  // define the "end"-point of each voxel path
  MatI pnt = stamp_points(roi);

  // correlation
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i=0 ; i<pnt.shape(1) ; i++ )
      end[i] = pnt(ipnt,i);
    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over image
    for ( size_t h = mh ; h < src.shape(0) - mh ; ++h )
    {
      for ( size_t i = mi ; i < src.shape(1) - mi ; ++i )
      {
        for ( size_t j = mj ; j < src.shape(2) - mj ; ++j )
        {

          for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
          {
            int dh = pix(ip,0);
            int di = pix(ip,1);
            int dj = pix(ip,2);
            // new voxel in path not "true" in f: terminate this path
            if ( !src(h+dh,i+di,j+dj) )
              break;
            // positive correlation: add to result
            out(dh+mh,di+mi,dj+mj) += 1.;
          }
        }
      }
    }
  }

  // normalization
  for ( size_t ipnt = 0 ; ipnt < pnt.shape(0) ; ++ipnt )
  {
    // copy end-point
    for ( size_t i=0 ; i<pnt.shape(1) ; ++i )
      end[i] = pnt(ipnt,i);
    // voxel-path
    MatI pix = path(begin,end,mode);

    // loop over voxel-path
    double V = static_cast<double>((src.shape(0)-mh)*(src.shape(1)-mi)*(src.shape(2)-mj));

    for ( size_t ip = 0 ; ip < pix.shape(0) ; ++ip )
      norm(pix(ip,0)+mh,pix(ip,1)+mi,pix(ip,2)+mj) += V;
  }

  // apply normalization
  for ( size_t i = 0 ; i < out.size() ; ++i )
    out[i] /= std::max(norm[i],1.);

  return std::make_tuple(out,norm);
}

// =================================================================================================

} // namespace Image
} // namespace GooseEYE


#include "image.h"

namespace Image {

template class Matrix<int>;
template class Matrix<double>;

// =============================================================================
// voxel path
// =============================================================================

Matrix<int> path ( std::vector<int> &xa, std::vector<int> &xb, std::string mode )
{
  int ndim = xa.size();

  if ( xb.size()!=ndim )
    throw std::length_error("'xa' and 'xb' must have the same dimension");
  if ( ndim<1 || ndim>3 )
    throw std::length_error("Only allowed in 1, 2, or 3 dimensions");

  std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

  std::vector<int> ret;

  // see http://www.luberth.com/plotter/line3d.c.txt.html
  if ( mode=="bresenham" )
  {
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
        Matrix<int> retmat({(size_t)nnz,(size_t)ndim},&ret[0]);
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

  if ( mode=="actual" || mode=="full" )
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
          if ( mode=="full" ) {
            for ( i=0 ; i<ndim ; i++ )
              ret.push_back(cindex[i]);
            nnz++;
          }
        }
      }
      // store only the next voxel ("actual")
      if ( mode=="actual" ) {
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

    Matrix<int> retmat({(size_t)nnz,(size_t)ndim},&ret[0]);
    return retmat;
  }

  throw std::out_of_range("Unknown 'mode'");
}

// =============================================================================
// create end-points of ROI-stamp used in path-based correlations
// =============================================================================

Matrix<int> stamp_points ( std::vector<size_t> &N )
{
  if ( N.size()<1 || N.size()>3 )
    throw std::length_error("'shape' must be 1-, 2-, or 3-D");

  for ( auto i : N )
    if ( i%2==0 )
      throw std::length_error("'shape' must be odd shaped");

  int n,i,j;
  int idx = 0;
  int nd  = N.size();

  // number of points
  if ( nd==1 ) n =              2;
  if ( nd==2 ) n =              2*N[0]+2*POS(N[1]-2);
  if ( nd==3 ) n = POS(N[2]-2)*(2*N[0]+2*POS(N[1]-2))+2*N[0]*N[1];

  // allocate
  Matrix<int> ret({(size_t)n,(size_t)nd});
  std::vector<size_t> mid = midpoint(N);

  // 1-D
  // ---

  if ( nd==1 )
  {
    ret(idx,0) = -mid[0]; idx++;
    ret(idx,0) = +mid[0]; idx++;
    return ret;
  }

  // 2-D
  // ---

  if ( nd==2 )
  {
    for ( i=0 ; i<N[0]   ; i++ ) {
      ret(idx,0) = i-mid[0]; ret(idx,1) =  -mid[1]; idx++;
      ret(idx,0) = i-mid[0]; ret(idx,1) =  +mid[1]; idx++;
    }
    for ( i=1 ; i<N[1]-1 ; i++ ) {
      ret(idx,0) =  -mid[0]; ret(idx,1) = i-mid[1]; idx++;
      ret(idx,0) =  +mid[0]; ret(idx,1) = i-mid[1]; idx++;
    }
    return ret;
  }

  // 3-D
  // ---

  for ( j=1 ; j<N[2]-1 ; j++ ) {
    for ( i=0 ; i<N[0]   ; i++ ) {
      ret(idx,0) = i-mid[0]; ret(idx,1) =  -mid[1]; ret(idx,2) = j-mid[2]; idx++;
      ret(idx,0) = i-mid[0]; ret(idx,1) =  +mid[1]; ret(idx,2) = j-mid[2]; idx++;
    }
    for ( i=1 ; i<N[1]-1 ; i++ ) {
      ret(idx,0) =  -mid[0]; ret(idx,1) = i-mid[1]; ret(idx,2) = j-mid[2]; idx++;
      ret(idx,0) =  +mid[0]; ret(idx,1) = i-mid[1]; ret(idx,2) = j-mid[2]; idx++;
    }
  }
  for ( i=0 ; i<N[0]   ; i++ ) {
    for ( j=0 ; j<N[1]   ; j++ ) {
      ret(idx,0) = i-mid[0]; ret(idx,1) = j-mid[1]; ret(idx,2) =  -mid[2]; idx++;
      ret(idx,0) = i-mid[0]; ret(idx,1) = j-mid[1]; ret(idx,2) =  +mid[2]; idx++;
    }
  }
  return ret;
}

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

  for ( auto i : shape )
    if ( !(i%2) )
      throw std::domain_error("Only allowed for odd-shaped matrices");

  for ( int i=0 ; i<shape.size() ; i++ )
    ret[i] = (shape[i]-1)/2;

  return ret;

}

// =============================================================================
// pad "pad_shape" entries on each side of "src" with a certain "value"
// =============================================================================

Matrix<int> pad ( Matrix<int> &src , std::vector<size_t> &pad_shape ,
  int value )
{
  std::vector<size_t> shape = src.shape();

  for ( int i=0 ; i<pad_shape.size() ; i++ )
    shape[i] += 2*pad_shape[i];

  Matrix<int> ret(shape);

  int h,i,j,H,I,J,dH,dI,dJ;

  std::tie( H, I, J) = unpack3d(src.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(pad_shape  ,0);

  if ( value!=0 )
    for ( auto &i : ret )
      i = value;

  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        ret(h+dH,i+dI,j+dJ) = src(h,i,j);

  return ret;
}

// =============================================================================

Matrix<double> pad ( Matrix<double> &src , std::vector<size_t> &pad_shape ,
  double value )
{
  std::vector<size_t> shape = src.shape();

  for ( int i=0 ; i<pad_shape.size() ; i++ )
    shape[i] += 2*pad_shape[i];

  Matrix<double> ret(shape);

  int h,i,j,H,I,J,dH,dI,dJ;

  std::tie( H, I, J) = unpack3d(src.shape(),1);
  std::tie(dH,dI,dJ) = unpack3d(pad_shape  ,0);

  if ( value!=0. )
    for ( auto &i : ret )
      i = value;

  for ( h=0 ; h<H ; h++ )
    for ( i=0 ; i<I ; i++ )
      for ( j=0 ; j<J ; j++ )
        ret(h+dH,i+dI,j+dJ) = src(h,i,j);

  return ret;
}

// =============================================================================
// create a dummy image with circles at position "row","col" with radius "r"
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

// =============================================================================
// kernels
// =============================================================================

Matrix<int> kernel ( int ndim , std::string mode )
{
  std::transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

  if ( mode=="default" )
  {
    std::vector<size_t> shape(ndim);
    for ( int i=0 ; i<ndim ; i++ )
      shape[i] = 3;

    Matrix<int> kern(shape);

    if      ( ndim==1 ) {
      kern(0) = 1; kern(1) = 1; kern(2) = 1;
    }
    else if ( ndim==2 ) {
      kern(1,0) = 1; kern(1,1) = 1; kern(1,2) = 1;
      kern(0,1) = 1; kern(2,1) = 1;
    }
    else if ( ndim==3 ) {
      kern(1,1,0) = 1; kern(1,1,1) = 1; kern(1,1,2) = 1;
      kern(1,0,1) = 1; kern(1,2,1) = 1;
      kern(0,1,1) = 1; kern(2,1,1) = 1;
    }
    else
      throw std::length_error("'shape' must be 1-, 2-, or 3-D");

    return kern;
  }

  throw std::length_error("Unknown mode");
}

// =============================================================================
// determine clusters from image
// =============================================================================

void _clusters_link ( std::vector<int> &linked, int a, int b )
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

  if ( linked[a]==a && linked[b]==b ) {
    linked[a] = b;
    linked[b] = a;
    return;
  }

  // a linked / b unlinked
  // ---------------------

  if ( linked[a]!=a && linked[b]==b ) {
    linked[b] = linked[a];
    linked[a] = b;
    return;
  }

  // a unlinked / b linked
  // ---------------------

  if ( linked[a]==a && linked[b]!=b ) {
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

// -----------------------------------------------------------------------------

std::tuple<Matrix<int>,Matrix<int>> clusters (
  Matrix<int> &f, Matrix<int> &kern, int min_size, bool periodic )
{
  int  h,i,j,di,dj,dh,ii,jj,nlab;
  int  lb[3],ub[3];               // upper/lower bound of kernel ==(shape[i]-1)/2 except for edges
  int  ilab = 0;                  // current label
  std::vector<int> lnk(f.size()); // cluster links (e.g. 4 coupled to 2: lnk[4]=2)
  std::vector<int> inc(f.size()); // saved clusters: 1=saved, 0=not-saved

  Matrix<int> l(f.shape());
  Matrix<int> c(f.shape());

  std::vector<size_t> mid = midpoint(kern.shape(3));
  std::vector<size_t> N   = f.shape(3);

  // --------------
  // basic labeling
  // --------------

  // initialize cluster-links (only coupled to self)
  for ( i=0 ; i<f.size() ; i++ )
    lnk[i] = i;
  // initialize: no saved clusters, except background (inc[0]=1)
  inc[0] = 1;
  for ( i=1 ; i<f.size() ; i++ )
    inc[i] = 0;

  // periodic: lower/upper bound of the kernel always == (shape[i]-1)/2
  if ( periodic ) {
    for ( i=0 ; i<3 ; i++ ) {
      lb[i] = -mid[i];
      ub[i] = +mid[i];
    }
  }

  // loop over voxels (in all directions)
  for ( h=0 ; h<N[0] ; h++ ) {
    for ( i=0 ; i<N[1] ; i++ ) {
      for ( j=0 ; j<N[2] ; j++ ) {

        // only continue for non-zero voxels
        if ( f(h,i,j) )
        {

          // set lower/upper bound of the kernel near edges
          // -> avoids reading out-of-bounds
          if ( !periodic )
          {
            if ( h <       mid[0] ) lb[0]=0; else lb[0]=-mid[0];
            if ( i <       mid[1] ) lb[1]=0; else lb[1]=-mid[1];
            if ( j <       mid[2] ) lb[2]=0; else lb[2]=-mid[2];
            if ( h >= N[0]-mid[0] ) ub[0]=0; else ub[0]=+mid[0];
            if ( i >= N[1]-mid[1] ) ub[1]=0; else ub[1]=+mid[1];
            if ( j >= N[2]-mid[2] ) ub[2]=0; else ub[2]=+mid[2];
          }

          // cluster not yet labeled: try to couple to labeled neighbors
          if ( l(h,i,j)==0 )
          {
            for ( dh=lb[0] ; dh<=ub[0] ; dh++ ) {
              for ( di=lb[1] ; di<=ub[1] ; di++ ) {
                for ( dj=lb[2] ; dj<=ub[2] ; dj++ ) {
                  if ( kern(dh+mid[0],di+mid[1],dj+mid[2]) ) {
                    if ( l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2])) ) {
                      l(h,i,j) = l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2]));
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
            l(h,i,j) = ilab;
            inc[ilab] = 1;
          }

          // try to couple neighbors to current label
          // - not yet labeled -> label neighbor
          // - labeled         -> link labels
          for ( dh=lb[0] ; dh<=ub[0] ; dh++ ) {
            for ( di=lb[1] ; di<=ub[1] ; di++ ) {
              for ( dj=lb[2] ; dj<=ub[2] ; dj++ ) {
                if ( kern(dh+mid[0],di+mid[1],dj+mid[2]) ) {
                  if ( f(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2])) )
                  {
                    if ( l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2]))==0 )
                      l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2])) = l(h,i,j);
                    else
                      _clusters_link(lnk,l(h,i,j),l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2])));
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
  for ( i=0 ; i<f.size() ; i++ )
    l[i] = lnk[l[i]];

  // --------------------------
  // threshold for cluster size
  // --------------------------

  if ( min_size>0 )
  {

    // find the size of all clusters
    for ( i=0 ; i<nlab ; i++ ) {
      lnk[i] = 0;    // now: size of the cluster with the label
      inc[i] = 0;    // now: included label (true/false)
    }
    for ( i=0 ; i<l.size() ; i++ ) {
      lnk[l[i]]++;
      inc[l[i]] = 1;
    }
    // remove clusters with too small size
    for ( i=0 ; i<nlab ; i++ )
      if ( lnk[i]<min_size )
        inc[i] = 0;

    // remove clusters with too small size
    for ( i=0 ; i<l.size() ; i++ )
      if ( !inc[l[i]] )
        l[i] = 0;

    // new numbering for the included labels
    // inc[i] will contain the new cluster numbers, j-1 will be the number of clusters
    j = 0;
    for ( i=0 ; i<nlab ; i++ ) {
      if ( inc[i] ) {
        inc[i] = j;
        j++;
      }
    }
    nlab = j;

    // renumber the labels
    for ( i=0 ; i<l.size() ; i++ )
      l[i] = inc[l[i]];

  }

  // cluster centers: not periodic
  // -----------------------------

  if ( !periodic )
  {
    int h,i,j,ilab,nlab;

    // number of labels
    nlab = l.max()+1;

    // allocate matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    Matrix<int> x({(size_t)nlab,4});

    // loop over the image to update the position and size of each label
    for ( h=0 ; h<N[0] ; h++ ) {
      for ( i=0 ; i<N[1] ; i++ ) {
        for ( j=0 ; j<N[2] ; j++ ) {
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

        if ( h <  0    ) { h = 0;    }
        if ( i <  0    ) { i = 0;    }
        if ( j <  0    ) { j = 0;    }
        if ( h >= N[0] ) { h = N[0]; }
        if ( i >= N[1] ) { i = N[1]; }
        if ( j >= N[2] ) { j = N[2]; }

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
    Matrix<int> l_,c_;
    std::tie(l_,c_) = clusters(f,kern,min_size,false);

    // delete clusters that are also not present in "l"
    for ( i=0 ; i<f.size() ; i++ )
      if ( f[i] )
        if ( !l[i] )
          l_[i] = 0;

    // number of labels
    nlab  = l .max()+1;
    nlab_ = l_.max()+1;

    // matrix to contain the average position and total size:
    // [ [ h,i,j , size_feature ] , ... ]
    Matrix<int>  x({(size_t)nlab ,4});
    // if dx(ilab,ix)==1 than N[ix] is subtracted from the position in averaging
    Matrix<int> dx({(size_t)nlab_,3});

    // label dependency: which labels in "l_" correspond to which labels in "l"
    std::vector<int> lnk(nlab_);

    for ( i=0 ; i<l.size() ; i++ )
      lnk[l_[i]] = l[i];

    // check periodicity: to which sides the clusters are connected
    // ------------------------------------------------------------

    // i-j plane
    for ( i=0 ; i<N[1] ; i++ )
      for ( j=0 ; j<N[2] ; j++ )
        for ( dh=1 ; dh<=mid[0] ; dh++ )
          for ( di=0 ; di<=mid[1] ; di++ )
            for ( dj=0 ; dj<=mid[2] ; dj++ )
              if ( i+di<N[1] && j+dj<N[2] )
                if ( l_(N[0]-1,i,j) && l(N[0]-1,i,j)==l(PER(N[0]-1+dh,N[0]),PER(i+di,N[1]),PER(j+dj,N[2])) )
                  dx(l_(N[0]-1,i,j),0) = 1;

    // h-j plane
    for ( h=0 ; h<N[0] ; h++ )
      for ( j=0 ; j<N[2] ; j++ )
        for ( dh=0 ; dh<=mid[0] ; dh++ )
          for ( di=1 ; di<=mid[1] ; di++ )
            for ( dj=0 ; dj<=mid[2] ; dj++ )
              if ( h+dh<N[0] && j+dj<N[2] )
                if ( l_(h,N[1]-1,j) && l(h,N[1]-1,j)==l(PER(h+dh,N[0]),PER(N[1]-1+di,N[1]),PER(j+dj,N[2])) )
                  dx(l_(h,N[1]-1,j),1) = 1;

    // h-i plane
    for ( h=0 ; h<N[0] ; h++ )
      for ( i=0 ; j<N[1] ; j++ )
        for ( dh=0 ; dh<=mid[0] ; dh++ )
          for ( di=0 ; di<=mid[1] ; di++ )
            for ( dj=1 ; dj<=mid[2] ; dj++ )
              if ( h+dh<N[0] && i+di<N[1] )
                if ( l_(h,i,N[2]-1) && l(h,i,N[2]-1)==l(PER(h+dh,N[0]),PER(i+di,N[1]),PER(N[2]-1+dj,N[2])) )
                  dx(l_(h,i,N[2]-1),2) = 1;

    // calculate centers
    // -----------------

    // loop over the image to update the position and size of each label
    for ( h=0 ; h<N[0] ; h++ ) {
      for ( i=0 ; i<N[1] ; i++ ) {
        for ( j=0 ; j<N[2] ; j++ ) {
          ilab = l_(h,i,j);
          if ( ilab>0 ) {
            if ( dx(ilab,0) ) dh = -N[0]; else dh = 0;
            if ( dx(ilab,1) ) di = -N[1]; else di = 0;
            if ( dx(ilab,2) ) dj = -N[2]; else dj = 0;

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

        while ( h <  0    ) { h += N[0]; }
        while ( i <  0    ) { i += N[1]; }
        while ( j <  0    ) { j += N[2]; }
        while ( h >= N[0] ) { h -= N[0]; }
        while ( i >= N[1] ) { i -= N[1]; }
        while ( j >= N[2] ) { j -= N[2]; }

        c(h,i,j) = ilab;
      }
    }

  }

  return std::make_tuple(l,c);
}

// =============================================================================
// determine clusters from image: default kernel
// =============================================================================

std::tuple<Matrix<int>,Matrix<int>> clusters (
  Matrix<int> &f, int min_size, bool periodic )
{
  Matrix<int> kern = kernel(f.ndim());
  return clusters(f,kern,min_size,periodic);
}



// =============================================================================
// 2-point probability (binary) / 2-point cluster function (int)      [periodic]
// =============================================================================

std::tuple<Matrix<double>,int> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
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
// 2-point correlation (floating-point)                               [periodic]
// =============================================================================

std::tuple<Matrix<double>,int> S2 ( Matrix<double> &f, Matrix<double> &g,
  std::vector<size_t> &roi )
{
  if ( f.shape()!=g.shape() )
    throw std::length_error("'f' and 'g' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
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

// =======x======================================================================
// masked 2-point probability (binary) / 2-point cluster function (int)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, Matrix<int> &gmask,
  bool zeropad, bool periodic )
{
  if ( f.shape()!=g.shape() || f.shape()!=fmask.shape() || f.shape()!=gmask.shape() )
    throw std::length_error("'f', 'g', 'fmask', and 'gmask' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<fmask.size() ; i++ )
    if ( !(fmask[i]==0 || fmask[i]==1) || !(gmask[i]==0 || gmask[i]==1) )
      throw std::out_of_range("'fmask' and 'gmask' must be binary");

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
// masked 2-point probability (binary) / 2-point cluster function (int)
// - default "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, bool zeropad, bool periodic )
{
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,zeropad,periodic);
}

// =============================================================================
// masked 2-point probability (binary) / 2-point cluster function (int)
// - default "fmask" and "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<int> &f, Matrix<int> &g,
  std::vector<size_t> &roi, bool zeropad, bool periodic )
{
  Matrix<int> fmask(f.shape());
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,zeropad,periodic);
}

// =======x======================================================================
// masked 2-point correlation (floating-point)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<double> &f, Matrix<double> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, Matrix<int> &gmask,
  bool zeropad, bool periodic )
{
  if ( f.shape()!=g.shape() || f.shape()!=fmask.shape() || f.shape()!=gmask.shape() )
    throw std::length_error("'f', 'g', 'fmask', and 'gmask' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<fmask.size() ; i++ )
    if ( !(fmask[i]==0 || fmask[i]==1) || !(gmask[i]==0 || gmask[i]==1) )
      throw std::out_of_range("'fmask' and 'gmask' must be binary");

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
        if ( !fmask(h,i,j) )
          for ( dh=-dH ; dh<=dH ; dh++ )
            for ( di=-dI ; di<=dI ; di++ )
              for ( dj=-dJ ; dj<=dJ ; dj++ )
                if ( !gmask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                  ret(dh+dH,di+dI,dj+dJ) += \
                    f(h,i,j)*g(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

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
// masked 2-point correlation (floating-point)
// - default "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<double> &f, Matrix<double> &g,
  std::vector<size_t> &roi, Matrix<int> &fmask, bool zeropad, bool periodic )
{
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,zeropad,periodic);
}

// =============================================================================
// masked 2-point correlation (floating-point)
// - default "fmask" and "gmask"
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> S2 ( Matrix<double> &f, Matrix<double> &g,
  std::vector<size_t> &roi, bool zeropad, bool periodic )
{
  Matrix<int> fmask(f.shape());
  Matrix<int> gmask(g.shape());
  return S2(f,g,roi,fmask,gmask,zeropad,periodic);
}

// =============================================================================
// conditional 2-point probability (binary image, binary weight)      [periodic]
// =============================================================================

std::tuple<Matrix<double>,int> W2 ( Matrix<int> &W, Matrix<int> &src,
  std::vector<size_t> &roi )
{
  if ( W.shape()!=src.shape() )
    throw std::length_error("'W' and 'I' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<W.size() ; i++ )
    if ( !(W[i]==0 || W[i]==1) || !(src[i]==0 || src[i]==1) )
      throw std::out_of_range("'W' and 'I' must be binary");

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
// conditional 2-point correlation (float. image, binary weight)      [periodic]
// =============================================================================

std::tuple<Matrix<double>,int> W2 ( Matrix<int> &W, Matrix<double> &src,
  std::vector<size_t> &roi )
{
  if ( W.shape()!=src.shape() )
    throw std::length_error("'W' and 'I' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<W.size() ; i++ )
    if ( !(W[i]==0 || W[i]==1) )
      throw std::out_of_range("'W' must be binary");

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
                ret(dh+dH,di+dI,dj+dJ) += src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

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
// weighted 2-point correlation (float. image, float. weight)         [periodic]
// =============================================================================

std::tuple<Matrix<double>,double> W2 ( Matrix<double> &W, Matrix<double> &src,
  std::vector<size_t> &roi )
{
  if ( W.shape()!=src.shape() )
    throw std::length_error("'W' and 'I' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
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
        for ( dh=-dH ; dh<=dH ; dh++ )
          for ( di=-dI ; di<=dI ; di++ )
            for ( dj=-dJ ; dj<=dJ ; dj++ )
              ret(dh+dH,di+dI,dj+dJ) += \
                W(h,i,j)*src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

  // compute normalization: sum of weight factors (binary)
  double norm = 0.;
  for ( i=0 ; i<W.size() ; i++ )
    norm += W[i];

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= norm;

  return std::make_tuple(ret,norm);
}

// =============================================================================
// masked conditional 2-point probability (binary image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<int> &src,
  std::vector<size_t> &roi, Matrix<int> &mask, bool zeropad, bool periodic )
{
  if ( W.shape()!=src.shape() || W.shape()!=mask.shape() )
    throw std::length_error("'W', 'I', and 'mask' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<W.size() ; i++ )
    if ( !(W[i]==0 || W[i]==1) || !(src[i]==0 || src[i]==1) || !(mask[i]==0 || mask[i]==1) )
      throw std::out_of_range("'W', 'I', and 'mask' must be binary");

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
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
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
  std::vector<size_t> &roi, bool zeropad, bool periodic )
{
  Matrix<int> mask(src.shape());
  return W2(W,src,roi,mask,zeropad,periodic);
}

// =============================================================================
// masked conditional 2-point correlation (float. image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<double> &src,
  std::vector<size_t> &roi, Matrix<int> &mask, bool zeropad, bool periodic )
{
  if ( W.shape()!=src.shape() || W.shape()!=mask.shape() )
    throw std::length_error("'W', 'I', and 'mask' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<W.size() ; i++ )
    if ( !(W[i]==0 || W[i]==1) || !(mask[i]==0 || mask[i]==1) )
      throw std::out_of_range("'W' and 'mask' must be binary");

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
                  ret(dh+dH,di+dI,dj+dJ) += src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

  // compute normalization: sum of weight factors (binary) of unmasked voxels
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
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
// conditional 2-point correlation (float. image, binary weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<int>> W2 ( Matrix<int> &W, Matrix<double> &src,
  std::vector<size_t> &roi, bool zeropad, bool periodic )
{
  Matrix<int> mask(src.shape());
  return W2(W,src,roi,mask,zeropad,periodic);
}

// =============================================================================
// weighted conditional 2-point correlation (float. image, float. weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<double>> W2 ( Matrix<double> &W, Matrix<double> &src,
  std::vector<size_t> &roi, Matrix<int> &mask, bool zeropad, bool periodic )
{
  if ( W.shape()!=src.shape() || W.shape()!=mask.shape() )
    throw std::length_error("'W', 'I', and 'mask' are inconsistent");

  for ( auto i : roi )
    if ( i%2==0 )
      throw std::length_error("'roi' must be odd shaped");

  for ( int i=0 ; i<W.size() ; i++ )
    if ( !(mask[i]==0 || mask[i]==1) )
      throw std::out_of_range("'mask' must be binary");

  int h,i,j,dh,di,dj,H,I,J,dH,dI,dJ,bH=0,bI=0,bJ=0;

  Matrix<double> ret (roi);
  Matrix<double> norm(roi);

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
        for ( dh=-dH ; dh<=dH ; dh++ )
          for ( di=-dI ; di<=dI ; di++ )
            for ( dj=-dJ ; dj<=dJ ; dj++ )
              if ( !mask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                ret(dh+dH,di+dI,dj+dJ) += \
                  W(h,i,j)*src(PER(h+dh,H),PER(i+di,I),PER(j+dj,J));

  // compute normalization: sum of weight factors (binary) of unmasked voxels
  for ( h=bH ; h<H-bH ; h++ )
    for ( i=bI ; i<I-bI ; i++ )
      for ( j=bJ ; j<J-bJ ; j++ )
        for ( dh=-dH ; dh<=dH ; dh++ )
          for ( di=-dI ; di<=dI ; di++ )
            for ( dj=-dJ ; dj<=dJ ; dj++ )
              if ( !mask(PER(h+dh,H),PER(i+di,I),PER(j+dj,J)) )
                norm(dh+dH,di+dI,dj+dJ) += W(h,i,j);

  // apply normalization
  for ( i=0 ; i<ret.size() ; i++ )
    ret[i] /= norm[i];

  return std::make_tuple(ret,norm);
}

// =============================================================================
// weighted 2-point correlation (float. image, float. weight)
// =============================================================================

std::tuple<Matrix<double>,Matrix<double>> W2 ( Matrix<double> &W, Matrix<double> &src,
  std::vector<size_t> &roi, bool zeropad, bool periodic )
{
  Matrix<int> mask(src.shape());
  return W2(W,src,roi,mask,zeropad,periodic);
}

// =============================================================================

} // namespace Image

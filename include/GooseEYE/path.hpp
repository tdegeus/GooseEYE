/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_PATH_HPP
#define GOOSEEYE_PATH_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

xt::xarray<int> path(const std::vector<int> &xa, const std::vector<int> &xb, path_mode mode)
{
  GOOSEEYE_ASSERT(xa.size() == xb.size());
  GOOSEEYE_ASSERT(xa.size() > 0 && xa.size() <= 3);

  int ndim = static_cast<int>(xa.size());

  std::vector<int> ret;

  if (mode == path_mode::Bresenham)
  {
    // see http://www.luberth.com/plotter/line3d.c.txt.html
    int a[3], s[3], x[3], d[3], in[2], j, i, iin, nnz=0;

    // set defaults
    for (i=0; i<3; i++) {
      a[i] = 1;
      s[i] = 0;
      x[i] = 0;
      d[i] = 0;
    }

    // calculate:
    // absolute distance, set to "1" if the distance is zero
    // sign of the distance (can be -1/+1 or 0)
    // current position (temporary value)
    for (i=0; i<ndim; i++) {
      a[i] = std::abs(xb[i]-xa[i]) << 1;
      s[i] = SIGN    (xb[i]-xa[i]);
      x[i] = xa[i];
    }

    // determine which direction is dominant
    for (j=0; j<3; j++) {
      // set the remaining directions
      iin = 0;
      for (i=0; i<3; i++) {
        if (i!=j) {
          in[iin] = i;
          iin    += 1;
        }
      }
      // determine if the current direction is dominant
      if (a[j] >= std::max(a[in[0]],a[in[1]]))
        break;
    }

    // set increment in non-dominant directions
    for (i=0; i<2; i++)
      d[in[i]] = a[in[i]]-(a[j]>>1);
    // loop until "x" coincides with "xb"
    while (true) {
      // add current voxel to path
      for (i=0; i<ndim; i++)
        ret.push_back(x[i]);
      // update voxel counter
      nnz += 1;
      // check convergence
      if (x[j]==xb[j])
        return xt::adapt(ret, {static_cast<size_t>(nnz), static_cast<size_t>(ndim)});
      // check increment in other directions
      for (i=0; i<2; i++) {
        if (d[in[i]]>=0) {
          x[in[i]] += s[in[i]];
          d[in[i]] -= a[j];
        }
      }
      // increment
      x[j] += s[j];
      for (i=0; i<2; i++)
        d[in[i]] += a[in[i]];
    }
  }

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
  for (i=0 ; i<ndim ; i++) {
    // set origin, store to output array; initiate the position
    cindex[i] = xa[i];
    ret.push_back(cindex[i]);
    // initiate position, set slope
    x[i] = (double)(xa[i]);
    v[i] = (double)(xb[i]-xa[i]);
    // non-zero slope: calculate the sign and the next intersection
    // with a voxel's edges, and update the list to include this dimension
    // in the further analysis
    if (v[i]) {
      sign[i]  = v[i]/fabs(v[i]);
      isign[i] = (int)sign[i];
      next[i]  = sign[i]*0.5;
      in[nin]  = i;
      nin++;
    }
  }

  // starting from "xa" loop to "xb"
  while (true)
  {
    // find translation coefficient "t" for each next intersection
    // (only include dimensions with non-zero slope)
    for (iin=0 ; iin<nin ; iin++) {
      i      = in[iin];
      t[iin] = (next[i]-x[i])/v[i];
    }
    // find the minimum "t": the intersection which is closet along the line
    // from the current position -> proceed in this direction
    imin = 0;
    for (iin=1 ; iin<nin ; iin++)
      if (t[iin]<t[imin])
        imin = iin;

    // update path: proceed in dimension of minimum "t"
    // note: if dimensions have equal "t" -> proceed in each dimension
    for (iin=0 ; iin<nin ; iin++) {
      if (fabs(t[iin]-t[imin])<1.e-6) {
        cindex[in[iin]] += isign[in[iin]];
        next[in[iin]]   += sign[in[iin]];
        // store all the face crossings ("mode")
        if (mode == path_mode::full) {
          for (i=0 ; i<ndim ; i++)
            ret.push_back(cindex[i]);
          nnz++;
        }
      }
    }
    // store only the next voxel ("actual")
    if (mode == path_mode::actual) {
      for (i=0 ; i<ndim ; i++)
        ret.push_back(cindex[i]);
      nnz++;
    }
    // update position, and current path
    for (i=0 ; i<ndim ; i++)
      x[i] = xa[i]+v[i]*t[imin];

    // check convergence: stop when "xb" is reached
    n = 0;
    for (i=0 ; i<ndim; i++)
      if (cindex[i]==xb[i])
        n++;
    if (n==ndim)
      break;
  }

  return xt::adapt(ret, {static_cast<size_t>(nnz), static_cast<size_t>(ndim)});
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

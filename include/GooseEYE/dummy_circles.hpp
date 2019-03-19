/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DUMMY_CIRCLES_HPP
#define GOOSEEYE_DUMMY_CIRCLES_HPP

// =================================================================================================

#include "config.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// create a dummy image with circles at position "row","col" with radius "r"
// =================================================================================================

xt::xarray<int> dummy_circles(
  const std::vector<size_t>& shape,
  const std::vector<int>& row,
  const std::vector<int>& col,
  const std::vector<int>& r,
  bool periodic)
{
  GOOSEEYE_ASSERT(row.size() == col.size());
  GOOSEEYE_ASSERT(row.size() == r.size());
  GOOSEEYE_ASSERT(shape.size() == 2);

  xt::xarray<int> out = xt::zeros<int>(shape);

  if (periodic) {
    for (size_t i = 0; i < row.size(); ++i)
      for (int di = -r[i]; di <= r[i]; ++di)
        for (int dj = -r[i]; dj <= r[i]; ++dj)
          if ((int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i])
            out.periodic(row[i]+di, col[i]+dj) = 1;

    return out;
  }

  for (size_t i = 0; i < row.size(); ++i)
    for (int di = -r[i]; di <= r[i]; ++di)
      for (int dj = -r[i]; dj <= r[i]; ++dj)
        if ( out.in_bounds(row[i]+di, col[i]+dj) )
          if ((int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i])
            out(row[i]+di, col[i]+dj) = 1;

  return out;
}

// =================================================================================================
// create a dummy image with a default number of circles
// at random positions and random radii
// =================================================================================================

xt::xarray<int> dummy_circles(const std::vector<size_t>& shape, bool periodic)
{
  GOOSEEYE_ASSERT(shape.size() == 2);

  std::srand(std::time(0));

  // set default: number of circles in both directions and (constant) radius
  int N = (int)(.05*(double)shape[0]);
  int M = (int)(.05*(double)shape[1]);
  int R = (int)(pow((.3*(double)(shape[0]*shape[1]))/(M_PI*(double)(N*M)),.5));

  std::vector<int> row(N*M), col(N*M), r(N*M);

  // define regular grid of circles
  for (int i=0; i<N; i++) {
    for (int j=0; j<M; j++) {
      row[i*M+j] = (int)((double)i*(double)shape[0]/(double)N);
      col[i*M+j] = (int)((double)j*(double)shape[1]/(double)M);
      r  [i*M+j] = R;
    }
  }

  // distance between circles
  int dN = (int)(.5*(double)shape[0]/(double)N);
  int dM = (int)(.5*(double)shape[1]/(double)M);

  // randomly perturb circles (move in any direction, enlarge/shrink)
  for ( int i=0; i<N*M; i++) {
    row[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dN;
    col[i] += (int)(((double)(std::rand()%2)-.5)*2.)*std::rand()%dM;
    r  [i]  = (int)(((double)(std::rand()%100)/100.*2.+.1)*(double)(r[i]));
  }

  // convert to image
  return dummy_circles(shape,row,col,r,periodic);
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

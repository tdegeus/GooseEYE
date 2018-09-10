/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DUMMY_CIRCLES_HPP
#define GOOSEEYE_DUMMY_CIRCLES_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// create a dummy image with circles at position "row","col" with radius "r"
// =================================================================================================

MatI dummy_circles(const VecS &shape, const VecI &row, const VecI &col, const VecI &r, bool periodic)
{
  if ( row.size() != col.size() or row.size() != r.size() )
    throw std::runtime_error("'row', 'col', and 'r' are inconsistent");

  if ( shape.size() != 2 )
    throw std::runtime_error("Only allowed in 2 dimensions");

  MatI out = MatI::Zero(shape[0], shape[1]);

  out.setPeriodic(periodic);

  for ( size_t i = 0 ; i < row.size() ; ++i )
    for ( int di = -r[i] ; di <= r[i] ; ++di )
      for ( int dj = -r[i] ; dj <= r[i] ; ++dj )
        if ( out.inBounds(row[i]+di, col[i]+dj) )
          if ( (int)(ceil(pow((double)(pow(di,2)+pow(dj,2)),0.5))) < r[i] )
            out(row[i]+di, col[i]+dj) = 1;

  return out;
}

// =================================================================================================
// create a dummy image with a default number of circles
// at random positions and random radii
// =================================================================================================

MatI dummy_circles(const VecS &shape, bool periodic)
{
  if ( shape.size() != 2 )
    throw std::runtime_error("Only allowed in 2 dimensions");

  std::srand(std::time(0));

  // set default: number of circles in both directions and (constant) radius
  int N = (int)(.05*(double)shape[0]);
  int M = (int)(.05*(double)shape[1]);
  int R = (int)(pow((.3*(double)(shape[0]*shape[1]))/(M_PI*(double)(N*M)),.5));

  VecI row(N*M), col(N*M), r(N*M);

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

} // namespace ...

// =================================================================================================

#endif

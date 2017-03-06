
#include "image.h"

namespace image {

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
Matrix<T>::Matrix(int nh, int ni, int nj, bool periodic_ )
{
  size     = nh*nj*ni;
  periodic = periodic_;

  if ( ni==1 && nj==1 )
    nd = 1;
  else if ( nj==1 )
    nd = 2;

  shape.push_back(nh);
  shape.push_back(ni);
  shape.push_back(nj);

  for ( int i=0 ; i<size ; i++ )
    data.push_back((T)0);
}

// =============================================================================
// initialize different types of matrices
// =============================================================================

template class Matrix<int>;
template class Matrix<bool>;
template class Matrix<double>;




std::vector<double> S2 ( std::vector<double>& pos )
{

  int N = (int)pos.size()/2;

  std::vector<double> output(N*3);

  for ( int i=0 ; i<N ; i++ ) {
    output[i*3+0] = pos[i*2+0];
    output[i*3+1] = pos[i*2+1];
    output[i*3+2] = pow(pos[i*2+0]*pos[i*2+1],.5);
  }

  return output;

}



}

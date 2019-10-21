/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP
#define GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<size_t,2> Ensemble::stampPoints(
  size_t nd
) const
{
  xt::xtensor<size_t,2> points;
  bool xdir=false, ydir=false, zdir=false;
  size_t n, idx;

  if( nd >= 1 )
    xdir = m_Shape[0] > 1;
  if( nd >= 2 )
    ydir = m_Shape[1] > 1;
  if( nd >= 3 )
    zdir = m_Shape[2] > 1;


  if( xdir && !ydir && !zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(1,0) = m_Shape[0] - 1;
  }
  else if( !xdir && ydir && !zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(1,1) = m_Shape[1] - 1;
  }
  else if( !xdir && !ydir && zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(1,2) = m_Shape[2] - 1;
  }
  else if( xdir && ydir && !zdir ) {
    n = 2*m_Shape[0] + 2*m_Shape[1] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      points(idx,0) = i;
      points(idx++,1) = 0;
      points(idx,0) = i;
      points(idx++,1) = m_shape[1] - 1;
    }
    for( size_t i = 1; i < m_Shape[1]-1; ++i ) {
      points(idx,0) = 0;
      points(idx++,1) = i;
      points(idx,0) = m_Shape[0] - 1;
      points(idx++,1) = i;
    }
  }
  else if( xdir && !ydir && zdir ) {
    n = 2*m_Shape[0] + 2*m_Shape[2] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      points(idx,0) = i;
      points(idx++,2) = 0;
      points(idx,0) = i;
      points(idx++,2) = m_Shape[2] - 1;
    }
    for( size_t i = 1; i < m_Shape[2]-1; ++i ) {
      points(idx,0) = 0;
      points(idx++,2) = i;
      points(idx,0) = m_Shape[0] - 1;
      points(idx++,2) = i;
    }
  }
  else if( !xdir && ydir && zdir ) {
    n = 2*m_Shape[1] + 2*m_Shape[2] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[1]; ++i ) {
      points(idx,1) = i;
      points(idx++,2) = 0;
      points(idx,1) = i;
      points(idx++,2) = m_Shape[2] - 1;
    }
    for( size_t i = 1; i < m_Shape[2]-1; ++i ) {
      points(idx,1) = 0;
      points(idx++,2) = i;
      points(idx,1) = m_Shape[1] - 1;
      points(idx++,2) = i;
    }
  }
  else if( xdir && ydir && zdir ) {
    n = 2*m_Shape[0]*m_Shape[1] + 2*m_Shape[0]*(m_Shape[2]-2) \
      + 2*(m_Shape[1]-2)*(m_Shape[2]-2);
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      for( size_t j = 0; j < m_Shape[1]; ++j ) {
        points(idx,0) = i;
        points(idx,1) = j;
        points(idx++,2) = 0;
        points(idx,0) = i;
        points(idx,1) = j;
        points(idx++,2) = m_Shape[2] - 1;
      }
    }
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      for( size_t j = 1; j < m_Shape[2]-1; ++j ) {
        points(idx,0) = i;
        points(idx,1) = 0;
        points(idx++,2) = j;
        points(idx,0) = i;
        points(idx,1) = m_Shape[1] - 1;
        points(idx++,2) = j;
      }
    }
    for( size_t i = 1; i < m_Shape[1]-1; ++i ) {
      for( size_t j = 1; j < m_Shape[2]-1; ++j ) {
        points(idx,0) = 0;
        points(idx,1) = i;
        points(idx++,2) = j;
        points(idx,0) = m_Shape[0] - 1;
        points(idx,1) = i;
        points(idx++,2) = j;
      }
    }
  }

  return points;
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

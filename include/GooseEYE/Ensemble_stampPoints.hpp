/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP
#define GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<int,2> Ensemble::stampPoints(
  size_t nd
) const
{
  xt::xtensor<int,2> points;
  bool xdir=false, ydir=false, zdir=false;
  int xm=0, ym=0, zm=0;
  size_t n, idx;

  if( nd >= 1 ) {
    xdir = m_Shape[0] > 1;
    xm = m_Shape[0] / 2;
  }
  if( nd >= 2 ) {
    ydir = m_Shape[1] > 1;
    ym = m_Shape[1] / 2;
  }
  if( nd >= 3 ) {
    zdir = m_Shape[2] > 1;
    zm = m_Shape[2] / 2;
  }


  if( xdir && !ydir && !zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(0,0) = -xm;
    points(1,0) = xm;
  }
  else if( !xdir && ydir && !zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(0,1) = -ym;
    points(1,1) = ym;
  }
  else if( !xdir && !ydir && zdir ) {
    n = 2;
    points = xt::zeros<int>( {n,nd} );
    points(0,2) = -zm;
    points(1,2) = -zm;
  }
  else if( xdir && ydir && !zdir ) {
    n = 2*m_Shape[0] + 2*m_Shape[1] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      points(idx,0) = i - xm;
      points(idx++,1) = -ym;
      points(idx,0) = i - xm;
      points(idx++,1) = ym;
    }
    for( size_t i = 1; i < m_Shape[1]-1; ++i ) {
      points(idx,0) = -xm;
      points(idx++,1) = i - ym;
      points(idx,0) = xm;
      points(idx++,1) = i - ym;
    }
  }
  else if( xdir && !ydir && zdir ) {
    n = 2*m_Shape[0] + 2*m_Shape[2] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      points(idx,0) = i - xm;
      points(idx++,2) = -zm;
      points(idx,0) = i - xm;
      points(idx++,2) = zm;
    }
    for( size_t i = 1; i < m_Shape[2]-1; ++i ) {
      points(idx,0) = -xm;
      points(idx++,2) = i - zm;
      points(idx,0) = xm;
      points(idx++,2) = i - zm;
    }
  }
  else if( !xdir && ydir && zdir ) {
    n = 2*m_Shape[1] + 2*m_Shape[2] - 4;
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[1]; ++i ) {
      points(idx,1) = i - ym;
      points(idx++,2) = -zm;
      points(idx,1) = i - ym;
      points(idx++,2) = zm;
    }
    for( size_t i = 1; i < m_Shape[2]-1; ++i ) {
      points(idx,1) = -ym;
      points(idx++,2) = i - zm;
      points(idx,1) = ym;
      points(idx++,2) = i - zm;
    }
  }
  else if( xdir && ydir && zdir ) {
    n = 2*m_Shape[0]*m_Shape[1] + 2*m_Shape[0]*(m_Shape[2]-2) \
      + 2*(m_Shape[1]-2)*(m_Shape[2]-2);
    points = xt::zeros<int>( {n,nd} );

    idx = 0;
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      for( size_t j = 0; j < m_Shape[1]; ++j ) {
        points(idx,0) = i - xm;
        points(idx,1) = j - ym;
        points(idx++,2) = -zm;
        points(idx,0) = i - xm;
        points(idx,1) = j - ym;
        points(idx++,2) = zm;
      }
    }
    for( size_t i = 0; i < m_Shape[0]; ++i ) {
      for( size_t j = 1; j < m_Shape[2]-1; ++j ) {
        points(idx,0) = i - xm;
        points(idx,1) = -ym;
        points(idx++,2) = j - zm;
        points(idx,0) = i - xm;
        points(idx,1) = ym;
        points(idx++,2) = j - zm;
      }
    }
    for( size_t i = 1; i < m_Shape[1]-1; ++i ) {
      for( size_t j = 1; j < m_Shape[2]-1; ++j ) {
        points(idx,0) = -xm;
        points(idx,1) = i - ym;
        points(idx++,2) = j - zm;
        points(idx,0) = xm;
        points(idx,1) = i - ym;
        points(idx++,2) = j - zm;
      }
    }
  }

  return points;
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

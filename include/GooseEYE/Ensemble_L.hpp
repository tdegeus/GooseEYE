/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

TODO:
- Normalization
- Account for mask
- Comparison sub-matrix is now copied, passing a view to the roi_L function would be faster

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_L_HPP
#define GOOSEEYE_ENSEMBLE_L_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

template <class T>
inline void Ensemble::L(
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask
)
{

  // lock statistics
  m_stat = Type::L;

  // padding default not periodic: mask padded items
  xt::pad_mode pad_mode = xt::pad_mode::constant;
  int mask_value = 1;

  // padding optionally periodic: unmask padded items
  if (m_periodic) {
    pad_mode = xt::pad_mode::periodic;
    mask_value = 0;
  }

  // apply padding
  xt::xarray<T> F = xt::pad(f, m_pad, pad_mode);
  xt::xarray<int> Fmask = xt::pad(fmask, m_pad, xt::pad_mode::constant, mask_value);

  // make matrices virtually 3-d matrices
  std::vector<size_t> shape = detail::shape_as_dim(MAX_DIM, F, 1);
  F.reshape(shape);
  Fmask.reshape(shape);

  // local output and normalisation
  xt::xarray<T> first = xt::zeros<T>(m_Shape);
  xt::xarray<T> norm = xt::zeros<T>(m_Shape);

  // sub-matrix and path-matrix
  xt::xarray<T> Fi = xt::empty<T>(m_Shape);
  xt::xarray<int> Li = xt::empty<T>(m_Shape);

  // roi stamp
  xt::xtensor<size_t,2> stamp = stampPoints( 3 );
  size_t nstamp = stamp.shape(0);

  // Position vectors
  xt::xtensor_fixed<size_t, xt::xshape<3>> x0, x1;

  x0[0] = m_Shape[0] / 2;
  x0[1] = m_Shape[1] / 2;
  x0[2] = m_Shape[2] / 2;

  // compute correlation
  for (size_t h = m_Pad[0][0]; h < shape[0]-m_Pad[0][1]; ++h) {
    for (size_t i = m_Pad[1][0]; i < shape[1]-m_Pad[1][1]; ++i) {
      for (size_t j = m_Pad[2][0]; j < shape[2]-m_Pad[2][1]; ++j) {
        // - skip masked

        if (F(h,i,j) != (T) 1 || Fmask(h,i,j))
          continue;

        // Get comparison sub-matrix
        Fi = xt::view(F,
          xt::range(h-m_Pad[0][0], h+m_Pad[0][1]+1),
          xt::range(i-m_Pad[1][0], i+m_Pad[1][1]+1),
          xt::range(j-m_Pad[2][0], j+m_Pad[2][1]+1));

        Li = xt::zeros<T>( m_Shape );
        Li(x0[0],x0[1],x0[2]) = 1;

        for( size_t istamp = 0; istamp < nstamp; ++istamp ) {
          x1 = x0 + xt::view(stamp, istamp, xt::range(0,3));

          // TODO: implement other pixel-path algorithms
          bressenham( Li, Fi, x0, x1 );
        }

        first += F(h,i,j) * Li;
        //norm += xt::ones<T>(m_Shape);
      }
    }
  }
  m_first += first;
  m_norm += xt::ones<T>(m_Shape);
}

// -------------------------------------------------------------------------------------------------

template <class T>
inline void Ensemble::L(
  const xt::xarray<T>& f)
{
  xt::xarray<int> fmask = xt::zeros<int>(f.shape());
  L(f, fmask);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

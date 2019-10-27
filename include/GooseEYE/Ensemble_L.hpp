/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_L_HPP
#define GOOSEEYE_ENSEMBLE_L_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
inline void Ensemble::L(
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  path_mode mode)
{
  GOOSEEYE_ASSERT(f.shape() == fmask.shape());
  GOOSEEYE_ASSERT(f.dimension() == m_shape.size());
  GOOSEEYE_ASSERT(xt::all(xt::equal(fmask,0) || xt::equal(fmask,1)));
  GOOSEEYE_ASSERT(m_stat == Type::L || m_stat == Type::Unset);

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
  xt::xarray<int> Li = xt::empty<T>(m_Shape);

  // ROI-shaped array used to extract a pixel stamp:
  // a set of end-points over which to loop and check the statics
  // - initialize to 1
  xt::xtensor<size_t,3> r = xt::ones<size_t>(m_Shape);
  // - determine interior pixels (account for quasi-3D images)
  auto ix = m_Shape[0] > 1 ? xt::range(1, m_Shape[0] - 1) : xt::range(0, m_Shape[0]);
  auto iy = m_Shape[1] > 1 ? xt::range(1, m_Shape[1] - 1) : xt::range(0, m_Shape[1]);
  auto iz = m_Shape[2] > 1 ? xt::range(1, m_Shape[2] - 1) : xt::range(0, m_Shape[2]);
  // - set interior pixels to 0
  xt::view(r, ix, iy, iz) = 0;

  // get stamp, from the matrix "r"
  xt::xtensor<size_t,2> stamp = xt::from_indices(xt::argwhere(r));
  size_t nstamp = stamp.shape(0);

  // position vectors: begin and end-points spanning a pixel-path
  xt::xtensor_fixed<size_t, xt::xshape<3>> x0, x1;

  // set the mid-point
  x0(0) = m_Pad[0][0];
  x0(1) = m_Pad[1][0];
  x0(2) = m_Pad[2][0];

  // compute correlation
  for (size_t h = m_Pad[0][0]; h < shape[0] - m_Pad[0][1]; ++h) {
    for (size_t i = m_Pad[1][0]; i < shape[1] - m_Pad[1][1]; ++i) {
      for (size_t j = m_Pad[2][0]; j < shape[2] - m_Pad[2][1]; ++j) {

        // - skip masked
        if (Fmask(h,i,j))
          continue;

        // - get comparison sub-matrix
        auto Fi = xt::view(F,
          xt::range(h-m_Pad[0][0], h+m_Pad[0][1]+1),
          xt::range(i-m_Pad[1][0], i+m_Pad[1][1]+1),
          xt::range(j-m_Pad[2][0], j+m_Pad[2][1]+1));

        // - get inverse of comparison mask
        auto Fmii = T(1) - xt::view(Fmask,
          xt::range(h-m_Pad[0][0], h+m_Pad[0][1]+1),
          xt::range(i-m_Pad[1][0], i+m_Pad[1][1]+1),
          xt::range(j-m_Pad[2][0], j+m_Pad[2][1]+1));

        // - zero-initialise
        Li.fill(0);

        // - walk over stamp
        for (size_t istamp = 0; istamp < nstamp; ++istamp) {
          // - get comparison pixel
          x1 = xt::view(stamp, istamp, xt::all());
          // - get pixel path connecting "x0" and "x1"
          xt::xtensor<size_t,2> ppath = path(x0, x1, mode);
          // - walk over pixel path
          for (size_t ipath = 0; ipath < ppath.shape(0); ++ipath) {
            if (Fmii(ppath(ipath,0), ppath(ipath,1), ppath(ipath,2)) == (T)0)
              continue;
            if (Fi(ppath(ipath,0), ppath(ipath,1), ppath(ipath,2)) == (T)1)
              Li(ppath(ipath,0), ppath(ipath,1), ppath(ipath,2)) = 1;
            else
              break;
          }
        }

        // - correlation
        first += Li * Fmii;
        norm += Fmii;
      }
    }
  }

  m_first += first;
  m_norm += norm;
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
inline void Ensemble::L(
  const xt::xarray<T>& f,
  path_mode mode)
{
  xt::xarray<int> fmask = xt::zeros<int>(f.shape());
  L(f, fmask, mode);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

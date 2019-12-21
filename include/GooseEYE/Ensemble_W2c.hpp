/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_W2C_HPP
#define GOOSEEYE_ENSEMBLE_W2C_HPP

#include "GooseEYE.h"

namespace GooseEYE {


template <class T>
inline void Ensemble::W2c(
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    const xt::xarray<int>& fmask,
    path_mode mode)
{
  GOOSEEYE_ASSERT(f.shape() == clusters.shape());
  GOOSEEYE_ASSERT(f.shape() == centers.shape());
  GOOSEEYE_ASSERT(f.shape() == fmask.shape());
  GOOSEEYE_ASSERT(f.dimension() == m_shape.size());
  GOOSEEYE_ASSERT(xt::all(xt::equal(fmask,0) || xt::equal(fmask,1)));
  GOOSEEYE_ASSERT(m_stat == Type::W2c || m_stat == Type::Unset);

  // lock statistic
  m_stat = Type::W2c;

  // padding default not periodic: mask padded items
  xt::pad_mode pad_mode = xt::pad_mode::constant;

  // padding optionally periodic: unmask padded items
  if (m_periodic) {
    pad_mode = xt::pad_mode::periodic;
  }

  // apply padding
  xt::xarray<T> F = xt::pad(f, m_pad, pad_mode);
  xt::xarray<int> Fmask = xt::pad(fmask, m_pad, pad_mode);
  xt::xarray<int> Clusters = xt::pad(clusters, m_pad, pad_mode);
  xt::xarray<int> Centers = xt::pad(centers, m_pad, pad_mode);

  // make matrices virtually 3-d matrices
  std::vector<size_t> shape = detail::shape_as_dim(MAX_DIM, F, 1);
  F.reshape(shape);

  // local output and normalisation
  xt::xarray<T> first = xt::zeros<T>(m_Shape);
  xt::xarray<int> norm = xt::zeros<int>(m_Shape);

  // ROI-shaped array used to extract a pixel stamp:
  // a set of end-points over which to loop and check the statics
  // - initialize to 1
  xt::xtensor<int,3> r = xt::ones<int>(m_Shape);
  // - determine interior pixels (account for quasi-3D images)
  auto ix = m_Shape[0] > 1 ? xt::range(1, m_Shape[0] - 1) : xt::range(0, m_Shape[0]);
  auto iy = m_Shape[1] > 1 ? xt::range(1, m_Shape[1] - 1) : xt::range(0, m_Shape[1]);
  auto iz = m_Shape[2] > 1 ? xt::range(1, m_Shape[2] - 1) : xt::range(0, m_Shape[2]);
  // - set interior pixels to 0
  xt::view(r, ix, iy, iz) = 0;

  // get stamp, from the matrix "r"
  xt::xtensor<int,2> stamp = xt::from_indices(xt::argwhere(r));
  for (size_t i = 0; i < MAX_DIM; ++i)
    xt::view(stamp, xt::all(), xt::keep(i)) -= m_Pad[i][0];

  // correlation
  // N.B. getting the pixel paths is relatively expensive, so it is the output-most loop
  for (size_t istamp = 0; istamp < stamp.shape(0); ++istamp) {

    // pixel path between the center of the ROI and the current stamp point
    xt::xtensor<int,2> path = GooseEYE::path(
      {0, 0, 0},
      {stamp(istamp, 0), stamp(istamp, 1), stamp(istamp, 2)},
      mode);

    for (size_t h = m_Pad[0][0]; h < shape[0] - m_Pad[0][1]; ++h) {
      for (size_t i = m_Pad[1][0]; i < shape[1] - m_Pad[1][1]; ++i) {
        for (size_t j = m_Pad[2][0]; j < shape[2] - m_Pad[2][1]; ++j) {

          auto label = Centers(h, i, j);
          int q = -1;

          // proceed only when the centre is inside the cluster
          if (!label || Clusters(h, i, j) != label) {
            continue;
          }

          for (size_t p = 0; p < path.shape(0); ++p) {

            int dh = path(p, 0);
            int di = path(p, 1);
            int dj = path(p, 2);

            // loop through the voxel-path until the end of a cluster
            if (Clusters(h + dh, i + di, j + dj) != label && q < 0) {
              q = 0;
            }

            // loop from the beginning of the path and store there
            if (q >= 0) {
              if (!Fmask(h + dh, i + di, j + dj) ) {
                norm(
                  m_Pad[0][0] + path(q, 0),
                  m_Pad[1][0] + path(q, 1),
                  m_Pad[1][0] + path(q, 2)) += 1;

                first(
                  m_Pad[0][0] + path(q, 0),
                  m_Pad[1][0] + path(q, 1),
                  m_Pad[1][0] + path(q, 2)) += F(h + dh, i + di, j + dj);
              }
            }

            q++;
          }
        }
      }
    }
  }

  m_first += first;
  m_norm += norm;
}


template <class T>
inline void Ensemble::W2c(
    const xt::xarray<int>& clusters,
    const xt::xarray<int>& centers,
    const xt::xarray<T>& f,
    path_mode mode)
{
  xt::xarray<int> mask = xt::zeros<int>(f.shape());
  W2c(clusters, centers, f, mask, mode);
}


} // namespace ...

#endif

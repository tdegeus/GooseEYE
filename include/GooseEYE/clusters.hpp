/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_CLUSTERS_HPP
#define GOOSEEYE_CLUSTERS_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

inline Clusters::Clusters(
  const xt::xarray<int>& f,
  const xt::xarray<int>& kernel,
  bool periodic) :
  m_kernel(kernel),
  m_periodic(periodic),
  m_l(f)
{
  GOOSEEYE_ASSERT(xt::all(xt::equal(f,0) || xt::equal(f,1)));
  GOOSEEYE_ASSERT(xt::all(xt::equal(kernel,0) || xt::equal(kernel,1)));
  GOOSEEYE_ASSERT(f.dimension() == kernel.dimension());

  // extract shape from input
  // ------------------------

  m_shape = detail::shape(f);
  m_shape_kernel = detail::shape(m_kernel);
  m_pad = detail::pad_width(m_kernel);
  m_Shape = detail::as_dim(MAX_DIM, m_shape, 1);
  m_Shape_kernel = detail::as_dim(MAX_DIM, m_shape_kernel, 1);
  m_Pad = detail::as_dim(MAX_DIM, m_pad, 0);

  // make input pseudo 3-d and pad it (to make things a lot simpler)
  // ---------------------------------------------------------------

  // make pseudo 3-d
  m_l.reshape(m_Shape);
  m_kernel.reshape(m_Shape_kernel);

  // padding default not periodic: mask padded items
  xt::pad_mode pad_mode = xt::pad_mode::constant;
  int pad_value = 0;

  // padding optionally periodic
  if (periodic)
    pad_mode = xt::pad_mode::periodic;

  // apply padding
  // note that "m_l" are also the initialised labels whereby:
  // "0" = background, "1" = unlabelled
  m_l = xt::pad(m_l, m_Pad, pad_mode, pad_value);

  // basic clustering
  // ----------------

  // first new label (start at "2" to distinguish: "0" = background, "1" = unlabelled)
  int ilab = 2;

  // list to renumber: used to link clusters to each other
  // N.B. By default the algorithm simply loops over the image, consequently it will miss that
  // clusters may touch further down in the image, labelling one cluster with several labels.
  // Using "renum" these touching clusters will glued and assigned one single label.
  xt::xtensor<int,1> renum = xt::arange<int>(m_l.size());

  // loop over the image
  for (size_t h = m_Pad[0][0]; h < m_l.shape(0)-m_Pad[0][1]; ++h) {
    for (size_t i = m_Pad[1][0]; i < m_l.shape(1)-m_Pad[1][1]; ++i) {
      for (size_t j = m_Pad[2][0]; j < m_l.shape(2)-m_Pad[2][1]; ++j) {
        // - skip background voxels
        if (!m_l(h,i,j))
          continue;
        // - get current labels in the ROI
        auto Li = xt::view(m_l,
          xt::range(h-m_Pad[0][0], h+m_Pad[0][1]+1),
          xt::range(i-m_Pad[1][0], i+m_Pad[1][1]+1),
          xt::range(j-m_Pad[2][0], j+m_Pad[2][1]+1));
        // - apply kernel to the labels in the ROI
        auto Ni = Li * m_kernel;
        // - extract label to apply
        int l = xt::amax(Ni)[0];
        // - draw a new label, only if needed (if there were no other entries than 0 and 1)
        if (l == 1) {
          l = ilab;
          ++ilab;
        }
        // - apply label to all unlabelled voxels that are unlabelled within the m_kernel
        Li = xt::where(xt::equal(Ni, 1), l, Li);
        // - check if clusters have to be merged, if not: continue to the next voxel
        if (xt::all(xt::equal(Li, l) || xt::equal(Li, 0) || xt::equal(Li, 1)))
          continue;
        // - get the labels to be merged (discard 0 and 1 by settings them to "l" in this copy)
        xt::xarray<int> merge = xt::where(xt::less_equal(Li, 1), l, Li);
        merge = xt::unique(merge);
        // - merge labels (apply merge to other labels in cluster)
        int linkto = xt::amin(xt::view(renum, xt::keep(merge)))[0];
        for (auto& a: merge)
          renum = xt::where(xt::equal(renum, renum(a)), linkto, renum);
      }
    }
  }

  // remove padding & renumber
  // -------------------------

  // remove padding
  m_l = xt::view(m_l,
    xt::range(m_Pad[0][0], m_l.shape(0)-m_Pad[0][1]),
    xt::range(m_Pad[1][0], m_l.shape(1)-m_Pad[1][1]),
    xt::range(m_Pad[2][0], m_l.shape(2)-m_Pad[2][1]));

  // apply renumbering: merges clusters
  for (auto& i: m_l)
    i = renum[i];

  // rename labels to lowest possible label starting from 1)
  xt::xtensor<int,1> labels = xt::unique(m_l);
  xt::view(renum, xt::keep(labels)) = xt::arange<int>(labels.size());
  for (auto& i: m_l)
    i = renum[i];

  // make periodic
  // -------------

  // apply periodicity
  if (m_periodic)
  {
    // pad image
    m_l = xt::pad(m_l, m_Pad, pad_mode, pad_value);

    // list to renumber: used to link clusters to each other
    renum = xt::arange<int>(m_l.size());

    // loop over the image
    for (size_t h = m_Pad[0][0]; h < m_l.shape(0)-m_Pad[0][1]; ++h) {
      for (size_t i = m_Pad[1][0]; i < m_l.shape(1)-m_Pad[1][1]; ++i) {
        for (size_t j = m_Pad[2][0]; j < m_l.shape(2)-m_Pad[2][1]; ++j) {
          // - skip background voxels
          if (!m_l(h,i,j))
            continue;
          // - get current labels in the ROI
          auto Li = xt::view(m_l,
            xt::range(h-m_Pad[0][0], h+m_Pad[0][1]+1),
            xt::range(i-m_Pad[1][0], i+m_Pad[1][1]+1),
            xt::range(j-m_Pad[2][0], j+m_Pad[2][1]+1));
          // - apply kernel to the labels in the ROI
          auto Ni = Li * m_kernel;
          // - extract one label
          int l = xt::amax(Ni)[0];
          // - check if clusters have to be merged, if not: continue to the next voxel
          if (xt::all(xt::equal(Li, l) || xt::equal(Li, 0)))
            continue;
          // - get the labels to be merged (discard 0 background)
          xt::xarray<int> merge = xt::where(xt::equal(Li, 0), l, Li);
          merge = xt::unique(merge);
          // - merge labels (apply merge to other labels in cluster)
          int linkto = xt::amin(xt::view(renum, xt::keep(merge)))[0];
          for (auto& a: merge)
            renum = xt::where(xt::equal(renum, renum(a)), linkto, renum);
        }
      }
    }

    // remove padding
    m_l = xt::view(m_l,
      xt::range(m_Pad[0][0], m_l.shape(0)-m_Pad[0][1]),
      xt::range(m_Pad[1][0], m_l.shape(1)-m_Pad[1][1]),
      xt::range(m_Pad[2][0], m_l.shape(2)-m_Pad[2][1]));

    // apply renumbering: merges clusters
    for (auto& i: m_l)
      i = renum[i];
  }

  // reset to original dimension
  m_l.reshape(m_shape);
  m_kernel.reshape(m_shape_kernel);
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> Clusters::get() const
{
  return m_l;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> clusters(const xt::xarray<int>& f, bool periodic)
{
  return Clusters(f, GooseEYE::kernel::nearest(f.dimension()), periodic).get();
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

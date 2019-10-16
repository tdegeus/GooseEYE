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

  // read/convert input
  m_shape = detail::shape(f);
  m_shape_kernel = detail::shape(m_kernel);
  m_pad = detail::pad_width(m_kernel);
  m_Shape = detail::as_dim(MAX_DIM, m_shape, 1);
  m_Shape_kernel = detail::as_dim(MAX_DIM, m_shape_kernel, 1);
  m_Pad = detail::as_dim(MAX_DIM, m_pad, 0);

  // make pseudo 3-d
  m_l.reshape(m_Shape);
  m_kernel.reshape(m_Shape_kernel);

  // compute clusters (labels >= 2)
  this->compute();

  // connect labels periodically
  if (m_periodic) {
    m_l_np = m_l;
    this->compute();
  }

  // rename labels to lowest possible label starting from 1
  xt::xtensor<int,1> labels = xt::unique(m_l);
  xt::xtensor<int,1> renum = xt::empty<int>({m_l.size()});
  xt::view(renum, xt::keep(labels)) = xt::arange<int>(labels.size());
  for (auto& i: m_l) {
    i = renum[i];
  }
}

// -------------------------------------------------------------------------------------------------

inline void Clusters::compute()
{
  // padding default not periodic: mask padded items
  xt::pad_mode pad_mode = xt::pad_mode::constant;
  int pad_value = 0;

  // padding optionally periodic
  if (m_periodic)
    pad_mode = xt::pad_mode::periodic;

  // apply padding
  // note that "m_l" contains the labels, but also the image:
  // 0: background, 1: unlabelled, >= 2: labels
  m_l = xt::pad(m_l, m_Pad, pad_mode, pad_value);

  // first new label (start at 2 to distinguish: 0 = background, 1 = unlabelled)
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
        // - draw a new label, only if there is no previous label (>= 2)
        if (l == 1) {
          l = ilab;
          ++ilab;
        }
        // - apply label to all unlabelled voxels
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

  // remove padding
  m_l = xt::view(m_l,
    xt::range(m_Pad[0][0], m_l.shape(0)-m_Pad[0][1]),
    xt::range(m_Pad[1][0], m_l.shape(1)-m_Pad[1][1]),
    xt::range(m_Pad[2][0], m_l.shape(2)-m_Pad[2][1]));

  // apply renumbering: merges clusters
  for (auto& i: m_l)
    i = renum[i];
}

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<size_t,1> Clusters::relabel(
  const xt::xarray<int>& a,
  const xt::xarray<int>& b) const
{
  size_t nb = xt::amax(b)[0] + 1;

  xt::xtensor<size_t,1> out = xt::zeros<size_t>({nb});

  for (size_t h = 0; h < a.shape(0); ++h) {
    for (size_t i = 0; i < a.shape(1); ++i) {
      for (size_t j = 0; j < a.shape(2); ++j) {
        out(b(h, i, j)) = a(h, i, j);
      }
    }
  }

  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<size_t> Clusters::average_position(const xt::xarray<int>& lab) const
{
  // number of labels
  size_t n = xt::amax(lab)(0) + 1;

  // allocate position average
  xt::xarray<size_t> x = xt::zeros<size_t>({n, (size_t)4});

  // loop over the image
  for (size_t h = 0; h < lab.shape(0); ++h) {
    for (size_t i = 0; i < lab.shape(1); ++i) {
      for (size_t j = 0; j < lab.shape(2); ++j) {
        // get label
        int l = lab(h, i, j);
        // update average position
        if (l) {
          x(l, 0) += h;
          x(l, 1) += i;
          x(l, 2) += j;
          x(l, 3) += 1;
        }
      }
    }
  }

  // average position
  xt::xarray<size_t> xbar = xt::zeros<size_t>({n, (size_t)3});

  // fill the centers of gravity
  for (size_t i = 0; i < x.shape(0); ++i) {
    if (x(i,3) > 0) {
      for (size_t j = 0; j < 3; ++j) {
        xbar(i, j) = (size_t)round((double)x(i, j) / (double)x(i, 3));
      }
    }
  }

  return xbar;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> Clusters::centers_periodic() const
{
  // get relabelling "m_l_np" -> "m_l"
  auto relabel = this->relabel(m_l, m_l_np);

  // compute position average for the non-periodic labels
  xt::xarray<size_t> x_np = this->average_position(m_l_np);

  // get half size
  std::vector<size_t> mid = detail::half_shape(m_Shape);

  // set shift to apply
  xt::xarray<int> shift = xt::zeros<int>({x_np.shape(0), (size_t)3});

  // apply shift if needed
  for (size_t i = 0; i < shift.shape(0); ++i) {
    for (size_t j = 0; j < shift.shape(1); ++j) {
      if (x_np(i, j) > mid[j]) {
        shift(i, j) = - m_Shape[j];
      }
    }
  }

  // number of labels
  size_t n = xt::amax(m_l)(0) + 1;

  // allocate position average
  xt::xarray<int> x = xt::zeros<int>({n, (size_t)4});

  // loop over the image
  for (size_t h = 0; h < m_l.shape(0); ++h) {
    for (size_t i = 0; i < m_l.shape(1); ++i) {
      for (size_t j = 0; j < m_l.shape(2); ++j) {
        // get label
        int l = m_l_np(h, i, j);
        // update average position
        if (l) {
          x(relabel(l), 0) += h + shift(l, 0);
          x(relabel(l), 1) += i + shift(l, 1);
          x(relabel(l), 2) += j + shift(l, 2);
          x(relabel(l), 3) += 1;
        }
      }
    }
  }

  // average position
  xt::xarray<int> xbar = xt::zeros<int>({n, (size_t)3});

  // fill the centers of gravity
  for (size_t i = 0; i < x.shape(0); ++i) {
    if (x(i,3) > 0) {
      for (size_t j = 0; j < 3; ++j) {
        int xi = (int)round((double)x(i, j) / (double)x(i, 3));
        if (xi < 0) {
          xi += m_Shape[j];
        }
        xbar(i, j) = xi;
      }
    }
  }

  // allocate centers of gravity
  xt::xarray<int> c = xt::zeros<int>(m_l.shape());

  // fill the centers of gravity
  for (size_t l = 1; l < x.shape(0); ++l) {
    c(xbar(l,0), xbar(l,1), xbar(l,2)) = l;
  }

  return c;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> Clusters::centers() const
{
  // return centers for a periodic image
  if (m_periodic) {
    return centers_periodic();
  }

  // compute position average
  xt::xarray<size_t> x = this->average_position(m_l);

  // allocate centers of gravity
  xt::xarray<int> c = xt::zeros<int>(m_l.shape());

  // fill the centers of gravity
  for (size_t l = 1; l < x.shape(0); ++l) {
    c(x(l,0), x(l,1), x(l,2)) = l;
  }

  return c;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> Clusters::labels() const
{
  auto out = m_l;
  out.reshape(m_shape);
  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> clusters(const xt::xarray<int>& f, bool periodic)
{
  return Clusters(f, GooseEYE::kernel::nearest(f.dimension()), periodic).labels();
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

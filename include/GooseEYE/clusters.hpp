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
  bool periodic) :
  Clusters(f, kernel::nearest(f.dimension()), periodic)
{
}

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

inline xt::xtensor<double,2> Clusters::average_position(const xt::xarray<int>& lab) const
{
  // number of labels
  size_t N = xt::amax(lab)(0) + 1;

  // allocate average position
  xt::xtensor<double,2> x = xt::zeros<double>({N, (size_t)3});
  xt::xtensor<double,1> n = xt::zeros<double>({N});

  // loop over the image
  for (size_t h = 0; h < lab.shape(0); ++h) {
    for (size_t i = 0; i < lab.shape(1); ++i) {
      for (size_t j = 0; j < lab.shape(2); ++j) {
        // get label
        int l = lab(h, i, j);
        // update average position
        if (l) {
          x(l, 0) += (double)h;
          x(l, 1) += (double)i;
          x(l, 2) += (double)j;
          n(l) += 1.0;
        }
      }
    }
  }

  // avoid zero division
  n = xt::where(xt::equal(n, 0), 1, n);

  // normalise
  for (size_t i = 0; i < x.shape(1); ++i) {
    auto xi = xt::view(x, xt::all(), i);
    xi = xi / n;
  }

  return x;
}

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<double,2> Clusters::average_position_periodic() const
{
  // get relabelling "m_l_np" -> "m_l"
  auto relabel = relabel_map(m_l_np, m_l);

  // compute average position for the non-periodic labels
  auto x_np = this->average_position(m_l_np);

  // get half size
  auto mid = detail::half_shape(m_Shape);

  // initialise shift to apply
  xt::xtensor<double,2> shift = xt::zeros<double>({x_np.shape(0), (size_t)3});

  // check to apply shift
  for (size_t i = 0; i < shift.shape(0); ++i) {
    for (size_t j = 0; j < shift.shape(1); ++j) {
      if (x_np(i, j) > mid[j]) {
        shift(i, j) = - (double)m_Shape[j];
      }
    }
  }

  // number of labels
  size_t N = xt::amax(m_l)(0) + 1;

  // allocate average position
  xt::xtensor<double,2> x = xt::zeros<double>({N, (size_t)3});
  xt::xtensor<double,1> n = xt::zeros<double>({N});

  // loop over the image
  for (size_t h = 0; h < m_l.shape(0); ++h) {
    for (size_t i = 0; i < m_l.shape(1); ++i) {
      for (size_t j = 0; j < m_l.shape(2); ++j) {
        // get label
        int l = m_l_np(h, i, j);
        // update average position
        if (l) {
          x(relabel(l), 0) += (double)h + shift(l, 0);
          x(relabel(l), 1) += (double)i + shift(l, 1);
          x(relabel(l), 2) += (double)j + shift(l, 2);
          n(relabel(l)) += 1.0;
        }
      }
    }
  }

  // avoid zero division
  n = xt::where(xt::equal(n, 0), 1, n);

  // normalise
  for (size_t i = 0; i < x.shape(1); ++i) {
    auto xi = xt::view(x, xt::all(), i);
    xi = xi / n;
    xi = xt::where(xi < 0, xi + m_Shape[i], xi);
  }

  return x;
}

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<double,2> Clusters::center_positions(bool as3d) const
{
  xt::xtensor<double,2> x;

  if (m_periodic) {
    x = this->average_position_periodic();
  }
  else {
    x = this->average_position(m_l);
  }

  if (as3d) {
    return x;
  }

  return xt::view(x, xt::all(), xt::range(3 - m_shape.size(), 3));

  return x;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> Clusters::centers() const
{
  // get positions of the centers
  xt::xtensor<size_t,2> x = xt::floor(this->center_positions(true));

  // allocate centers of gravity
  xt::xarray<int> c = xt::zeros<int>(m_l.shape());

  // fill the centers of gravity
  for (size_t l = 1; l < x.shape(0); ++l) {
    c(x(l,0), x(l,1), x(l,2)) = l;
  }

  // return using the original rank
  c.reshape(m_shape);
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

inline xt::xtensor<size_t,1> Clusters::sizes() const
{
  xt::xtensor<size_t,1> out = xt::zeros<size_t>({xt::amax(m_l)(0) + 1ul});

  for (size_t h = 0; h < m_l.shape(0); ++h) {
    for (size_t i = 0; i < m_l.shape(1); ++i) {
      for (size_t j = 0; j < m_l.shape(2); ++j) {
        out(m_l(h,i,j))++;
      }
    }
  }

  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<int> clusters(const xt::xarray<int>& f, bool periodic)
{
  return Clusters(f, kernel::nearest(f.dimension()), periodic).labels();
}

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<size_t,1> relabel_map(const xt::xarray<int>& src, const xt::xarray<int>& dest)
{
  GOOSEEYE_ASSERT(src.shape() == dest.shape());

  size_t n_src = xt::amax(src)[0] + 1;

  auto shape = detail::shape_as_dim(3, dest, 1);

  xt::xtensor<size_t,1> out = xt::zeros<size_t>({n_src});

  for (size_t h = 0; h < shape[0]; ++h) {
    for (size_t i = 0; i < shape[1]; ++i) {
      for (size_t j = 0; j < shape[2]; ++j) {
        out(src(h, i, j)) = dest(h, i, j);
      }
    }
  }

  return out;
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

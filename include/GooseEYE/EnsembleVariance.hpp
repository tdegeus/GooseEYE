/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLEVARIANCE_HPP
#define GOOSEEYE_ENSEMBLEVARIANCE_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// =================================================================================================
// constructor
// =================================================================================================

inline EnsembleVariance::EnsembleVariance(const std::vector<size_t>& roi, bool periodic) :
m_periodic(periodic), m_shape(roi)
{
  GOOSEEYE_ASSERT(m_shape.size() <= MAX_DIM);

  m_pad = detail::pad_width(m_shape);
  m_Shape = detail::as_dim(MAX_DIM, m_shape, 1);
  m_Pad = detail::as_dim(MAX_DIM, m_pad, 0);

  m_first = xt::zeros<double>(m_Shape);
  m_second = xt::zeros<double>(m_Shape);
  m_norm = xt::zeros<double>(m_Shape);
}

// =================================================================================================
// return normalised result, or raw data
// =================================================================================================

inline xt::xarray<double> EnsembleVariance::result() const
{
  xt::xarray<double> I = xt::ones<double>(m_norm.shape());
  xt::xarray<double> norm = xt::where(m_norm <= 0., I, m_norm);
  xt::xarray<double> out = m_first / norm;
  out.reshape(m_shape);

  if (m_stat == Type::roughness)
    out = xt::pow(out, 0.5);

  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> EnsembleVariance::variance() const
{
  xt::xarray<double> I = xt::ones<double>(m_norm.shape());
  xt::xarray<double> norm = xt::where(m_norm <= 0., I, m_norm);
  xt::xarray<double> out = (m_second / norm - xt::pow(m_first / norm, 2.0)) * norm / (norm - 1);
  out.reshape(m_shape);

  if (m_stat == Type::roughness)
    out = xt::pow(out, 0.5);

  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> EnsembleVariance::distance(size_t dim) const
{
  GOOSEEYE_ASSERT(dim < m_shape.size());

  dim += MAX_DIM - m_shape.size();

  xt::xarray<double> out = xt::empty<double>(m_Shape);

  double start = -1 * static_cast<double>(m_Pad[dim][0]);
  double stop  =      static_cast<double>(m_Pad[dim][1]);

  xt::xarray<double> D = xt::linspace<double>(start, stop, m_Shape[dim]);

  for (size_t h = 0; h < m_Shape[0]; ++h) {
    for (size_t i = 0; i < m_Shape[1]; ++i) {
      for (size_t j = 0; j < m_Shape[2]; ++j) {
        if (dim == 0)
          out(h,i,j) = D(h);
        else if (dim == 1)
          out(h,i,j) = D(i);
        else if (dim == 2)
          out(h,i,j) = D(j);
      }
    }
  }

  out.reshape(m_shape);

  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> EnsembleVariance::distance() const
{
  xt::xarray<double> out = xt::zeros<double>(m_shape);

  for (size_t i = 0; i < m_shape.size(); ++i)
    out += xt::pow(distance(i), 2.);

  return xt::pow(out, .5);
}

// =================================================================================================

} // namespace ...

#endif

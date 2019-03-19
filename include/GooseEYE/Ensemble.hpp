/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_HPP
#define GOOSEEYE_ENSEMBLE_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// =================================================================================================
// constructor
// =================================================================================================

inline Ensemble::Ensemble(const std::vector<size_t>& roi, bool periodic) :
m_periodic(periodic), m_shape(roi)
{
  GOOSEEYE_ASSERT(m_shape.size() <= MAX_DIM);

  m_pad = detail::pad_width(m_shape);
  m_Shape = detail::as_dim(MAX_DIM, m_shape, 1);
  m_Pad = detail::as_dim(MAX_DIM, m_pad, 0);

  m_data = xt::zeros<double>(m_Shape);
  m_norm = xt::zeros<double>(m_Shape);
}

// =================================================================================================
// return normalised result, or raw data
// =================================================================================================

inline xt::xarray<double> Ensemble::result() const
{
  xt::xarray<double> I = xt::ones<double>(m_norm.shape());
  xt::xarray<double> norm = xt::where(m_norm <= 0., I, m_norm);
  xt::xarray<double> out = m_data / norm;
  out.reshape(m_shape);
  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> Ensemble::data() const
{
  xt::xarray<double> out = m_data;
  out.reshape(m_shape);
  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> Ensemble::norm() const
{
  xt::xarray<double> out = m_norm;
  out.reshape(m_shape);
  return out;
}

// -------------------------------------------------------------------------------------------------

inline xt::xarray<double> Ensemble::distance(size_t dim) const
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

inline xt::xarray<double> Ensemble::distance() const
{
  xt::xarray<double> out = xt::zeros<double>(m_shape);

  for (size_t i = 0; i < m_shape.size(); ++i)
    out += xt::pow(distance(i), 2.);

  return xt::pow(out, .5);
}

// =================================================================================================

} // namespace ...

#endif

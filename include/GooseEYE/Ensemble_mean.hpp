/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_MEAN_HPP
#define GOOSEEYE_ENSEMBLE_MEAN_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

template <class T>
void Ensemble::mean(const xt::xarray<T>& f)
{
  GOOSEEYE_ASSERT(m_Shape == std::vector<size_t>(MAX_DIM,1));
  GOOSEEYE_ASSERT(m_stat == Type::mean || m_stat == Type::Unset);

  m_stat = Type::mean;

  m_first(0) += static_cast<double>(xt::sum(f)[0]);
  m_second(0) += static_cast<double>(xt::sum(xt::pow(f,2.0))[0]);
  m_norm(0) += static_cast<double>(f.size());
}

// -------------------------------------------------------------------------------------------------

template <class T>
void Ensemble::mean(const xt::xarray<T>& f, const xt::xarray<int>& fmask)
{
  GOOSEEYE_ASSERT(f.shape() == fmask.shape());
  GOOSEEYE_ASSERT(xt::all(xt::equal(fmask,0) || xt::equal(fmask,1)));
  GOOSEEYE_ASSERT(m_Shape == std::vector<size_t>(MAX_DIM,1));
  GOOSEEYE_ASSERT(m_stat == Type::mean || m_stat == Type::Unset);

  m_stat = Type::mean;

  m_first(0) += static_cast<double>(xt::sum(xt::where(xt::equal(fmask, 0), f, 0.0))[0]);
  m_second(0) += static_cast<double>(xt::sum(xt::where(xt::equal(fmask, 0), xt::pow(f,2.0), 0.0))[0]);
  m_norm(0) += static_cast<double>(xt::sum(1 - fmask)[0]);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

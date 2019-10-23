/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP
#define GOOSEEYE_ENSEMBLE_STAMPPOINTS_HPP

#include "GooseEYE.h"

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

inline xt::xtensor<size_t,2> Ensemble::stampPoints() const
{
  // Initialize roi-sized matrix to 1
  xt::xtensor<size_t,3> r = xt::ones<size_t>(m_Shape);

  // Determine inner pixels (account for quasi-3D images)
  auto ix = m_Shape[0] > 1 ? xt::range(1, m_Shape[0]-1) : xt::range(0, m_Shape[0]);
  auto iy = m_Shape[1] > 1 ? xt::range(1, m_Shape[1]-1) : xt::range(0, m_Shape[1]);
  auto iz = m_Shape[2] > 1 ? xt::range(1, m_Shape[2]-1) : xt::range(0, m_Shape[2]);

  // Set inner pixels to 0
  xt::view(r, ix, iy, iz) = 0;

  // Get stamp
  xt::xtensor<size_t,2> stamp = xt::from_indices(xt::argwhere(r));

  return stamp;
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

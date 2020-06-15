/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_ENSEMBLE_C2_HPP
#define GOOSEEYE_ENSEMBLE_C2_HPP

#include "GooseEYE.h"

namespace GooseEYE {

inline void Ensemble::C2(
    const xt::xarray<int>& f,
    const xt::xarray<int>& g,
    const xt::xarray<int>& fmask,
    const xt::xarray<int>& gmask)
{
    GOOSEEYE_ASSERT(f.shape() == g.shape());
    GOOSEEYE_ASSERT(f.shape() == fmask.shape());
    GOOSEEYE_ASSERT(f.shape() == gmask.shape());
    GOOSEEYE_ASSERT(f.dimension() == m_shape_orig.size());
    GOOSEEYE_ASSERT(xt::all(xt::equal(fmask, 0) || xt::equal(fmask, 1)));
    GOOSEEYE_ASSERT(xt::all(xt::equal(gmask, 0) || xt::equal(gmask, 1)));
    GOOSEEYE_ASSERT(m_stat == Type::C2 || m_stat == Type::Unset);

    // lock statistic
    m_stat = Type::C2;

    // not periodic (default): mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int mask_value = 1;

    // periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
        mask_value = 0;
    }

    // apply padding
    xt::xtensor<int,3> F = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    xt::xtensor<int,3> G = xt::pad(xt::atleast_3d(g), m_pad, pad_mode);
    xt::xtensor<int,3> Fmask = xt::pad(xt::atleast_3d(fmask), m_pad, xt::pad_mode::constant, mask_value);
    xt::xtensor<int,3> Gmask = xt::pad(xt::atleast_3d(gmask), m_pad, xt::pad_mode::constant, mask_value);
    std::vector<size_t> shape = detail::shape(F);

    // compute correlation
    for (size_t h = m_pad[0][0]; h < shape[0] - m_pad[0][1]; ++h) {
        for (size_t i = m_pad[1][0]; i < shape[1] - m_pad[1][1]; ++i) {
            for (size_t j = m_pad[2][0]; j < shape[2] - m_pad[2][1]; ++j) {
                // - skip masked
                if (Fmask(h, i, j)) {
                    continue;
                }
                // - get comparison sub-matrix
                auto Gi = xt::view(G,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - get inverse of comparison mask
                auto Gmii = int(1) - xt::view(Gmask,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - correlation (account for mask)
                if (F(h, i, j) != 0) {
                    m_first += xt::where(xt::equal(F(h, i, j), Gi), Gmii, int(0));
                }
                // - normalisation
                m_norm += Gmii;
            }
        }
    }
}

inline void Ensemble::C2(const xt::xarray<int>& f, const xt::xarray<int>& g)
{
    xt::xarray<int> mask = xt::zeros<int>(f.shape());
    C2(f, g, mask, mask);
}

} // namespace GooseEYE

#endif

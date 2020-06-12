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
    GOOSEEYE_ASSERT(f.dimension() == m_shape.size());
    GOOSEEYE_ASSERT(xt::all(xt::equal(fmask, 0) || xt::equal(fmask, 1)));
    GOOSEEYE_ASSERT(xt::all(xt::equal(gmask, 0) || xt::equal(gmask, 1)));
    GOOSEEYE_ASSERT(m_stat == Type::C2 || m_stat == Type::Unset);

    // lock statistic
    m_stat = Type::C2;

    // padding default not periodic: mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int mask_value = 1;

    // padding optionally periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
        mask_value = 0;
    }

    // apply padding
    xt::xarray<int> F = xt::pad(f, m_pad, pad_mode);
    xt::xarray<int> G = xt::pad(g, m_pad, pad_mode);
    xt::xarray<int> Fmask = xt::pad(fmask, m_pad, xt::pad_mode::constant, mask_value);
    xt::xarray<int> Gmask = xt::pad(gmask, m_pad, xt::pad_mode::constant, mask_value);

    // make matrices virtually 3-d matrices
    std::vector<size_t> shape = detail::shape_as_dim(MAX_DIM, F, 1);
    F.reshape(shape);
    G.reshape(shape);
    Fmask.reshape(shape);
    Gmask.reshape(shape);

    // local output and normalisation
    xt::xarray<int> first = xt::zeros<int>(m_Shape);
    xt::xarray<int> norm = xt::zeros<int>(m_Shape);

    // compute correlation
    for (size_t h = m_Pad[0][0]; h < shape[0] - m_Pad[0][1]; ++h) {
        for (size_t i = m_Pad[1][0]; i < shape[1] - m_Pad[1][1]; ++i) {
            for (size_t j = m_Pad[2][0]; j < shape[2] - m_Pad[2][1]; ++j) {
                // - skip masked
                if (Fmask(h, i, j)) {
                    continue;
                }
                // - get comparison sub-matrix
                auto Gi = xt::view(G,
                    xt::range(h - m_Pad[0][0], h + m_Pad[0][1] + 1),
                    xt::range(i - m_Pad[1][0], i + m_Pad[1][1] + 1),
                    xt::range(j - m_Pad[2][0], j + m_Pad[2][1] + 1));
                // - get inverse of comparison mask
                auto Gmii = int(1) - xt::view(Gmask,
                    xt::range(h - m_Pad[0][0], h + m_Pad[0][1] + 1),
                    xt::range(i - m_Pad[1][0], i + m_Pad[1][1] + 1),
                    xt::range(j - m_Pad[2][0], j + m_Pad[2][1] + 1));
                // - correlation (account for mask)
                if (F(h, i, j) != 0) {
                    first += xt::where(xt::equal(F(h, i, j), Gi), Gmii, int(0));
                }
                // - normalisation
                norm += Gmii;
            }
        }
    }

    // add to ensemble average
    m_first += first;
    m_norm += norm;
}

inline void Ensemble::C2(const xt::xarray<int>& f, const xt::xarray<int>& g)
{
    xt::xarray<int> mask = xt::zeros<int>(f.shape());
    C2(f, g, mask, mask);
}

} // namespace GooseEYE

#endif

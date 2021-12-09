/**
\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the GPLv3 License.
*/

#ifndef GOOSEEYE_ENSEMBLE_S2_HPP
#define GOOSEEYE_ENSEMBLE_S2_HPP

#include "GooseEYE.h"

namespace GooseEYE {

template <class T, class M>
inline void Ensemble::S2(const T& f, const T& g, const M& fmask, const M& gmask)
{
    using value_type = typename T::value_type;
    using mask_type = typename M::value_type;

    static_assert(std::is_integral<mask_type>::value, "Integral mask required.");

    GOOSEEYE_ASSERT(xt::has_shape(f, g.shape()));
    GOOSEEYE_ASSERT(xt::has_shape(f, fmask.shape()));
    GOOSEEYE_ASSERT(xt::has_shape(f, gmask.shape()));
    GOOSEEYE_ASSERT(f.dimension() == m_shape_orig.size());
    GOOSEEYE_ASSERT(xt::all(xt::equal(fmask, 0) || xt::equal(fmask, 1)));
    GOOSEEYE_ASSERT(xt::all(xt::equal(gmask, 0) || xt::equal(gmask, 1)));
    GOOSEEYE_ASSERT(m_stat == Type::S2 || m_stat == Type::Unset);

    // lock statistic
    m_stat = Type::S2;

    // not periodic (default): mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int mask_value = 1;

    // periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
        mask_value = 0;
    }

    // apply padding
    xt::xtensor<value_type, 3> F = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    xt::xtensor<double, 3> Fd = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    xt::xtensor<double, 3> G = xt::pad(xt::atleast_3d(g), m_pad, pad_mode);
    xt::xtensor<mask_type, 3> Fmask =
        xt::pad(xt::atleast_3d(fmask), m_pad, xt::pad_mode::constant, mask_value);
    xt::xtensor<double, 3> Gmask =
        xt::pad(xt::atleast_3d(gmask), m_pad, xt::pad_mode::constant, mask_value);

    // compute correlation
    for (size_t h = m_pad[0][0]; h < F.shape(0) - m_pad[0][1]; ++h) {
        for (size_t i = m_pad[1][0]; i < F.shape(1) - m_pad[1][1]; ++i) {
            for (size_t j = m_pad[2][0]; j < F.shape(2) - m_pad[2][1]; ++j) {
                // - skip masked
                if (Fmask(h, i, j)) {
                    continue;
                }
                // - get comparison sub-matrix
                auto Gi = xt::view(
                    G,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - get inverse of comparison mask
                auto Gmii = 1.0 - xt::view(
                                      Gmask,
                                      xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                                      xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                                      xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - correlation (account for mask)
                if (F(h, i, j) != 0) {
                    m_first += Fd(h, i, j) * Gi * Gmii;
                }
                // - normalisation
                m_norm += Gmii;
            }
        }
    }
}

template <class T>
inline void Ensemble::S2(const T& f, const T& g)
{
    xt::xarray<int> mask = xt::zeros<int>(f.shape());
    S2(f, g, mask, mask);
}

} // namespace GooseEYE

#endif

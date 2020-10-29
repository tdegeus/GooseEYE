/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_ENSEMBLE_HEIGHTHEIGHT_HPP
#define GOOSEEYE_ENSEMBLE_HEIGHTHEIGHT_HPP

#include "GooseEYE.h"

namespace GooseEYE {

template <class T, class M>
inline void Ensemble::heightheight(const T& f, const M& fmask)
{
    using mask_type = typename M::value_type;

    static_assert(std::is_integral<typename M::value_type>::value, "Integral mask required.");

    GOOSEEYE_ASSERT(xt::has_shape(f, fmask.shape()));
    GOOSEEYE_ASSERT(f.dimension() == m_shape_orig.size());
    GOOSEEYE_ASSERT(xt::all(xt::equal(fmask, 0) || xt::equal(fmask, 1)));
    GOOSEEYE_ASSERT(m_stat == Type::heightheight || m_stat == Type::Unset);

    // lock statistic
    m_stat = Type::heightheight;

    // not periodic (default): mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int mask_value = 1;

    // periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
        mask_value = 0;
    }

    // apply padding
    xt::xtensor<double, 3> F = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    xt::xtensor<mask_type, 3> Fmask =
        xt::pad(xt::atleast_3d(fmask), m_pad, xt::pad_mode::constant, mask_value);
    xt::xtensor<double, 3> Fmaskd =
        xt::pad(xt::atleast_3d(fmask), m_pad, xt::pad_mode::constant, mask_value);

    // compute correlation
    for (size_t h = m_pad[0][0]; h < F.shape(0) - m_pad[0][1]; ++h) {
        for (size_t i = m_pad[1][0]; i < F.shape(1) - m_pad[1][1]; ++i) {
            for (size_t j = m_pad[2][0]; j < F.shape(2) - m_pad[2][1]; ++j) {
                // - skip masked
                if (Fmask(h, i, j)) {
                    continue;
                }
                // - get comparison sub-matrix
                auto Fi = xt::view(F,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - get inverse of comparison mask
                auto Fmii = 1.0 - xt::view(Fmaskd,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - update sum of the m_first moment
                m_first += xt::pow(Fi - F(h, i, j), 2.0) * Fmii;
                // - update sum of the m_second moment
                if (m_variance) {
                    m_second += xt::pow(Fi - F(h, i, j), 4.0) * Fmii;
                }
                // - normalisation
                m_norm += Fmii;
            }
        }
    }
}

template <class T>
inline void Ensemble::heightheight(const T& f)
{
    xt::xarray<int> mask = xt::zeros<int>(f.shape());
    heightheight(f, mask);
}

} // namespace GooseEYE

#endif

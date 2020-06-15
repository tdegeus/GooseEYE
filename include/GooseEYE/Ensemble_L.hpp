/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_ENSEMBLE_L_HPP
#define GOOSEEYE_ENSEMBLE_L_HPP

#include "GooseEYE.h"

namespace GooseEYE {

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
inline void Ensemble::L(const xt::xarray<T>& f, path_mode mode)
{
    GOOSEEYE_ASSERT(f.dimension() == m_shape_orig.size());
    GOOSEEYE_ASSERT(m_stat == Type::L || m_stat == Type::Unset);

    // lock statistics
    m_stat = Type::L;

    // not periodic (default): mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;

    // periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
    }

    // apply padding & convert to quasi-3d
    xt::xtensor<T,3> F = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    std::vector<size_t> shape = detail::shape(F);

    // ROI-shaped array used to extract a pixel stamp:
    // a set of end-points over which to loop and check the statics
    // - initialize to 1
    xt::xtensor<int,3> r = xt::ones<int>(m_shape);
    // - determine interior pixels (account for quasi-3D images)
    auto ix = m_shape[0] > 1 ? xt::range(1, m_shape[0] - 1) : xt::range(0, m_shape[0]);
    auto iy = m_shape[1] > 1 ? xt::range(1, m_shape[1] - 1) : xt::range(0, m_shape[1]);
    auto iz = m_shape[2] > 1 ? xt::range(1, m_shape[2] - 1) : xt::range(0, m_shape[2]);
    // - set interior pixels to 0
    xt::view(r, ix, iy, iz) = 0;

    // get stamp, from the matrix "r"
    xt::xtensor<int,2> stamp = xt::from_indices(xt::argwhere(r));
    for (size_t i = 0; i < MAX_DIM; ++i) {
        xt::view(stamp, xt::all(), xt::keep(i)) -= m_pad[i][0];
    }

    // correlation
    // N.B. getting the pixel paths is relatively expensive, so it is the output-most loop
    for (size_t istamp = 0; istamp < stamp.shape(0); ++istamp) {

        // pixel path between the center of the ROI and the current stamp point
        xt::xtensor<int,2> path = GooseEYE::path(
            {0, 0, 0},
            {stamp(istamp, 0), stamp(istamp, 1), stamp(istamp, 2)},
            mode);

        // compute correlation along this path, for the entire image
        for (size_t h = m_pad[0][0]; h < shape[0] - m_pad[0][1]; ++h) {
            for (size_t i = m_pad[1][0]; i < shape[1] - m_pad[1][1]; ++i) {
                for (size_t j = m_pad[2][0]; j < shape[2] - m_pad[2][1]; ++j) {
                    for (size_t p = 0; p < path.shape(0); ++p) {
                        // - get current relative position
                        int dh = path(p, 0);
                        int di = path(p, 1);
                        int dj = path(p, 2);
                        // - check to terminal walking along this path
                        if (!F(h + dh, i + di, j + dj)) {
                            break;
                        }
                        // - update the result
                        m_first(m_pad[0][0] + dh, m_pad[1][0] + di, m_pad[2][0] + dj) += 1.0;
                    }
                }
            }
        }

        // normalisation
        for (size_t p = 0; p < path.shape(0); ++p) {
            int dh = path(p, 0);
            int di = path(p, 1);
            int dj = path(p, 2);
            m_norm(m_pad[0][0] + dh, m_pad[1][0] + di, m_pad[2][0] + dj) += f.size();
        }
    }
}

} // namespace GooseEYE

#endif

/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_ENSEMBLE_W2C_HPP
#define GOOSEEYE_ENSEMBLE_W2C_HPP

#include "GooseEYE.h"

namespace GooseEYE {

template <class C, class T, class M>
inline void
Ensemble::W2c(const C& clusters, const C& centers, const T& f, const M& fmask, path_mode mode)
{
    using value_type = typename T::value_type;
    using mask_type = typename M::value_type;
    using cluster_type = typename C::value_type;

    static_assert(std::is_integral<cluster_type>::value, "Integral clusters required.");
    static_assert(std::is_integral<mask_type>::value, "Integral mask required.");

    GOOSEEYE_ASSERT(f.shape() == clusters.shape());
    GOOSEEYE_ASSERT(f.shape() == centers.shape());
    GOOSEEYE_ASSERT(xt::has_shape(f, fmask.shape()));
    GOOSEEYE_ASSERT(f.dimension() == m_shape_orig.size());
    GOOSEEYE_ASSERT(xt::all(xt::equal(fmask, 0) || xt::equal(fmask, 1)));
    GOOSEEYE_ASSERT(m_stat == Type::W2c || m_stat == Type::Unset);

    // lock statistic
    m_stat = Type::W2c;

    // not periodic (default): mask padded items
    xt::pad_mode pad_mode = xt::pad_mode::constant;

    // periodic: unmask padded items
    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
    }

    // apply padding
    array_type::tensor<value_type, 3> F = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    array_type::tensor<double, 3> Fd = xt::pad(xt::atleast_3d(f), m_pad, pad_mode);
    array_type::tensor<mask_type, 3> Fmask = xt::pad(xt::atleast_3d(fmask), m_pad, pad_mode);
    array_type::tensor<cluster_type, 3> Clusters =
        xt::pad(xt::atleast_3d(clusters), m_pad, pad_mode);
    array_type::tensor<cluster_type, 3> Centers = xt::pad(xt::atleast_3d(centers), m_pad, pad_mode);

    // ROI-shaped array used to extract a pixel stamp:
    // a set of end-points over which to loop and check the statics
    // - initialize to 1
    array_type::tensor<int, 3> r = xt::ones<int>(m_shape);
    // - determine interior pixels (account for quasi-3D images)
    auto ix = m_shape[0] > 1 ? xt::range(1, m_shape[0] - 1) : xt::range(0, m_shape[0]);
    auto iy = m_shape[1] > 1 ? xt::range(1, m_shape[1] - 1) : xt::range(0, m_shape[1]);
    auto iz = m_shape[2] > 1 ? xt::range(1, m_shape[2] - 1) : xt::range(0, m_shape[2]);
    // - set interior pixels to 0
    xt::view(r, ix, iy, iz) = 0;

    // get stamp, from the matrix "r"
    array_type::tensor<int, 2> stamp = xt::from_indices(xt::argwhere(r));
    for (size_t i = 0; i < MAX_DIM; ++i) {
        xt::view(stamp, xt::all(), xt::keep(i)) -= m_pad[i][0];
    }

    // correlation
    // N.B. getting the pixel paths is relatively expensive, so it is the output-most loop
    for (size_t istamp = 0; istamp < stamp.shape(0); ++istamp) {

        // pixel path between the center of the ROI and the current stamp point
        array_type::tensor<int, 2> path =
            GooseEYE::path({0, 0, 0}, {stamp(istamp, 0), stamp(istamp, 1), stamp(istamp, 2)}, mode);

        for (size_t h = m_pad[0][0]; h < F.shape(0) - m_pad[0][1]; ++h) {
            for (size_t i = m_pad[1][0]; i < F.shape(1) - m_pad[1][1]; ++i) {
                for (size_t j = m_pad[2][0]; j < F.shape(2) - m_pad[2][1]; ++j) {

                    auto label = Centers(h, i, j);
                    int q = -1;

                    // proceed only when the centre is inside the cluster
                    if (!label || Clusters(h, i, j) != label) {
                        continue;
                    }

                    for (size_t p = 0; p < path.shape(0); ++p) {

                        int dh = path(p, 0);
                        int di = path(p, 1);
                        int dj = path(p, 2);

                        // loop through the voxel-path until the end of a cluster
                        if (Clusters(h + dh, i + di, j + dj) != label && q < 0) {
                            q = 0;
                        }

                        // loop from the beginning of the path and store there
                        if (q >= 0) {
                            if (!Fmask(h + dh, i + di, j + dj)) {
                                m_norm(
                                    m_pad[0][0] + path(q, 0),
                                    m_pad[1][0] + path(q, 1),
                                    m_pad[1][0] + path(q, 2)) += 1;

                                m_first(
                                    m_pad[0][0] + path(q, 0),
                                    m_pad[1][0] + path(q, 1),
                                    m_pad[1][0] + path(q, 2)) += Fd(h + dh, i + di, j + dj);
                            }
                        }

                        q++;
                    }
                }
            }
        }
    }
}

template <class C, class T>
inline void Ensemble::W2c(const C& clusters, const C& centers, const T& f, path_mode mode)
{
    array_type::array<int> mask = xt::zeros<int>(f.shape());
    W2c(clusters, centers, f, mask, mode);
}

} // namespace GooseEYE

#endif

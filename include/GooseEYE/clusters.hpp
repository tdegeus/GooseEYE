/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_CLUSTERS_HPP
#define GOOSEEYE_CLUSTERS_HPP

#include "GooseEYE.h"

namespace GooseEYE {

template <typename T, typename U, typename V>
inline T pos2img(const T& img, const U& pos, const V& labels)
{
    GOOSEEYE_ASSERT(img.dimension() > 0);
    GOOSEEYE_ASSERT(img.dimension() <= 3);
    GOOSEEYE_ASSERT(img.dimension() == pos.shape(1));
    GOOSEEYE_ASSERT(pos.shape(0) == labels.size());
    GOOSEEYE_ASSERT(labels.dimension() == 1);

    using value_type = typename T::value_type;
    T res = img;

    if (res.dimension() == 1) {
        xt::view(res, xt::keep(pos)) = labels;
    }
    else if (res.dimension() == 2) {
        for (size_t i = 0; i < pos.shape(0); ++i) {
            res(pos(i, 0), pos(i, 1)) = static_cast<value_type>(labels(i));
        }
    }
    else {
        for (size_t i = 0; i < pos.shape(0); ++i) {
            res(pos(i, 0), pos(i, 1), pos(i, 2)) = static_cast<value_type>(labels(i));
        }
    }

    return res;
}

// For periodic algorithm, see:
// https://en.wikipedia.org/wiki/Center_of_mass#Systems_with_periodic_boundary_conditions
template <class T>
inline array_type::tensor<double, 2> center_of_mass(const T& labels, bool periodic)
{
    static_assert(std::is_integral<typename T::value_type>::value, "Integral labels required.");

    GOOSEEYE_ASSERT(labels.dimension() > 0);
    GOOSEEYE_ASSERT(xt::all(labels >= 0));

    double pi = xt::numeric_constants<double>::PI;
    size_t N = static_cast<size_t>(xt::amax(labels)(0)) + 1ul;
    size_t rank = labels.dimension();
    auto axes = detail::atleast_3d_axes(rank);
    array_type::tensor<double, 1> shape = xt::adapt(labels.shape());
    array_type::tensor<double, 2> ret = xt::zeros<double>({N, rank});

    for (size_t l = 0; l < N; ++l) {
        array_type::tensor<double, 2> positions =
            xt::from_indices(xt::argwhere(xt::equal(labels, l)));
        if (positions.size() == 0) {
            continue;
        }
        if (!periodic) {
            xt::view(ret, l, xt::all()) = xt::mean(positions, 0);
        }
        else {
            if (xt::all(xt::equal(positions, 0.0))) {
                continue;
            }
            auto theta = 2.0 * pi * positions / shape;
            auto xi = xt::cos(theta);
            auto zeta = xt::sin(theta);
            auto xi_bar = xt::mean(xi, 0);
            auto zeta_bar = xt::mean(zeta, 0);
            auto theta_bar = xt::atan2(-zeta_bar, -xi_bar) + pi;
            auto positions_bar = shape * theta_bar / (2.0 * pi);
            xt::view(ret, l, xt::all()) = positions_bar;
        }
    }

    return xt::view(ret, xt::all(), xt::keep(axes));
}

template <class T>
inline Clusters::Clusters(const T& f, bool periodic)
    : Clusters(f, kernel::nearest(f.dimension()), periodic)
{
}

template <class T, class S>
inline Clusters::Clusters(const T& f, const S& kernel, bool periodic) : m_periodic(periodic)
{
    static_assert(std::is_integral<typename T::value_type>::value, "Integral labels required.");
    static_assert(std::is_integral<typename S::value_type>::value, "Integral kernel required.");

    GOOSEEYE_ASSERT(xt::all(xt::equal(f, 0) || xt::equal(f, 1)));
    GOOSEEYE_ASSERT(xt::all(xt::equal(kernel, 0) || xt::equal(kernel, 1)));
    GOOSEEYE_ASSERT(f.dimension() == kernel.dimension());

    m_shape = detail::shape(f);
    m_kernel = xt::atleast_3d(kernel);
    m_pad = detail::pad_width(m_kernel);
    m_l = xt::atleast_3d(f);

    // note that "m_l" contains the labels, but also the image:
    // 0: background, 1: unlabelled, >= 2: labels
    this->compute();

    // connect labels periodically
    if (m_periodic) {
        m_l_np = m_l;
        this->compute();
    }

    // rename labels to lowest possible label starting from 1
    array_type::tensor<int, 1> labels = xt::unique(m_l);
    array_type::tensor<int, 1> renum = xt::empty<int>({m_l.size()});
    xt::view(renum, xt::keep(labels)) = xt::arange<int>(static_cast<int>(labels.size()));
    for (auto& i : m_l) {
        i = renum(i);
    }
}

inline void Clusters::compute()
{
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int pad_value = 0;

    if (m_periodic) {
        pad_mode = xt::pad_mode::periodic;
    }

    m_l = xt::pad(m_l, m_pad, pad_mode, pad_value);

    // first new label (start at 2 to distinguish: 0 = background, 1 = unlabelled)
    int ilab = 2;

    // list to renumber: used to link clusters to each other
    // N.B. By default the algorithm simply loops over the image, consequently it will miss that
    // clusters may touch further down in the image, labelling one cluster with several labels.
    // Using "renum" these touching clusters will glued and assigned one single label.
    array_type::tensor<int, 1> renum = xt::arange<int>(static_cast<int>(m_l.size()));

    for (size_t h = m_pad[0][0]; h < m_l.shape(0) - m_pad[0][1]; ++h) {
        for (size_t i = m_pad[1][0]; i < m_l.shape(1) - m_pad[1][1]; ++i) {
            for (size_t j = m_pad[2][0]; j < m_l.shape(2) - m_pad[2][1]; ++j) {
                // - skip background voxels
                if (!m_l(h, i, j)) {
                    continue;
                }
                // - get current labels in the ROI
                auto Li = xt::view(
                    m_l,
                    xt::range(h - m_pad[0][0], h + m_pad[0][1] + 1),
                    xt::range(i - m_pad[1][0], i + m_pad[1][1] + 1),
                    xt::range(j - m_pad[2][0], j + m_pad[2][1] + 1));
                // - apply kernel to the labels in the ROI
                auto Ni = Li * m_kernel;
                // - extract label to apply
                int l = xt::amax(Ni)();
                // - draw a new label, only if there is no previous label (>= 2)
                if (l == 1) {
                    l = ilab;
                    ++ilab;
                }
                // - apply label to all unlabelled voxels
                Li = xt::where(xt::equal(Ni, 1), l, Li);
                // - check if clusters have to be merged, if not: continue to the next voxel
                if (xt::all(xt::equal(Li, l) || xt::equal(Li, 0) || xt::equal(Li, 1))) {
                    continue;
                }
                // - get the labels to be merged
                //  (discard 0 and 1 by settings them to "l" in this copy)
                array_type::array<int> merge = xt::where(xt::less_equal(Li, 1), l, Li);
                merge = xt::unique(merge);
                // - merge labels (apply merge to other labels in cluster)
                int linkto = xt::amin(xt::view(renum, xt::keep(merge)))[0];
                for (auto& a : merge) {
                    renum = xt::where(xt::equal(renum, renum(a)), linkto, renum);
                }
            }
        }
    }

    // remove padding
    m_l = xt::view(
        m_l,
        xt::range(m_pad[0][0], m_l.shape(0) - m_pad[0][1]),
        xt::range(m_pad[1][0], m_l.shape(1) - m_pad[1][1]),
        xt::range(m_pad[2][0], m_l.shape(2) - m_pad[2][1]));

    // apply renumbering: merges clusters
    for (auto& i : m_l) {
        i = renum(i);
    }
}

template <class T>
inline array_type::tensor<double, 2> Clusters::average_position(const T& lab) const
{
    // number of labels
    size_t N = xt::amax(lab)() + 1;

    // allocate average position
    array_type::tensor<double, 2> x = xt::zeros<double>({N, size_t(3)});
    array_type::tensor<double, 1> n = xt::zeros<double>({N});

    for (size_t h = 0; h < lab.shape(0); ++h) {
        for (size_t i = 0; i < lab.shape(1); ++i) {
            for (size_t j = 0; j < lab.shape(2); ++j) {
                // get label
                int l = lab(h, i, j);
                // update average position
                if (l) {
                    x(l, 0) += (double)h;
                    x(l, 1) += (double)i;
                    x(l, 2) += (double)j;
                    n(l) += 1.0;
                }
            }
        }
    }

    // avoid zero division
    n = xt::where(xt::equal(n, 0), 1, n);

    // normalise
    for (size_t i = 0; i < x.shape(1); ++i) {
        auto xi = xt::view(x, xt::all(), i);
        xi = xi / n;
    }

    return x;
}

inline array_type::tensor<double, 2> Clusters::average_position_periodic() const
{
    // get relabelling "m_l_np" -> "m_l"
    auto relabel = relabel_map(m_l_np, m_l);

    // compute average position for the non-periodic labels
    auto x_np = this->average_position(m_l_np);

    // get half size
    auto mid = detail::half_shape(m_shape);

    // initialise shift to apply
    array_type::tensor<double, 2> shift = xt::zeros<double>({x_np.shape(0), size_t(3)});

    // check to apply shift
    for (size_t i = 0; i < shift.shape(0); ++i) {
        for (size_t j = 0; j < shift.shape(1); ++j) {
            if (x_np(i, j) > mid[j]) {
                shift(i, j) = -(double)m_shape[j];
            }
        }
    }

    // number of labels
    size_t N = xt::amax(m_l)() + 1;

    // allocate average position
    array_type::tensor<double, 2> x = xt::zeros<double>({N, size_t(3)});
    array_type::tensor<double, 1> n = xt::zeros<double>({N});

    for (size_t h = 0; h < m_l.shape(0); ++h) {
        for (size_t i = 0; i < m_l.shape(1); ++i) {
            for (size_t j = 0; j < m_l.shape(2); ++j) {
                // get label
                int l = m_l_np(h, i, j);
                // update average position
                if (l) {
                    x(relabel(l), 0) += (double)h + shift(l, 0);
                    x(relabel(l), 1) += (double)i + shift(l, 1);
                    x(relabel(l), 2) += (double)j + shift(l, 2);
                    n(relabel(l)) += 1.0;
                }
            }
        }
    }

    // avoid zero division
    n = xt::where(xt::equal(n, 0), 1, n);

    // normalise
    for (size_t i = 0; i < x.shape(1); ++i) {
        auto xi = xt::view(x, xt::all(), i);
        xi = xi / n;
        xi = xt::where(xi < 0, xi + m_shape[i], xi);
    }

    return x;
}

inline array_type::tensor<double, 2> Clusters::center_positions(bool as3d) const
{
    array_type::tensor<double, 2> x;

    if (m_periodic) {
        x = this->average_position_periodic();
    }
    else {
        x = this->average_position(m_l);
    }

    if (as3d) {
        return x;
    }

    array_type::tensor<size_t, 1> axes = detail::atleast_3d_axes(m_shape.size());
    return xt::view(x, xt::all(), xt::keep(axes));
}

inline array_type::array<int> Clusters::centers() const
{
    array_type::tensor<size_t, 2> x = xt::floor(this->center_positions(true));
    array_type::array<int> c = xt::zeros<int>(m_l.shape());

    for (int l = 1; l < static_cast<int>(x.shape(0)); ++l) {
        c(x(l, 0), x(l, 1), x(l, 2)) = l;
    }

    c.reshape(m_shape);
    return c;
}

inline array_type::array<int> Clusters::labels() const
{
    return xt::adapt(m_l.data(), m_shape);
}

inline array_type::tensor<size_t, 1> Clusters::sizes() const
{
    array_type::tensor<size_t, 1> ret = xt::zeros<size_t>({xt::amax(m_l)() + size_t(1)});

    for (size_t h = 0; h < m_l.shape(0); ++h) {
        for (size_t i = 0; i < m_l.shape(1); ++i) {
            for (size_t j = 0; j < m_l.shape(2); ++j) {
                ret(m_l(h, i, j))++;
            }
        }
    }

    return ret;
}

template <class T>
inline array_type::array<int> clusters(const T& f, bool periodic)
{
    return Clusters(f, kernel::nearest(f.dimension()), periodic).labels();
}

template <class T, class S>
inline array_type::tensor<size_t, 1> relabel_map(const T& src, const S& dest)
{
    GOOSEEYE_ASSERT(xt::has_shape(src, dest.shape()));

    array_type::tensor<size_t, 1> ret =
        xt::zeros<size_t>({static_cast<size_t>(xt::amax(src)() + 1)});
    auto A = xt::atleast_3d(src);
    auto B = xt::atleast_3d(dest);

    for (size_t h = 0; h < A.shape(0); ++h) {
        for (size_t i = 0; i < A.shape(1); ++i) {
            for (size_t j = 0; j < A.shape(2); ++j) {
                ret(A(h, i, j)) = B(h, i, j);
            }
        }
    }

    return ret;
}

} // namespace GooseEYE

#endif

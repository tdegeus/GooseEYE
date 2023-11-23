/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_DILATE_HPP
#define GOOSEEYE_DILATE_HPP

#include "GooseEYE.h"
#include <xtensor/xstrided_view.hpp>

namespace GooseEYE {

namespace detail {

template <class S, class U>
inline void periodic_copy_below_to_above(S&& F, const U& Pad)
{
    xt::xstrided_slice_vector sbelow(F.dimension());
    xt::xstrided_slice_vector sabove(F.dimension());

    for (size_t ax = 0; ax < F.dimension(); ++ax) {
        if (F.shape(ax) <= 1) {
            continue;
        }

        for (size_t i = 0; i < ax; ++i) {
            sbelow[i] = xt::all();
            sabove[i] = xt::all();
        }

        sbelow[ax] = xt::range(0, Pad[ax][0]);
        sabove[ax] = xt::range(F.shape(ax) - Pad[ax][1] - Pad[ax][0], F.shape(ax) - Pad[ax][1]);

        for (size_t i = ax + 1; i < F.dimension(); ++i) {
            sbelow[i] = xt::all();
            sabove[i] = xt::all();
        }

        auto below = xt::strided_view(F, sbelow);
        auto above = xt::strided_view(F, sabove);

        above = xt::where(xt::not_equal(below, 0), below, above);
    }
}

template <class S, class U>
inline void periodic_copy_above_to_below(S&& F, const U& Pad)
{
    xt::xstrided_slice_vector sbelow(F.dimension());
    xt::xstrided_slice_vector sabove(F.dimension());

    for (size_t ax = 0; ax < F.dimension(); ++ax) {
        if (F.shape(ax) <= 1) {
            continue;
        }

        for (size_t i = 0; i < ax; ++i) {
            sbelow[i] = xt::all();
            sabove[i] = xt::all();
        }

        sbelow[ax] = xt::range(Pad[ax][0], Pad[ax][0] + Pad[ax][1]);
        sabove[ax] = xt::range(F.shape(ax) - Pad[ax][1], F.shape(ax));

        for (size_t i = ax + 1; i < F.dimension(); ++i) {
            sbelow[i] = xt::all();
            sabove[i] = xt::all();
        }

        auto below = xt::strided_view(F, sbelow);
        auto above = xt::strided_view(F, sabove);

        below = xt::where(xt::not_equal(above, 0), above, below);
    }
}

template <class S, class U>
inline void periodic_copy(S&& F, const U& Pad)
{
    periodic_copy_above_to_below(F, Pad);
    periodic_copy_below_to_above(F, Pad);
}

} // namespace detail

template <
    class T,
    class S,
    std::enable_if_t<
        std::is_integral<typename T::value_type>::value &&
            std::is_integral<typename S::value_type>::value,
        int>>
inline T
dilate(const T& f, const S& kernel, const array_type::tensor<size_t, 1>& iterations, bool periodic)
{
    using value_type = typename T::value_type;
    GOOSEEYE_ASSERT(f.dimension() <= 3);
    GOOSEEYE_ASSERT(f.dimension() == kernel.dimension());
    GOOSEEYE_ASSERT(xt::all(xt::equal(kernel, 0) || xt::equal(kernel, 1)));
    GOOSEEYE_ASSERT(static_cast<size_t>(xt::amax(f)()) <= iterations.size() + 1);

    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int pad_value = 0;

    if (periodic) {
        pad_mode = xt::pad_mode::periodic;
    }

    array_type::tensor<typename S::value_type, 3> K = xt::atleast_3d(kernel);
    auto Pad = detail::pad_width(K);
    array_type::tensor<value_type, 3> F = xt::pad(xt::atleast_3d(f), Pad, pad_mode, pad_value);
    array_type::tensor<value_type, 3> G = F; // copy to list added labels added in the iteration

    for (size_t iter = 0; iter < xt::amax(iterations)(0); ++iter) {

        for (size_t h = Pad[0][0]; h < F.shape(0) - Pad[0][1]; ++h) {
            for (size_t i = Pad[1][0]; i < F.shape(1) - Pad[1][1]; ++i) {
                for (size_t j = Pad[2][0]; j < F.shape(2) - Pad[2][1]; ++j) {

                    // get label
                    value_type l = F(h, i, j);

                    // skip if needed:
                    // - do not dilate background or labels added in this iteration
                    // - check the maximum number of iterations per label
                    if (l == 0) {
                        continue;
                    }
                    if (iter >= iterations(l)) {
                        continue;
                    }

                    // get sub-matrix around (h, i, j)
                    auto Fi = xt::view(
                        F,
                        xt::range(h - Pad[0][0], h + Pad[0][1] + 1),
                        xt::range(i - Pad[1][0], i + Pad[1][1] + 1),
                        xt::range(j - Pad[2][0], j + Pad[2][1] + 1));

                    auto Gi = xt::view(
                        G,
                        xt::range(h - Pad[0][0], h + Pad[0][1] + 1),
                        xt::range(i - Pad[1][0], i + Pad[1][1] + 1),
                        xt::range(j - Pad[2][0], j + Pad[2][1] + 1));

                    // dilate where needed: dilated labels are added as negative numbers
                    Gi = xt::where(xt::equal(Fi, 0) && xt::equal(K, 1), l, Gi);
                }
            }
        }

        // apply periodicity
        if (periodic) {
            detail::periodic_copy(G, Pad);
        }

        // flip added label
        F = G;
        G = F;
    }

    // remove padding
    F = xt::view(
        F,
        xt::range(Pad[0][0], F.shape(0) - Pad[0][1]),
        xt::range(Pad[1][0], F.shape(1) - Pad[1][1]),
        xt::range(Pad[2][0], F.shape(2) - Pad[2][1]));

    T ret = f;
    std::copy(F.cbegin(), F.cend(), ret.begin());
    return ret;
}

template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int>>
inline T dilate(const T& f, const array_type::tensor<size_t, 1>& iterations, bool periodic)
{
    return dilate(f, kernel::nearest(f.dimension()), iterations, periodic);
}

template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int>>
inline T dilate(const T& f, size_t iterations, bool periodic)
{
    array_type::tensor<size_t, 1> iter = iterations * xt::ones<size_t>({xt::amax(f)(0) + 1ul});
    return dilate(f, kernel::nearest(f.dimension()), iter, periodic);
}

template <
    class T,
    class S,
    std::enable_if_t<
        std::is_integral<typename T::value_type>::value &&
            std::is_integral<typename S::value_type>::value,
        int>>
inline T dilate(const T& f, const S& kernel, size_t iterations, bool periodic)
{
    array_type::tensor<size_t, 1> iter = iterations * xt::ones<size_t>({xt::amax(f)(0) + 1ul});
    return dilate(f, kernel, iter, periodic);
}

} // namespace GooseEYE

#endif

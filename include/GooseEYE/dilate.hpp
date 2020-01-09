/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DILATE_HPP
#define GOOSEEYE_DILATE_HPP

#include "GooseEYE.h"
#include <xtensor/xstrided_view.hpp>

namespace GooseEYE {

// -------------------------------------------------------------------------------------------------

namespace detail {

template <class S, class T, class U>
void periodic_copy_above(S&& F, const T& Shape, const U& Pad)
{
    for (size_t axis = 0; axis < Shape.size(); ++axis)
    {
        if (Shape[axis] <= 1) {
            continue;
        }

        xt::xstrided_slice_vector sbelow;
        xt::xstrided_slice_vector sabove;

        for (size_t i = 0; i < axis; ++i) {
            sbelow.push_back(xt::all());
            sabove.push_back(xt::all());
        }

        sbelow.push_back(xt::range(0, Pad[axis][0]));

        sabove.push_back(xt::range(
                Shape[axis] + Pad[axis][0] - 1,
                Shape[axis] + 2 * Pad[axis][0] - 1));

        for (size_t i = axis + 1; i < Shape.size(); ++i) {
            sbelow.push_back(xt::all());
            sabove.push_back(xt::all());
        }

        auto below = xt::strided_view(F, sbelow);
        auto above = xt::strided_view(F, sabove);

        above = xt::where(xt::not_equal(below, 0), below, above);
    }
}

// ---

template <class S, class T, class U>
void periodic_copy_below(S&& F, const T& Shape, const U& Pad)
{
    for (size_t axis = 0; axis < Shape.size(); ++axis)
    {
        if (Shape[axis] <= 1) {
            continue;
        }

        xt::xstrided_slice_vector sbelow;
        xt::xstrided_slice_vector sabove;

        for (size_t i = 0; i < axis; ++i) {
            sbelow.push_back(xt::all());
            sabove.push_back(xt::all());
        }

        sbelow.push_back(xt::range(Pad[axis][0], Pad[axis][0] + Pad[axis][1]));
        sabove.push_back(xt::range(F.shape(axis) - Pad[axis][1], F.shape(axis)));

        for (size_t i = axis + 1; i < Shape.size(); ++i) {
            sbelow.push_back(xt::all());
            sabove.push_back(xt::all());
        }

        auto below = xt::strided_view(F, sbelow);
        auto above = xt::strided_view(F, sabove);

        below = xt::where(xt::not_equal(above, 0), above, below);
    }
}

// --

template <class S, class T, class U>
void periodic_copy(S&& F, const T& Shape, const U& Pad)
{
    periodic_copy_above(F, Shape, Pad);
    periodic_copy_below(F, Shape, Pad);
}

}

// -------------------------------------------------------------------------------------------------

template <
        class T,
        class S,
        std::enable_if_t<std::is_integral<T>::value, int>,
        std::enable_if_t<std::is_integral<S>::value, int>>
xt::xarray<T> dilate(
        const xt::xarray<T>& f,
        const xt::xarray<S>& kernel,
        const xt::xtensor<size_t,1>& iterations,
        bool periodic)
{
    GOOSEEYE_ASSERT(xt::all(xt::equal(kernel, 0) || xt::equal(kernel, 1)));
    GOOSEEYE_ASSERT(f.dimension() == kernel.dimension());
    GOOSEEYE_ASSERT((size_t)(xt::amax(f)(0)) <= iterations.size() + 1ul);

    // maximum number of dimensions
    static const size_t MAX_DIM=3;

    // read/convert input
    auto shape = detail::shape(f);
    auto shape_kernel = detail::shape(kernel);
    auto pad = detail::pad_width(kernel);
    auto Shape = detail::as_dim(MAX_DIM, shape, 1);
    auto Shape_kernel = detail::as_dim(MAX_DIM, shape_kernel, 1);
    auto Pad = detail::as_dim(MAX_DIM, pad, 0);

    // copy input, make pseudo 3-d
    auto F = f;
    auto K = kernel;
    F.reshape(Shape);
    K.reshape(Shape_kernel);

    // padding
    xt::pad_mode pad_mode = xt::pad_mode::constant;
    int pad_value = 0;

    if (periodic) {
        pad_mode = xt::pad_mode::periodic;
    }

    F = xt::pad(F, Pad, pad_mode, pad_value);

    // keep copy to check which labels were added in the iteration
    auto G = F;

    for (size_t iter = 0; iter < xt::amax(iterations)(0); ++iter)
    {

        for (size_t h = Pad[0][0]; h < F.shape(0) - Pad[0][1]; ++h) {
            for (size_t i = Pad[1][0]; i < F.shape(1) - Pad[1][1]; ++i) {
                for (size_t j = Pad[2][0]; j < F.shape(2) - Pad[2][1]; ++j) {

                    // get label
                    T l = F(h,i,j);

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
                    auto Fi = xt::view(F,
                            xt::range(h - Pad[0][0], h + Pad[0][1] + 1),
                            xt::range(i - Pad[1][0], i + Pad[1][1] + 1),
                            xt::range(j - Pad[2][0], j + Pad[2][1] + 1));

                    auto Gi = xt::view(G,
                            xt::range(h - Pad[0][0], h + Pad[0][1] + 1),
                            xt::range(i - Pad[1][0], i + Pad[1][1] + 1),
                            xt::range(j - Pad[2][0], j + Pad[2][1] + 1));

                  // dilate where needed: dilated labels are added as negative numbers
                  Gi = xt::where(xt::equal(Fi, 0) && xt::equal(kernel, 1), l, Gi);
                }
            }
        }

        // apply periodicity
        if (periodic) {
            detail::periodic_copy(G, Shape, Pad);
        }

        // flip added label
        F = G;
        G = F;

    }

    // remove padding
    F = xt::view(F,
            xt::range(Pad[0][0], F.shape(0) - Pad[0][1]),
            xt::range(Pad[1][0], F.shape(1) - Pad[1][1]),
            xt::range(Pad[2][0], F.shape(2) - Pad[2][1]));

    F.reshape(shape);

    return F;
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
        const xt::xarray<T>& f,
        const xt::xtensor<size_t,1>& iterations,
        bool periodic)
{
    return dilate(f, kernel::nearest(f.dimension()), iterations, periodic);
}

// -------------------------------------------------------------------------------------------------

template <class T, std::enable_if_t<std::is_integral<T>::value, int>>
xt::xarray<T> dilate(
        const xt::xarray<T>& f,
        size_t iterations,
        bool periodic)
{
    xt::xtensor<size_t,1> iter = iterations * xt::ones<size_t>({xt::amax(f)(0) + 1ul});
    return dilate(f, kernel::nearest(f.dimension()), iter, periodic);
}

// -------------------------------------------------------------------------------------------------

template <
        class T,
        class S,
        std::enable_if_t<std::is_integral<T>::value, int>,
        std::enable_if_t<std::is_integral<S>::value, int>>
xt::xarray<T> dilate(
        const xt::xarray<T>& f,
        const xt::xarray<S>& kernel,
        size_t iterations,
        bool periodic)
{
    xt::xtensor<size_t,1> iter = iterations * xt::ones<size_t>({xt::amax(f)(0) + 1ul});
    return dilate(f, kernel, iter, periodic);
}

// -------------------------------------------------------------------------------------------------

} // namespace ...

#endif

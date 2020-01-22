/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_DUMMY_CIRCLES_HPP
#define GOOSEEYE_DUMMY_CIRCLES_HPP

// =================================================================================================

#include "config.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// create a dummy image with circles at position "row","col" with radius "r"
// =================================================================================================

xt::xarray<int> dummy_circles(
    const std::vector<size_t>& shape,
    const xt::xtensor<int,1>& row,
    const xt::xtensor<int,1>& col,
    const xt::xtensor<int,1>& r,
    bool periodic)
{
    GOOSEEYE_ASSERT(row.shape() == col.shape());
    GOOSEEYE_ASSERT(row.shape() == r.shape());
    GOOSEEYE_ASSERT(shape.size() == 2);

    xt::xarray<int> out = xt::zeros<int>(shape);

    if (periodic)
    {
        for (size_t i = 0; i < row.size(); ++i)
        {
            for (int di = -r(i); di <= r(i); ++di)
            {
                for (int dj = -r(i); dj <= r(i); ++dj)
                {
                    int dr = (int)(ceil(pow((double)(pow(di, 2) + pow(dj, 2)), 0.5)));
                    if (dr < r(i))
                    {
                        out.periodic(row(i) + di, col(i) + dj) = 1;
                    }
                }
            }
        }

        return out;
    }

    for (size_t i = 0; i < row.size(); ++i)
    {
        for (int di = -r(i); di <= r(i); ++di)
        {
            for (int dj = -r(i); dj <= r(i); ++dj)
            {
                if (out.in_bounds(row(i) + di, col(i) + dj))
                {
                    int dr = (int)(ceil(pow((double)(pow(di, 2) + pow(dj, 2)), 0.5)));
                    if (dr < r(i))
                    {
                        out(row(i)+di, col(i)+dj) = 1;
                    }
                }
            }
        }
    }

    return out;
}

// =================================================================================================
// create a dummy image with a default number of circles
// at random positions and random radii
// =================================================================================================

xt::xarray<int> dummy_circles(const std::vector<size_t>& shape, bool periodic)
{
    GOOSEEYE_ASSERT(shape.size() == 2);

    std::srand((size_t)std::time(0));

    // set default: number of circles in both directions and (constant) radius
    size_t N = (size_t)(0.05 * (double)shape[0]);
    size_t M = (size_t)(0.05 * (double)shape[1]);
    size_t R = (size_t)(pow((0.3 * (double)(shape[0] * shape[1])) / (M_PI * (double)(N * M)), 0.5));

    xt::xtensor<int,1> row = xt::empty<int>({M * N});
    xt::xtensor<int,1> col = xt::empty<int>({M * N});
    xt::xtensor<int,1> r = xt::empty<int>({M * N});

    // define regular grid of circles
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < M; j++)
        {
            row[i * M + j] = (int)((double)i * (double)shape[0] / (double)N);
            col[i * M + j] = (int)((double)j * (double)shape[1] / (double)M);
            r[i * M + j] = (int)R;
        }
    }

    // distance between circles
    int dN = (int)(0.5 * (double)shape[0] / (double)N);
    int dM = (int)(0.5 * (double)shape[1] / (double)M);

    // randomly perturb circles (move in any direction, enlarge/shrink)
    for (size_t i = 0; i < N * M; i++)
    {
        row(i) += (int)(((double)(std::rand() % 2) - 0.5) * 2.0) * std::rand() % dN;
        col(i) += (int)(((double)(std::rand() % 2) - 0.5) * 2.0) * std::rand() % dM;
        r(i) = (int)(((double)(std::rand() % 100) / 100.0 * 2.0 + 0.1) * (double)(r(i)));
    }

    // convert to image
    return dummy_circles(shape, row, col, r, periodic);
}

// =================================================================================================

} // namespace ...

// =================================================================================================

#endif

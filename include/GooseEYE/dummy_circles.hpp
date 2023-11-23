/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_DUMMY_CIRCLES_HPP
#define GOOSEEYE_DUMMY_CIRCLES_HPP

#include "config.h"
#include <prrng.h>

namespace GooseEYE {

inline array_type::array<int> dummy_circles(
    const std::vector<size_t>& shape,
    const array_type::tensor<int, 1>& row,
    const array_type::tensor<int, 1>& col,
    const array_type::tensor<int, 1>& r,
    bool periodic)
{
    GOOSEEYE_ASSERT(row.shape() == col.shape());
    GOOSEEYE_ASSERT(row.shape() == r.shape());
    GOOSEEYE_ASSERT(shape.size() == 2);

    array_type::array<int> out = xt::zeros<int>(shape);

    if (periodic) {
        for (size_t i = 0; i < row.size(); ++i) {
            for (int di = -r(i); di <= r(i); ++di) {
                for (int dj = -r(i); dj <= r(i); ++dj) {
                    int dr = (int)(ceil(pow((double)(pow(di, 2) + pow(dj, 2)), 0.5)));
                    if (dr < r(i)) {
                        out.periodic(row(i) + di, col(i) + dj) = 1;
                    }
                }
            }
        }

        return out;
    }

    for (size_t i = 0; i < row.size(); ++i) {
        for (int di = -r(i); di <= r(i); ++di) {
            for (int dj = -r(i); dj <= r(i); ++dj) {
                if (out.in_bounds(row(i) + di, col(i) + dj)) {
                    int dr = (int)(ceil(pow((double)(pow(di, 2) + pow(dj, 2)), 0.5)));
                    if (dr < r(i)) {
                        out(row(i) + di, col(i) + dj) = 1;
                    }
                }
            }
        }
    }

    return out;
}

inline array_type::array<int>
dummy_circles(const std::vector<size_t>& shape, bool periodic, uint64_t seed = 0)
{
    GOOSEEYE_ASSERT(shape.size() == 2);
    prrng::pcg32 rng(seed);

    // set default: number of circles in both directions and (constant) radius
    size_t N = (size_t)(0.05 * (double)shape[0]);
    size_t M = (size_t)(0.05 * (double)shape[1]);
    size_t R = (size_t)(pow((0.3 * (double)(shape[0] * shape[1])) / (M_PI * (double)(N * M)), 0.5));

    array_type::tensor<int, 1> row = xt::empty<int>({M * N});
    array_type::tensor<int, 1> col = xt::empty<int>({M * N});
    array_type::tensor<int, 1> r = xt::empty<int>({M * N});

    // define regular grid of circles
    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < M; j++) {
            row[i * M + j] = (int)((double)i * (double)shape[0] / (double)N);
            col[i * M + j] = (int)((double)j * (double)shape[1] / (double)M);
            r[i * M + j] = (int)R;
        }
    }

    // distance between circles
    int dN = (int)(0.5 * (double)shape[0] / (double)N);
    int dM = (int)(0.5 * (double)shape[1] / (double)M);

    // randomly perturb circles (move in any direction, enlarge/shrink)
    for (size_t i = 0; i < N * M; i++) {
        row(i) += rng.randint(2 * dN) - dN;
        col(i) += rng.randint(2 * dM) - dM;
        r(i) = (double)r(i) * 2.0 * rng.random();
    }

    // convert to image
    return dummy_circles(shape, row, col, r, periodic);
}

} // namespace GooseEYE

#endif

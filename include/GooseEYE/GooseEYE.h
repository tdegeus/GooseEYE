/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

#include "config.h"
#include "detail.hpp"
#include "version.h"
#include <prrng.h>

namespace GooseEYE {

/**
 * Collect kernels.
 */
namespace kernel {

/**
 * Return kernel with nearest neighbours.
 * @param ndim Number of dimensions (rank).
 * @return The kernel.
 */
inline array_type::array<int> nearest(size_t ndim);

} // namespace kernel

/**
 * Different methods to compute a pixel-path.
 */
enum class path_mode {
    Bresenham, ///< Bresenham algorithm.
    actual, ///< The actual path.
    full ///< Similar to actual selecting every voxel that is crossed.
};

/**
 * Compute a path between two pixels.
 * @param x0 Pixel coordinate (e.g. {0, 0}).
 * @param x1 Pixel coordinate (e.g. {10, 5}).
 * @param mode Method to use (see "path_mode").
 * @return The path: the coordinate of one pixel per row.
 */
inline array_type::tensor<int, 2> path(
    const array_type::tensor<int, 1>& x0,
    const array_type::tensor<int, 1>& x1,
    path_mode mode = path_mode::Bresenham);

/**
 * Dummy image with circles.
 * @param shape Shape of the output image.
 * @param row The x-position of each circle.
 * @param col The y-position of each circle.
 * @param r The radius of each circle.
 * @param periodic Switch to assume image periodic.
 * @return The dummy image.
 */
inline array_type::array<int> dummy_circles(
    const std::vector<size_t>& shape,
    const array_type::tensor<int, 1>& row,
    const array_type::tensor<int, 1>& col,
    const array_type::tensor<int, 1>& r,
    bool periodic = true)
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

/**
 * Dummy image with circles. The positions and radii of the circles are randomly generated.
 * @param shape Shape of the output image.
 * @param periodic Switch to assume image periodic.
 * @param seed Seed for the random number generator.
 * @return The dummy image.
 */
inline array_type::array<int>
dummy_circles(const std::vector<size_t>& shape, bool periodic = true, uint64_t seed = 0)
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

/**
 * Dilate image. The input image can be binary (1), or have integer labels (>=1).
 * In each case the background are 0.
 * @param f The image (integer).
 * @param kernel The kernel with which to dilate (binary).
 * @param iterations Number of iterations per label.
 * @param periodic Switch to assume image periodic.
 * @return The dilated image.
 */
template <
    class T,
    class S,
    std::enable_if_t<
        std::is_integral<typename T::value_type>::value &&
            std::is_integral<typename S::value_type>::value,
        int> = 0>
inline T dilate(
    const T& f,
    const S& kernel,
    const array_type::tensor<size_t, 1>& iterations,
    bool periodic = true);

/**
 * Dilate image. Select "kernel::nearest" as kernel.
 * See above for parameters.
 */
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(const T& f, const array_type::tensor<size_t, 1>& iterations, bool periodic = true);

/**
 * Dilate image. Fixed number of iterations for all labels.
 * See above for parameters.
 */
template <
    class T,
    class S,
    std::enable_if_t<
        std::is_integral<typename T::value_type>::value &&
            std::is_integral<typename S::value_type>::value,
        int> = 0>
inline T dilate(const T& f, const S& kernel, size_t iterations = 1, bool periodic = true);

/**
 * Dilate image. Fixed number of iterations for all labels. Select "kernel::nearest" as kernel.
 * See above for parameters.
 */
template <class T, std::enable_if_t<std::is_integral<typename T::value_type>::value, int> = 0>
inline T dilate(const T& f, size_t iterations = 1, bool periodic = true);

/**
 * Find map to relabel from `a` to `b`.
 * @param a Image.
 * @param b Image.
 * @return List of length `max(a) + 1` with per label in `a` the corresponding label in `b`.
 */
template <class T, class S>
array_type::tensor<size_t, 1> relabel_map(const T& a, const S& b)
{
    GOOSEEYE_ASSERT(xt::has_shape(a, b.shape()));

    array_type::tensor<size_t, 1> ret = xt::zeros<size_t>({static_cast<size_t>(xt::amax(a)() + 1)});

    for (size_t i = 0; i < a.size(); ++i) {
        ret(a.flat(i)) = b.flat(i);
    }

    return ret;
}

/**
 * @brief Size per label.
 * @param f Image with labels (0..n).
 * @return List of size n + 1 with the size per label.
 */
template <class T>
array_type::tensor<size_t, 1> labels_sizes(const T& f)
{
    array_type::tensor<size_t, 1> ret = xt::zeros<size_t>({xt::amax(f)() + size_t(1)});

    for (size_t i = 0; i < f.size(); ++i) {
        ret(f.flat(i))++;
    }

    return ret;
}

/**
 * Compute clusters and obtain certain characteristic about them.
 */
class Clusters {
public:
    Clusters() = default;

    /**
     * Constructor. Use default kernel::nearest().
     *
     * @param f Image.
     * @param periodic Interpret image as periodic.
     */
    template <class T>
    Clusters(const T& f, bool periodic = true)
        : Clusters(f, kernel::nearest(f.dimension()), periodic)
    {
    }

    /**
     * Constructor.
     *
     * @param f Image.
     * @param kernel Kernel.
     * @param periodic Interpret image as periodic.
     */
    template <class T, class S>
    Clusters(const T& f, const S& kernel, bool periodic = true) : m_periodic(periodic)
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

    /**
     * Return labels (1..n).
     * @return 'Image'.
     */
    array_type::array<int> labels() const
    {
        return xt::adapt(m_l.data(), m_shape);
    }

    /**
     * Return label only in the center of gravity (1..n).
     * @return 'Image'.
     */
    array_type::array<int> centers() const
    {
        array_type::tensor<size_t, 2> x = xt::floor(this->center_positions(true));
        array_type::array<int> c = xt::zeros<int>(m_l.shape());

        for (int l = 1; l < static_cast<int>(x.shape(0)); ++l) {
            c(x(l, 0), x(l, 1), x(l, 2)) = l;
        }

        c.reshape(m_shape);
        return c;
    }

    /**
     * Return positions of the centers of gravity (in the original rank, or as 3-d).
     * @return List of positions.
     */
    array_type::tensor<double, 2> center_positions(bool as3d = false) const
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

    /**
     * Return size per cluster
     * @return List.
     */
    [[deprecated]] array_type::tensor<size_t, 1> sizes() const
    {
        GOOSEEYE_WARNING_PYTHON("Clusters.sizes() is deprecated, use labels_sizes() instead");
        return labels_sizes(m_l);
    }

private:
    void compute()
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
    array_type::tensor<double, 2> average_position(const T& lab) const
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

    array_type::tensor<double, 2> average_position_periodic() const
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

    static const size_t MAX_DIM = 3;
    std::vector<size_t> m_shape; // shape of the input image
    std::vector<std::vector<size_t>> m_pad;
    array_type::tensor<int, 3> m_kernel;
    bool m_periodic;
    array_type::tensor<int, 3> m_l; // labels (>= 1, 0 = background), 3-d
    array_type::tensor<int, 3> m_l_np; // labels before applying periodicity
};

/**
 * @brief Compute clusters.
 * @param f Image.
 * @param periodic Interpret image as periodic.
 * @return 'Image' with labels (1..n) for labels, 0 for background.
 */
template <class T>
array_type::array<int> clusters(const T& f, bool periodic = true)
{
    return Clusters(f, kernel::nearest(f.dimension()), periodic).labels();
}

/**
 * Convert positions to an image.
 * @param img The image.
 * @param positions The position.
 * @param labels The label to apply for each image.
 * @return The image, with labels inserted (overwritten) at the positions.
 */
template <typename T, typename U, typename V>
inline T pos2img(const T& img, const U& positions, const V& labels)
{
    GOOSEEYE_ASSERT(img.dimension() > 0);
    GOOSEEYE_ASSERT(img.dimension() <= 3);
    GOOSEEYE_ASSERT(img.dimension() == positions.shape(1));
    GOOSEEYE_ASSERT(positions.shape(0) == labels.size());
    GOOSEEYE_ASSERT(labels.dimension() == 1);

    using value_type = typename T::value_type;
    T res = img;

    if (res.dimension() == 1) {
        xt::view(res, xt::keep(positions)) = labels;
    }
    else if (res.dimension() == 2) {
        for (size_t i = 0; i < positions.shape(0); ++i) {
            res(positions(i, 0), positions(i, 1)) = static_cast<value_type>(labels(i));
        }
    }
    else {
        for (size_t i = 0; i < positions.shape(0); ++i) {
            res(positions(i, 0), positions(i, 1), positions(i, 2)) =
                static_cast<value_type>(labels(i));
        }
    }

    return res;
}

/**
 * @brief Get the position of the center of each label.
 *
 * @details
 *      For periodic algorithm, see:
 *      https://en.wikipedia.org/wiki/Center_of_mass#Systems_with_periodic_boundary_conditions
 *
 * @param labels An image with labels [0..N].
 * @param periodic Switch to assume image periodic.
 * @return The position of the center of each label.
 */
template <class T>
array_type::tensor<double, 2> center_of_mass(const T& labels, bool periodic = true)
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

/**
 * Compute ensemble averaged statistics, by repetitively calling the member-function of a certain
 * statistical measure with different data.
 *
 * Note:
 * - One class instance can only be used to compute one statistical measure.
 * - Simple wrapper functions are provided to compute the statistical measure on one image.
 * - The normalized result can be read at any moment using 'result', without affecting the raw-data.
 * - The raw-data and normalization are separately stored as class member variables.
 */
class Ensemble {
public:
    /**
     * Constructor.
     */
    Ensemble() = default;

    /**
     * Constructor.
     * @param roi Shape of the 'region-of-interest' on which the statistics are computed.
     * @param periodic Switch to assume image periodic.
     * @param variance Switch to compute the variance together with the mean.
     */
    Ensemble(const std::vector<size_t>& roi, bool periodic = true, bool variance = true);

    /**
     * Get ensemble average.
     * @return The average along the 'region-of-interest' set at construction.
     */
    array_type::array<double> result() const;

    /**
     * Get ensemble variance.
     * @return The variance along the 'region-of-interest' set at construction.
     */
    array_type::array<double> variance() const;

    /**
     * Get raw-data: ensemble sum of the first moment: x_1 + x_2 + ...
     * @return The sum along the 'region-of-interest' set at construction.
     */
    array_type::array<double> data_first() const;

    /**
     * Get raw-data: ensemble sum of the second moment: x_1^2 + x_2^2 + ...
     * @return The sum along the 'region-of-interest' set at construction.
     */
    array_type::array<double> data_second() const;

    /**
     * Get raw-data: normalisation (number of measurements per pixel).
     * @return The norm along the 'region-of-interest' set at construction.
     */
    array_type::array<double> norm() const;

    /**
     * Get the relative distance of each pixel in the 'region-of-interest' to its center.
     * @return The distances along the 'region-of-interest' set at construction.
     */
    array_type::array<double> distance() const;

    /**
     * Get the relative distance of each pixel in the 'region-of-interest' to its center,
     * along a specific axis (distance can now be positive and negative).
     * @param axis Select axis.
     * @return The distances along the 'region-of-interest' set at construction.
     */
    array_type::array<double> distance(size_t axis) const;

    /**
     * Get the relative distance of each pixel in the 'region-of-interest' to its center.
     * Convert the distance to physical dimensions.
     * @param h The physical dimensions of one pixel (in each direction).
     * @return The distances along the 'region-of-interest' set at construction.
     */
    array_type::array<double> distance(const std::vector<double>& h) const;

    /**
     * Get the relative distance of each pixel in the 'region-of-interest' to its center,
     * along a specific axis (distance can now be positive and negative).
     * Convert the distance to physical dimensions.
     * @param h The physical dimensions of one pixel (in each direction).
     * @param axis Select axis.
     * @return The distances along the 'region-of-interest' set at construction.
     */
    array_type::array<double> distance(const std::vector<double>& h, size_t axis) const;

    /**
     * Add realization to arithmetic mean.
     * @param f The image.
     */
    template <class T>
    void mean(const T& f);

    /**
     * Add realization to arithmetic mean.
     * @param f The image.
     * @param fmask Mask certain pixels (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void mean(const T& f, const M& fmask);

    /**
     * Add realization to 2-point correlation: P(f(i) * g(i + di)).
     * @param f The image.
     * @param g The comparison image.
     */
    template <class T>
    void S2(const T& f, const T& g);

    /**
     * Add realization to 2-point correlation: P(f(i) * g(i + di)).
     * @param f The image.
     * @param g The comparison image.
     * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     * @param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void S2(const T& f, const T& g, const M& fmask, const M& gmask);

    /**
     * Add realization to 2-point cluster function: P(f(i) == g(i + di)).
     * @param f The image.
     * @param g The comparison image.
     */
    template <class T>
    void C2(const T& f, const T& g);

    /**
     * Add realization to 2-point cluster function: P(f(i) == g(i + di)).
     * @param f The image.
     * @param g The comparison image.
     * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     * @param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void C2(const T& f, const T& g, const M& fmask, const M& gmask);

    /**
     * Add realization to weighted 2-point correlation.
     * @param w The weights.
     * @param f The image.
     */
    template <class T>
    void W2(const T& w, const T& f);

    /**
     * Add realization to weighted 2-point correlation.
     * @param w The weights.
     * @param f The image.
     * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void W2(const T& w, const T& f, const M& fmask);

    /**
     * Add realization to collapsed weighted 2-point correlation
     * @param clusters The clusters.
     * @param centers The cluster-centers: label only at the center.
     * @param f The image.
     * @param mode Method to use (see "path_mode").
     */
    template <class C, class T>
    void
    W2c(const C& clusters, const C& centers, const T& f, path_mode mode = path_mode::Bresenham);

    /**
     * Add realization to collapsed weighted 2-point correlation
     * @param clusters The clusters.
     * @param centers The cluster-centers: label only at the center.
     * @param f The image.
     * @param mode Method to use (see "path_mode").
     * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
     */
    template <class C, class T, class M>
    void
    W2c(const C& clusters,
        const C& centers,
        const T& f,
        const M& fmask,
        path_mode mode = path_mode::Bresenham);

    /**
     * Add realization to height-height correlation.
     * The height-height correlation, at position \f$ \Delta i \f$, is defined as:
     * \f$ \frac{1}{N} \left( \sum_{i=1}^{N} (f_i - f_{i + \Delta i})^2 \right)^{1/2} \f$
     *
     * @param f The image.
     */
    template <class T>
    void heightheight(const T& f);

    /**
     * @copydoc heightheight(const T& f)
     * @param fmask Mask certain pixels of `f` (binary, 1: masked, 0: not masked).
     */
    template <class T, class M>
    void heightheight(const T& f, const M& fmask);

    /**
     * Add realization to lineal-path function
     * @param f The image.
     * @param mode Method to use (see "path_mode").
     */
    template <class T>
    void L(const T& f, path_mode mode = path_mode::Bresenham);

private:
    // Type: used to lock the ensemble to a certain measure.
    enum class Type { Unset, mean, S2, C2, W2, W2c, L, heightheight };

    // Initialize class as unlocked.
    Type m_stat = Type::Unset;

    // Maximum number of dimensions.
    static const size_t MAX_DIM = 3;

    // Switch to assume periodicity (for the entire ensemble).
    bool m_periodic;

    // Switch to compute the variance (for the entire ensemble).
    bool m_variance;

    // Raw (not normalized) result, and normalization:
    // - sum of the first moment: x_1 + x_2 + ...
    array_type::tensor<double, 3> m_first;
    // - sum of the second moment: x_1^2 + x_2^2 + ...
    array_type::tensor<double, 3> m_second;
    // - number of measurements per pixel
    array_type::tensor<double, 3> m_norm;

    // Shape of the region-of-interest, as specified.
    std::vector<size_t> m_shape_orig;

    // 3d equivalent of "m_shape_orig".
    std::vector<size_t> m_shape;

    // Pad size (3d).
    std::vector<std::vector<size_t>> m_pad;
};

// ---------------------------------------------------------
// Wrapper functions to compute the statistics for one image
// ---------------------------------------------------------

/**
 * Get the relative distance of each pixel in the 'region-of-interest' to its center.
 * @param roi Region-of-interest.
 * @return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi);

/**
 * Get the relative distance of each pixel in the 'region-of-interest' to its center.
 * @param roi Region-of-interest.
 * @param axis Select axis.
 * @return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, size_t axis);

/**
 * Get the relative distance of each pixel in the 'region-of-interest' to its center.
 * @param roi Region-of-interest.
 * @param h The physical dimensions of one pixel (in each direction).
 * @return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h);

/**
 * Get the relative distance of each pixel in the 'region-of-interest' to its center.
 * @param roi Region-of-interest.
 * @param h The physical dimensions of one pixel (in each direction).
 * @param axis Select axis.
 * @return The distances along the 'region-of-interest' set at construction.
 */
inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t axis);

/**
 * 2-point correlation: P(f(i) * g(i + di)).
 * @param roi Region-of-interest.
 * @param f The image.
 * @param g The comparison image.
 * @param periodic Switch to assume image periodic.
 */
template <class T>
inline auto S2(const std::vector<size_t>& roi, const T& f, const T& g, bool periodic = true);

/**
 * 2-point correlation: P(f(i) * g(i + di)).
 * @param roi Region-of-interest.
 * @param f The image.
 * @param g The comparison image.
 * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 * @param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
 * @param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto
S2(const std::vector<size_t>& roi,
   const T& f,
   const T& g,
   const M& fmask,
   const M& gmask,
   bool periodic = true);

/**
 * 2-point cluster function: P(f(i) == g(i + di)).
 * @param roi Region-of-interest.
 * @param f The image.
 * @param g The comparison image.
 * @param periodic Switch to assume image periodic.
 */
template <class T>
inline auto C2(const std::vector<size_t>& roi, const T& f, const T& g, bool periodic = true);

/**
 * 2-point cluster function: P(f(i) == g(i + di)).
 * @param roi Region-of-interest.
 * @param f The image.
 * @param g The comparison image.
 * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 * @param gmask Mask certain pixels of g (binary, 1: masked, 0: not masked).
 * @param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto
C2(const std::vector<size_t>& roi,
   const T& f,
   const T& g,
   const M& fmask,
   const M& gmask,
   bool periodic = true);

/**
 * Weighted 2-point correlation.
 * @param roi Region-of-interest.
 * @param w Weights.
 * @param f The image.
 * @param periodic Switch to assume image periodic.
 */
template <class T>
inline auto W2(const std::vector<size_t>& roi, const T& w, const T& f, bool periodic = true);

/**
 * Weighted 2-point correlation.
 * @param roi Region-of-interest.
 * @param w Weights.
 * @param f The image.
 * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 * @param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto
W2(const std::vector<size_t>& roi, const T& w, const T& f, const M& fmask, bool periodic = true);

/**
 * Collapsed weighted 2-point correlation
 * @param roi Region-of-interest.
 * @param clusters The clusters.
 * @param centers The cluster-centers: label only at the center.
 * @param f The image.
 * @param mode Method to use (see path_mode()).
 * @param periodic Switch to assume image periodic.
 */
template <class C, class T>
inline auto
W2c(const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

/**
 * Collapsed weighted 2-point correlation
 * @param roi Region-of-interest.
 * @param clusters The clusters.
 * @param centers The cluster-centers: label only at the center.
 * @param f The image.
 * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 * @param mode Method to use (see path_mode()).
 * @param periodic Switch to assume image periodic.
 */
template <class C, class T, class M>
inline auto
W2c(const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    const M& fmask,
    path_mode mode = path_mode::Bresenham,
    bool periodic = true);

/**
 * Height-height correlation.
 * @param roi Region-of-interest.
 * @param f The image.
 * @param periodic Switch to assume image periodic.
 */
template <class T>
inline auto heightheight(const std::vector<size_t>& roi, const T& f, bool periodic = true);

/**
 * Height-height correlation.
 * @param roi Region-of-interest.
 * @param f The image.
 * @param fmask Mask certain pixels of f (binary, 1: masked, 0: not masked).
 * @param periodic Switch to assume image periodic.
 */
template <class T, class M>
inline auto
heightheight(const std::vector<size_t>& roi, const T& f, const M& fmask, bool periodic = true);

/**
 * Lineal-path function
 * @param roi Region-of-interest.
 * @param f The image.
 * @param periodic Switch to assume image periodic.
 * @param mode Method to use (see path_mode()).
 */
template <class T>
inline auto
L(const std::vector<size_t>& roi,
  const T& f,
  bool periodic = true,
  path_mode mode = path_mode::Bresenham);

} // namespace GooseEYE

#include "Ensemble.hpp"
#include "Ensemble_C2.hpp"
#include "Ensemble_L.hpp"
#include "Ensemble_S2.hpp"
#include "Ensemble_W2.hpp"
#include "Ensemble_W2c.hpp"
#include "Ensemble_heightheight.hpp"
#include "Ensemble_mean.hpp"
#include "GooseEYE.hpp"
#include "dilate.hpp"
#include "kernel.hpp"

#endif

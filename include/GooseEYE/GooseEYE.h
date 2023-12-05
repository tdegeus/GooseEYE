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

namespace detail {

/**
 * @brief Get identifier of the namespace.
 */
inline std::string get_namespace()
{
    std::string ret = "GooseEYE";
#ifdef GOOSEEYE_USE_XTENSOR_PYTHON
    return ret + ".";
#else
    return ret + "::";
#endif
}

/**
 * @brief Convert shape to string.
 * @param shape Shape.
 */
template <class T>
inline std::string shape_to_string(const T& shape)
{
    std::string ret = "[";
    for (size_t i = 0; i < shape.size(); ++i) {
        ret += std::to_string(shape[i]);
        if (i < shape.size() - 1) {
            ret += ", ";
        }
    }
    ret += "]";
    return ret;
}

} // namespace detail

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
    GOOSEEYE_ASSERT(row.shape() == col.shape(), std::out_of_range);
    GOOSEEYE_ASSERT(row.shape() == r.shape(), std::out_of_range);
    GOOSEEYE_ASSERT(shape.size() == 2, std::out_of_range);

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
    GOOSEEYE_ASSERT(shape.size() == 2, std::out_of_range);
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
[[deprecated]] array_type::tensor<size_t, 1> relabel_map(const T& a, const S& b)
{
    GOOSEEYE_WARNING_PYTHON("relabel_map is deprecated, use labels_map instead (new API) instead");
    GOOSEEYE_ASSERT(xt::has_shape(a, b.shape()), std::out_of_range);

    array_type::tensor<size_t, 1> ret = xt::zeros<size_t>({static_cast<size_t>(xt::amax(a)() + 1)});

    for (size_t i = 0; i < a.size(); ++i) {
        ret(a.flat(i)) = b.flat(i);
    }

    return ret;
}

/**
 * @brief Get a map to relabel from `a` to `b`.
 * @param a Image with labels.
 * @param b Image with labels.
 * @return Array with each row the pair (old_label, new_label).
 */
template <class T>
inline array_type::tensor<typename T::value_type, 2> labels_map(const T& a, const T& b)
{
    using value_type = typename T::value_type;
    std::map<value_type, value_type> map;

    for (size_t i = 0; i < a.size(); ++i) {
        map.try_emplace(a.flat(i), b.flat(i));
    }

    size_t i = 0;
    array_type::tensor<typename T::value_type, 2> ret =
        xt::empty<typename T::value_type>(std::array<size_t, 2>{map.size(), 2});

    for (auto const& [key, val] : map) {
        ret(i, 0) = key;
        ret(i, 1) = val;
        ++i;
    }

    return ret;
}

/**
 * @brief Rename labels.
 * @param labels Image with labels.
 * @param rename Array with each row the pair (old_label, new_label).
 * @return Image with reordered labels.
 */
template <class L, class A>
inline L labels_rename(const L& labels, const A& rename)
{
    GOOSEEYE_ASSERT(rename.dimension() == 2, std::out_of_range);
    GOOSEEYE_ASSERT(rename.shape(1) == 2, std::out_of_range);
    using value_type = typename A::value_type;
    std::map<value_type, value_type> map;
    for (size_t i = 0; i < rename.shape(0); ++i) {
        map.emplace(rename(i, 0), rename(i, 1));
    }

#ifdef GOOSEEYE_ENABLE_ASSERT
    auto l = xt::unique(labels);
    for (size_t i = 0; i < l.size(); ++i) {
        GOOSEEYE_ASSERT(map.count(l(i)) > 0, std::out_of_range);
    }
#endif

    L ret = xt::empty_like(labels);
    for (size_t i = 0; i < labels.size(); ++i) {
        ret.flat(i) = map[labels.flat(i)];
    }

    return ret;
}

/**
 * @brief Prune labels: renumber labels to lowest possible label starting from 1.
 * Note that the background 0 is always kept 0.
 *
 * @param labels Image with labels.
 * @return Image with reordered labels.
 */
template <class T>
inline T labels_prune(const T& labels)
{
    using value_type = typename T::value_type;
    auto unq = xt::unique(labels);
    bool background = xt::any(xt::equal(unq, 0));

    std::array<size_t, 2> shape = {unq.size(), 2};
    array_type::tensor<value_type, 2> rename = xt::empty<value_type>(shape);

    if (background) {
        rename(0, 0) = 0;
        rename(0, 1) = 0;
        if (unq(0) == 0) {
            for (size_t i = 1; i < unq.size(); ++i) {
                rename(i, 0) = unq(i);
                rename(i, 1) = i;
            }
        }
        else {
            size_t row = 1;
            for (size_t i = 0; i < unq.size(); ++i) {
                if (unq(i) == 0) {
                    continue;
                }
                rename(row, 0) = unq(i);
                rename(row, 1) = row;
                row++;
            }
        }
    }
    else {
        for (size_t i = 0; i < unq.size(); ++i) {
            rename(i, 0) = unq(i);
            rename(i, 1) = i + 1;
        }
    }
    return labels_rename(labels, rename);
}

/**
 * @brief Reorder labels.
 * @param labels Image with labels.
 * @param order List of new order of labels (`unique(labels)` in desired order).
 * @return Image with reordered labels.
 */
template <class L, class A>
inline L labels_reorder(const L& labels, const A& order)
{
#ifdef GOOSEEYE_ENABLE_ASSERT
    auto a = xt::unique(labels);
    auto b = xt::unique(order);
    GOOSEEYE_ASSERT(a.size() == b.size(), std::out_of_range);
    GOOSEEYE_ASSERT(xt::all(xt::equal(a, b)), std::out_of_range);
#endif

    auto maxlab = *std::max_element(order.begin(), order.end());
    std::vector<typename A::value_type> renum(maxlab + 1);

    for (size_t i = 0; i < order.size(); ++i) {
        renum[order[i]] = i;
    }

    L ret = xt::empty_like(labels);
    for (size_t i = 0; i < labels.size(); ++i) {
        ret.flat(i) = renum[labels.flat(i)];
    }

    return ret;
}

/**
 * @brief Size per label.
 * @param labels Image with labels (0..n).
 * @return List of size n + 1 with the size per label.
 */
template <class T>
array_type::tensor<typename T::value_type, 2> labels_sizes(const T& labels)
{
    using value_type = typename T::value_type;
    std::map<value_type, value_type> map;

    for (size_t i = 0; i < labels.size(); ++i) {
        if (map.count(labels.flat(i)) == 0) {
            map.emplace(labels.flat(i), 1);
        }
        else {
            map[labels.flat(i)]++;
        }
    }

    size_t i = 0;
    array_type::tensor<value_type, 2> ret =
        xt::empty<value_type>(std::array<size_t, 2>{map.size(), 2});

    for (auto const& [key, val] : map) {
        ret(i, 0) = key;
        ret(i, 1) = val;
        ++i;
    }

    return ret;
}

namespace detail {

/**
 * @brief Convert kernel to array of distances and remove zero distance.
 * @param kernel Kernel.
 * @return Array of distances.
 */
template <size_t Dim, class T>
inline array_type::tensor<ptrdiff_t, 2> kernel_to_dx(T kernel)
{
#ifdef GOOSEEYE_ENABLE_ASSERT
    for (size_t i = 0; i < Dim; ++i) {
        GOOSEEYE_ASSERT(kernel.shape(i) % 2 == 1, std::out_of_range);
    }
#endif

    std::array<size_t, Dim> mid;
    for (size_t i = 0; i < Dim; ++i) {
        mid[i] = (kernel.shape(i) - 1) / 2;
    }
    size_t idx = 0;
    for (size_t i = 0; i < Dim; ++i) {
        idx += mid[i] * kernel.strides()[i];
    }
    GOOSEEYE_ASSERT(kernel.flat(idx) == 1, std::out_of_range);
    kernel.flat(idx) = 0;

    if constexpr (Dim == 1) {
        auto i = xt::flatten_indices(xt::argwhere(kernel)) - mid[0];
        array_type::tensor<ptrdiff_t, 2> ret = xt::empty<ptrdiff_t>({i.size(), size_t(1)});
        std::copy(i.begin(), i.end(), ret.begin());
        return ret;
    }

    auto ret = xt::from_indices(xt::argwhere(kernel));
    for (size_t i = 0; i < Dim; ++i) {
        xt::view(ret, xt::all(), i) -= mid[i];
    }
    return ret;
}

} // namespace detail

/**
 * @brief (Incrementally) Label clusters (0 as background, 1..n as labels).
 * @tparam Dimension The rank (a.k.a. `dimension`) of the image.
 * @note The default kernel is `GooseEYE::kernel::nearest()`.
 */
template <size_t Dimension, bool Periodicity = true>
class ClusterLabeller {
public:
    static constexpr size_t Dim = Dimension; ///< Dimensionality of the system.
    static constexpr bool Periodic = Periodicity; ///< Periodicity of the system.

private:
    std::array<ptrdiff_t, Dim> m_shape; ///< Shape of the system.
    array_type::tensor<ptrdiff_t, 2> m_dx; ///< Kernel (in distances along each dimension).
    array_type::tensor<ptrdiff_t, Dim> m_label; ///< Per block, the label (`0` for background).
    ptrdiff_t m_new_label = 1; ///< The next label number to assign.
    size_t m_nmerge = 0; ///< Number of times that clusters have been merged.
    std::array<ptrdiff_t, Dim> m_strides; ///< Strides of the array.

    /**
     * @brief Memory used for relabeling.
     * @note
     *      -   The maximum label is `maxlab = prod(shape) + 1`.
     *      -   `m_renum` is allocated to `arange(maxlab)`.
     */
    std::vector<ptrdiff_t> m_renum;

    /**
     * @brief Linked list of labels connected to a certain label.
     * @warning Each label can only be connected to one other label.
     * @note To get the labels connected to label `i`:
     *
     *      labels = []
     *      while m_next[i] != -1:
     *          labels.append(m_next[i])
     *          i = m_next[i]
     */
    std::vector<ptrdiff_t> m_next;
    std::vector<ptrdiff_t> m_connected; ///< List of labels connected to the current block.

    typedef ptrdiff_t (ClusterLabeller<Dimension, Periodicity>::*CompareImpl)(size_t, size_t);
    CompareImpl get_compare = &ClusterLabeller<Dimension, Periodicity>::get_compare_default;

public:
    /**
     * @param shape @copydoc ClusterLabeller::m_shape
     */
    template <class T>
    ClusterLabeller(const T& shape)
    {
        if constexpr (Dim == 1) {
            // kernel = {1, 1, 1}
            m_dx = {{-1}, {1}};
        }
        else if constexpr (Dim == 2) {
            // kernel = {{0, 1, 0}, {1, 1, 1}, {0, 1, 0}};
            m_dx = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};
        }
        else if constexpr (Dim == 3) {
            m_dx = {{-1, 0, 0}, {0, -1, 0}, {0, 0, -1}, {0, 0, 1}, {0, 1, 0}, {1, 0, 0}};
        }
        else {
            throw std::runtime_error("Please specify the kernel in dimensions > 3.");
        }
        this->init(shape);
    }

    /**
     * @param shape @copydoc ClusterLabeller::m_shape
     * @param kernel Kernel (e.g. GooseEYE::kernel::nearest()).
     */
    template <class T, class K>
    ClusterLabeller(const T& shape, const K& kernel)
    {
        m_dx = detail::kernel_to_dx<Dim>(kernel);
        this->init(shape);
    }

private:
    template <class T>
    void init(const T& shape)
    {
        m_label = xt::empty<ptrdiff_t>(shape);
        m_renum.resize(m_label.size() + 1);
        m_next.resize(m_label.size() + 1);
        for (size_t i = 0; i < Dim; ++i) {
            m_shape[i] = static_cast<ptrdiff_t>(shape[i]);
            if constexpr (Dim >= 2) {
                m_strides[i] = static_cast<ptrdiff_t>(m_label.strides()[i]);
            }
        }
        this->reset();
        m_connected.resize(m_dx.shape(0));

        // Dim == 2: by default strides are assumed non-zero to avoid extra checks
        // check once if zeros strides occur and if so use a special implementation of unravel_index
        if constexpr (Dim == 2) {
            if (m_shape[0] == 1) {
                get_compare = &ClusterLabeller<Dimension, Periodicity>::get_compare_2d_1n;
            }
            else if (m_shape[1] == 1) {
                get_compare = &ClusterLabeller<Dimension, Periodicity>::get_compare_2d_n1;
            }
        }
    }

public:
    /**
     * @brief Reset labels to zero.
     */
    void reset()
    {
        std::fill(m_label.begin(), m_label.end(), 0);
        std::iota(m_renum.begin(), m_renum.end(), 0);
        m_new_label = 1;
        this->clean_next();
    }

    /**
     * @brief Prune: renumber labels to lowest possible label, see also AvalancheSegmenter::nlabels.
     * @note This might change all labels.
     */
    void prune()
    {
        ptrdiff_t n = static_cast<ptrdiff_t>(m_new_label);
        m_new_label = 1;
        m_renum[0] = 0;
        for (ptrdiff_t i = 1; i < n; ++i) {
            if (m_renum[i] == i) {
                m_renum[i] = m_new_label;
                ++m_new_label;
            }
        }
        this->private_renumber(m_renum);
        std::iota(m_renum.begin(), m_renum.begin() + n, 0);
    }

private:
    /**
     * @brief Clean linked list.
     */
    void clean_next()
    {
        std::fill(m_next.begin(), m_next.end(), -1);
    }

    /**
     * @brief Apply renumbering without assertions.
     * @param renum List of new label for each label in used.
     */
    template <class T>
    void private_renumber(const T& renum)
    {
        for (size_t i = 0; i < m_label.size(); ++i) {
            m_label.flat(i) = renum[m_label.flat(i)];
        }
    }

    /**
     * @brief Link `b` to `head[a]`.
     * @details
     *      -   `head[list(b)] = head[a]`
     *      -   `list(head[a]).append(list(b))`
     * @param a Target label.
     * @param b Label to merge into `a`.
     */
    void merge_detail(ptrdiff_t a, ptrdiff_t b)
    {
        // -> head[list(b)] = head[a]
        ptrdiff_t i = m_renum[b];
        ptrdiff_t target = m_renum[a];
        m_renum[b] = target;
        while (true) {
            i = m_next[i];
            if (i == -1) {
                break;
            }
            m_renum[i] = target;
        }
        // -> list(head[a]).append(list(b))
        while (m_next[a] != -1) {
            a = m_next[a];
        }
        m_next[a] = b;
    }

    /**
     * @brief Mark list of labels as merged.
     * @note Link all labels to the lowest label in the list.
     * @warning `m_labels` is not updated.
     * @param labels List of labels to merge.
     * @param nlabels Number of labels in the list.
     */
    ptrdiff_t merge(ptrdiff_t* labels, size_t nlabels)
    {
        std::sort(labels, labels + nlabels);
        nlabels = std::unique(labels, labels + nlabels) - labels;
        ptrdiff_t target = labels[0];
        for (size_t i = 1; i < nlabels; ++i) {
            this->merge_detail(target, labels[i]);
        }
        return target;
    }

    void apply_merge()
    {
        if (m_nmerge == 0) {
            return;
        }

        this->private_renumber(m_renum);
        this->clean_next();
        m_nmerge = 0;
    }

    /**
     * @copydoc ClusterLabeller::get_compare_default
     * @warning Implementation only for 2D, `shape = [1, n]`.
     */
    ptrdiff_t get_compare_2d_1n(size_t idx, size_t j)
    {
        if constexpr (Periodic) {
            return (m_shape[1] + idx + m_dx(j, 1)) % m_shape[1];
        }
        if constexpr (!Periodic) {
            ptrdiff_t compare = idx + m_dx(j, 1);
            if (compare < 0 || compare >= m_shape[1]) {
                return -1;
            }
            return compare;
        }
    }

    /**
     * @copydoc ClusterLabeller::get_compare_default
     * @warning Implementation only for 2D, `shape = [n, 1]`.
     */
    ptrdiff_t get_compare_2d_n1(size_t idx, size_t j)
    {
        if constexpr (Periodic) {
            return (m_shape[0] + idx + m_dx(j, 0)) % m_shape[0];
        }
        if constexpr (!Periodic) {
            ptrdiff_t compare = idx + m_dx(j, 0);
            if (compare < 0 || compare >= m_shape[0]) {
                return -1;
            }
            return compare;
        }
    }

    /**
     * @brief Get the pixel to compare with.
     * @note If the pixel is out of bounds, return -1.
     *
     * @param idx Flat index of the pixel.
     * @param j Index of the kernel (in `m_dx`).
     * @return ptrdiff_t Index of the pixel to compare with.
     */
    ptrdiff_t get_compare_default(size_t idx, size_t j)
    {
        if constexpr (Dim == 1 && Periodic) {
            return (m_shape[0] + idx + m_dx.flat(j)) % m_shape[0];
        }
        if constexpr (Dim == 1 && !Periodic) {
            ptrdiff_t compare = idx + m_dx.flat(j);
            if (compare < 0 || compare >= m_shape[0]) {
                return -1;
            }
            return idx + m_dx.flat(j);
        }
        if constexpr (Dim == 2 && Periodic) {
            ptrdiff_t ii = (m_shape[0] + (idx / m_strides[0]) + m_dx(j, 0)) % m_shape[0];
            ptrdiff_t jj = (m_shape[1] + (idx % m_strides[0]) + m_dx(j, 1)) % m_shape[1];
            return ii * m_shape[1] + jj;
        }
        if constexpr (Dim == 2 && !Periodic) {
            ptrdiff_t ii = (idx / m_strides[0]) + m_dx(j, 0);
            ptrdiff_t jj = (idx % m_strides[0]) + m_dx(j, 1);
            if (ii < 0 || ii >= m_shape[0] || jj < 0 || jj >= m_shape[1]) {
                return -1;
            }
            return ii * m_shape[1] + jj;
        }
        else {
            auto index = xt::unravel_from_strides(idx, m_strides, xt::layout_type::row_major);
            for (size_t d = 0; d < Dim; ++d) {
                index[d] += m_dx(j, d);
                if constexpr (!Periodic) {
                    if (index[d] < 0 || index[d] >= m_shape[d]) {
                        return -1;
                    }
                }
                else {
                    auto n = m_shape[d];
                    index[d] = (n + (index[d] % n)) % n;
                }
            }
            return xt::ravel_index(index, m_shape, xt::layout_type::row_major);
        }
    }

    void label_impl(size_t idx)
    {
        size_t nconnected = 0;

        for (size_t j = 0; j < m_dx.shape(0); ++j) {
            ptrdiff_t compare = (this->*get_compare)(idx, j);
            if constexpr (!Periodic) {
                if (compare == -1) {
                    continue;
                }
            }
            if (m_label.flat(compare) != 0) {
                m_connected[nconnected] = m_renum[m_label.flat(compare)];
                nconnected++;
            }
        }

        if (nconnected == 0) {
            m_label.flat(idx) = m_new_label;
            m_new_label += 1;
            return;
        }

        if (nconnected == 1) {
            m_label.flat(idx) = m_connected[0];
            return;
        }

        // mark all labels in the list for merging
        // `m_label` is not yet updated to avoid looping over all blocks too frequently
        // the new label can be read by `m_renum[lab]` (as done above)
        m_label.flat(idx) = this->merge(&m_connected[0], nconnected);
        m_nmerge++;
    }

public:
    /**
     * @brief Add image. Previous labels are not overwritten.
     * @param img Image (can be incremental only).
     */
    template <class T>
    void add_image(const T& img)
    {
        GOOSEEYE_ASSERT(xt::has_shape(img, m_label.shape()), std::out_of_range);

        for (size_t idx = 0; idx < img.size(); ++idx) {
            if (img.flat(idx) == 0) {
                continue;
            }
            if (m_label.flat(idx) != 0) {
                continue;
            }
            this->label_impl(idx);
        }
        this->apply_merge();
    }

    /**
     * @brief Add sequence of points.
     * @param begin Iterator to first point.
     * @param end Iterator to last point.
     */
    template <class T>
    void add_points(const T& begin, const T& end)
    {
#ifdef GOOSEEYE_ENABLE_ASSERT
        size_t n = m_label.size();
        if constexpr (std::is_signed_v<typename T::value_type>) {
            GOOSEEYE_ASSERT(
                !std::any_of(begin, end, [n](size_t i) { return i < 0 || i >= n; }),
                std::out_of_range);
        }
        else {
            GOOSEEYE_ASSERT(
                !std::any_of(begin, end, [n](size_t i) { return i >= n; }), std::out_of_range);
        }
#endif

        for (auto it = begin; it != end; ++it) {
            if (m_label.flat(*it) != 0) {
                continue;
            }
            this->label_impl(*it);
        }
        this->apply_merge();
    }

    /**
     * @brief Add sequence of points.
     * @param idx List of points.
     */
    template <class T>
    void add_points(const T& idx)
    {
        GOOSEEYE_ASSERT(idx.dimension() == 1, std::out_of_range);
        return this->add_points(idx.begin(), idx.end());
    }

    /**
     * @brief Basic class info.
     * @return std::string
     */
    std::string repr() const
    {
        return detail::get_namespace() + "ClusterLabeller" + std::to_string(Dim) + " " +
               detail::shape_to_string(m_shape);
    }

    /**
     * @brief Shape of ClusterLabeller::s and ClusterLabeller::labels.
     * @return Shape
     */
    const auto& shape() const
    {
        return m_label.shape();
    }

    /**
     * @brief Size of ClusterLabeller::s and ClusterLabeller::labels (`== prod(shape)`).
     * @return Size
     */
    auto size() const
    {
        return m_label.size();
    }

    // todo: allow resetting a cluster map ?

    /**
     * @brief @copydoc ClusterLabeller::m_label
     * @return array of signed integers.
     */
    const auto& labels() const
    {
        return m_label;
    }
};

/**
 * Compute clusters and obtain certain characteristic about them.
 */
class [[deprecated]] Clusters {
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
        GOOSEEYE_WARNING_PYTHON("Clusters is deprecated, use ClusterLabeller (new API) instead "
                                "(please open a PR for missing functions)");

        static_assert(std::is_integral<typename T::value_type>::value, "Integral labels required.");
        static_assert(std::is_integral<typename S::value_type>::value, "Integral kernel required.");

        GOOSEEYE_ASSERT(xt::all(xt::equal(f, 0) || xt::equal(f, 1)), std::out_of_range);
        GOOSEEYE_ASSERT(xt::all(xt::equal(kernel, 0) || xt::equal(kernel, 1)), std::out_of_range);
        GOOSEEYE_ASSERT(f.dimension() == kernel.dimension(), std::out_of_range);

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
        GOOSEEYE_WARNING_PYTHON("Clusters.sizes() is deprecated, use labels_sizes() (new API)");
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

namespace detail {

template <size_t Dimension, bool Periodicity>
class ClusterLabellerOverload : public ClusterLabeller<Dimension, Periodicity> {
public:
    template <class T>
    ClusterLabellerOverload(const T& img) : ClusterLabeller<Dimension, Periodicity>(img.shape())
    {
        this->add_image(img);
        this->prune();
    }

    auto get() const
    {
        return this->labels();
    }
};

} // namespace detail

/**
 * @brief Compute clusters.
 * @param f Image.
 * @param periodic Interpret image as periodic.
 * @return 'Image' with labels (1..n) for labels, 0 for background.
 */
template <class T>
array_type::array<int> clusters(const T& f, bool periodic = true)
{
    GOOSEEYE_ASSERT(f.layout() == xt::layout_type::row_major, std::runtime_error);

    auto n = f.dimension();
    if (n == 1 && periodic) {
        return detail::ClusterLabellerOverload<1, true>(f).get();
    }
    if (n == 1 && !periodic) {
        return detail::ClusterLabellerOverload<1, false>(f).get();
    }
    if (n == 2 && periodic) {
        return detail::ClusterLabellerOverload<2, true>(f).get();
    }
    if (n == 2 && !periodic) {
        return detail::ClusterLabellerOverload<2, false>(f).get();
    }
    if (n == 3 && periodic) {
        return detail::ClusterLabellerOverload<3, true>(f).get();
    }
    if (n == 3 && !periodic) {
        return detail::ClusterLabellerOverload<3, false>(f).get();
    }
    throw std::runtime_error("Please use ClusterLabeller directly for dimensions > 3.");
}

/**
 * Convert positions to an image.
 * @param img The image.
 * @param positions The position.
 * @param labels The label to apply for each image.
 * @return The image, with labels inserted (overwritten) at the positions.
 */
template <typename T, typename U, typename V>
[[deprecated("Will not be supported in the future. See Python warning for new API.")]] inline T
pos2img(const T& img, const U& positions, const V& labels)
{
    GOOSEEYE_WARNING_PYTHON("pos2img(img, positions, labels) deprecated, use: "
                            "i = ravel_multi_index(positions.T, img.shape); img.flat[i] = labels")
    GOOSEEYE_ASSERT(img.dimension() > 0, std::out_of_range);
    GOOSEEYE_ASSERT(img.dimension() <= 3, std::out_of_range);
    GOOSEEYE_ASSERT(img.dimension() == positions.shape(1), std::out_of_range);
    GOOSEEYE_ASSERT(positions.shape(0) == labels.size(), std::out_of_range);
    GOOSEEYE_ASSERT(labels.dimension() == 1, std::out_of_range);

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
 * @brief Return the geometric center of a list of positions.
 *
 * @details
 *      For periodic algorithm, see:
 *      https://en.wikipedia.org/wiki/Center_of_mass#Systems_with_periodic_boundary_conditions
 *
 * @param shape Shape of the array.
 * @param positions List of positions (in array coordinates).
 * @param periodic Switch to assume array periodic.
 * @return Coordinates of the center (in array coordinates).
 */
inline array_type::tensor<double, 1> center(
    const array_type::tensor<double, 1>& shape,
    const array_type::tensor<double, 2>& positions,
    bool periodic = true)
{
    if (positions.size() == 0) {
        return xt::zeros<double>({shape.size()});
    }

    if (!periodic) {
        return xt::mean(positions, 0);
    }
    else {
        double pi = xt::numeric_constants<double>::PI;
        auto theta = 2.0 * pi * positions / shape;
        auto xi = xt::cos(theta);
        auto zeta = xt::sin(theta);
        auto xi_bar = xt::mean(xi, 0);
        auto zeta_bar = xt::mean(zeta, 0);
        auto theta_bar = xt::atan2(-zeta_bar, -xi_bar) + pi;
        return shape * theta_bar / (2.0 * pi);
    }
}

/**
 * @copydoc GooseEYE::center()
 * @param weights Weight for each position.
 */
inline array_type::tensor<double, 1> center_of_mass(
    const array_type::tensor<double, 1>& shape,
    const array_type::tensor<double, 2>& positions,
    const array_type::tensor<double, 1>& weights,
    bool periodic = true)
{
    if (positions.size() == 0) {
        return xt::zeros<double>({shape.size()});
    }

    if (!periodic) {
        return xt::average(positions, weights, 0);
    }
    else {
        double pi = xt::numeric_constants<double>::PI;
        auto theta = 2.0 * pi * positions / shape;
        auto xi = xt::cos(theta);
        auto zeta = xt::sin(theta);
        auto xi_bar = xt::average(xi, weights, 0);
        auto zeta_bar = xt::average(zeta, weights, 0);
        auto theta_bar = xt::atan2(-zeta_bar, -xi_bar) + pi;
        return shape * theta_bar / (2.0 * pi);
    }
}

namespace detail {

class PositionList {
public:
    array_type::tensor<double, 2> positions;
    array_type::tensor<double, 1> weights;

    PositionList() = default;

    template <class T>
    void set(const T& condition)
    {
        positions = xt::from_indices(xt::argwhere(condition));
    }

    template <class T, class W>
    void set(const T& condition, const W& w)
    {
        auto pos = xt::argwhere(condition);
        weights = xt::empty<double>({pos.size()});
        for (size_t i = 0; i < pos.size(); ++i) {
            weights(i) = w[pos[i]];
        }
        positions = xt::from_indices(pos);
    }
};

} // namespace detail

/**
 * @brief Get the position of the center of each label.
 *
 * @param labels An image with labels.
 * @param names List of labels to compute the center for.
 * @param periodic Switch to assume image periodic.
 * @return Coordinates of the center (in array coordinates), in order of the unique (sorted) labels.
 */
template <class T, class N>
inline array_type::tensor<double, 2>
labels_centers(const T& labels, const N& names, bool periodic = true)
{
    static_assert(std::is_integral<typename T::value_type>::value, "Integral labels required.");
    GOOSEEYE_ASSERT(labels.dimension() > 0, std::out_of_range);
    GOOSEEYE_ASSERT(names.dimension() == 1, std::out_of_range);

    size_t rank = labels.dimension();
    array_type::tensor<double, 1> shape = xt::adapt(labels.shape());
    array_type::tensor<double, 2> ret = xt::zeros<double>({names.size(), rank});
    detail::PositionList plist;

    for (size_t l = 0; l < names.size(); ++l) {
        plist.set(xt::equal(labels, names(l)));
        if (plist.positions.size() == 0) {
            continue;
        }
        xt::view(ret, l, xt::all()) = center(shape, plist.positions, periodic);
    }

    return ret;
}

/**
 * @copydoc GooseEYE::labels_centers()
 * @param weights Weight for each pixel.
 */
template <class T, class W, class N>
inline array_type::tensor<double, 2>
labels_centers_of_mass(const T& labels, const W& weights, const N& names, bool periodic = true)
{
    static_assert(std::is_integral<typename T::value_type>::value, "Integral labels required.");
    GOOSEEYE_ASSERT(xt::has_shape(labels, weights.shape()), std::out_of_range);
    GOOSEEYE_ASSERT(labels.dimension() > 0, std::out_of_range);
    GOOSEEYE_ASSERT(names.dimension() == 1, std::out_of_range);

    size_t rank = labels.dimension();
    array_type::tensor<double, 1> shape = xt::adapt(labels.shape());
    array_type::tensor<double, 2> ret = xt::zeros<double>({names.size(), rank});
    detail::PositionList plist;

    for (size_t l = 0; l < names.size(); ++l) {
        plist.set(xt::equal(labels, names(l)), weights);
        if (plist.positions.size() == 0) {
            continue;
        }
        xt::view(ret, l, xt::all()) =
            center_of_mass(shape, plist.positions, plist.weights, periodic);
    }

    return ret;
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

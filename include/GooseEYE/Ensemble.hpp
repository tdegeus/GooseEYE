/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_ENSEMBLE_HPP
#define GOOSEEYE_ENSEMBLE_HPP

#include "GooseEYE.h"

namespace GooseEYE {

inline Ensemble::Ensemble(const std::vector<size_t>& roi, bool periodic, bool variance)
    : m_periodic(periodic), m_variance(variance), m_shape_orig(roi)
{
    GOOSEEYE_ASSERT(m_shape_orig.size() <= MAX_DIM);

    m_first = xt::atleast_3d(xt::zeros<double>(m_shape_orig));
    m_second = zeros_like(m_first);
    m_norm = zeros_like(m_first);
    m_shape = std::vector<size_t>(m_first.shape().begin(), m_first.shape().end());
    m_pad = detail::pad_width(m_shape);
}

inline xt::xarray<double> Ensemble::result() const
{
    xt::xarray<double> ret = m_first / xt::where(m_norm <= 0, 1.0, m_norm);

    if (m_stat == Type::heightheight) {
        ret = xt::pow(ret, 0.5);
    }

    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::variance() const
{
    xt::xtensor<double,3> norm = xt::where(m_norm <= 0, 1.0, m_norm);
    xt::xarray<double> ret = (m_second / norm - xt::pow(m_first / norm, 2.0)) * norm / (norm - 1);

    if (m_stat == Type::heightheight) {
        ret = xt::pow(ret, 0.5);
    }
    else if (m_stat != Type::mean) {
        throw std::runtime_error("Not implemented");
    }

    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::data_first() const
{
    xt::xarray<double> ret = m_first;
    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::data_second() const
{
    xt::xarray<double> ret = m_second;
    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::norm() const
{
    xt::xarray<double> ret = m_norm;
    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::distance(size_t axis) const
{
    GOOSEEYE_ASSERT(axis < m_shape_orig.size());
    axis = detail::atleast_3d_axis(m_shape_orig.size(), axis);

    xt::xtensor<double,3> dist = xt::empty<double>(m_shape);

    xt::xarray<double> D = xt::linspace<double>(
        -1.0 * static_cast<double>(m_pad[axis][0]),
        static_cast<double>(m_pad[axis][1]),
        m_shape[axis]);

    for (size_t h = 0; h < m_shape[0]; ++h) {
        for (size_t i = 0; i < m_shape[1]; ++i) {
            for (size_t j = 0; j < m_shape[2]; ++j) {
                if (axis == 0) {
                    dist(h, i, j) = D(h);
                }
                else if (axis == 1) {
                    dist(h, i, j) = D(i);
                }
                else if (axis == 2) {
                    dist(h, i, j) = D(j);
                }
            }
        }
    }

    xt::xarray<double> ret = std::move(dist);
    return ret.reshape(m_shape_orig);
}

inline xt::xarray<double> Ensemble::distance() const
{
    xt::xarray<double> ret = xt::zeros<double>(m_shape_orig);

    for (size_t i = 0; i < m_shape_orig.size(); ++i) {
        ret += xt::pow(this->distance(i), 2.0);
    }

    return xt::pow(ret, 0.5);
}

inline xt::xarray<double> Ensemble::distance(const std::vector<double>& h) const
{
    GOOSEEYE_ASSERT(m_shape_orig.size() == h.size());

    xt::xarray<double> ret = xt::zeros<double>(m_shape_orig);

    for (size_t i = 0; i < m_shape_orig.size(); ++i) {
        ret += xt::pow(this->distance(i) * h[i], 2.0);
    }

    return xt::pow(ret, 0.5);
}

inline xt::xarray<double> Ensemble::distance(const std::vector<double>& h, size_t axis) const
{
    GOOSEEYE_ASSERT(m_shape_orig.size() == h.size());
    return this->distance(axis) * h[axis];
}

} // namespace GooseEYE

#endif

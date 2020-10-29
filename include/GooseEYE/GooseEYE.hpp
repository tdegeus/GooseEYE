/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_HPP
#define GOOSEEYE_HPP

#include "GooseEYE.h"

namespace GooseEYE {

inline xt::xtensor<int, 2> path(
    const xt::xtensor<int, 1>& x0,
    const xt::xtensor<int, 1>& x1,
    path_mode mode)
{
    if (mode == path_mode::Bresenham) {
        return detail::path::bresenham(x0, x1);
    }
    else if (mode == path_mode::actual) {
        return detail::path::actual(x0, x1);
    }
    else {
        return detail::path::full(x0, x1);
    }
}

inline auto distance(const std::vector<size_t>& roi)
{
    Ensemble ensemble(roi);
    return ensemble.distance();
}

inline auto distance(const std::vector<size_t>& roi, size_t dim)
{
    Ensemble ensemble(roi);
    return ensemble.distance(dim);
}

inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h)
{
    Ensemble ensemble(roi);
    return ensemble.distance(h);
}

inline auto distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t dim)
{
    Ensemble ensemble(roi);
    return ensemble.distance(h, dim);
}

template <class T>
inline auto S2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.S2(f, g);
    return ensemble.result();
}

template <class T, class M>
inline auto S2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    const M& fmask,
    const M& gmask,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.S2(f, g, fmask, gmask);
    return ensemble.result();
}

template <class T>
inline auto C2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.C2(f, g);
    return ensemble.result();
}

template <class T, class M>
inline auto C2(
    const std::vector<size_t>& roi,
    const T& f,
    const T& g,
    const M& fmask,
    const M& gmask,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.C2(f, g, fmask, gmask);
    return ensemble.result();
}

template <class T>
inline auto W2(
    const std::vector<size_t>& roi,
    const T& w,
    const T& f,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.W2(w, f);
    return ensemble.result();
}

template <class T, class M>
inline auto W2(
    const std::vector<size_t>& roi,
    const T& w,
    const T& f,
    const M& fmask,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.W2(w, f, fmask);
    return ensemble.result();
}

template <class C, class T>
inline auto W2c(
    const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    path_mode mode,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.W2c(clusters, centers, f, mode);
    return ensemble.result();
}

template <class C, class T, class M>
inline auto W2c(
    const std::vector<size_t>& roi,
    const C& clusters,
    const C& centers,
    const T& f,
    const M& fmask,
    path_mode mode,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.W2c(clusters, centers, f, fmask, mode);
    return ensemble.result();
}

template <class T>
inline auto heightheight(
    const std::vector<size_t>& roi,
    const T& f,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.heightheight(f);
    return ensemble.result();
}

template <class T, class M>
inline auto heightheight(
    const std::vector<size_t>& roi,
    const T& f,
    const M& fmask,
    bool periodic)
{
    Ensemble ensemble(roi, periodic);
    ensemble.heightheight(f, fmask);
    return ensemble.result();
}

template <class T>
inline auto L(
    const std::vector<size_t>& roi,
    const T& f,
    bool periodic,
    path_mode mode)
{
    Ensemble ensemble(roi, periodic);
    ensemble.L(f, mode);
    return ensemble.result();
}

} // namespace GooseEYE

#endif

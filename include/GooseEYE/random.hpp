/*

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

*/

#ifndef GOOSEEYE_RANDOM_HPP
#define GOOSEEYE_RANDOM_HPP

#include "config.h"
#include <xtensor/xrandom.hpp>

namespace GooseEYE {
namespace random {

inline void seed(size_t seed)
{
    std::srand(seed);
    xt::random::seed(seed);
}

inline auto random(std::initializer_list<size_t> shape)
{
    return xt::random::rand<double>(shape);
}

template <class T>
inline auto random(const T& shape)
{
    return xt::random::rand<double>(shape);
}

} // namespace random
} // namespace GooseEYE

#endif

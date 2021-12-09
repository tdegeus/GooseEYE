/**
\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the GPLv3 License.
*/

#ifndef GOOSEEYE_RANDOM_HPP
#define GOOSEEYE_RANDOM_HPP

#include "config.h"
#include <xtensor/xrandom.hpp>

namespace GooseEYE {
namespace random {

inline void seed(size_t seed)
{
    using default_engine_type = std::mt19937;
    using seed_type = default_engine_type::result_type;

    std::srand(static_cast<unsigned>(seed));
    xt::random::seed(static_cast<seed_type>(seed));
}

inline auto random(const std::vector<size_t>& shape)
{
    return xt::random::rand<double>(shape);
}

template <class T>
inline auto random(const T& shape)
{
    return xt::random::rand<double>(shape);
}

inline auto normal(const std::vector<size_t>& shape, double mean, double std_dev)
{
    return xt::random::randn<double>(shape, mean, std_dev);
}

template <class T>
inline auto normal(const T& shape, double mean, double std_dev)
{
    return xt::random::randn<double>(shape, mean, std_dev);
}

} // namespace random
} // namespace GooseEYE

#endif

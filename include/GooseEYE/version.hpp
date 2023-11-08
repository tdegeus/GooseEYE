/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_VERSION_HPP
#define GOOSEEYE_VERSION_HPP

#include "version.h"

namespace GooseEYE {

namespace detail {

inline std::string unquote(const std::string& arg)
{
    std::string ret = arg;
    ret.erase(std::remove(ret.begin(), ret.end(), '\"'), ret.end());
    return ret;
}

} // namespace detail

inline std::string version()
{
    return detail::unquote(std::string(QUOTE(GOOSEEYE_VERSION)));
}

} // namespace GooseEYE

#endif

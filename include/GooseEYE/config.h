/**
 *  \file
 *  \copyright Copyright 2017. Tom de Geus. All rights reserved.
 *  \license This project is released under the GPLv3 License.
 */

#ifndef GOOSEEYE_INCLUDE_H
#define GOOSEEYE_INCLUDE_H

/**
\cond
*/

#define _USE_MATH_DEFINES

/**
\endcond
*/

#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <math.h>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include <xtensor/xadapt.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xmath.hpp>
#include <xtensor/xpad.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>

/**
\cond
*/

#define GOOSEEYE_WARNING_IMPL(message, file, line, function) \
    std::cout << \
        std::string(file) + ":" + std::to_string(line) + \
        " (" + std::string(function) + ")" + \
        ": " message ") \n\t"; \

#define GOOSEEYE_ASSERT_IMPL(expr, file, line, function) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ":" + std::to_string(line) + \
            " (" + std::string(function) + ")" + \
            ": assertion failed (" #expr ") \n\t"); \
    }

#define SIGN(a) ((a < 0) ? -1 : a > 0 ? 1 : 0)

/**
\endcond
*/

/**
 *  All assertions are implementation as::
 *
 *      GOOSEEYE_ASSERT(...)
 *
 *  They can be enabled by::
 *
 *      #define GOOSEEYE_ENABLE_ASSERT
 *
 *  (before including GooseEYE).
 *  The advantage is that:
 *
 *  -   File and line-number are displayed if the assertion fails.
 *  -   GooseEYE's assertions can be enabled/disabled independently from those of other libraries.
 *
 *  \throw std::runtime_error
 */
#ifdef GOOSEEYE_ENABLE_ASSERT
#define GOOSEEYE_ASSERT(expr) GOOSEEYE_ASSERT_IMPL(expr, __FILE__, __LINE__, __FUNCTION__)
#else
#define GOOSEEYE_ASSERT(expr)
#endif

/**
 *  All warnings are implemented as::
 *
 *      GOOSEEYE_WARNING(...)
 *
 *  They can be disabled by::
 *
 *      #define GOOSEEYE_DISABLE_WARNING
 */
#ifdef GOOSEEYE_DISABLE_WARNING
#define GOOSEEYE_WARNING(message)
#else
#define GOOSEEYE_WARNING(message) GOOSEEYE_WARNING_IMPL(message, __FILE__, __LINE__, __FUNCTION__)
#endif

/**
 *  All warnings specific to the Python API are implemented as::
 *
 *      GOOSEEYE_WARNING_PYTHON(...)
 *
 *  They can be enabled by::
 *
 *      #define GOOSEEYE_ENABLE_WARNING_PYTHON
 */
#ifdef GOOSEEYE_ENABLE_WARNING_PYTHON
#define GOOSEEYE_WARNING_PYTHON(message) GOOSEEYE_WARNING_IMPL(message, __FILE__, __LINE__, __FUNCTION__)
#else
#define GOOSEEYE_WARNING_PYTHON(message)
#endif

/**
 *  Toolbox to compute statistics.
 */
namespace GooseEYE {}

#endif

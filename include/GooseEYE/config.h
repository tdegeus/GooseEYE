/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_INCLUDE_H
#define GOOSEEYE_INCLUDE_H


#define _USE_MATH_DEFINES

#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <limits>
#include <algorithm>

#include <xtensor/xtensor.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xpad.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>


#ifdef GOOSEEYE_ENABLE_ASSERT
#define GOOSEEYE_ASSERT(expr) GOOSEEYE_ASSERT_IMPL(expr, __FILE__, __LINE__)
#define GOOSEEYE_ASSERT_IMPL(expr, file, line) \
    if (!(expr)) \
    { \
        throw std::runtime_error( \
          std::string(file) + ':' + std::to_string(line) + \
          ": assertion failed (" #expr ") \n\t"); \
    }
#else
#define GOOSEEYE_ASSERT(expr)
#endif


#define GOOSEEYE_VERSION_MAJOR 0
#define GOOSEEYE_VERSION_MINOR 5
#define GOOSEEYE_VERSION_PATCH 0

#define GOOSEEYE_VERSION_AT_LEAST(x,y,z) \
    (GOOSEEYE_VERSION_MAJOR > x || (GOOSEEYE_VERSION_MAJOR >= x && \
    (GOOSEEYE_VERSION_MINOR > y || (GOOSEEYE_VERSION_MINOR >= y && \
                                    GOOSEEYE_VERSION_PATCH >= z))))

#define GOOSEEYE_VERSION(x,y,z) \
    (GOOSEEYE_VERSION_MAJOR == x && \
     GOOSEEYE_VERSION_MINOR == y && \
     GOOSEEYE_VERSION_PATCH == z)


#define SIGN(a)( (a<0) ? -1 : a>0 ? 1 : 0 )

#define SIGN(a)( (a<0) ? -1 : a>0 ? 1 : 0 )
// #define POS(a) ( (a<0) ?  0 : a           )


#endif

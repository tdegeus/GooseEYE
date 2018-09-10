/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_INCLUDE_H
#define GOOSEEYE_INCLUDE_H

// =================================================================================================

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
#include <cppmat/cppmat.h>

// =================================================================================================

#define SIGN(a)( (a<0) ? -1 : a>0 ? 1 : 0 )
#define POS(a) ( (a<0) ?  0 : a           )

// =================================================================================================

#define GOOSEEYE_WORLD_VERSION 0
#define GOOSEEYE_MAJOR_VERSION 2
#define GOOSEEYE_MINOR_VERSION 0

#define GOOSEEYE_VERSION_AT_LEAST(x,y,z) \
  (GOOSEEYE_WORLD_VERSION>x || (GOOSEEYE_WORLD_VERSION>=x && \
  (GOOSEEYE_MAJOR_VERSION>y || (GOOSEEYE_MAJOR_VERSION>=y && \
                                GOOSEEYE_MINOR_VERSION>=z))))

#define GOOSEEYE_VERSION(x,y,z) \
  (GOOSEEYE_WORLD_VERSION==x && \
   GOOSEEYE_MAJOR_VERSION==y && \
   GOOSEEYE_MINOR_VERSION==z)

// =================================================================================================

// dummy operation that can be use to suppress the "unused parameter" warnings
#define UNUSED(p) ( (void)(p) )

// =================================================================================================

// alias types
namespace GooseEYE
{
  typedef cppmat::array <double> ArrD;
  typedef cppmat::array <int>    ArrI;
  typedef cppmat::matrix<double> MatD;
  typedef cppmat::matrix<int>    MatI;
  typedef std::vector<size_t>    VecS;
  typedef std::vector<int>       VecI;
}

// -------------------------------------------------------------------------------------------------

// enumerate used in "Ensemble"
struct Stat {
  enum Value {
    Unset,
    mean,
    S2,
    W2,
    W2c,
    L,
  };
};

// =================================================================================================

#endif

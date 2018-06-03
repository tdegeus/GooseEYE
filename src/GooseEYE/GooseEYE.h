/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_H
#define GOOSEEYE_H

// =================================================================================================

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
  typedef cppmat::array<double> ArrD;
  typedef cppmat::array<size_t> ArrS;
}

// =================================================================================================

#include "Private.h"
#include "Private.hpp"
#include "Ensemble.h"
#include "Ensemble.hpp"
#include "Ensemble_S2.hpp"

// =================================================================================================

#endif

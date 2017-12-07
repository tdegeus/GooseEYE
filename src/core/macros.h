/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_MACROS_H
#define GOOSEEYE_MACROS_H

// =================================================================================================

#define _USE_MATH_DEFINES // to use "M_PI" from "math.h"

#include <math.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <tuple>
#include <ctime>

#include "cppmat/src/cppmat/cppmat.h"

// =================================================================================================

#define SIGN(a)  ( (a<0) ? -1 : a>0 ? 1 : 0 )
#define POS(a)   ( (a<0) ?  0           : a )

// =================================================================================================

#define GOOSEEYE_WORLD_VERSION 0
#define GOOSEEYE_MAJOR_VERSION 1
#define GOOSEEYE_MINOR_VERSION 1

#define GOOSEEYE_VERSION_AT_LEAST(x,y,z) \
  (GOOSEEYE_WORLD_VERSION>x || (GOOSEEYE_WORLD_VERSION>=x && \
  (GOOSEEYE_MAJOR_VERSION>y || (GOOSEEYE_MAJOR_VERSION>=y && \
                                GOOSEEYE_MINOR_VERSION>=z))))

#define GOOSEEYE_VERSION(x,y,z) \
  (GOOSEEYE_WORLD_VERSION==x && \
   GOOSEEYE_MAJOR_VERSION==y && \
   GOOSEEYE_MINOR_VERSION==z)

// =================================================================================================

#endif

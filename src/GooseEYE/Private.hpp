/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_PRIVATE_HPP
#define GOOSEEYE_PRIVATE_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {
namespace Private {

// =================================================================================================

std::vector<int> midpoint(const std::vector<size_t> &shape, size_t nd, const std::string &fname)
{
  if ( shape.size() > nd )
    throw std::domain_error(fname + ": rank too large");

  for ( auto &i : shape )
    if ( i%2 == 0 )
      throw std::domain_error(fname + ": must be odd shaped");

  std::vector<int> out(shape.size());

  for ( size_t i = 0 ; i < out.size() ; ++i )
    out[i] = static_cast<int>((shape[i]-1)/2);

  out.resize(nd, 0);

  return out;
}

// =================================================================================================

template<class T>
std::vector<size_t> shape(const cppmat::array<T> &A, size_t nd, const std::string &fname)
{
  if ( A.rank() > nd )
    throw std::domain_error(fname + ": rank too large");

  std::vector<size_t> shape = A.shape();

  shape.resize(nd, 1);

  return shape;
}

// =================================================================================================

}} // namespace ...

// =================================================================================================

#endif

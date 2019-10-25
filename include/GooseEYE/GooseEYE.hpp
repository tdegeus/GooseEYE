/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#ifndef GOOSEEYE_HPP
#define GOOSEEYE_HPP

// =================================================================================================

#include "GooseEYE.h"

// =================================================================================================

namespace GooseEYE {

// =================================================================================================
// wrapper functions: distance
// =================================================================================================

xt::xarray<double> distance(const std::vector<size_t>& roi)
{
  Ensemble ensemble(roi);
  return ensemble.distance();
}

// -------------------------------------------------------------------------------------------------

xt::xarray<double> distance(const std::vector<size_t>& roi, size_t dim)
{
  Ensemble ensemble(roi);
  return ensemble.distance(dim);
}

// -------------------------------------------------------------------------------------------------

xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h)
{
  Ensemble ensemble(roi);
  return ensemble.distance(h);
}

// -------------------------------------------------------------------------------------------------

xt::xarray<double> distance(const std::vector<size_t>& roi, const std::vector<double>& h, size_t dim)
{
  Ensemble ensemble(roi);
  return ensemble.distance(h, dim);
}

// =================================================================================================
// wrapper functions: 2-point correlation
// =================================================================================================

template <class T>
xt::xarray<double> S2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<T>& g,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.S2(f, g);
  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

template <class T>
xt::xarray<double> S2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<T>& g,
  const xt::xarray<int>& fmask,
  const xt::xarray<int>& gmask,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.S2(f, g, fmask, gmask);
  return ensemble.result();
}

// =================================================================================================
// wrapper functions: 2-point cluster
// =================================================================================================

xt::xarray<double> C2(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& f,
  const xt::xarray<int>& g,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.C2(f, g);
  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

xt::xarray<double> C2(
  const std::vector<size_t>& roi,
  const xt::xarray<int>& f,
  const xt::xarray<int>& g,
  const xt::xarray<int>& fmask,
  const xt::xarray<int>& gmask,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.C2(f, g, fmask, gmask);
  return ensemble.result();
}

// =================================================================================================
// wrapper functions: weighted 2-point correlation
// =================================================================================================

template <class T>
xt::xarray<double> W2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& w,
  const xt::xarray<T>& f,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.W2(w, f);
  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

template <class T>
xt::xarray<double> W2(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& w,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.W2(w, f, fmask);
  return ensemble.result();
}

// =================================================================================================
// wrapper functions: heightheight
// =================================================================================================

template <class T>
xt::xarray<double> heightheight(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.heightheight(f);
  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

template <class T>
xt::xarray<double> heightheight(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.heightheight(f, fmask );
  return ensemble.result();
}

// =================================================================================================
// wrapper functions: lineal-path
// =================================================================================================

template <class T>
xt::xarray<double> L(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  bool periodic)
{
  Ensemble ensemble(roi, periodic);
  ensemble.L(f);
  return ensemble.result();
}

// -------------------------------------------------------------------------------------------------

template <class T>
xt::xarray<double> L(
  const std::vector<size_t>& roi,
  const xt::xarray<T>& f,
  const xt::xarray<int>& fmask,
  bool periodic
)
{
  Ensemble ensemble(roi, periodic);
  ensemble.L(f, fmask);
  return ensemble.result();
}

// // =================================================================================================
// // wrapper functions: weighted 2-point correlation
// // =================================================================================================

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<int>& f,                    bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<int>& f, const xt::xarray<int>& fmask, bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f, fmask);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<double>& f,                    bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<double>& f, const xt::xarray<int>& fmask, bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f, fmask);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<double>& w, const xt::xarray<int>& f,                    bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<double>& w, const xt::xarray<int>& f, const xt::xarray<int>& fmask, bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f, fmask);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<double>& w, const xt::xarray<double>& f,                    bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2(const std::vector<size_t>& roi, const xt::xarray<double>& w, const xt::xarray<double>& f, const xt::xarray<int>& fmask, bool periodic, bool pad)
// {
//   Ensemble ensemble(roi, periodic, pad);

//   ensemble.W2(w, f, fmask);

//   return ensemble.result();
// }

// // =================================================================================================
// // wrapper functions: collapsed weighted 2-point correlation
// // =================================================================================================

// xt::xarray<double> W2c(const std::vector<size_t>& roi, const xt::xarray<int>& clus, const xt::xarray<int>& cntr, const xt::xarray<int>& f,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c(clus, cntr, f, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c(const std::vector<size_t>& roi, const xt::xarray<int>& clus, const xt::xarray<int>& cntr, const xt::xarray<int>& f, const xt::xarray<int>& fmask,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c(clus, cntr, f, fmask, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c(const std::vector<size_t>& roi, const xt::xarray<int>& clus, const xt::xarray<int>& cntr, const xt::xarray<double>& f,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c(clus, cntr, f, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c(const std::vector<size_t>& roi, const xt::xarray<int>& clus, const xt::xarray<int>& cntr, const xt::xarray<double>& f, const xt::xarray<int>& fmask,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c(clus, cntr, f, fmask, mode);

//   return ensemble.result();
// }

// // =================================================================================================
// // wrapper functions: collapsed weighted 2-point correlation
// // =================================================================================================

// xt::xarray<double> W2c_auto(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<int>& f,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c_auto(w, f, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c_auto(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<int>& f, const xt::xarray<int>& fmask,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c_auto(w, f, fmask, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c_auto(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<double>& f,
//   bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c_auto(w, f, mode);

//   return ensemble.result();
// }

// // -------------------------------------------------------------------------------------------------

// xt::xarray<double> W2c_auto(const std::vector<size_t>& roi, const xt::xarray<int>& w, const xt::xarray<double>& f, const xt::xarray<int>& fmask,
//   bool periodic, std::string mode)

// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.W2c_auto(w, f, fmask, mode);

//   return ensemble.result();
// }

// // =================================================================================================
// // wrapper functions: lineal path function
// // =================================================================================================

// xt::xarray<double> L(const std::vector<size_t>& roi, const xt::xarray<int>& f, bool periodic, std::string mode)
// {
//   Ensemble ensemble(roi, periodic);

//   ensemble.L(f, mode);

//   return ensemble.result();
// }

// // =================================================================================================

} // namespace ...

// =================================================================================================

#endif

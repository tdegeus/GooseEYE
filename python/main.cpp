/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#define XTENSOR_USE_XSIMD

#include <pybind11/pybind11.h>
#include <pyxtensor/pyxtensor.hpp>

#define GOOSEEYE_ENABLE_ASSERT
#include "../include/GooseEYE/GooseEYE.h"

namespace py = pybind11;

PYBIND11_MODULE(GooseEYE, m)
{

m.doc() = "Geometrical statistics";

// -------------------------------------------------------------------------------------------------

py::module kernel = m.def_submodule("kernel", "Kernel definition");

kernel.def("nearest", &GooseEYE::kernel::nearest, py::arg("ndim"));

// -------------------------------------------------------------------------------------------------

py::enum_<GooseEYE::path_mode>(m, "path_mode")
    .value("Bresenham", GooseEYE::path_mode::Bresenham)
    .value("actual", GooseEYE::path_mode::actual)
    .value("full", GooseEYE::path_mode::full)
    .export_values();

m.def("path", &GooseEYE::path,
  py::arg("xa"),
  py::arg("xb"),
  py::arg("mode")=GooseEYE::path_mode::Bresenham);

// -------------------------------------------------------------------------------------------------

m.def("dummy_circles",
  py::overload_cast<
    const std::vector<size_t>&,
    bool>(&GooseEYE::dummy_circles),
  py::arg("shape"),
  py::arg("periodic")=true);

m.def("dummy_circles",
  py::overload_cast<
    const std::vector<size_t>&,
    const std::vector<int>&,
    const std::vector<int>&,
    const std::vector<int>&,
    bool>(&GooseEYE::dummy_circles),
  py::arg("shape"),
  py::arg("row"),
  py::arg("col"),
  py::arg("r"),
  py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

m.def("clusters",
  &GooseEYE::clusters,
  py::arg("f"),
  py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

py::class_<GooseEYE::Ensemble>(m, "Ensemble")

  // Constructors

  .def(py::init<std::vector<size_t>&, bool>(),
    "Ensemble",
    py::arg("roi"),
    py::arg("periodic")=true)

  // Get ensemble averaged result or raw data, and distance

  .def("result", &GooseEYE::Ensemble::result)

  .def("data", &GooseEYE::Ensemble::data)

  .def("norm", &GooseEYE::Ensemble::norm)

  .def("distance", py::overload_cast<>(&GooseEYE::Ensemble::distance, py::const_))

  .def("distance", py::overload_cast<size_t>(&GooseEYE::Ensemble::distance, py::const_))

  // Mean

  .def("mean", py::overload_cast<
      const xt::xarray<double>&>(&GooseEYE::Ensemble::mean<double>),
    py::arg("f"))

  .def("mean", py::overload_cast<
      const xt::xarray<double>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::mean<double>),
    py::arg("f"),
    py::arg("fmask"))

  // 2-point correlation

  .def("S2", py::overload_cast<
      const xt::xarray<int>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::S2<int>),
    py::arg("f"),
    py::arg("g"))

  .def("S2", py::overload_cast<
      const xt::xarray<double>&,
      const xt::xarray<double>&>(&GooseEYE::Ensemble::S2<double>),
    py::arg("f"),
    py::arg("g"))

  .def("S2", py::overload_cast<
      const xt::xarray<double>&,
      const xt::xarray<double>&,
      const xt::xarray<int>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::S2<double>),
    py::arg("f"),
    py::arg("g"),
    py::arg("fmask"),
    py::arg("gmask"))

  // 2-point cluster

  .def("C2", py::overload_cast<
      const xt::xarray<int>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::C2),
    py::arg("f"),
    py::arg("g"))

  .def("C2", py::overload_cast<
      const xt::xarray<int>&,
      const xt::xarray<int>&,
      const xt::xarray<int>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::C2),
    py::arg("f"),
    py::arg("g"),
    py::arg("fmask"),
    py::arg("gmask"))

  // Weighted 2-point correlation

  .def("W2", py::overload_cast<
      const xt::xarray<int>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::W2<int>),
    py::arg("w"),
    py::arg("f"))

  .def("W2", py::overload_cast<
      const xt::xarray<double>&,
      const xt::xarray<double>&>(&GooseEYE::Ensemble::W2<double>),
    py::arg("w"),
    py::arg("f"))

  .def("W2", py::overload_cast<
      const xt::xarray<double>&,
      const xt::xarray<double>&,
      const xt::xarray<int>&>(&GooseEYE::Ensemble::W2<double>),
    py::arg("w"),
    py::arg("f"),
    py::arg("fmask"))

  .def("__repr__",
    [](const GooseEYE::Ensemble &){ return "<GooseEYE.Ensemble>"; }
  );

// -------------------------------------------------------------------------------------------------

// 2-point correlation

m.def("S2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::S2<int>),
  py::arg("roi"),
  py::arg("f"),
  py::arg("g"),
  py::arg("periodic")=true);


m.def("S2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<double>&,
    const xt::xarray<double>&,
    bool>(&GooseEYE::S2<double>),
  py::arg("roi"),
  py::arg("f"),
  py::arg("g"),
  py::arg("periodic")=true);

m.def("S2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<double>&,
    const xt::xarray<double>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::S2<double>),
  py::arg("roi"),
  py::arg("f"),
  py::arg("g"),
  py::arg("fmask"),
  py::arg("gmask"),
  py::arg("periodic")=true);

// 2-point cluster

m.def("C2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::C2),
  py::arg("roi"),
  py::arg("f"),
  py::arg("g"),
  py::arg("periodic")=true);

m.def("C2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::C2),
  py::arg("roi"),
  py::arg("f"),
  py::arg("g"),
  py::arg("fmask"),
  py::arg("gmask"),
  py::arg("periodic")=true);

// Weighted 2-point correlation

m.def("W2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<int>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::W2<int>),
  py::arg("roi"),
  py::arg("w"),
  py::arg("f"),
  py::arg("periodic")=true);

m.def("W2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<double>&,
    const xt::xarray<double>&,
    bool>(&GooseEYE::W2<double>),
  py::arg("roi"),
  py::arg("w"),
  py::arg("f"),
  py::arg("periodic")=true);

m.def("W2", py::overload_cast<
    const std::vector<size_t>&,
    const xt::xarray<double>&,
    const xt::xarray<double>&,
    const xt::xarray<int>&,
    bool>(&GooseEYE::W2<double>),
  py::arg("roi"),
  py::arg("w"),
  py::arg("f"),
  py::arg("fmask"),
  py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

}


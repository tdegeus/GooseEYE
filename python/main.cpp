/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#include <pybind11/pybind11.h>
#include <pyxtensor/pyxtensor.hpp>

#define GOOSEEYE_ENABLE_ASSERT
#include <GooseEYE/GooseEYE.h>

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

m.def("path",
    &GooseEYE::path,
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
        const xt::xtensor<int,1>&,
        const xt::xtensor<int,1>&,
        const xt::xtensor<int,1>&,
        bool>(&GooseEYE::dummy_circles),
    py::arg("shape"),
    py::arg("row"),
    py::arg("col"),
    py::arg("r"),
    py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

m.def("dilate",
    py::overload_cast<
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        const xt::xtensor<size_t,1>&,
        bool>(&GooseEYE::dilate<int, int>),
    py::arg("f"),
    py::arg("kernel"),
    py::arg("iterations"),
    py::arg("periodic")=true);

m.def("dilate",
    py::overload_cast<
        const xt::xarray<int>&,
        const xt::xtensor<size_t,1>&,
        bool>(&GooseEYE::dilate<int>),
    py::arg("f"),
    py::arg("iterations"),
    py::arg("periodic")=true);

m.def("dilate",
    py::overload_cast<
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        size_t,
        bool>(&GooseEYE::dilate<int, int>),
    py::arg("f"),
    py::arg("kernel"),
    py::arg("iterations")=1,
    py::arg("periodic")=true);

m.def("dilate",
    py::overload_cast<
        const xt::xarray<int>&,
        size_t,
        bool>(&GooseEYE::dilate<int>),
    py::arg("f"),
    py::arg("iterations")=1,
    py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

py::class_<GooseEYE::Clusters>(m, "Clusters")

    .def(py::init<const xt::xarray<int>&, bool>(),
        "Clusters",
        py::arg("f"),
        py::arg("periodic")=true)

    .def(py::init<const xt::xarray<int>&, const xt::xarray<int>&, bool>(),
        "Clusters",
        py::arg("f"),
        py::arg("kernel"),
        py::arg("periodic")=true)

    .def("labels",
        &GooseEYE::Clusters::labels)

    .def("centers",
        &GooseEYE::Clusters::centers)

    .def("center_positions",
        &GooseEYE::Clusters::center_positions,
        py::arg("as3d")=false)

    .def("sizes",
        &GooseEYE::Clusters::sizes)

    .def("__repr__",
        [](const GooseEYE::Clusters &){ return "<GooseEYE.Clusters>"; }
    );

// -------------------------------------------------------------------------------------------------

m.def("clusters",
    &GooseEYE::clusters,
    py::arg("f"),
    py::arg("periodic")=true);

// -------------------------------------------------------------------------------------------------

py::class_<GooseEYE::Ensemble>(m, "Ensemble")

    // Constructors

    .def(py::init<std::vector<size_t>&, bool, bool>(),
        "Ensemble",
        py::arg("roi"),
        py::arg("periodic")=true,
        py::arg("variance")=false)

    // Get ensemble averaged result or raw data, and distance

    .def("result",
        &GooseEYE::Ensemble::result)

    .def("variance",
        &GooseEYE::Ensemble::variance)

    .def("data_first",
        &GooseEYE::Ensemble::data_first)

    .def("data_second",
        &GooseEYE::Ensemble::data_second)

    .def("norm",
        &GooseEYE::Ensemble::norm)

    .def("distance",
        py::overload_cast<>(&GooseEYE::Ensemble::distance, py::const_))

    .def("distance",
        py::overload_cast<size_t>(&GooseEYE::Ensemble::distance, py::const_))

    .def("distance",
        py::overload_cast<const std::vector<double>&>(
            &GooseEYE::Ensemble::distance, py::const_))

    .def("distance",
        py::overload_cast<const std::vector<double>&, size_t>(
            &GooseEYE::Ensemble::distance, py::const_))

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

    // Height-Height Correlation Function

    .def("heightheight", py::overload_cast<
            const xt::xarray<double>&>(&GooseEYE::Ensemble::heightheight<double>),
        py::arg("f"))

    .def("heightheight", py::overload_cast<
            const xt::xarray<double>&,
            const xt::xarray<int>&>(&GooseEYE::Ensemble::heightheight<double>),
        py::arg("f"),
        py::arg("fmask"))

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

    // Collapsed weighted 2-point correlation

    .def("W2c", py::overload_cast<
            const xt::xarray<int>&,
            const xt::xarray<int>&,
            const xt::xarray<int>&,
            GooseEYE::path_mode>(&GooseEYE::Ensemble::W2c<int>),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("mode")=GooseEYE::path_mode::Bresenham)

    .def("W2c", py::overload_cast<
            const xt::xarray<int>&,
            const xt::xarray<int>&,
            const xt::xarray<double>&,
            GooseEYE::path_mode>(&GooseEYE::Ensemble::W2c<double>),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("mode")=GooseEYE::path_mode::Bresenham)

    .def("W2c", py::overload_cast<
            const xt::xarray<int>&,
            const xt::xarray<int>&,
            const xt::xarray<double>&,
            const xt::xarray<int>&,
            GooseEYE::path_mode>(&GooseEYE::Ensemble::W2c<double>),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("fmask"),
        py::arg("mode")=GooseEYE::path_mode::Bresenham)

    // Lineal-path function

    .def("L",
        &GooseEYE::Ensemble::L<int>,
        py::arg("f"),
        py::arg("mode")=GooseEYE::path_mode::Bresenham)

    .def("__repr__",
        [](const GooseEYE::Ensemble &){ return "<GooseEYE.Ensemble>"; }
    );

// -------------------------------------------------------------------------------------------------

// distance

m.def("distance", py::overload_cast<
        const std::vector<size_t>&>(&GooseEYE::distance),
    py::arg("roi"));

m.def("distance", py::overload_cast<
        const std::vector<size_t>&,
        size_t>(&GooseEYE::distance),
    py::arg("roi"),
    py::arg("dim"));

m.def("distance", py::overload_cast<
        const std::vector<size_t>&,
        const std::vector<double>&>(&GooseEYE::distance),
    py::arg("roi"),
    py::arg("h"));

m.def("distance", py::overload_cast<
        const std::vector<size_t>&,
        const std::vector<double>&,
        size_t>(&GooseEYE::distance),
    py::arg("roi"),
    py::arg("h"),
    py::arg("dim"));

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

// Collapsed weighted 2-point correlation

m.def("W2c", py::overload_cast<
        const std::vector<size_t>&,
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        GooseEYE::path_mode,
        bool>(&GooseEYE::W2c<int>),
    py::arg("roi"),
    py::arg("clusters"),
    py::arg("centers"),
    py::arg("f"),
    py::arg("mode")=GooseEYE::path_mode::Bresenham,
    py::arg("periodic")=true);

m.def("W2c", py::overload_cast<
        const std::vector<size_t>&,
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        const xt::xarray<double>&,
        GooseEYE::path_mode,
        bool>(&GooseEYE::W2c<double>),
    py::arg("roi"),
    py::arg("clusters"),
    py::arg("centers"),
    py::arg("f"),
    py::arg("mode")=GooseEYE::path_mode::Bresenham,
    py::arg("periodic")=true);

m.def("W2c", py::overload_cast<
        const std::vector<size_t>&,
        const xt::xarray<int>&,
        const xt::xarray<int>&,
        const xt::xarray<double>&,
        const xt::xarray<int>&,
        GooseEYE::path_mode,
        bool>(&GooseEYE::W2c<double>),
    py::arg("roi"),
    py::arg("clusters"),
    py::arg("centers"),
    py::arg("f"),
    py::arg("fmask"),
    py::arg("mode")=GooseEYE::path_mode::Bresenham,
    py::arg("periodic")=true);

// Height-Height Correlation Function

m.def("heightheight", py::overload_cast<
        const std::vector<size_t>&,
        const xt::xarray<double>&,
        bool>(&GooseEYE::heightheight<double>),
    py::arg("roi"),
    py::arg("f"),
    py::arg("periodic")=true);

m.def("heightheight", py::overload_cast<
        const std::vector<size_t>&,
        const xt::xarray<double>&,
        const xt::xarray<int>&,
        bool>(&GooseEYE::heightheight<double>),
    py::arg("roi"),
    py::arg("f"),
    py::arg("fmask"),
    py::arg("periodic")=true);

m.def("L",
    &GooseEYE::L<int>,
    py::arg("roi"),
    py::arg("f"),
    py::arg("periodic")=true,
    py::arg("mode")=GooseEYE::path_mode::Bresenham);

// -------------------------------------------------------------------------------------------------

}

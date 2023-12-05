/**
 * @file
 * @copyright Copyright 2017. Tom de Geus. All rights reserved.
 * @license This project is released under the GPLv3 License.
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pyarray.hpp>
#include <xtensor-python/pytensor.hpp>

#define GOOSEEYE_USE_XTENSOR_PYTHON
#include <GooseEYE/GooseEYE.h>

namespace py = pybind11;

template <size_t First, size_t Last, typename Lambda>
inline void static_for(Lambda const& f)
{
    if constexpr (First < Last) {
        f(std::integral_constant<size_t, First>{});
        static_for<First + 1, Last>(f);
    }
}

template <class Class>
void allocate_ClusterLabeller(py::module& mod)
{
    const size_t Dim = Class::Dim;
    std::string name = "ClusterLabeller" + std::to_string(Dim);
    if (Class::Periodic) {
        name += "p";
    }
    py::class_<Class> cls(mod, name.c_str());
    cls.def(py::init<const std::template array<size_t, Dim>&>(), name.c_str(), py::arg("shape"));
    cls.def(
        py::init<const std::template array<size_t, Dim>&, const xt::template pytensor<int, Dim>&>(),
        name.c_str(),
        py::arg("shape"),
        py::arg("kernel"));

    cls.def("__repr__", &Class::repr);
    cls.def_property_readonly("shape", &Class::shape, "Shape of system.");
    cls.def_property_readonly("size", &Class::shape, "Size of system.");
    cls.def_property_readonly("labels", &Class::labels, "Cluster of each block.");
    cls.def("prune", &Class::prune, "Prune: renumber to smallest index.");
    cls.def("reset", &Class::reset, "Reset labels to zero.");
    cls.def(
        "add_image",
        &Class::template add_image<xt::pytensor<int, Dim>>,
        "Add image",
        py::arg("img"));

    cls.def(
        "add_points",
        static_cast<void (Class::*)(const xt::pytensor<size_t, 1>&)>(&Class::add_points),
        "Add points",
        py::arg("idx"));

    cls.def(
        "add_sequence",
        &Class::template add_sequence<xt::pytensor<size_t, 1>>,
        "Add points",
        py::arg("idx"));
}

PYBIND11_MODULE(_GooseEYE, m)
{
    xt::import_numpy();

    m.doc() = "Geometrical statistics";

    m.def("version", &GooseEYE::version, "Return version string.");

    py::module kernel = m.def_submodule("kernel", "Kernel definition");

    kernel.def("nearest", &GooseEYE::kernel::nearest, py::arg("ndim"));

    py::enum_<GooseEYE::path_mode>(m, "path_mode")
        .value("Bresenham", GooseEYE::path_mode::Bresenham)
        .value("actual", GooseEYE::path_mode::actual)
        .value("full", GooseEYE::path_mode::full)
        .export_values();

    m.def(
        "path",
        &GooseEYE::path,
        py::arg("xa"),
        py::arg("xb"),
        py::arg("mode") = GooseEYE::path_mode::Bresenham);

    m.def(
        "dummy_circles",
        py::overload_cast<const std::vector<size_t>&, bool, uint64_t>(&GooseEYE::dummy_circles),
        py::arg("shape"),
        py::arg("periodic") = true,
        py::arg("seed") = 0);

    m.def(
        "dummy_circles",
        py::overload_cast<
            const std::vector<size_t>&,
            const xt::pytensor<int, 1>&,
            const xt::pytensor<int, 1>&,
            const xt::pytensor<int, 1>&,
            bool>(&GooseEYE::dummy_circles),
        py::arg("shape"),
        py::arg("row"),
        py::arg("col"),
        py::arg("r"),
        py::arg("periodic") = true);

    m.def(
        "dilate",
        py::overload_cast<
            const xt::pyarray<int>&,
            const xt::pyarray<int>&,
            const xt::pytensor<size_t, 1>&,
            bool>(&GooseEYE::dilate<xt::pyarray<int>, xt::pyarray<int>>),
        py::arg("f"),
        py::arg("kernel"),
        py::arg("iterations"),
        py::arg("periodic") = true);

    m.def(
        "dilate",
        py::overload_cast<const xt::pyarray<int>&, const xt::pytensor<size_t, 1>&, bool>(
            &GooseEYE::dilate<xt::pyarray<int>>),
        py::arg("f"),
        py::arg("iterations"),
        py::arg("periodic") = true);

    m.def(
        "dilate",
        py::overload_cast<const xt::pyarray<int>&, const xt::pyarray<int>&, size_t, bool>(
            &GooseEYE::dilate<xt::pyarray<int>, xt::pyarray<int>>),
        py::arg("f"),
        py::arg("kernel"),
        py::arg("iterations") = 1,
        py::arg("periodic") = true);

    m.def(
        "dilate",
        py::overload_cast<const xt::pyarray<int>&, size_t, bool>(
            &GooseEYE::dilate<xt::pyarray<int>>),
        py::arg("f"),
        py::arg("iterations") = 1,
        py::arg("periodic") = true);

    static_for<1, 4>(
        [&](auto i) { allocate_ClusterLabeller<GooseEYE::ClusterLabeller<i, true>>(m); });
    static_for<1, 4>(
        [&](auto i) { allocate_ClusterLabeller<GooseEYE::ClusterLabeller<i, false>>(m); });

    py::class_<GooseEYE::Clusters>(m, "Clusters")

        .def(
            py::init<const xt::pyarray<int>&, bool>(),
            "Clusters",
            py::arg("f"),
            py::arg("periodic") = true)

        .def(
            py::init<const xt::pyarray<int>&, const xt::pyarray<int>&, bool>(),
            "Clusters",
            py::arg("f"),
            py::arg("kernel"),
            py::arg("periodic") = true)

        .def("labels", &GooseEYE::Clusters::labels)

        .def("centers", &GooseEYE::Clusters::centers)

        .def("center_positions", &GooseEYE::Clusters::center_positions, py::arg("as3d") = false)

        .def("sizes", &GooseEYE::Clusters::sizes)

        .def("__repr__", [](const GooseEYE::Clusters&) { return "<GooseEYE.Clusters>"; });

    m.def(
        "clusters",
        &GooseEYE::clusters<xt::pyarray<int>>,
        py::arg("f"),
        py::arg("periodic") = true);

    m.def("labels_map", &GooseEYE::labels_map<xt::pyarray<int>>, py::arg("a"), py::arg("b"));

    m.def(
        "labels_rename",
        &GooseEYE::labels_rename<xt::pyarray<int>, xt::xtensor<int, 2>>,
        py::arg("labels"),
        py::arg("rename"));

    m.def("labels_prune", &GooseEYE::labels_prune<xt::pyarray<int>>, py::arg("labels"));

    m.def(
        "labels_reorder",
        &GooseEYE::labels_reorder<xt::pyarray<int>, xt::xtensor<int, 1>>,
        py::arg("labels"),
        py::arg("order"));

    m.def("labels_sizes", &GooseEYE::labels_sizes<xt::pyarray<int>>, py::arg("labels"));

    m.def(
        "relabel_map",
        &GooseEYE::relabel_map<xt::pyarray<int>, xt::pyarray<int>>,
        py::arg("a"),
        py::arg("b"));

    m.def(
        "pos2img",
        &GooseEYE::pos2img<xt::pyarray<size_t>, xt::pytensor<double, 2>, xt::pytensor<size_t, 1>>,
        py::arg("img"),
        py::arg("positions"),
        py::arg("labels"));

    m.def(
        "center_of_mass",
        &GooseEYE::center_of_mass<xt::pyarray<size_t>>,
        py::arg("labels"),
        py::arg("periodic") = true);

    py::class_<GooseEYE::Ensemble>(m, "Ensemble")

        // Constructors

        .def(
            py::init<std::vector<size_t>&, bool, bool>(),
            "Ensemble",
            py::arg("roi"),
            py::arg("periodic") = true,
            py::arg("variance") = false)

        // Get ensemble averaged result or raw data, and distance

        .def("result", &GooseEYE::Ensemble::result)

        .def("variance", &GooseEYE::Ensemble::variance)

        .def("data_first", &GooseEYE::Ensemble::data_first)

        .def("data_second", &GooseEYE::Ensemble::data_second)

        .def("norm", &GooseEYE::Ensemble::norm)

        .def("distance", py::overload_cast<>(&GooseEYE::Ensemble::distance, py::const_))

        .def("distance", py::overload_cast<size_t>(&GooseEYE::Ensemble::distance, py::const_))

        .def(
            "distance",
            py::overload_cast<const std::vector<double>&>(
                &GooseEYE::Ensemble::distance, py::const_))

        .def(
            "distance",
            py::overload_cast<const std::vector<double>&, size_t>(
                &GooseEYE::Ensemble::distance, py::const_))

        // Mean

        .def(
            "mean",
            py::overload_cast<const xt::pyarray<double>&>(
                &GooseEYE::Ensemble::mean<xt::pyarray<double>>),
            py::arg("f"))

        .def(
            "mean",
            py::overload_cast<const xt::pyarray<double>&, const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::mean<xt::pyarray<double>, xt::pyarray<int>>),
            py::arg("f"),
            py::arg("fmask"))

        // 2-point correlation

        .def(
            "S2",
            py::overload_cast<const xt::pyarray<int>&, const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::S2<xt::pyarray<int>>),
            py::arg("f"),
            py::arg("g"))

        .def(
            "S2",
            py::overload_cast<const xt::pyarray<double>&, const xt::pyarray<double>&>(
                &GooseEYE::Ensemble::S2<xt::pyarray<double>>),
            py::arg("f"),
            py::arg("g"))

        .def(
            "S2",
            py::overload_cast<
                const xt::pyarray<double>&,
                const xt::pyarray<double>&,
                const xt::pyarray<int>&,
                const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::S2<xt::pyarray<double>, xt::pyarray<int>>),
            py::arg("f"),
            py::arg("g"),
            py::arg("fmask"),
            py::arg("gmask"))

        // Height-Height Correlation Function

        .def(
            "heightheight",
            py::overload_cast<const xt::pyarray<double>&>(
                &GooseEYE::Ensemble::heightheight<xt::pyarray<double>>),
            py::arg("f"))

        .def(
            "heightheight",
            py::overload_cast<const xt::pyarray<double>&, const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::heightheight<xt::pyarray<double>, xt::pyarray<int>>),
            py::arg("f"),
            py::arg("fmask"))

        // 2-point cluster

        .def(
            "C2",
            py::overload_cast<const xt::pyarray<int>&, const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::C2<xt::pyarray<int>>),
            py::arg("f"),
            py::arg("g"))

        .def(
            "C2",
            py::overload_cast<
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::C2<xt::pyarray<int>, xt::pyarray<int>>),
            py::arg("f"),
            py::arg("g"),
            py::arg("fmask"),
            py::arg("gmask"))

        // Weighted 2-point correlation

        .def(
            "W2",
            py::overload_cast<const xt::pyarray<int>&, const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::W2<xt::pyarray<int>>),
            py::arg("w"),
            py::arg("f"))

        .def(
            "W2",
            py::overload_cast<const xt::pyarray<double>&, const xt::pyarray<double>&>(
                &GooseEYE::Ensemble::W2<xt::pyarray<double>>),
            py::arg("w"),
            py::arg("f"))

        .def(
            "W2",
            py::overload_cast<
                const xt::pyarray<double>&,
                const xt::pyarray<double>&,
                const xt::pyarray<int>&>(
                &GooseEYE::Ensemble::W2<xt::pyarray<double>, xt::pyarray<int>>),
            py::arg("w"),
            py::arg("f"),
            py::arg("fmask"))

        // Collapsed weighted 2-point correlation

        .def(
            "W2c",
            py::overload_cast<
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                GooseEYE::path_mode>(&GooseEYE::Ensemble::W2c<xt::pyarray<int>, xt::pyarray<int>>),
            py::arg("clusters"),
            py::arg("centers"),
            py::arg("f"),
            py::arg("mode") = GooseEYE::path_mode::Bresenham)

        .def(
            "W2c",
            py::overload_cast<
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                const xt::pyarray<double>&,
                GooseEYE::path_mode>(
                &GooseEYE::Ensemble::W2c<xt::pyarray<int>, xt::pyarray<double>>),
            py::arg("clusters"),
            py::arg("centers"),
            py::arg("f"),
            py::arg("mode") = GooseEYE::path_mode::Bresenham)

        .def(
            "W2c",
            py::overload_cast<
                const xt::pyarray<int>&,
                const xt::pyarray<int>&,
                const xt::pyarray<double>&,
                const xt::pyarray<int>&,
                GooseEYE::path_mode>(
                &GooseEYE::Ensemble::W2c<xt::pyarray<int>, xt::pyarray<double>, xt::pyarray<int>>),
            py::arg("clusters"),
            py::arg("centers"),
            py::arg("f"),
            py::arg("fmask"),
            py::arg("mode") = GooseEYE::path_mode::Bresenham)

        // Lineal-path function

        .def(
            "L",
            &GooseEYE::Ensemble::L<xt::pyarray<int>>,
            py::arg("f"),
            py::arg("mode") = GooseEYE::path_mode::Bresenham)

        .def("__repr__", [](const GooseEYE::Ensemble&) { return "<GooseEYE.Ensemble>"; });

    // distance

    m.def(
        "distance",
        py::overload_cast<const std::vector<size_t>&>(&GooseEYE::distance),
        py::arg("roi"));

    m.def(
        "distance",
        py::overload_cast<const std::vector<size_t>&, size_t>(&GooseEYE::distance),
        py::arg("roi"),
        py::arg("dim"));

    m.def(
        "distance",
        py::overload_cast<const std::vector<size_t>&, const std::vector<double>&>(
            &GooseEYE::distance),
        py::arg("roi"),
        py::arg("h"));

    m.def(
        "distance",
        py::overload_cast<const std::vector<size_t>&, const std::vector<double>&, size_t>(
            &GooseEYE::distance),
        py::arg("roi"),
        py::arg("h"),
        py::arg("dim"));

    // 2-point correlation

    m.def(
        "S2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& f,
           const xt::pyarray<int>& g,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.S2(f, g);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("g"),
        py::arg("periodic") = true);

    m.def(
        "S2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<double>& f,
           const xt::pyarray<double>& g,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.S2(f, g);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("g"),
        py::arg("periodic") = true);

    m.def(
        "S2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<double>& f,
           const xt::pyarray<double>& g,
           const xt::pyarray<int>& fmask,
           const xt::pyarray<int>& gmask,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.S2(f, g, fmask, gmask);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("g"),
        py::arg("fmask"),
        py::arg("gmask"),
        py::arg("periodic") = true);

    // 2-point cluster

    m.def(
        "C2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& f,
           const xt::pyarray<int>& g,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.C2(f, g);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("g"),
        py::arg("periodic") = true);

    m.def(
        "C2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& f,
           const xt::pyarray<int>& g,
           const xt::pyarray<int>& fmask,
           const xt::pyarray<int>& gmask,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.C2(f, g, fmask, gmask);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("g"),
        py::arg("fmask"),
        py::arg("gmask"),
        py::arg("periodic") = true);

    // Weighted 2-point correlation

    m.def(
        "W2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& w,
           const xt::pyarray<int>& f,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2(w, f);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("w"),
        py::arg("f"),
        py::arg("periodic") = true);

    m.def(
        "W2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<double>& w,
           const xt::pyarray<double>& f,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2(w, f);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("w"),
        py::arg("f"),
        py::arg("periodic") = true);

    m.def(
        "W2",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<double>& w,
           const xt::pyarray<double>& f,
           const xt::pyarray<int>& fmask,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2(w, f, fmask);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("w"),
        py::arg("f"),
        py::arg("fmask"),
        py::arg("periodic") = true);

    // Collapsed weighted 2-point correlation

    m.def(
        "W2c",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& clusters,
           const xt::pyarray<int>& centers,
           const xt::pyarray<int>& f,
           GooseEYE::path_mode mode,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2c(clusters, centers, f, mode);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("mode") = GooseEYE::path_mode::Bresenham,
        py::arg("periodic") = true);

    m.def(
        "W2c",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& clusters,
           const xt::pyarray<int>& centers,
           const xt::pyarray<double>& f,
           GooseEYE::path_mode mode,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2c(clusters, centers, f, mode);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("mode") = GooseEYE::path_mode::Bresenham,
        py::arg("periodic") = true);

    m.def(
        "W2c",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& clusters,
           const xt::pyarray<int>& centers,
           const xt::pyarray<double>& f,
           const xt::pyarray<int>& fmask,
           GooseEYE::path_mode mode,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.W2c(clusters, centers, f, fmask, mode);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("clusters"),
        py::arg("centers"),
        py::arg("f"),
        py::arg("fmask"),
        py::arg("mode") = GooseEYE::path_mode::Bresenham,
        py::arg("periodic") = true);

    // Height-Height Correlation Function

    m.def(
        "heightheight",
        [](const std::vector<size_t>& roi, const xt::pyarray<double>& f, bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.heightheight(f);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("periodic") = true);

    m.def(
        "heightheight",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<double>& f,
           const xt::pyarray<int>& fmask,
           bool periodic) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.heightheight(f, fmask);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("fmask"),
        py::arg("periodic") = true);

    m.def(
        "L",
        [](const std::vector<size_t>& roi,
           const xt::pyarray<int>& f,
           bool periodic,
           GooseEYE::path_mode mode) {
            GooseEYE::Ensemble ensemble(roi, periodic);
            ensemble.L(f, mode);
            return ensemble.result();
        },
        py::arg("roi"),
        py::arg("f"),
        py::arg("periodic") = true,
        py::arg("mode") = GooseEYE::path_mode::Bresenham);
}

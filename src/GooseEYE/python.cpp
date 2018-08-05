/* =================================================================================================

(c - GPLv3) T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me | github.com/tdegeus/GooseEYE

================================================================================================= */

#include <pybind11/pybind11.h>
#include <cppmat/cppmat.h>
#include <cppmat/pybind11.h>

#include "GooseEYE.h"

// =================================================================================================

// abbreviate name-space
namespace py = pybind11;
namespace M  = GooseEYE;

// abbreviate types
typedef GooseEYE::ArrD ArrD;
typedef GooseEYE::ArrI ArrI;
typedef GooseEYE::MatD MatD;
typedef GooseEYE::MatI MatI;
typedef GooseEYE::VecS VecS;
typedef GooseEYE::VecI VecI;

// abbreviate const types
typedef const GooseEYE::ArrD cArrD;
typedef const GooseEYE::ArrI cArrI;
typedef const GooseEYE::MatD cMatD;
typedef const GooseEYE::MatI cMatI;
typedef const GooseEYE::VecS cVecS;
typedef const GooseEYE::VecI cVecI;

// =================================================================================================

PYBIND11_MODULE(GooseEYE, m) {

m.doc() = "Geometrical statistics";

// =================================================================================================

py::class_<M::Ensemble>(m, "Ensemble")
  // -
  .def(py::init<cVecS &, bool, bool>(), "Ensemble", py::arg("roi"), py::arg("periodic")=true, py::arg("zero_pad")=false)
  // -
  .def("result"  , &M::Ensemble::result)
  .def("data"    , &M::Ensemble::data  )
  .def("norm"    , &M::Ensemble::norm  )
  // -
  .def("mean"    , py::overload_cast<cArrD&        >(&M::Ensemble::mean), py::arg("f"))
  .def("mean"    , py::overload_cast<cArrD&, cArrI&>(&M::Ensemble::mean), py::arg("f"), py::arg("fmask"))
  // -
  .def("S2"      , py::overload_cast<ArrI, ArrI            >(&M::Ensemble::S2), py::arg("f"), py::arg("g"))
  .def("S2"      , py::overload_cast<ArrI, ArrI, ArrI, ArrI>(&M::Ensemble::S2), py::arg("f"), py::arg("g"), py::arg("fmask"), py::arg("gmask"))
  .def("S2"      , py::overload_cast<ArrD, ArrD            >(&M::Ensemble::S2), py::arg("f"), py::arg("g"))
  .def("S2"      , py::overload_cast<ArrD, ArrD, ArrI, ArrI>(&M::Ensemble::S2), py::arg("f"), py::arg("g"), py::arg("fmask"), py::arg("gmask"))
  // -
  .def("W2"      , py::overload_cast<ArrI, ArrI      >(&M::Ensemble::W2), py::arg("w"), py::arg("f"))
  .def("W2"      , py::overload_cast<ArrI, ArrI, ArrI>(&M::Ensemble::W2), py::arg("w"), py::arg("f"), py::arg("fmask"))
  .def("W2"      , py::overload_cast<ArrI, ArrD      >(&M::Ensemble::W2), py::arg("w"), py::arg("f"))
  .def("W2"      , py::overload_cast<ArrI, ArrD, ArrI>(&M::Ensemble::W2), py::arg("w"), py::arg("f"), py::arg("fmask"))
  .def("W2"      , py::overload_cast<ArrD, ArrI      >(&M::Ensemble::W2), py::arg("w"), py::arg("f"))
  .def("W2"      , py::overload_cast<ArrD, ArrI, ArrI>(&M::Ensemble::W2), py::arg("w"), py::arg("f"), py::arg("fmask"))
  .def("W2"      , py::overload_cast<ArrI, ArrD      >(&M::Ensemble::W2), py::arg("w"), py::arg("f"))
  .def("W2"      , py::overload_cast<ArrI, ArrD, ArrI>(&M::Ensemble::W2), py::arg("w"), py::arg("f"), py::arg("fmask"))
  // -
  .def("W2c"     , py::overload_cast<ArrI, ArrI, ArrI,       std::string>(&M::Ensemble::W2c), py::arg("clus"), py::arg("cntr"), py::arg("f"),                   py::arg("mode")="Bresenham")
  .def("W2c"     , py::overload_cast<ArrI, ArrI, ArrI, ArrI, std::string>(&M::Ensemble::W2c), py::arg("clus"), py::arg("cntr"), py::arg("f"), py::arg("fmask"), py::arg("mode")="Bresenham")
  .def("W2c"     , py::overload_cast<ArrI, ArrI, ArrD,       std::string>(&M::Ensemble::W2c), py::arg("clus"), py::arg("cntr"), py::arg("f"),                   py::arg("mode")="Bresenham")
  .def("W2c"     , py::overload_cast<ArrI, ArrI, ArrD, ArrI, std::string>(&M::Ensemble::W2c), py::arg("clus"), py::arg("cntr"), py::arg("f"), py::arg("fmask"), py::arg("mode")="Bresenham")
  // -
  .def("W2c_auto", py::overload_cast<ArrI, ArrI,       std::string>(&M::Ensemble::W2c_auto), py::arg("w"), py::arg("f"),                   py::arg("mode")="Bresenham")
  .def("W2c_auto", py::overload_cast<ArrI, ArrI, ArrI, std::string>(&M::Ensemble::W2c_auto), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("mode")="Bresenham")
  .def("W2c_auto", py::overload_cast<ArrI, ArrD,       std::string>(&M::Ensemble::W2c_auto), py::arg("w"), py::arg("f"),                   py::arg("mode")="Bresenham")
  .def("W2c_auto", py::overload_cast<ArrI, ArrD, ArrI, std::string>(&M::Ensemble::W2c_auto), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("mode")="Bresenham")
  // -
  .def("L", &M::Ensemble::L, py::arg("f"), py::arg("mode")="Bresenham")
  // -
  .def("stampPoints", &M::Ensemble::stampPoints, py::arg("nd")=0)
  // -
  .def("__repr__",
    [](const M::Ensemble &){ return "<GooseEYE.Ensemble>"; }
  );

// =================================================================================================

m.def("S2"      , py::overload_cast<cVecS &, cArrI &, cArrI &,                   bool, bool>(&M::S2), py::arg("roi"), py::arg("f"), py::arg("g"),                                     py::arg("periodic")=true, py::arg("pad")=false);
m.def("S2"      , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrI &, cArrI &, bool, bool>(&M::S2), py::arg("roi"), py::arg("f"), py::arg("g"), py::arg("fmask"), py::arg("gmask"), py::arg("periodic")=true, py::arg("pad")=false);
m.def("S2"      , py::overload_cast<cVecS &, cArrD &, cArrD &,                   bool, bool>(&M::S2), py::arg("roi"), py::arg("f"), py::arg("g"),                                     py::arg("periodic")=true, py::arg("pad")=false);
m.def("S2"      , py::overload_cast<cVecS &, cArrD &, cArrD &, cArrI &, cArrI &, bool, bool>(&M::S2), py::arg("roi"), py::arg("f"), py::arg("g"), py::arg("fmask"), py::arg("gmask"), py::arg("periodic")=true, py::arg("pad")=false);
// -
m.def("W2"      , py::overload_cast<cVecS &, cArrI &, cArrI &,          bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrI &, bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrI &, cArrD &,          bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrI &, cArrD &, cArrI &, bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrD &, cArrI &,          bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrD &, cArrI &, cArrI &, bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrD &, cArrD &,          bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("pad")=false);
m.def("W2"      , py::overload_cast<cVecS &, cArrD &, cArrD &, cArrI &, bool, bool>(&M::W2), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("pad")=false);
// -
m.def("W2c"     , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrI &,          bool, std::string>(&M::W2c), py::arg("roi"), py::arg("clus"), py::arg("cntr"), py::arg("f"),                   py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c"     , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrI &, cArrI &, bool, std::string>(&M::W2c), py::arg("roi"), py::arg("clus"), py::arg("cntr"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c"     , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrD &,          bool, std::string>(&M::W2c), py::arg("roi"), py::arg("clus"), py::arg("cntr"), py::arg("f"),                   py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c"     , py::overload_cast<cVecS &, cArrI &, cArrI &, cArrD &, cArrI &, bool, std::string>(&M::W2c), py::arg("roi"), py::arg("clus"), py::arg("cntr"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("mode")="Bresenham");
// -
m.def("W2c_auto", py::overload_cast<cVecS &, cArrI &, cArrI &,          bool, std::string>(&M::W2c_auto), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c_auto", py::overload_cast<cVecS &, cArrI &, cArrI &, cArrI &, bool, std::string>(&M::W2c_auto), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c_auto", py::overload_cast<cVecS &, cArrI &, cArrD &,          bool, std::string>(&M::W2c_auto), py::arg("roi"), py::arg("w"), py::arg("f"),                   py::arg("periodic")=true, py::arg("mode")="Bresenham");
m.def("W2c_auto", py::overload_cast<cVecS &, cArrI &, cArrD &, cArrI &, bool, std::string>(&M::W2c_auto), py::arg("roi"), py::arg("w"), py::arg("f"), py::arg("fmask"), py::arg("periodic")=true, py::arg("mode")="Bresenham");
// -
m.def("L", &M::L, py::arg("roi"), py::arg("f"), py::arg("periodic")=true, py::arg("mode")="Bresenham");

// =================================================================================================

m.def("dummy_circles", py::overload_cast<cVecS &,                            bool>(&M::dummy_circles), py::arg("shape"),                                               py::arg("periodic")=true);
m.def("dummy_circles", py::overload_cast<cVecS &, cVecI &, cVecI &, cVecI &, bool>(&M::dummy_circles), py::arg("shape"), py::arg("row"), py::arg("col"), py::arg("r"), py::arg("periodic")=true);
// -
m.def("kernel", &M::kernel);
// -
m.def("clusters"      , py::overload_cast<cArrI &,               bool>(&M::clusters      ), py::arg("f"),                                         py::arg("periodic")=true);
m.def("clusters"      , py::overload_cast<cArrI &,          int, bool>(&M::clusters      ), py::arg("f"),                  py::arg("min_size")  , py::arg("periodic")=true);
m.def("clusters"      , py::overload_cast<cArrI &, cArrI &, int, bool>(&M::clusters      ), py::arg("f"), py::arg("kern"), py::arg("min_size")=0, py::arg("periodic")=true);
// -
m.def("clusterCenters", py::overload_cast<cArrI &,               bool>(&M::clusterCenters), py::arg("f"),                                         py::arg("periodic")=true);
m.def("clusterCenters", py::overload_cast<cArrI &,          int, bool>(&M::clusterCenters), py::arg("f"),                  py::arg("min_size")  , py::arg("periodic")=true);
m.def("clusterCenters", py::overload_cast<cArrI &, cArrI &, int, bool>(&M::clusterCenters), py::arg("f"), py::arg("kern"), py::arg("min_size")=0, py::arg("periodic")=true);
// -
m.def("dilate", py::overload_cast<cArrI &,          size_t , bool>(&M::dilate), py::arg("f"),                  py::arg("iterations")=1, py::arg("periodic")=true);
m.def("dilate", py::overload_cast<cArrI &,          cVecS &, bool>(&M::dilate), py::arg("f"),                  py::arg("iterations")  , py::arg("periodic")=true);
m.def("dilate", py::overload_cast<cArrI &, cArrI &, size_t , bool>(&M::dilate), py::arg("f"), py::arg("kern"), py::arg("iterations")=1, py::arg("periodic")=true);
m.def("dilate", py::overload_cast< ArrI  ,  ArrI  , cVecS &, bool>(&M::dilate), py::arg("f"), py::arg("kern"), py::arg("iterations")  , py::arg("periodic")=true);
// -
m.def("path", &M::path, py::arg("xa"), py::arg("xb"), py::arg("mode")="Bresenham");
// -
m.def("stampPoints", &M::stampPoints, py::arg("shape"));

// =================================================================================================

}


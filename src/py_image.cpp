
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "image.h"

namespace py = pybind11;

// =============================================================================
// type caster: Image::Matrix <-> NumPy-array
// =============================================================================

namespace pybind11 { namespace detail {
  template <typename T> struct type_caster<Image::Matrix<T>>
  {
    public:

      PYBIND11_TYPE_CASTER(Image::Matrix<T>, _("Image::Matrix<T>"));

      // Conversion part 1 (Python -> C++)
      bool load(py::handle src, bool convert) {

        if (!convert && !py::array_t<T>::check_(src))
          return false;

        auto buf = py::array_t<T, py::array::c_style | py::array::forcecast>::ensure(src);
        if (!buf)
          return false;

        auto dims = buf.ndim();
        if (dims < 1 || dims > 3)
          return false;

        std::vector<size_t> shape(buf.ndim());

        for ( int i=0 ; i<buf.ndim() ; i++ )
          shape[i] = buf.shape()[i];

        value = Image::Matrix<T>(shape,buf.data());

        return true;
      }

      //Conversion part 2 (C++ -> Python)
      static py::handle cast(const Image::Matrix<T>& src, py::return_value_policy policy, py::handle parent) {

        py::array a(std::move(src.shape()), std::move(src.strides(true)), src.data() );

        return a.release();

      }
  };
}} // namespace pybind11::detail

// =============================================================================
// wrap as Python module
// =============================================================================

PYBIND11_PLUGIN(gooseeye) {
  py::module m("gooseeye", "Geometrical statistics");
  py::module mi = m.def_submodule("image", "Image-based input");


  mi.def("dummy_circles", py::overload_cast<std::vector<size_t>&,                                                      bool>(&Image::dummy_circles), "Dummy image", py::arg("shape"),                                           py::arg("periodic")=true );
  mi.def("dummy_circles", py::overload_cast<std::vector<size_t>&,std::vector<int>&,std::vector<int>&,std::vector<int>&,bool>(&Image::dummy_circles), "Dummy image", py::arg("shape"), py::arg("x"), py::arg("y"), py::arg("r"), py::arg("periodic")=true );

  mi.def("S2", py::overload_cast<Image::Matrix<int   >&,Image::Matrix<int   >&,std::vector<size_t>&                                                  >(&Image::S2), "2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi")                                                                                         );
  mi.def("S2", py::overload_cast<Image::Matrix<int   >&,Image::Matrix<int   >&,std::vector<size_t>&,                                        bool,bool>(&Image::S2), "2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi"),                                     py::arg("periodic")=false, py::arg("zeropad")=true );
  mi.def("S2", py::overload_cast<Image::Matrix<int   >&,Image::Matrix<int   >&,std::vector<size_t>&,Image::Matrix<int>&,                    bool,bool>(&Image::S2), "2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi"), py::arg("fmask"),                   py::arg("periodic")=false, py::arg("zeropad")=true );
  mi.def("S2", py::overload_cast<Image::Matrix<int   >&,Image::Matrix<int   >&,std::vector<size_t>&,Image::Matrix<int>&,Image::Matrix<int>&,bool,bool>(&Image::S2), "2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi"), py::arg("fmask"), py::arg("gmask"), py::arg("periodic")=false, py::arg("zeropad")=true );
  mi.def("S2", py::overload_cast<Image::Matrix<double>&,Image::Matrix<double>&,std::vector<size_t>&                                                  >(&Image::S2), "2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi")                                                                                         );

  // mi.def("S2_mask", &Image::S2_mask, "(Masked) 2-point correlation", py::arg("f"), py::arg("g"), py::arg("roi"), py::arg("periodic")=false, py::arg("zeropad")=true, py::arg("fmask")=Image::Matrix<int>({0}) );

  return m.ptr();
}

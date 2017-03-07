
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
      bool load(py::handle src, bool) {

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

        std::vector<size_t> shape  (src.ndim);
        std::vector<size_t> strides(src.ndim);

        for ( int i=0 ; i<src.ndim ; i++ ) {
          shape  [i] = src.shape  [i];
          strides[i] = src.strides[i]*sizeof(T);
        }

        py::array a(std::move(shape), std::move(strides), src.data.data() );

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

  mi.def("S2", &Image::S2, "2-point correlation" , py::arg("f") , py::arg("g") );

  return m.ptr();
}

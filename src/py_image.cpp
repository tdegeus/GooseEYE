
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
      bool load(py::handle src, bool convert)
      {
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
      static py::handle cast(const Image::Matrix<T>& src,
        py::return_value_policy policy, py::handle parent)
      {
        py::array a(std::move(src.shape()), std::move(src.strides(true)), src.data() );

        return a.release();
      }
  };
}} // namespace pybind11::detail

// =============================================================================
// wrap as Python module
// =============================================================================

// abbreviate data types to enhance readability -> templates fit on one line
using Md = Image::Matrix<double>;
using Mi = Image::Matrix<int>;
using Vs = std::vector<size_t>;
using Vi = std::vector<int>;
using d  = double;
using i  = int;
using b  = bool;

PYBIND11_PLUGIN(gooseeye) {

py::module m("gooseeye", "Geometrical statistics");
py::module mi = m.def_submodule("image", "Image-based input");

mi.def("dummy_circles",py::overload_cast<Vs&,            bool>(&Image::dummy_circles),"Dummy image",py::arg("shape"),                                           py::arg("periodic")=true);
mi.def("dummy_circles",py::overload_cast<Vs&,Vi&,Vi&,Vi&,bool>(&Image::dummy_circles),"Dummy image",py::arg("shape"),py::arg("row"),py::arg("col"),py::arg("r"),py::arg("periodic")=true);

mi.def("kernel",&Image::kernel,py::arg("ndim"),py::arg("mode")="default");

mi.def("clusters",py::overload_cast<Mi&,    i,b>(&Image::clusters),"Identify clusters",py::arg("im"),                  py::arg("min_size")=0,py::arg("periodic")=true);
mi.def("clusters",py::overload_cast<Mi&,Mi&,i,b>(&Image::clusters),"Identify clusters",py::arg("im"),py::arg("kernel"),py::arg("min_size")=0,py::arg("periodic")=true);

mi.def("dilate",py::overload_cast<Mi&    ,i  ,b>(&Image::dilate),"Dilate image",py::arg("im"),                  py::arg("iterations")=1,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&    ,Vi&,b>(&Image::dilate),"Dilate image",py::arg("im"),                  py::arg("iterations")  ,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&,Mi&,i  ,b>(&Image::dilate),"Dilate image",py::arg("im"),py::arg("kernel"),py::arg("iterations")=1,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&,Mi&,Vi&,b>(&Image::dilate),"Dilate image",py::arg("im"),py::arg("kernel"),py::arg("iterations")  ,py::arg("periodic")=true );

mi.def("S2",py::overload_cast<Mi&,Mi&,Vs&            >(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi")                                                                                    );
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs&,        b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),                                  py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs&,Mi&,    b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),                 py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs&,Mi&,Mi&,b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),py::arg("gmask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs&            >(&Image::S2),"2-point correlation",py::arg("f"),py::arg("g"),py::arg("roi")                                                                                    );
mi.def("S2",py::overload_cast<Md&,Md&,Vs&,        b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),                                  py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs&,Mi&,    b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),                 py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs&,Mi&,Mi&,b,b>(&Image::S2),"2-point probability",py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),py::arg("gmask"),py::arg("zeropad")=false,py::arg("periodic")=true);

mi.def("W2",py::overload_cast<Mi&,Mi&,Vs&        >(&Image::W2),"Conditional 2-point probability",py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Mi&,Mi&,Vs&,    b,b>(&Image::W2),"Conditional 2-point probability",py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Mi&,Vs&,Mi&,b,b>(&Image::W2),"Conditional 2-point probability",py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Md&,Vs&        >(&Image::W2),"Conditional 2-point correlation",py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Mi&,Md&,Vs&,    b,b>(&Image::W2),"Conditional 2-point correlation",py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Md&,Vs&,Mi&,b,b>(&Image::W2),"Conditional 2-point correlation",py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Md&,Vs&        >(&Image::W2),"Weighted 2-point correlation"   ,py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Md&,Md&,Vs&,    b,b>(&Image::W2),"Weighted 2-point correlation"   ,py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Md&,Vs&,Mi&,b,b>(&Image::W2),"Weighted 2-point correlation"   ,py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);

mi.def("path",&Image::path,"Voxel path",py::arg("xa"),py::arg("xb"),py::arg("mode")="Bresenham");

mi.def("stamp_points",&Image::stamp_points,"End points of voxel stamp",py::arg("shape"));

mi.def("pad",py::overload_cast<Mi&,Vs&,i>(&Image::pad),"Pad image",py::arg("im"),py::arg("pad_shape"),py::arg("value")=0 );
mi.def("pad",py::overload_cast<Md&,Vs&,d>(&Image::pad),"Pad image",py::arg("im"),py::arg("pad_shape"),py::arg("value")=0.);

return m.ptr();

} // PYBIND11_PLUGIN

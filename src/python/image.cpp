
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "../core/image.h"
#include "../core/cppmat/py_cppmat.h"

// abbreviate data types to enhance readability -> templates fit on one line
using     Md = mat::matrix<double>;
using     Mi = mat::matrix<int>;
using     Vs = std::vector<size_t>;
using     Vi = std::vector<int>;
using     d  = double;
using     i  = int;
using     b  = bool;
using     s  = std::string;
namespace py = pybind11;

PYBIND11_PLUGIN(gooseeye) {

py::module m("gooseeye", "Geometrical statistics");
py::module mi = m.def_submodule("image", "Image-based input");

// toolbox
// -------

mi.def("path",&Image::path,"Voxel path between two points",py::arg("xa"),py::arg("xb"),py::arg("mode")="Bresenham");

mi.def("stamp_points",&Image::stamp_points,"End points of voxel stamp",py::arg("shape"));

mi.def("pad",py::overload_cast<Md&,Vs,d>(&Image::pad<double>),"Pad image",py::arg("im").noconvert(),py::arg("pad_shape"),py::arg("value")=0.);
mi.def("pad",py::overload_cast<Mi&,Vs,i>(&Image::pad<int>   ),"Pad image",py::arg("im")            ,py::arg("pad_shape"),py::arg("value")=0 );

mi.def("kernel",&Image::kernel,"Define a kernel to use in dilation",py::arg("ndim"),py::arg("mode")="default");

// image manipulation
// ------------------

mi.def("dummy_circles",py::overload_cast<               bool>(&Image::dummy_circles),"Dummy image",                                                            py::arg("periodic")=true);
mi.def("dummy_circles",py::overload_cast<Vs,            bool>(&Image::dummy_circles),"Dummy image",py::arg("shape"),                                           py::arg("periodic")=true);
mi.def("dummy_circles",py::overload_cast<Vs,Vi&,Vi&,Vi&,bool>(&Image::dummy_circles),"Dummy image",py::arg("shape"),py::arg("row"),py::arg("col"),py::arg("r"),py::arg("periodic")=true);

mi.def("clusters",py::overload_cast<Mi&,    i,b>(&Image::clusters),"Identify clusters",py::arg("im"),                  py::arg("min_size")=0,py::arg("periodic")=true);
mi.def("clusters",py::overload_cast<Mi&,Mi&,i,b>(&Image::clusters),"Identify clusters",py::arg("im"),py::arg("kernel"),py::arg("min_size")=0,py::arg("periodic")=true);

mi.def("dilate",py::overload_cast<Mi&    ,i  ,b>(&Image::dilate),"Dilate image",py::arg("im"),                  py::arg("iterations")=1,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&    ,Vi&,b>(&Image::dilate),"Dilate image",py::arg("im"),                  py::arg("iterations")  ,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&,Mi&,i  ,b>(&Image::dilate),"Dilate image",py::arg("im"),py::arg("kernel"),py::arg("iterations")=1,py::arg("periodic")=true );
mi.def("dilate",py::overload_cast<Mi&,Mi&,Vi&,b>(&Image::dilate),"Dilate image",py::arg("im"),py::arg("kernel"),py::arg("iterations")  ,py::arg("periodic")=true );

// statistics
// ----------

mi.def("mean",py::overload_cast<Mi&    >(&Image::mean<int>   ),"Spatial average"                     ,py::arg("im")                );
mi.def("mean",py::overload_cast<Mi&,Mi&>(&Image::mean<int>   ),"Spatial average, accounting for mask",py::arg("im"),py::arg("mask"));
mi.def("mean",py::overload_cast<Md&    >(&Image::mean<double>),"Spatial average"                     ,py::arg("im")                );
mi.def("mean",py::overload_cast<Md&,Mi&>(&Image::mean<double>),"Spatial average, accounting for mask",py::arg("im"),py::arg("mask"));

mi.def("S2",py::overload_cast<Mi&,Mi&,Vs            >(&Image::S2<int>   ),"2-point probability (periodic)",py::arg("f"),py::arg("g"),py::arg("roi")                                                                                    );
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs,        b,b>(&Image::S2<int>   ),"2-point probability"           ,py::arg("f"),py::arg("g"),py::arg("roi"),                                  py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs,Mi&,    b,b>(&Image::S2<int>   ),"2-point probability (masked)"  ,py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),                 py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Mi&,Mi&,Vs,Mi&,Mi&,b,b>(&Image::S2<int>   ),"2-point probability (masked)"  ,py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),py::arg("gmask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs            >(&Image::S2<double>),"2-point correlation (periodic)",py::arg("f"),py::arg("g"),py::arg("roi")                                                                                    );
mi.def("S2",py::overload_cast<Md&,Md&,Vs,        b,b>(&Image::S2<double>),"2-point correlation"           ,py::arg("f"),py::arg("g"),py::arg("roi"),                                  py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs,Mi&,    b,b>(&Image::S2<double>),"2-point correlation (masked)"  ,py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),                 py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("S2",py::overload_cast<Md&,Md&,Vs,Mi&,Mi&,b,b>(&Image::S2<double>),"2-point correlation (masked)"  ,py::arg("f"),py::arg("g"),py::arg("roi"),py::arg("fmask"),py::arg("gmask"),py::arg("zeropad")=false,py::arg("periodic")=true);

mi.def("W2",py::overload_cast<Mi&,Mi&,Vs        >(&Image::W2<int   ,int   >),"Conditional 2-point probability (periodic)",py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Mi&,Mi&,Vs,    b,b>(&Image::W2<int   ,int   >),"Conditional 2-point probability"           ,py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Mi&,Vs,Mi&,b,b>(&Image::W2<int   ,int   >),"Conditional 2-point probability (masked)"  ,py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Md&,Vs        >(&Image::W2<int   ,double>),"Conditional 2-point correlation (periodic)",py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Mi&,Md&,Vs,    b,b>(&Image::W2<int   ,double>),"Conditional 2-point correlation"           ,py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Mi&,Md&,Vs,Mi&,b,b>(&Image::W2<int   ,double>),"Conditional 2-point correlation (masked)"  ,py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Mi&,Vs        >(&Image::W2<double,int   >),"Weighted 2-point probability (periodic)"   ,py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Md&,Mi&,Vs,    b,b>(&Image::W2<double,int   >),"Weighted 2-point probability"              ,py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Mi&,Vs,Mi&,b,b>(&Image::W2<double,int   >),"Weighted 2-point probability (masked)"     ,py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Md&,Vs        >(&Image::W2<double,double>),"Weighted 2-point correlation (periodic)"   ,py::arg("W"), py::arg("I"), py::arg("roi")                                                                  );
mi.def("W2",py::overload_cast<Md&,Md&,Vs,    b,b>(&Image::W2<double,double>),"Weighted 2-point correlation"              ,py::arg("W"), py::arg("I"), py::arg("roi"),                py::arg("zeropad")=false,py::arg("periodic")=true);
mi.def("W2",py::overload_cast<Md&,Md&,Vs,Mi&,b,b>(&Image::W2<double,double>),"Weighted 2-point correlation (masked)"     ,py::arg("W"), py::arg("I"), py::arg("roi"),py::arg("mask"),py::arg("zeropad")=false,py::arg("periodic")=true);

mi.def("W2c",py::overload_cast<Mi&,Mi&,Mi&,Vs,    s,b>(&Image::W2c<int   >),"Collapsed conditional 2-point probability",py::arg("I"),py::arg("clusters"),py::arg("centers"),py::arg("roi"),                py::arg("mode")="Bresenham",py::arg("periodic")=true);
mi.def("W2c",py::overload_cast<Mi&,Mi&,Mi&,Vs,Mi&,s,b>(&Image::W2c<int   >),"Collapsed conditional 2-point probability",py::arg("I"),py::arg("clusters"),py::arg("centers"),py::arg("roi"),py::arg("mask"),py::arg("mode")="Bresenham",py::arg("periodic")=true);
mi.def("W2c",py::overload_cast<Mi&,Mi&,Md&,Vs,    s,b>(&Image::W2c<double>),"Collapsed conditional 2-point correlation",py::arg("I"),py::arg("clusters"),py::arg("centers"),py::arg("roi"),                py::arg("mode")="Bresenham",py::arg("periodic")=true);
mi.def("W2c",py::overload_cast<Mi&,Mi&,Md&,Vs,Mi&,s,b>(&Image::W2c<double>),"Collapsed conditional 2-point correlation",py::arg("I"),py::arg("clusters"),py::arg("centers"),py::arg("roi"),py::arg("mask"),py::arg("mode")="Bresenham",py::arg("periodic")=true);

mi.def("L",&Image::L,"Lineal path function",py::arg("im"),py::arg("roi"),py::arg("mode")="Bresenham",py::arg("periodic")=true);

return m.ptr();

} // PYBIND11_PLUGIN


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "../core/image.h"
#include "../core/cppmat/include/cppmat/pybind11_matrix.h"

// abbreviate data types to enhance readability
using MatD = cppmat::matrix<double>;
using MatI = cppmat::matrix<int>;
using VecS = std::vector<size_t>;
using VecI = std::vector<int>;
using str  = std::string;

// abbreviate namespace to enhance readability
namespace py = pybind11;
namespace gi = GooseEYE::Image;

PYBIND11_MODULE(GooseEYE, m) {

m.doc() = "Geometrical statistics";

py::module mi = m.def_submodule(
  "image",
  "Image-based input"
);

// toolbox
// -------

mi.def(
  "path", &gi::path,
  "Return voxel path between two points",
  py::arg( "xa"   ),
  py::arg( "xb"   ),
  py::arg( "mode" ) = "Bresenham"
);

mi.def(
  "stamp_points", &gi::stamp_points,
  "Return all end-points of a voxel stamp for a certain shape",
  py::arg( "shape" )
);

mi.def(
  "pad", py::overload_cast<MatD&,VecS,double>(&gi::pad<double>),
  "(Zero) Pad image",
  py::arg( "im"        ).noconvert(),
  py::arg( "pad_shape" ),
  py::arg( "value"     ) = 0.
);

mi.def(
  "pad", py::overload_cast<MatI&,VecS,int>(&gi::pad<int>),
  "(Zero) Pad image",
  py::arg( "im"        ),
  py::arg( "pad_shape" ),
  py::arg( "value"     ) = 0
);

mi.def(
  "kernel",&gi::kernel,
  "Define a kernel, for example to use in dilation",
  py::arg( "ndim" ),
  py::arg( "mode" ) = "default"
);

// image manipulation
// ------------------

mi.def(
  "dummy_circles", py::overload_cast<bool>(&gi::dummy_circles),
  "Generate dummy black/white image with circles, of a fixed shape",
  py::arg( "periodic" ) = true
);

mi.def(
  "dummy_circles", py::overload_cast<VecS,bool>(&gi::dummy_circles),
  "Generate dummy black/white image with circles, with a specific shape",
  py::arg( "shape"    ),
  py::arg( "periodic" ) = true
);

mi.def(
  "dummy_circles", py::overload_cast<VecS,VecI&,VecI&,VecI&,bool>(&gi::dummy_circles),
  "Generate dummy black/white image with circles, with a specific shape and with specific circles",
  py::arg( "shape"    ),
  py::arg( "row"      ),
  py::arg( "col"      ),
  py::arg( "r"        ),
  py::arg( "periodic" ) = true
);

mi.def(
  "clusters", py::overload_cast<MatI&,int,bool>(&gi::clusters),
  "Identify clusters (using the default kernel)",
  py::arg( "im"       ),
  py::arg( "min_size" ) = 0,
  py::arg( "periodic" ) = true
);

mi.def(
  "clusters", py::overload_cast<MatI&,MatI&,int,bool>(&gi::clusters),
  "Identify clusters using a specific kernel",
  py::arg( "im"       ),
  py::arg( "kernel"   ),
  py::arg( "min_size" ) = 0,
  py::arg( "periodic" ) = true
);

mi.def(
  "dilate", py::overload_cast<MatI&,int,bool>(&gi::dilate),
  "Dilate image (using the default kernel)",
  py::arg( "im"         ),
  py::arg( "iterations" ) = 1,
  py::arg( "periodic"   ) = true
);

mi.def(
  "dilate", py::overload_cast<MatI&,VecI&,bool>(&gi::dilate),
  "Dilate image (using the default kernel)",
  py::arg( "im"         ),
  py::arg( "iterations" ) ,
  py::arg( "periodic"   ) = true
);

mi.def(
  "dilate", py::overload_cast<MatI&,MatI&,int,bool>(&gi::dilate),
  "Dilate image",
  py::arg( "im"         ),
  py::arg( "kernel"     ),
  py::arg( "iterations" ) = 1,
  py::arg( "periodic"   ) = true
);

mi.def(
  "dilate", py::overload_cast<MatI&,MatI&,VecI&,bool>(&gi::dilate),
  "Dilate image",
  py::arg( "im"         ),
  py::arg( "kernel"     ),
  py::arg( "iterations" ),
  py::arg( "periodic"   ) = true
);

// statistics
// ----------

mi.def(
  "mean", py::overload_cast<MatI&>(&gi::mean<int>),
  "Spatial average",
  py::arg( "im" )
);

mi.def(
  "mean", py::overload_cast<MatI&,MatI&>(&gi::mean<int>),
  "Spatial average, accounting for mask",
  py::arg( "im"   ),
  py::arg( "mask" )
);

mi.def(
  "mean", py::overload_cast<MatD&>(&gi::mean<double>),
  "Spatial average",
  py::arg( "im" )
);

mi.def(
  "mean", py::overload_cast<MatD&,MatI&>(&gi::mean<double>),
  "Spatial average, accounting for mask",
  py::arg( "im"   ),
  py::arg( "mask" )
);

mi.def(
  "S2", py::overload_cast<MatI&,MatI&,VecS>(&gi::S2<int>),
  "2-point probability (periodic)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      )
);

mi.def(
  "S2", py::overload_cast<MatI&,MatI&,VecS,bool,bool>(&gi::S2<int>),
  "2-point probability",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "S2", py::overload_cast<MatI&,MatI&,VecS,MatI&,bool,bool>(&gi::S2<int>),
  "2-point probability (masked)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "fmask"    ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "S2", py::overload_cast<MatI&,MatI&,VecS,MatI&,MatI&,bool,bool>(&gi::S2<int>),
  "2-point probability (masked)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "fmask"    ),
  py::arg( "gmask"    ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "S2", py::overload_cast<MatD&,MatD&,VecS>(&gi::S2<double>),
  "2-point correlation (periodic)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      )
);

mi.def(
  "S2", py::overload_cast<MatD&,MatD&,VecS,bool,bool>(&gi::S2<double>),
  "2-point correlation",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "S2", py::overload_cast<MatD&,MatD&,VecS,MatI&,bool,bool>(&gi::S2<double>),
  "2-point correlation (masked)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "fmask"    ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "S2", py::overload_cast<MatD&,MatD&,VecS,MatI&,MatI&,bool,bool>(&gi::S2<double>),
  "2-point correlation (masked)",
  py::arg( "f"        ),
  py::arg( "g"        ),
  py::arg( "roi"      ),
  py::arg( "fmask"    ),
  py::arg( "gmask"    ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatI&,MatI&,VecS>(&gi::W2<int,int>),
  "Conditional 2-point probability (periodic)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      )
);

mi.def(
  "W2", py::overload_cast<MatI&,MatI&,VecS,bool,bool>(&gi::W2<int,int>),
  "Conditional 2-point probability",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatI&,MatI&,VecS,MatI&,bool,bool>(&gi::W2<int,int>),
  "Conditional 2-point probability (masked)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatI&,MatD&,VecS>(&gi::W2<int,double>),
  "Conditional 2-point correlation (periodic)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      )
);

mi.def(
  "W2", py::overload_cast<MatI&,MatD&,VecS,bool,bool>(&gi::W2<int,double>),
  "Conditional 2-point correlation",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatI&,MatD&,VecS,MatI&,bool,bool>(&gi::W2<int,double>),
  "Conditional 2-point correlation (masked)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatD&,MatI&,VecS>(&gi::W2<double,int>),
  "Weighted 2-point probability (periodic)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      )
);

mi.def(
  "W2", py::overload_cast<MatD&,MatI&,VecS,bool,bool>(&gi::W2<double,int>),
  "Weighted 2-point probability",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatD&,MatI&,VecS,MatI&,bool,bool>(&gi::W2<double,int>),
  "Weighted 2-point probability (masked)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatD&,MatD&,VecS>(&gi::W2<double,double>),
  "Weighted 2-point correlation (periodic)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      )
);

mi.def(
  "W2", py::overload_cast<MatD&,MatD&,VecS,bool,bool>(&gi::W2<double,double>),
  "Weighted 2-point correlation",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2", py::overload_cast<MatD&,MatD&,VecS,MatI&,bool,bool>(&gi::W2<double,double>),
  "Weighted 2-point correlation (masked)",
  py::arg( "W"        ),
  py::arg( "I"        ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "zeropad"  ) = false,
  py::arg( "periodic" ) = true
);

mi.def(
  "W2c", py::overload_cast<MatI&,MatI&,MatI&,VecS,str,bool>(&gi::W2c<int>),
  "Collapsed conditional 2-point probability",
  py::arg( "I"        ),
  py::arg( "clusters" ),
  py::arg( "centers"  ),
  py::arg( "roi"      ),
  py::arg( "mode"     ) = "Bresenham",
  py::arg( "periodic" ) = true
);

mi.def(
  "W2c", py::overload_cast<MatI&,MatI&,MatI&,VecS,MatI&,str,bool>(&gi::W2c<int>),
  "Collapsed conditional 2-point probability",
  py::arg( "I"        ),
  py::arg( "clusters" ),
  py::arg( "centers"  ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "mode"     ) = "Bresenham",
  py::arg( "periodic" ) = true
);

mi.def(
  "W2c", py::overload_cast<MatI&,MatI&,MatD&,VecS,str,bool>(&gi::W2c<double>),
  "Collapsed conditional 2-point correlation",
  py::arg( "I"        ),
  py::arg( "clusters" ),
  py::arg( "centers"  ),
  py::arg( "roi"      ),
  py::arg( "mode"     ) = "Bresenham",
  py::arg( "periodic" ) = true
);

mi.def(
  "W2c", py::overload_cast<MatI&,MatI&,MatD&,VecS,MatI&,str,bool>(&gi::W2c<double>),
  "Collapsed conditional 2-point correlation",
  py::arg( "I"        ),
  py::arg( "clusters" ),
  py::arg( "centers"  ),
  py::arg( "roi"      ),
  py::arg( "mask"     ),
  py::arg( "mode"     ) = "Bresenham",
  py::arg( "periodic" ) = true
);

mi.def(
  "L" , &gi::L,
  "Lineal path function",
  py::arg( "im"       ),
  py::arg( "roi"      ),
  py::arg( "mode"     ) = "Bresenham",
  py::arg( "periodic" ) = true
);

} // PYBIND11_MODULE

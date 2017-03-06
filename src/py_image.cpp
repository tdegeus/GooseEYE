
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include "image.h"

namespace py {

// =============================================================================
// convert (copy) array -> image::Matrix
// =============================================================================

template <typename T>
void array2matrix ( pybind11::array_t<T, pybind11::array::c_style | pybind11::array::forcecast> array , bool periodic , image::Matrix<T>& mat )
{

  int i;
  int nd = array.ndim();

  if ( nd<1 || nd>3 )
    throw std::runtime_error("Input should be 1-D, 2-D, or 3-D");

  for ( i=0  ; i<nd ; i++ ) { mat.shape.push_back(array.shape()[i]); }
  for ( i=nd ; i<3  ; i++ ) { mat.shape.push_back(1);                }

  mat.size     = mat.shape[0]*mat.shape[1]*mat.shape[2];
  mat.nd       = nd;
  mat.periodic = periodic;

  for ( i=0 ; i<mat.size ; i++ )
    mat.data.push_back(array.data()[i]);

}


void S2_new(
  const pybind11::array_t<bool, pybind11::array::c_style | pybind11::array::forcecast>& f ,
  const pybind11::array_t<bool, pybind11::array::c_style | pybind11::array::forcecast>& g ,
  const std::vector<int>& roi , bool periodic
) {


  std::cout << "S2_new (bool)" << std::endl;
  std::cout << roi[0] << std::endl;

}


void S2_new(
  const pybind11::array_t<double, pybind11::array::c_style | pybind11::array::forcecast>& f ,
  const pybind11::array_t<double, pybind11::array::c_style | pybind11::array::forcecast>& g ,
  const std::vector<int>& roi , bool periodic
) {


  std::cout << "S2_new (double)" << std::endl;
  std::cout << roi[0] << std::endl;

}



pybind11::array S2(pybind11::array_t<double, pybind11::array::c_style | pybind11::array::forcecast> array , bool periodic ) {

  image::Matrix<double> f;

  array2matrix<double>(array,periodic,f);

  std::cout << f.size << std::endl;
  std::cout << f.shape[0] << std::endl;
  std::cout << f.shape[1] << std::endl;
  std::cout << f.shape[2] << std::endl;

  for ( int i=0 ; i<f.size ; i++ )
    std::cout << f.data[i] << std::endl;

  // check input dimensions
  if ( array.ndim()     != 2 )
    throw std::runtime_error("Input should be 2-D NumPy array");
  if ( array.shape()[1] != 2 )
    throw std::runtime_error("Input should have size [N,2]");

  // allocate std::vector (to pass to the C++ function)
  std::vector<double> pos(array.size());

  // copy pybind11::array -> std::vector
  std::memcpy(pos.data(),array.data(),array.size()*sizeof(double));

  // call pure C++ function
  std::vector<double> result = image::S2(pos);

  // return 2-D NumPy array
  return pybind11::array(pybind11::buffer_info(
    result.data(),                           /* data as contiguous array  */
    sizeof(double),                          /* size of one scalar        */
    pybind11::format_descriptor<double>::format(), /* data type                 */
    2,                                       /* number of dimensions      */
    { array.shape()[0] , 3 },                /* shape of the matrix       */
    { sizeof(double)*3 , sizeof(double) }    /* strides for each axis     */
  ));

}

}

// wrap as Python module
PYBIND11_PLUGIN(gooseeye) {
  pybind11::module m("gooseeye", "Geometrical statistics");
  pybind11::module mi = m.def_submodule("image", "Image-based input");

  mi.def("S2", &py::S2, "2-point correlation" , pybind11::arg() , pybind11::arg("periodic")=false );
  mi.def("S2_new", pybind11::overload_cast<const pybind11::array_t<bool  , pybind11::array::c_style | pybind11::array::forcecast>&,const pybind11::array_t<bool  , pybind11::array::c_style | pybind11::array::forcecast>&,const std::vector<int>&,bool>(&py::S2_new), "2-point correlation" , pybind11::arg("f") , pybind11::arg("g") , pybind11::arg("roi") , pybind11::arg("periodic")=false );
  mi.def("S2_new", pybind11::overload_cast<const pybind11::array_t<double, pybind11::array::c_style | pybind11::array::forcecast>&,const pybind11::array_t<double, pybind11::array::c_style | pybind11::array::forcecast>&,const std::vector<int>&,bool>(&py::S2_new), "2-point correlation" , pybind11::arg("f") , pybind11::arg("g") , pybind11::arg("roi") , pybind11::arg("periodic")=false );

  return m.ptr();
}

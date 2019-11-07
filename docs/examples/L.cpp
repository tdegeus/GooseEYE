#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
  // generate image, store 'volume-fraction'
  xt::xarray<int> I = GooseEYE::dummy_circles({500, 500});
  double phi = xt::mean(I)();

  // lineal path function
  xt::xarray<double> L = GooseEYE::L({101, 101}, I);

  return 0;
}

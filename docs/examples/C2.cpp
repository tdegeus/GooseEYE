#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
  // generate image, store 'volume-fraction'
  xt::xarray<int> I = GooseEYE::dummy_circles({500, 500});
  double phi = xt::mean(I)();

  // 2-point probability
  xt::xarray<double> S2 = GooseEYE::S2({101, 101}, I, I);

  // determine clusters, based on the binary image
  xt::xarray<int> C = GooseEYE::clusters(I);

  // 2-point cluster function
  xt::xarray<double> C2 = GooseEYE::C2({101, 101}, C, C);

  return 0;
}

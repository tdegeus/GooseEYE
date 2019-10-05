#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
  GooseEYE::Ensemble ensemble({101, 101});

  for (size_t i = 0; i < 5; ++i)
  {
    xt::xarray<int> I = GooseEYE::dummy_circles({200, 200});
    ensemble.S2(I, I);
  }

  xt::xarray<double> S2 = ensemble.result();

  return 0;
}

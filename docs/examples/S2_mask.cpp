#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
  // binary image + correlation
  // --------------------------

  // generate image, store 'volume-fraction'
  xt::xarray<int> I = GooseEYE::dummy_circles({500, 500});
  double phi = xt::mean(I)();

  // 2-point probability
  xt::xarray<double> S2 = GooseEYE::S2({101, 101}, I, I);

  // artefact + (masked) correlation
  // -------------------------------

  // define image with artefact and the corresponding mask
  xt::xarray<int> mask = xt::zeros<int>(I.shape());
  xt::xarray<int> Ierr = I;
  xt::view(mask, xt::range(0, 150), xt::range(0, 150)) = 1;
  xt::view(Ierr, xt::range(0, 150), xt::range(0, 150)) = 1;

  // 2-point correlation on image with artefact (no mask)
  xt::xarray<double> S2err = GooseEYE::S2({101, 101}, Ierr, Ierr);

  // 2-point correlation on image with artefact, with artefact masked
  xt::xarray<double> S2mask = GooseEYE::S2({101, 101}, Ierr, Ierr, mask, mask);

  return 0;
}

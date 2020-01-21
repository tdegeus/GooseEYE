#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image, store 'volume-fraction'
    xt::xarray<int> I = GooseEYE::dummy_circles({50, 50});
    double phi = xt::mean(I)(0);

    // 2-point probability
    xt::xarray<double> S2 = GooseEYE::S2({11, 11}, I, I);

    // define image with artefact and the corresponding mask
    xt::xarray<int> mask = xt::zeros<int>(I.shape());
    xt::xarray<int> Ierr = I;
    xt::view(mask, xt::range(0, 15), xt::range(0, 15)) = 1;
    xt::view(Ierr, xt::range(0, 15), xt::range(0, 15)) = 1;

    // 2-point correlation on image with artefact (no mask)
    xt::xarray<double> S2err = GooseEYE::S2({11, 11}, Ierr, Ierr);

    // 2-point correlation on image with artefact, with artefact masked
    xt::xarray<double> S2mask = GooseEYE::S2({11, 11}, Ierr, Ierr, mask, mask);

    return 0;
}

#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // binary image + correlation
    // --------------------------

    // generate image, store 'volume-fraction'
    xt::xarray<int> I = GooseEYE::dummy_circles({50, 50});
    double phi = xt::mean(I)();

    // 2-point probability
    xt::xarray<double> S2 = GooseEYE::S2({11, 11}, I, I);

    // grey image + correlation
    // ------------------------

    // noise
    xt::xarray<double> noise = 0.1 * (2.0 * xt::random::rand<double>(I.shape()) - 1.0);

    // convert to grey-scale image and introduce noise
    xt::xarray<double> Igr = I;
    Igr = (Igr + 0.1) / 1.2 + noise;

    // 2-point correlation ('auto-correlation')
    xt::xarray<double>S2gr = GooseEYE::S2({11, 11}, Igr, Igr);

    // correlation bounds: mean intensity squared and mean of the intensity squared
    double Iav_sq = std::pow(xt::mean(Igr)(), 2.0);
    double Isq_av = xt::mean(xt::pow(Igr, 2.0))();

    return 0;
}

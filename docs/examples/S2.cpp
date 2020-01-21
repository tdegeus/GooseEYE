#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image, store 'volume-fraction'
    xt::xarray<int> I = GooseEYE::dummy_circles({50, 50});
    double phi = xt::mean(I)();

    // 2-point probability
    xt::xarray<double> S2 = GooseEYE::S2({11, 11}, I, I);

    return 0;
}

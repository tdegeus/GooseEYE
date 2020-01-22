#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image, store 'volume-fraction'
    xt::xarray<int> I = GooseEYE::dummy_circles({50, 50});
    double phi = xt::mean(I)();

    // lineal path function
    xt::xarray<double> L = GooseEYE::L({11, 11}, I);

    return 0;
}

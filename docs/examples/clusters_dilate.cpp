#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image
    xt::xarray<int> I = xt::zeros<int>({21, 21});
    I(4, 4) = 1;
    I(14, 15) = 1;
    I(15, 15) = 1;
    I(16, 15) = 1;
    I(15, 14) = 1;
    I(15, 16) = 1;

    // clusters
    auto C = GooseEYE::Clusters(I).labels();

    // dilate
    auto CD = GooseEYE::dilate(C);

    return 0;
}

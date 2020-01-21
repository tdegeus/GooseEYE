#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({100, 100}, true);

    // clusters
    auto clusters = GooseEYE::clusters(I, false);

    // clusters, if the image is periodic
    auto clusters_periodic = GooseEYE::clusters(I, true);

    return 0;
}

#include <GooseEYE/GooseEYE.h>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({100, 100}, true);

    // clusters
    auto labels = GooseEYE::clusters(I, false);

    // clusters, if the image is periodic
    auto labels_periodic = GooseEYE::clusters(I, true);

    return 0;
}

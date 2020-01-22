#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({100, 100}, true);

    // clusters
    GooseEYE::Clusters clusters(I, false);
    auto labels = clusters.labels();
    auto centers = clusters.center_positions();

    // clusters, if the image is periodic
    GooseEYE::Clusters clusters_periodic(I, true);
    auto labels_periodic = clusters_periodic.labels();
    auto centers_periodic = clusters_periodic.center_positions();

    return 0;
}

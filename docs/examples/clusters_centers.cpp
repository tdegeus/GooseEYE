#include <GooseEYE/GooseEYE.h>
#include <xtensor.hpp>
#include <highfive/H5Easy.hpp>

#define MYASSERT(expr) MYASSERT_IMPL(expr, __FILE__, __LINE__)
#define MYASSERT_IMPL(expr, file, line) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ':' + std::to_string(line) + \
            ": assertion failed (" #expr ") \n\t"); \
    }

int main()
{
    // generate image
    auto I = GooseEYE::dummy_circles({500, 500}, true);

    // clusters
    GooseEYE::Clusters clusters(I, false);
    auto labels = clusters.labels();
    auto centers = clusters.center_positions();

    // clusters, if the image is periodic
    GooseEYE::Clusters clusters_periodic(I, true);
    auto labels_periodic = clusters_periodic.labels();
    auto centers_periodic = clusters_periodic.center_positions();

    H5Easy::File data("clusters_centers.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(labels, H5Easy::load<decltype(labels)>(data, "labels"))));
    MYASSERT(xt::allclose(centers, H5Easy::load<decltype(centers)>(data, "centers")));
    MYASSERT(xt::all(xt::equal(labels_periodic, H5Easy::load<decltype(labels_periodic)>(data, "labels_periodic"))));
    MYASSERT(xt::allclose(centers_periodic, H5Easy::load<decltype(centers_periodic)>(data, "centers_periodic")));

    return 0;
}

#include <GooseEYE/GooseEYE.h>
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
    auto labels = GooseEYE::clusters(I, false);
    auto names = xt::unique(labels);
    auto centers = GooseEYE::labels_centers(labels, names, false);

    // clusters, if the image is periodic
    auto labels_periodic = GooseEYE::clusters(I, true);
    auto names_periodic = xt::unique(labels_periodic);
    auto centers_periodic = GooseEYE::labels_centers(labels_periodic, names_periodic, true);

    // check against previous versions
    H5Easy::File data("clusters_centers.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(labels, H5Easy::load<decltype(labels)>(data, "labels"))));
    MYASSERT(xt::allclose(centers, H5Easy::load<decltype(centers)>(data, "centers")));
    MYASSERT(xt::all(xt::equal(
        labels_periodic, H5Easy::load<decltype(labels_periodic)>(data, "labels_periodic"))));
    MYASSERT(xt::allclose(
        centers_periodic, H5Easy::load<decltype(centers_periodic)>(data, "centers_periodic")));

    return 0;
}

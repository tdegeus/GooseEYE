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

    // check against previous versions
    // note that the stored data based unix,
    // the random data is expected to be different on Windows
#ifndef _WIN32
    H5Easy::File data("clusters_dilate.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(C, H5Easy::load<decltype(C)>(data, "C"))));
    MYASSERT(xt::all(xt::equal(CD, H5Easy::load<decltype(CD)>(data, "CD"))));
#endif

    return 0;
}

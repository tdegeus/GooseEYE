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
    double L = 2.0 * M_PI;
    size_t N = 1000;
    double h = L / (double)N;
    xt::xarray<double> x = xt::linspace<double>(0, L, N);
    xt::xarray<double> y1 = xt::sin(x);
    xt::xarray<double> y2 = xt::sin(2. * x);

    xt::xarray<double> hh1 = GooseEYE::heightheight({200,}, y1, true);
    xt::xarray<double> hh2 = GooseEYE::heightheight({200,}, y2, true);
    xt::xarray<double> dx = GooseEYE::distance({200}, {h,}, 0);

    // check against previous versions
    // note that the stored data based unix,
    // the random data is expected to be different on Windows
#ifndef _WIN32
    H5Easy::File data("heightheight.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::allclose(y1, H5Easy::load<decltype(y1)>(data, "y1")));
    MYASSERT(xt::allclose(y2, H5Easy::load<decltype(y2)>(data, "y2")));
    MYASSERT(xt::allclose(hh1, H5Easy::load<decltype(hh1)>(data, "hh1")));
    MYASSERT(xt::allclose(hh2, H5Easy::load<decltype(hh2)>(data, "hh2")));
    MYASSERT(xt::allclose(dx, H5Easy::load<decltype(dx)>(data, "dx")));
#endif

    return 0;
}

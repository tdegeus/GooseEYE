#include <GooseEYE/GooseEYE.h>
#include <highfive/H5Easy.hpp>
#include <prrng.h>

#define MYASSERT(expr) MYASSERT_IMPL(expr, __FILE__, __LINE__)
#define MYASSERT_IMPL(expr, file, line) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ':' + std::to_string(line) + \
            ": assertion failed (" #expr ") \n\t"); \
    }

int main()
{
    prrng::pcg32 rng(0);
    auto random_data = rng.random({40, 40, 40});

    GooseEYE::Ensemble ensemble({1}, true, true);

    for (size_t i = 0; i < random_data.shape(0); ++i) {
        xt::xarray<double> slice = xt::view(random_data, i, xt::all(), xt::all());
        ensemble.mean(slice);
    }

    auto mean = ensemble.result();
    auto variance = ensemble.variance();

    // check against previous versions
    H5Easy::File data("mean.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::allclose(mean, H5Easy::load<decltype(mean)>(data, "mean")));
    MYASSERT(xt::allclose(variance, H5Easy::load<decltype(variance)>(data, "variance")));

    return 0;
}

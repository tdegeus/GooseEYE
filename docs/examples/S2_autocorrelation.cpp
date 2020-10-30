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
    // binary image + correlation
    // --------------------------

    // generate image, store 'volume-fraction'
    auto I = GooseEYE::dummy_circles({500, 500});

    // 2-point probability
    auto S2 = GooseEYE::S2({101, 101}, I, I);

    // grey image + correlation
    // ------------------------

    // noise
    auto noise = 0.1 * (2.0 * GooseEYE::random::random(I.shape()) - 1.0);

    // convert to grey-scale image and introduce noise
    xt::xarray<double> Igr = I;
    Igr = (Igr + 0.1) / 1.2 + noise;

    // 2-point correlation ('auto-correlation')
    auto S2gr = GooseEYE::S2({101, 101}, Igr, Igr);

    // check against previous versions
    // note that the stored data based unix,
    // the random data is expected to be different on Windows
#ifndef _WIN32
    H5Easy::File data("S2_autocorrelation.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::allclose(S2, H5Easy::load<decltype(S2)>(data, "S2")));
    MYASSERT(xt::allclose(Igr, H5Easy::load<decltype(Igr)>(data, "Igr")));
    MYASSERT(xt::allclose(S2gr, H5Easy::load<decltype(S2gr)>(data, "S2gr")));
#endif

    return 0;
}

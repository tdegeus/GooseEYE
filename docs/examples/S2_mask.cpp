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
    xt::xarray<int> I = GooseEYE::dummy_circles({500, 500});

    // 2-point probability
    xt::xarray<double> S2 = GooseEYE::S2({101, 101}, I, I);

    // define image with artefact and the corresponding mask
    xt::xarray<int> mask = xt::zeros<int>(I.shape());
    xt::xarray<int> Ierr = I;
    xt::view(mask, xt::range(0, 150), xt::range(0, 150)) = 1;
    xt::view(Ierr, xt::range(0, 150), xt::range(0, 150)) = 1;

    // 2-point correlation on image with artefact (no mask)
    xt::xarray<double> S2err = GooseEYE::S2({101, 101}, Ierr, Ierr);

    // 2-point correlation on image with artefact, with artefact masked
    xt::xarray<double> S2mask = GooseEYE::S2({101, 101}, Ierr, Ierr, mask, mask);

    // check against previous versions
    H5Easy::File data("S2_mask.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(Ierr, H5Easy::load<decltype(Ierr)>(data, "Ierr"))));
    MYASSERT(xt::all(xt::equal(mask, H5Easy::load<decltype(mask)>(data, "mask"))));
    MYASSERT(xt::allclose(S2, H5Easy::load<decltype(S2)>(data, "S2")));
    MYASSERT(xt::allclose(S2err, H5Easy::load<decltype(S2err)>(data, "S2err")));
    MYASSERT(xt::allclose(S2mask, H5Easy::load<decltype(S2mask)>(data, "S2mask")));

    return 0;
}

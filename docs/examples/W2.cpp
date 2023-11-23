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
    // image + "damage" + correlation
    // ------------------------------

    // square grid of circles
    size_t N = 15;
    size_t M = 500;
    auto row = xt::linspace<double>(0, M, N);
    auto col = xt::linspace<double>(0, M, N);
    xt::xarray<double> rowmat;
    xt::xarray<double> colmat;
    std::tie(rowmat, colmat) = xt::meshgrid(row, col);
    rowmat = xt::ravel(rowmat);
    colmat = xt::ravel(colmat);
    xt::xtensor<double, 1> r = (double)(M) / (double)(N) / 4.0 * xt::ones<double>({N * N});

    // random perturbation
    prrng::pcg32 rng(0);
    rowmat += rng.normal({N * N}, 0.0, (double)(M) / (double)(N));
    colmat += rng.normal({N * N}, 0.0, (double)(M) / (double)(N));
    auto dr = rng.random({N * N}) * 2.0 + 0.1;
    r = r * dr;

    // generate image
    auto I = GooseEYE::dummy_circles({M, M}, xt::round(rowmat), xt::round(colmat), xt::round(r));

    // create 'damage' -> right of inclusion
    colmat += 1.1 * r;
    r *= 0.4;
    auto W = GooseEYE::dummy_circles({M, M}, xt::round(rowmat), xt::round(colmat), xt::round(r));
    W = xt::where(xt::equal(I, 1), 0, W);

    // weighted correlation
    auto WI = GooseEYE::W2({101, 101}, W, I, W);

    // gray-scale image + correlation
    // ------------------------------

    // convert to gray-scale image and introduce noise
    xt::xarray<double> Igr = I;
    Igr += 0.1 * (2.0 * rng.random(Igr.shape()) - 1.0) + 0.1;
    Igr /= 1.2;

    // weighted correlation
    auto WIgr = GooseEYE::W2({101, 101}, xt::xarray<double>(W), Igr, W);

    // check against previous versions
    H5Easy::File data("W2.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(W, H5Easy::load<decltype(W)>(data, "W"))));
    MYASSERT(xt::allclose(Igr, H5Easy::load<decltype(Igr)>(data, "Igr")));
    MYASSERT(xt::allclose(WI, H5Easy::load<decltype(WI)>(data, "WI")));
    MYASSERT(xt::allclose(WIgr, H5Easy::load<decltype(WIgr)>(data, "WIgr")));

    return 0;
}

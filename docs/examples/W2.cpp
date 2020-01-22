#include <xtensor/xarray.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

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
    xt::xtensor<double,1> r = (double)(M) / (double)(N) / 4.0 * xt::ones<double>({N * N});

    // random perturbation
    rowmat += xt::random::randn<double>({N * N}, 0.0, (double)(M) / (double)(N));
    colmat += xt::random::randn<double>({N * N}, 0.0, (double)(M) / (double)(N));
    xt::xtensor<double,1> dr = xt::random::rand<double>({N * N}) * 2.0 + 0.1;;
    r = r * dr;

    // generate image, store 'volume-fraction'
    auto I = GooseEYE::dummy_circles({M, M}, rowmat, colmat, r);
    double phi = xt::mean(I)(0);

    // create 'damage' -> right of inclusion
    colmat += 1.1 * r;
    r *= 0.4;
    auto W = GooseEYE::dummy_circles({M, M}, rowmat, colmat, r);
    W = xt::where(xt::equal(I, 1), 0, W);

    // weighted correlation
    auto WI = GooseEYE::W2({101, 101}, W, I, W);

    // gray-scale image + correlation
    // ------------------------------

    // convert to gray-scale image and introduce noise
    xt::xarray<double> Igr = I;
    Igr += 0.1 * (2.0 * xt::random::rand<double>(Igr.shape()) - 1.0) + 0.1;
    Igr /= 1.2;

    // mean intensity (for bounds)
    double Iav = xt::mean(Igr)(0);

    // weighted correlation
    xt::xarray<double> Wd = W;
    auto WIgr = GooseEYE::W2({101, 101}, Wd, Igr, W);

    return 0;
}


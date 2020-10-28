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
    rowmat += GooseEYE::random::normal({N * N}, 0.0, (double)(M) / (double)(N));
    colmat += GooseEYE::random::normal({N * N}, 0.0, (double)(M) / (double)(N));
    xt::xtensor<double,1> dr = GooseEYE::random::random({N * N}) * 2.0 + 0.1;;
    r = r * dr;

    // generate image, store 'volume-fraction'
    auto I = GooseEYE::dummy_circles({M, M}, rowmat, colmat, r);

    // create 'damage' -> right of inclusion
    colmat += 1.1 * r;
    r *= 0.4;
    auto W = GooseEYE::dummy_circles({M, M}, rowmat, colmat, r);
    W = xt::where(xt::equal(I, 1), 0, W);

    // compute individual damage clusters and their centers
    GooseEYE::Clusters Clusters(W);
    auto clusters = Clusters.labels();
    auto centers = Clusters.centers();

    // weighted correlation
    auto W_I = GooseEYE::W2({101, 101}, W, I, W);

    // collapsed weighted correlation
    auto W_I_c = GooseEYE::W2c({101, 101}, clusters, centers, I, W);

    // check against previous versions
    H5Easy::File data("W2c.h5", H5Easy::File::ReadOnly);
    MYASSERT(xt::all(xt::equal(I, H5Easy::load<decltype(I)>(data, "I"))));
    MYASSERT(xt::all(xt::equal(clusters, H5Easy::load<decltype(clusters)>(data, "clusters"))));
    MYASSERT(xt::all(xt::equal(centers, H5Easy::load<decltype(centers)>(data, "centers"))));
    MYASSERT(xt::all(xt::equal(W, H5Easy::load<decltype(W)>(data, "W"))));
    MYASSERT(xt::allclose(W_I, H5Easy::load<decltype(W_I)>(data, "W_I")));
    MYASSERT(xt::allclose(W_I_c, H5Easy::load<decltype(W_I_c)>(data, "W_I_c")));

    return 0;
}


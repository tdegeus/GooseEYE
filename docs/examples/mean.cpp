#include <xtensor/xarray.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    xt::xarray<double> data = xt::random::rand<double>({40, 40, 40});

    GooseEYE::Ensemble ensemble({1}, true, true);

    for (size_t i = 0; i < data.shape(0); ++i)
    {
        xt::xarray<double> slice = xt::view(data, i, xt::all(), xt::all());
        ensemble.mean(slice);
    }


    std::cout << ensemble.result() << " " << xt::mean(data)(0) << std::endl;
    std::cout << ensemble.variance() << " " << xt::variance(data)(0) << std::endl;

    return 0;
}


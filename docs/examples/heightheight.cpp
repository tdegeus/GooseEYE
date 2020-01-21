#include <math.h>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <GooseEYE/GooseEYE.h>

int main()
{
    double L = M_PI;
    size_t N = 100;
    double h = L / (double)N;
    xt::xarray<double> x = xt::linspace<double>(0, L, N);
    xt::xarray<double> y1 = xt::sin(x);
    xt::xarray<double> y2 = xt::sin(2. * x);

    xt::xarray<double> hh1 = GooseEYE::heightheight({20,}, y1, true);
    xt::xarray<double> hh2 = GooseEYE::heightheight({20,}, y2, true);
    xt::xarray<double> dx = GooseEYE::distance({20}, {h,}, 0);

    return 0;
}

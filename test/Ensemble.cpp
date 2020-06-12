
#include <GooseEYE/GooseEYE.h>
#include <catch2/catch.hpp>

TEST_CASE("GooseEYE::Ensemble", "Ensemble.hpp")
{

SECTION("S2")
{
    xt::xarray<int> I = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    xt::xarray<double> R = {
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {1, 2, 3, 2, 1},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}
    };
    R /= static_cast<double>(I.size());

    xt::xarray<double> res = GooseEYE::S2({5, 5}, I, I, true);

    REQUIRE(xt::allclose(R, res));
}

SECTION("L - (a)")
{
    xt::xarray<int> I = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    xt::xarray<double> R = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 2, 3, 2, 1, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    R /= static_cast<double>(I.size());

    xt::xarray<double> res = GooseEYE::L({7, 7}, I, true);

    REQUIRE(xt::allclose(R, res));
}

SECTION("L - (b)")
{
    xt::xarray<int> I = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    xt::xarray<double> R = {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 2, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };
    R /= static_cast<double>(I.size());

    xt::xarray<double> res = GooseEYE::L({7, 7}, I, true);

    REQUIRE(xt::allclose(R, res));
}

}

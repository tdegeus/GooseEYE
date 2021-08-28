#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <GooseEYE/GooseEYE.h>

TEST_CASE("GooseEYE::Ensemble", "Ensemble.hpp")
{

SECTION("distance - (a)")
{
    xt::xarray<double> d = {
        {std::sqrt(8.0), std::sqrt(5.0), 2.0, std::sqrt(5.0), std::sqrt(8.0)},
        {std::sqrt(5.0), std::sqrt(2.0), 1.0, std::sqrt(2.0), std::sqrt(5.0)},
        {2.0           , 1.0           , 0.0, 1.0           , 2.0           },
        {std::sqrt(5.0), std::sqrt(2.0), 1.0, std::sqrt(2.0), std::sqrt(5.0)},
        {std::sqrt(8.0), std::sqrt(5.0), 2.0, std::sqrt(5.0), std::sqrt(8.0)},
    };

    xt::xarray<double> res = GooseEYE::Ensemble({5, 5}).distance();

    REQUIRE(xt::allclose(d, res));
}

SECTION("distance - (b)")
{
    xt::xarray<double> d0 = {
        {-2.0, -2.0, -2.0, -2.0, -2.0},
        {-1.0, -1.0, -1.0, -1.0, -1.0},
        { 0.0,  0.0,  0.0,  0.0,  0.0},
        { 1.0,  1.0,  1.0,  1.0,  1.0},
        { 2.0,  2.0,  2.0,  2.0,  2.0},
    };

    xt::xarray<double> d1 = {
        {-2.0, -1.0, 0.0, 1.0, 2.0},
        {-2.0, -1.0, 0.0, 1.0, 2.0},
        {-2.0, -1.0, 0.0, 1.0, 2.0},
        {-2.0, -1.0, 0.0, 1.0, 2.0},
        {-2.0, -1.0, 0.0, 1.0, 2.0},
    };

    xt::xarray<double> res0 = GooseEYE::Ensemble({5, 5}).distance(0);
    xt::xarray<double> res1 = GooseEYE::Ensemble({5, 5}).distance(1);

    REQUIRE(xt::allclose(d0, res0));
    REQUIRE(xt::allclose(d1, res1));
}

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

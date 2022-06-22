#define CATCH_CONFIG_MAIN
#include <GooseEYE/GooseEYE.h>
#include <catch2/catch_all.hpp>

TEST_CASE("GooseEYE::dilate", "dilate.hpp")
{

    SECTION("dilate - basic")
    {
        xt::xtensor<size_t, 2> f = {
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
        };

        xt::xtensor<size_t, 2> g = {
            {0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {1, 1, 1, 1, 1},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
        };

        REQUIRE(xt::allclose(GooseEYE::dilate(f), g));
    }

    SECTION("dilate - 1d")
    {
        xt::xtensor<int, 1> I = xt::zeros<int>({5});
        xt::xtensor<int, 1> D = xt::zeros<int>({5});

        I(0) = 2;

        D(0) = 2;
        D(1) = 2;
        D(4) = 2;

        auto d = GooseEYE::dilate(I);

        REQUIRE(xt::all(xt::equal(D, d)));
    }

    SECTION("dilate - 2d - 1")
    {
        xt::xarray<int> I = xt::zeros<int>({5, 5});
        xt::xarray<int> D = xt::zeros<int>({5, 5});

        I(0, 0) = -1;

        D(0, 0) = -1;
        D(1, 0) = -1;
        D(4, 0) = -1;
        D(0, 1) = -1;
        D(0, 4) = -1;

        auto d = GooseEYE::dilate(I);

        REQUIRE(xt::all(xt::equal(D, d)));
    }

    SECTION("dilate - 2d - 2")
    {
        xt::xarray<int> I = xt::zeros<int>({6, 6});
        xt::xarray<int> D = xt::zeros<int>({6, 6});

        I(0, 0) = 2;

        D(0, 0) = 2;
        D(1, 0) = 2;
        D(2, 0) = 2;
        D(1, 1) = 2;
        D(1, 5) = 2;
        D(5, 0) = 2;
        D(4, 0) = 2;
        D(0, 1) = 2;
        D(5, 1) = 2;
        D(0, 2) = 2;
        D(0, 4) = 2;
        D(0, 5) = 2;
        D(5, 5) = 2;

        auto d = GooseEYE::dilate(I, 2);

        REQUIRE(xt::all(xt::equal(D, d)));
    }
}

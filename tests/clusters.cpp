#define CATCH_CONFIG_MAIN
#include <GooseEYE/GooseEYE.h>
#include <catch2/catch_all.hpp>

TEST_CASE("GooseEYE::clusters", "clusters.hpp")
{

    SECTION("center_of_mass - not periodic")
    {
        xt::xtensor<size_t, 2> l = {
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
        };

        xt::xtensor<double, 2> centers = {{2.0, 2.0}, {2.0, 2.0}};

        REQUIRE(xt::allclose(GooseEYE::center_of_mass(l, false), centers));
    }

    SECTION("center_of_mass - periodic")
    {
        xt::xtensor<size_t, 2> l = {
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
        };

        xt::xtensor<double, 2> centers = {{4.5, 4.5}, {2.0, 2.0}};

        double row = static_cast<double>(l.shape(0));
        double col = static_cast<double>(l.shape(1));

        for (size_t i = 0; i < l.shape(0); ++i) {
            for (size_t j = 0; j < l.shape(1); ++j) {
                REQUIRE(xt::allclose(GooseEYE::center_of_mass(l), centers));
                l = xt::roll(l, 1, 1);
                auto y = xt::view(centers, xt::all(), 1);
                y += 1.0;
                y = xt::where(y > col, y - col, y);
            }
            l = xt::roll(l, 1, 0);
            auto x = xt::view(centers, xt::all(), 0);
            x += 1.0;
            x = xt::where(x > row, x - row, x);
        }
    }

    SECTION("center_of_mass - labels")
    {
        xt::xtensor<size_t, 2> l = {
            {2, 3, 3, 3, 2},
            {0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 1, 0, 0},
            {2, 4, 4, 4, 2},
        };

        xt::xtensor<size_t, 2> c = {
            {0, 0, 3, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 0, 4, 0, 2},
        };

        xt::xtensor<double, 2> centers = {
            {2.0, 4.5}, {2.0, 2.0}, {4.5, 4.5}, {0.0, 2.0}, {4.0, 2.0}};

        auto res_centers = GooseEYE::center_of_mass(l);
        xt::xtensor<size_t, 2> res_c = xt::zeros_like(l);
        res_c =
            GooseEYE::pos2img(res_c, xt::floor(res_centers), xt::arange<size_t>(centers.shape(0)));

        REQUIRE(xt::allclose(res_centers, centers));
        REQUIRE(xt::all(xt::equal(res_c, c)));
    }

    SECTION("Clusters::centers")
    {
        xt::xarray<size_t> I = xt::zeros<size_t>({5, 5});
        xt::xarray<size_t> C = xt::zeros<size_t>({5, 5});

        I(0, 0) = 1;
        I(0, 3) = 1;
        I(0, 4) = 1;
        I(3, 0) = 1;
        I(3, 3) = 1;
        I(3, 4) = 1;

        C(0, 4) = 1;
        C(3, 4) = 2;

        GooseEYE::Clusters clusters(I, true);
        REQUIRE(xt::all(xt::equal(C, clusters.centers())));
        REQUIRE(xt::all(xt::equal(GooseEYE::clusters(I, true), clusters.labels())));
    }
}

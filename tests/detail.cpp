#define CATCH_CONFIG_MAIN
#include <GooseEYE/GooseEYE.h>
#include <catch2/catch.hpp>

TEST_CASE("GooseEYE::detail", "detail.hpp")
{

    SECTION("periodic_copy - 1")
    {
        xt::xtensor<int, 1> C1 = {-1, 0, 0, 0, 0};
        xt::xtensor<int, 1> C2 = {-1, 0, 0, 0, 0};
        xt::xtensor<int, 1> S1 = {-1, 0, 0, 0, 0};
        xt::xtensor<int, 1> C = {-1, 0, 0, -1, 0};
        xt::xtensor<int, 1> S = {-1, 0, 0, 0, 0};
        std::vector<std::vector<size_t>> pad(1);
        pad[0] = {1, 1};
        GooseEYE::detail::periodic_copy_below_to_above(C1, pad);
        GooseEYE::detail::periodic_copy_above_to_below(S1, pad);
        GooseEYE::detail::periodic_copy(C2, pad);
        REQUIRE(xt::all(xt::equal(C1, C)));
        REQUIRE(xt::all(xt::equal(C2, C)));
        REQUIRE(xt::all(xt::equal(S1, S)));
    }

    SECTION("periodic_copy - 2")
    {
        xt::xtensor<int, 1> C1 = {0, 0, 0, 0, -1};
        xt::xtensor<int, 1> C2 = {0, 0, 0, 0, -1};
        xt::xtensor<int, 1> S1 = {0, 0, 0, 0, -1};
        xt::xtensor<int, 1> C = {0, -1, 0, 0, -1};
        xt::xtensor<int, 1> S = {0, 0, 0, 0, -1};
        std::vector<std::vector<size_t>> pad(1);
        pad[0] = {1, 1};
        GooseEYE::detail::periodic_copy_below_to_above(S1, pad);
        GooseEYE::detail::periodic_copy_above_to_below(C1, pad);
        GooseEYE::detail::periodic_copy(C2, pad);
        REQUIRE(xt::all(xt::equal(C1, C)));
        REQUIRE(xt::all(xt::equal(C2, C)));
        REQUIRE(xt::all(xt::equal(S1, S)));
    }

    SECTION("atleast_3d_axis - 3d")
    {
        xt::xarray<int> a = xt::arange<int>(3 * 4 * 5).reshape({3, 4, 5});
        xt::xarray<int> b = xt::atleast_3d(a);
        std::vector<size_t> shape = {3, 4, 5};
        REQUIRE(shape == GooseEYE::detail::shape(b));
        REQUIRE(GooseEYE::detail::atleast_3d_axis(3, 0) == 0);
        REQUIRE(GooseEYE::detail::atleast_3d_axis(3, 1) == 1);
        REQUIRE(GooseEYE::detail::atleast_3d_axis(3, 2) == 2);
    }

    SECTION("atleast_3d_axis - 2d")
    {
        xt::xarray<int> a = xt::arange<int>(3 * 4).reshape({3, 4});
        xt::xarray<int> b = xt::atleast_3d(a);
        std::vector<size_t> shape = {3, 4, 1};
        REQUIRE(shape == GooseEYE::detail::shape(b));
        REQUIRE(GooseEYE::detail::atleast_3d_axis(2, 0) == 0);
        REQUIRE(GooseEYE::detail::atleast_3d_axis(2, 1) == 1);
    }

    SECTION("atleast_3d_axis - 1d")
    {
        xt::xarray<int> a = xt::arange<int>(3).reshape({3});
        xt::xarray<int> b = xt::atleast_3d(a);
        std::vector<size_t> shape = {1, 3, 1};
        REQUIRE(shape == GooseEYE::detail::shape(b));
        REQUIRE(GooseEYE::detail::atleast_3d_axis(1, 0) == 1);
    }
}


#include <GooseEYE/GooseEYE.h>
#include <catch2/catch.hpp>

TEST_CASE("GooseEYE::detail", "detail.hpp")
{

SECTION("atleast3d_axis - 3d")
{
    xt::xarray<int> a = xt::arange<int>(3 * 4 * 5).reshape({3, 4, 5});
    xt::xarray<int> b = xt::atleast_3d(a);
    std::vector<size_t> shape = {3, 4, 5};
    REQUIRE(shape == GooseEYE::detail::shape(b));
    REQUIRE(GooseEYE::detail::atleast3d_axis(3, 0) == 0);
    REQUIRE(GooseEYE::detail::atleast3d_axis(3, 1) == 1);
    REQUIRE(GooseEYE::detail::atleast3d_axis(3, 2) == 2);
}

SECTION("atleast3d_axis - 2d")
{
    xt::xarray<int> a = xt::arange<int>(3 * 4).reshape({3, 4});
    xt::xarray<int> b = xt::atleast_3d(a);
    std::vector<size_t> shape = {3, 4, 1};
    REQUIRE(shape == GooseEYE::detail::shape(b));
    REQUIRE(GooseEYE::detail::atleast3d_axis(2, 0) == 0);
    REQUIRE(GooseEYE::detail::atleast3d_axis(2, 1) == 1);

}

SECTION("atleast3d_axis - 1d")
{
    xt::xarray<int> a = xt::arange<int>(3).reshape({3});
    xt::xarray<int> b = xt::atleast_3d(a);
    std::vector<size_t> shape = {1, 3, 1};
    REQUIRE(shape == GooseEYE::detail::shape(b));
    REQUIRE(GooseEYE::detail::atleast3d_axis(1, 0) == 1);
}

}

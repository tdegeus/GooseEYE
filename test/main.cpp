
#include <GooseEYE/GooseEYE.h>

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("GooseFEM::Clusters", "clusters.hpp")
{

SECTION("relabel_map")
{
  xt::xarray<size_t> a = xt::zeros<size_t>({5,5});
  xt::view(a, xt::range(0,2), xt::range(0,2)) = 1;
  xt::view(a, xt::range(3,4), xt::range(3,4)) = 2;

  xt::xarray<size_t> b = xt::zeros<size_t>({5,5});
  xt::view(b, xt::range(0,2), xt::range(0,2)) = 3;
  xt::view(b, xt::range(3,4), xt::range(3,4)) = 4;

  REQUIRE(xt::all(xt::equal(
    GooseEYE::relabel_map(a, b),
    xt::xtensor<size_t,1>{0, 3, 4}
  )));

  REQUIRE(xt::all(xt::equal(
    GooseEYE::relabel_map(b, a),
    xt::xtensor<size_t,1>{0, 0, 0, 1, 2}
  )));
}

}

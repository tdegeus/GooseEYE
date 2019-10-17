
#define GOOSEEYE_ENABLE_ASSERT
#include "../include/GooseEYE/GooseEYE.h"

#define CATCH_CONFIG_MAIN  // tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

// =================================================================================================

TEST_CASE("GooseFEM::Clusters", "clusters.hpp")
{

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

SECTION("centers")
{
  xt::xarray<size_t> I = xt::zeros<size_t>({5,5});
  xt::xarray<size_t> C = xt::zeros<size_t>({5,5});

  I(0, 0) = 1;
  I(0, 3) = 1;
  I(0, 4) = 1;
  I(3, 0) = 1;
  I(3, 3) = 1;
  I(3, 4) = 1;

  C(0, 4) = 1;
  C(3, 4) = 2;

  GooseEYE::Clusters clusters(I, true);

  auto c = clusters.centers();

  REQUIRE(xt::all(xt::equal(C, c)));
}

// -------------------------------------------------------------------------------------------------

}

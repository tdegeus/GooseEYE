
#include <GooseEYE/GooseEYE.h>

#define CATCH_CONFIG_MAIN
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

SECTION("dilate - 1d")
{
  xt::xarray<int> I = xt::zeros<int>({5});
  xt::xarray<int> D = xt::zeros<int>({5});

  I(0) = 2;

  D(0) = 2;
  D(1) = 2;
  D(4) = 2;

  auto d = GooseEYE::dilate(I);

  REQUIRE(xt::all(xt::equal(D, d)));
}

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

SECTION("centers")
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

  auto c = clusters.centers();

  REQUIRE(xt::all(xt::equal(C, c)));
}

// -------------------------------------------------------------------------------------------------

}

// =================================================================================================

TEST_CASE("GooseEYE::Ensemble", "Ensemble.hpp")
{

// -------------------------------------------------------------------------------------------------

SECTION("L")
{
  xt::xarray<int> I = xt::zeros<int>({5, 5});
  I(2,1) = 1;
  I(2,2) = 1;
  I(2,3) = 1;

  xt::xarray<double> O = GooseEYE::L({7, 7}, I, false);

  // Check if middle pixel is equal to phase probability
  REQUIRE(O(3,3) == 0.12);

  // Check if correct number of pixels are non-zero
  REQUIRE(xt::nonzero(O)[0].size() == 5);

  // Remove midlle pixel, no pixels connected anymore
  I(2,2) = 0;

  O = GooseEYE::L({7,7}, I, false);

  // Check if middle pixel is equal to phase probability
  REQUIRE(O(3,3) == 0.08);

  // Check if correct number of pixels are non-zero
  REQUIRE(xt::nonzero(O)[0].size() == 1);
}

// -------------------------------------------------------------------------------------------------

}

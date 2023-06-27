#include <catch2/catch_amalgamated.hpp>

#include "semigroup_string.hpp"
#include "semigroup.hpp"

TEST_CASE("semigroup string", "[semigroup]") {
  using Semigroup = Semigroup<std::string>;
  SECTION("zero and zero") {
    REQUIRE(Semigroup::op(Semigroup::zero, Semigroup::zero) == Semigroup::zero);
  }

  SECTION("zero and value") {
    std::string value = "test";
    REQUIRE(Semigroup::op(Semigroup::zero, value) == value);
  }

  SECTION("value and zero") {
    std::string value = "test";
    REQUIRE(Semigroup::op(value, Semigroup::zero) == value);
  }

  SECTION("value and value") {
    std::string value1 = "first";
    std::string value2 = "second";
    REQUIRE(Semigroup::op(value1, value2) == "first; second");
  }
}

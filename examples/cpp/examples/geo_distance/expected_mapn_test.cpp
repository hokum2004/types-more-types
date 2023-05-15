#include <catch2/catch_amalgamated.hpp>

#include "combiner.hpp"
#include "expected.hpp"
#include "expected_mapn.hpp"
#include "util.hpp"

#include <ostream>
#include <string>

template <>
struct Combiner<std::string> {
  static std::string combine(const std::string &a, const std::string &b) {
    return a + "; " + b;
  }
};
namespace {

struct Foo {
  int v;
};

bool operator==(const Foo &a, const Foo &b) { return a.v == b.v; }

std::ostream &operator<<(std::ostream &os, const Foo &v) {
  return os << "Foo{v:" << v.v << "}";
}

} // namespace

TEST_CASE("tie std::expected instances", "[tie]") {
  auto v1 = std::expected<double, std::string>(5.5);
  auto v2 = std::expected<int, std::string>(9);
  auto v3 = std::expected<std::string, std::string>(std::string("12"));
  auto v4 = std::expected<Foo, std::string>(Foo{14});

  SECTION("single") {
    auto got = tie_expected(v1);
    REQUIRE(got);

    auto expected = std::tuple<const double &>(v1.value());
    REQUIRE(got.value() == expected);

    REQUIRE(&std::get<0>(got.value()) == &v1.value());
  }

  SECTION("two") {
    auto got = tie_expected(v1, v2);
    REQUIRE(got);

    auto expected =
        std::tuple<const double &, const int &>(v1.value(), v2.value());
    REQUIRE(got.value() == expected);

    REQUIRE(&std::get<0>(got.value()) == &v1.value());
    REQUIRE(&std::get<1>(got.value()) == &v2.value());
  }

  SECTION("three") {
    auto got = tie_expected(v1, v2, v3);
    REQUIRE(got);

    auto expected =
        std::tuple<const double &, const int &, const std::string &>(
            v1.value(), v2.value(), v3.value());
    REQUIRE(got.value() == expected);

    REQUIRE(&std::get<0>(got.value()) == &v1.value());
    REQUIRE(&std::get<1>(got.value()) == &v2.value());
    REQUIRE(&std::get<2>(got.value()) == &v3.value());
  }

  SECTION("four") {
    auto got = tie_expected(v1, v2, v3, v4);
    REQUIRE(got);

    auto expected =
        std::tuple<const double &, const int &, const std::string &,
                   const Foo &>(v1.value(), v2.value(), v3.value(), v4.value());
    REQUIRE(got.value() == expected);

    REQUIRE(&std::get<0>(got.value()) == &v1.value());
    REQUIRE(&std::get<1>(got.value()) == &v2.value());
    REQUIRE(&std::get<2>(got.value()) == &v3.value());
    REQUIRE(&std::get<3>(got.value()) == &v4.value());
  }
}

TEST_CASE("tie std::unexpected instances", "[tie]") {
  std::expected<double, std::string> v1 = std::unexpected(std::string("first"));
  std::expected<int, std::string> v2 = std::unexpected(std::string("second"));
  std::expected<std::string, std::string> v3 =
      std::unexpected(std::string("third"));
  std::expected<Foo, std::string> v4 = std::unexpected(std::string("fourth"));

  SECTION("one") {
    auto got = tie_expected(v1);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == v1.error());
  }

  SECTION("two") {
    auto got = tie_expected(v1, v2);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() ==
            Combiner<std::string>::combine(v1.error(), v2.error()));
  }

  SECTION("three") {
    auto got = tie_expected(v1, v2, v3);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "first; second; third");
  }

  SECTION("four") {
    auto got = tie_expected(v1, v2, v3, v4);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "first; second; third; fourth");
  }
}

TEST_CASE("tie std::expected and std::unexpected instances", "[tie]") {
  std::expected<double, std::string> v1(0.5);
  std::expected<int, std::string> v2 = std::unexpected(std::string("second"));
  std::expected<std::string, std::string> v3(std::string("43"));
  std::expected<Foo, std::string> v4 = std::unexpected(std::string("fourth"));

  SECTION("two") {
    auto got = tie_expected(v1, v2);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "second");
  }

  SECTION("two: first unexpected") {
    auto got = tie_expected(v2, v1);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "second");
  }

  SECTION("three - 1") {
    auto got = tie_expected(v1, v2, v3);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "second");
  }

  SECTION("three - 2") {
    auto got = tie_expected(v2, v3, v4);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "second; fourth");
  }

  SECTION("three - 2") {
    auto got = tie_expected(v4, v2, v3);
    REQUIRE_FALSE(got);
    REQUIRE(got.error() == "fourth; second");
  }
}

#include <catch2/catch_amalgamated.hpp>

#include "expected_mapn.hpp"
#include "semigroup_string.hpp"
#include "ostream_printers.hpp"

#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace {

struct Foo {
  int v1;
  std::string v2;
  double v3;
};

bool operator==(const Foo &lhv, const Foo &rhv) {
  return lhv.v1 == rhv.v1 && lhv.v2 == rhv.v2 && lhv.v3 == rhv.v3;
}

std::ostream &operator<<(std::ostream &os, const Foo &v) {
  return os << "Foo{v1:" << v.v1 << "; v2: " << v.v2 << "; v3:" << v.v3 << "}";
}

} // namespace

TEST_CASE("all params have values", "[mapn]") {
  SECTION("one") {
    std::expected<int, std::string> v1(1);
    auto f = [](int v) -> std::string { return std::to_string(2 * v); };
    auto obtained = mapn(f, v1);
    REQUIRE(std::is_same_v<decltype(obtained),
                           std::expected<std::string, std::string>>);
    REQUIRE(*obtained == "2");
  }

  SECTION("two") {
    std::expected<int, std::string> v1(2);
    std::expected<std::string, std::string> v2("test");
    auto f = [](int v1, const std::string &v2) -> std::vector<std::string> {
      return std::vector<std::string>(v1, v2);
    };
    auto obtained = mapn(f, v1, v2);
    REQUIRE(
        std::is_same_v<decltype(obtained),
                       std::expected<std::vector<std::string>, std::string>>);
    REQUIRE(*obtained == (std::vector<std::string>{"test", "test"}));
  }

  SECTION("three") {

    std::expected<int, std::string> v1(2);
    std::expected<std::string, std::string> v2("test");
    std::expected<double, std::string> v3(0.5);
    auto f = [](int v1, const std::string &v2, double v3) -> Foo {
      return Foo{v1, v2, v3};
    };
    auto obtained = mapn(f, v1, v2, v3);
    Foo expected{2, "test", 0.5};
    REQUIRE(
        std::is_same_v<decltype(obtained), std::expected<Foo, std::string>>);
    REQUIRE(*obtained == expected);
  }
}

TEST_CASE("all params have errors", "[mapn]") {
  SECTION("one") {
    std::expected<int, std::string> v1 = std::unexpected("one");
    auto f = [](int v) -> std::string { return std::to_string(2 * v); };
    auto obtained = mapn(f, v1);
    REQUIRE(std::is_same_v<decltype(obtained),
                           std::expected<std::string, std::string>>);
    REQUIRE(obtained.error() == "one");
  }

  SECTION("two") {
    std::expected<int, std::string> v1 = std::unexpected("one");
    std::expected<std::string, std::string> v2 = std::unexpected("two");
    auto f = [](int v1, const std::string &v2) -> std::vector<std::string> {
      return std::vector<std::string>(v1, v2);
    };
    auto obtained = mapn(f, v1, v2);
    REQUIRE(
        std::is_same_v<decltype(obtained),
                       std::expected<std::vector<std::string>, std::string>>);
    REQUIRE(obtained.error() == "one; two");
  }

  SECTION("three") {
    std::expected<int, std::string> v1 = std::unexpected("one");
    std::expected<std::string, std::string> v2 = std::unexpected("two");
    std::expected<double, std::string> v3 = std::unexpected("three");
    auto f = [](int v1, const std::string &v2, double v3) -> Foo {
      return Foo{v1, v2, v3};
    };
    auto obtained = mapn(f, v1, v2, v3);
    Foo expected{2, "test", 0.5};
    REQUIRE(
        std::is_same_v<decltype(obtained), std::expected<Foo, std::string>>);
    REQUIRE(obtained.error() == "one; two; three");
  }
}

TEST_CASE("mixed values and errors", "[mapn]") {
  SECTION("two") {
    auto f = [](int v1, const std::string &v2) -> std::vector<std::string> {
      return std::vector<std::string>(v1, v2);
    };

    std::expected<int, std::string> v1 = std::unexpected("one");
    std::expected<std::string, std::string> v2("value");
    REQUIRE(mapn(f, v1, v2).error() == "one");

    v1 = 12;
    v2 = std::unexpected("two");
    REQUIRE(mapn(f, v1, v2).error() == "two");
  }

  SECTION("three") {
    auto f = [](int v1, const std::string &v2, double v3) -> Foo {
      return Foo{v1, v2, v3};
    };

    std::expected<int, std::string> v1 = std::unexpected("one");
    std::expected<std::string, std::string> v2 = std::unexpected("two");
    std::expected<double, std::string> v3 = std::unexpected("three");
    auto obtained = mapn(f, v1, v2, v3);
    Foo expected{2, "test", 0.5};
    REQUIRE(
        std::is_same_v<decltype(obtained), std::expected<Foo, std::string>>);
    REQUIRE(obtained.error() == "one; two; three");
  }
}

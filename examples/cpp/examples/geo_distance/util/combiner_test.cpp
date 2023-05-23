#include <catch2/catch_amalgamated.hpp>

#include "combiner.hpp"
#include "semigroup.hpp"

namespace {
struct Foo {
  int v;
};

bool operator==(const Foo& lhv, const Foo& rhv) {
  return lhv.v == rhv.v;
}

std::ostream& operator<< (std::ostream&os, const Foo& v) {
  return os << "Foo{" << v.v << "}";
}

} // namespace

template <>
struct Semigroup<Foo> {
  static int counter;
  static Foo op(const Foo &lhv, const Foo &rhv) {
    ++counter;
    return Foo{lhv.v + rhv.v};
  }
  static constexpr Foo zero = Foo{0};
};

int Semigroup<Foo>::counter = 0;

TEST_CASE("combine values", "[combiner]") {
  using Combiner = Combiner<Foo>;
  using Semigroup = Semigroup<Foo>;

  SECTION("one") {
    Semigroup::counter = 0;
    CHECK(Combiner::combine(Foo{1}) == Foo{1});
    CHECK(Semigroup::counter == 0);
  }

  SECTION("two") {
    Semigroup::counter = 0;
    CHECK(Combiner::combine(Foo{1}, Foo{10}) == Foo{11});
    CHECK(Semigroup::counter == 1);
  }

  SECTION("three") {
    Semigroup::counter = 0;
    CHECK(Combiner::combine(Foo{1}, Foo{10}, Foo{100}) == Foo{111});
    CHECK(Semigroup::counter == 2);
  }

  SECTION("four") {
    Semigroup::counter = 0;
    CHECK(Combiner::combine(Foo{1}, Foo{10}, Foo{100}, Foo{1000}) == Foo{1111});
    CHECK(Semigroup::counter == 3);
  }
}

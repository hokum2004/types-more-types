#include "new_type.hpp"

#include <functional>
#include <iostream>
#include <optional>
#include <tuple>

#include "expected_mapn.hpp"
#include "util.hpp"

struct LatitudeTag {
  using Type = double;
  static const std::vector<Validation<Type>> validations;
};

const std::vector<Validation<LatitudeTag::Type>> LatitudeTag::validations = {
    [](double v) -> std::optional<std::string> {
      return v <= -85 ? std::optional<std::string>(
                            "longitude must be greater than or equal to-85")
                      : std::optional<std::string>();
    },
    [](double v) -> std::optional<std::string> {
      return v >= 85 ? std::optional<std::string>(
                           "longitude must be less than or equal to 85")
                     : std::optional<std::string>();
    }};

using Latitude = ValidatedNewType<LatitudeTag>;

struct LongitudeTag {
  using Type = double;
  static const std::vector<Validation<Type>> validations;
};

const std::vector<Validation<LongitudeTag::Type>> LongitudeTag::validations = {
    [](double v) -> std::optional<std::string> {
      return v <= -180 ? std::optional<std::string>(
                             "longitude must be greater than or equal to -180")
                       : std::optional<std::string>();
    },
    [](double v) -> std::optional<std::string> {
      return v >= 180 ? std::optional<std::string>(
                            "longitude must be less than or equal to 180")
                      : std::optional<std::string>();
    }};

using Longitude = ValidatedNewType<LongitudeTag>;

std::ostream &operator<<(std::ostream &os, MultipleErrors const &errs) {
  os << errs.first;
  for (auto const &err : errs.rest) {
    os << "; " << err;
  }
  return os;
}

struct Point {
  Latitude lat;
  Longitude lon;
};

template <typename F, typename E, typename T, typename... Ts>
void foo(F, std::expected<T, E> const &v, std::expected<Ts, E> const &...vs){};

template <typename R, typename A, typename B>
void bar(std::function<R(std::tuple<A const &, B const &>)> f){};

Point newPoint(std::tuple<Latitude const &, Longitude const &> args) {
  return Point{std::get<0>(args), std::get<1>(args)};
};

template <typename E, typename... Args>
void qoo(std::expected<Args, E> const &...) {}

int main() {
  auto lat1 = Latitude::New(60);
  auto lon1 = Longitude::New(40);
  auto lat2 = Latitude::New(-89);
  auto lon2 = Longitude::New(-181);

  std::cout << std::make_tuple(1, 2, 3) << std::endl;
  auto r = tie_expected(lat1, lon1, lat1, lon1);
  std::cout << r << std::endl;

  auto r1 = tie_expected(lat2, lon2, lat2, lon2);
  std::cout << r1 << std::endl;

  std::cout << tie_expected(lat1, lon2) << "\n";
  std::cout << tie_expected(lat2, lon1) << "\n";
  std::cout << tie_expected(lat1, lon1, lon2) << "\n";
  std::cout << tie_expected(lat1, lon2, lon1, lat2) << "\n";

  std::cout << "lat1: " << lat1 << "\n";
  std::cout << "lon1: " << lon1 << "\n";
  std::cout << "lat2: " << lat2 << "\n";
  std::cout << "lon2: " << lon2 << "\n";

  auto newPointL =
      [](std::tuple<Latitude const &, Longitude const &> args) -> Point {
    return Point{std::get<0>(args), std::get<1>(args)};
  };

  foo(newPoint, lat1, lon2);
  bar<Point, Latitude, Longitude>(newPoint);
  qoo(lat1, lon1);
  // mapN<Point, MultipleErrors, Latitude, Longitude>(newPoint, lat1, lon2);
  /*
  auto p1 = mapN(
      [](std::tuple<Latitude const &, Longitude const &> args) -> Point{
        return Point{std::get<0>(args), std::get<1>(args)};
      },
      lat1, lon2);
  */

  return 0;
}

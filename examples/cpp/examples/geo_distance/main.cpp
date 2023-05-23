#include "new_type.hpp"

#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <tuple>

#include "util/expected_mapn.hpp"
#include "util/expected_tie.hpp"
#include "util/ostream_printers.hpp"

struct LatitudeTag {
  using Type = double;
  static const std::vector<Validation<Type>> validations;
};

const std::vector<Validation<LatitudeTag::Type>> LatitudeTag::validations = {
    [](double v) -> std::optional<std::string> {
      return v <= -85 ? std::optional<std::string>(
                            "longitude must be greater than or equal to -85")
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

std::ostream &operator<<(std::ostream &os, const Point &v) {
  return os << "Point(" << v.lat << ", " << v.lon << ")";
}

Point NewPoint(Latitude lat, Longitude lon) {
  return Point{lat, lon};
}

Point newPoint(Latitude lat, Longitude lon) { return Point{lat, lon}; };

template <typename E, typename... Args>
void qoo(std::expected<Args, E> const &...) {}

int main() {
  auto lat1 = Latitude::New(60);
  auto lon1 = Longitude::New(40);
  auto p1 = mapn(NewPoint, lat1, lon1);

  std::cout << "lat1: " << lat1 << "\n";
  std::cout << "lon1: " << lon1 << "\n";
  std::cout << p1 << "\n";

  auto lat2 = Latitude::New(-89);
  auto lon2 = Longitude::New(-181);
  auto p2 = mapn(NewPoint, lat2, lon2);

  std::cout << "lat2: " << lat2 << "\n";
  std::cout << "lon2: " << lon2 << "\n";
  std::cout << p2 << "\n";

  return 0;
}

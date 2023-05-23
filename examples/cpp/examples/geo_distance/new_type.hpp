#pragma once

#include "multiple_errors.hpp"

#ifdef __clang__
#include "util/expected.hpp"
#else
#include <expected>
#endif
#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

template <typename T>
using Result = std::expected<T, MultipleErrors>;

template <typename T>
using Validation = std::function<std::optional<std::string>(const T &)>;

template <typename Tag>
struct ValidatedNewType {
  friend Tag;
  using Type = ValidatedNewType<Tag>;
  using Raw = typename Tag::Type;
  using Result = std::expected<Type, MultipleErrors>;

  static Result New(const Raw &);

  friend std::ostream &operator<<(std::ostream &os, const Type &v) {
    return os << v.value_;
  }

private:
  explicit ValidatedNewType(const Raw &value) : value_(value) {}
  Raw value_;
};

template <typename Tag>
typename ValidatedNewType<Tag>::Result
ValidatedNewType<Tag>::New(typename ValidatedNewType<Tag>::Raw const &value) {
  std::vector<std::string> errs;
  for (Validation validation : Tag::validations) {
    if (auto err = validation(value); err) {
      errs.emplace_back(*err);
    }
  }

  if (errs.empty()) {
    return Result(Type(value));
  }

  return std::unexpected(
      MultipleErrors{errs.front(), std::vector(++errs.begin(), errs.end())});
}

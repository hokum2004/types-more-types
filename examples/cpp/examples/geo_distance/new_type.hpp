#pragma once

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

#include "util/semigroup.hpp"

struct MultipleErrors {
  std::string first;
  std::vector<std::string> rest;
};

template <typename T>
using Result = std::expected<T, MultipleErrors>;

template <>
struct Semigroup<MultipleErrors> {
  static MultipleErrors op(const MultipleErrors &e1, const MultipleErrors &e2) {
    if (e1.first.empty())
      return e2;
    if (e2.first.empty())
      return e1;

    std::vector<std::string> errs(e1.rest.begin(), e1.rest.end());
    errs.push_back(e2.first);
    std::copy(e2.rest.begin(), e2.rest.end(), std::back_inserter(errs));

    return MultipleErrors{e1.first, std::move(errs)};
  }

  static const MultipleErrors zero;
};

const MultipleErrors Semigroup<MultipleErrors>::zero{"", {}};

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

#pragma once

#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif
#include <algorithm>
#include <concepts>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>
#include <ostream>

#include "combiner.hpp"

struct MultipleErrors {
  std::string first;
  std::vector<std::string> rest;
};

template <typename T>
using Result = std::expected<T, MultipleErrors>;

template <>
struct Combiner<MultipleErrors> {
  static MultipleErrors combine(const MultipleErrors &e1,
                                const MultipleErrors &e2) {
    std::vector<std::string> errs(e1.rest.begin(), e1.rest.end());
    errs.push_back(e2.first);
    std::copy(e2.rest.begin(), e2.rest.end(), std::back_inserter(errs));
    return MultipleErrors{e1.first, std::move(errs)};
  }
};

template <typename T>
concept ValidatedTagable = true;
//    (not std::is_void_v<typename T::Type>) &&
//    std::is_invocable_v<decltype(T::validations)::value_type> &&
//    std::is_convertible_v<std::invoke_result_t<decltype(T::validations[0])>,
//                          std::string>;
//

template <typename T>
using Validation = std::function<std::optional<std::string>(const T &)>;

template <typename Tag>
  requires ValidatedTagable<Tag>
struct ValidatedNewType {
  friend Tag;
  using Type = ValidatedNewType<Tag>;
  using Raw = typename Tag::Type;
  using Result = std::expected<Type, MultipleErrors>;

  static Result New(const Raw &);

  friend std::ostream& operator<<(std::ostream &os, const Type& v) {
    return os << v.value_;
  }
private:
  explicit ValidatedNewType(const Raw &value) : value_(value) {}
  Raw value_;
};

template <typename Tag>
  requires ValidatedTagable<Tag>
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

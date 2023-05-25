#pragma once

#include <bits/utility.h>
#include <cstddef>
#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif
#include <ostream>
#include <tuple>

namespace std {

inline ostream &operator<<(ostream &os, const tuple<> &) { return os << "()"; }

template <typename... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &v) {
  auto p = [&os, &v ]<size_t... Ns>(std::index_sequence<Ns...>) {
    os << std::get<0>(v);
    ((os << ", " << std::get<Ns + 1>(v)), ...);
  };
  os << "(";
  p(std::make_index_sequence<sizeof...(Ts) - 1>{});
  return os << ")";
}

template <typename T, typename E>
ostream &operator<<(ostream &os, expected<T, E> const &value) {
  if (value) {
    return os << "expected(" << value.value() << ")";
  }
  return os << "unexpected(" << value.error() << ")";
}

} // namespace std

#pragma once

#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif
#include <ostream>
#include <tuple>

namespace std {

template <size_t I, size_t N, typename... Ts>
struct __TuplePrinter {
  static ostream &print(ostream &os, const std::tuple<Ts...> &v) {
    os << ", " << get<I>(v);
    return __TuplePrinter<I + 1, N, Ts...>::print(os, v);
  }
};

template <size_t N, typename... Ts>
struct __TuplePrinter<0, N, Ts...> {
  static ostream &print(ostream &os, const std::tuple<Ts...> &v) {
    os << get<0>(v);
    return __TuplePrinter<1, N, Ts...>::print(os, v);
  }
};

template <size_t N, typename... Ts>
struct __TuplePrinter<N, N, Ts...> {
  static ostream &print(ostream &os, const std::tuple<Ts...> &v) { return os; }
};

template <typename... Ts>
ostream &operator<<(ostream &os, const tuple<Ts...> &v) {
  os << "(";
  return __TuplePrinter<0, sizeof...(Ts), Ts...>::print(os, v) << ")";
}

template <typename T, typename E>
ostream &operator<<(ostream &os, expected<T, E> const &value) {
  if (value) {
    return os << "expected(" << value.value() << ")";
  }
  return os << "unexpected(" << value.error() << ")";
}

} // namespace std

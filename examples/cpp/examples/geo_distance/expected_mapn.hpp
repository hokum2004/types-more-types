#pragma once

#include <utility>
#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif
#include <functional>
#include <iterator>
#include <tuple>

#include "combiner.hpp"

template <typename R, typename E, typename T>
std::expected<R, E> __tie_expected_fail(const E &e,
                                        const std::expected<T, E> &v) {
  if (!v) {
    return std::unexpected(Combiner<E>::combine(e, v.error()));
  }
  return std::unexpected(e);
}

template <typename R, typename E, typename T, typename... Ts>
std::expected<R, E> __tie_expected_fail(const E &e,
                                        const std::expected<T, E> &v,
                                        const std::expected<Ts, E> &...vs) {
  if (!v) {
    return __tie_expected_fail<R, E, Ts...>(Combiner<E>::combine(e, v.error()), vs...);
  }
  return __tie_expected_fail<R, E, Ts...>(e, vs...);
}

template <typename R, typename S, typename E, typename T>
std::expected<R, E> __tie_expected(S res, const std::expected<T, E> &v) {
  if (!v) {
    return std::unexpected(v.error());
  }
  return std::expected<R, E>(
      std::tuple_cat(res, std::tuple<const T &>(v.value())));
}

template <typename R, typename S, typename E, typename T, typename... Ts>
std::expected<R, E> __tie_expected(S res, const std::expected<T, E> &v,
                                   const std::expected<Ts, E> &...vs) {
  using Sn = decltype(std::tuple_cat(res, std::tuple<const T &>(v.value())));
  if (!v) {
    return __tie_expected_fail<R, E, Ts...>(v.error(), vs...);
  }
  return __tie_expected<R, Sn, E, Ts...>(
      std::tuple_cat(res, std::tuple<const T &>(v.value())), vs...);
}

template <typename E, typename T, typename... Ts>
std::expected<std::tuple<const T &, const Ts &...>, E>
tie_expected(const std::expected<T, E> &v, const std::expected<Ts, E> &...vs) {
  using R = std::tuple<const T &, const Ts &...>;
  using S = std::tuple<const T &>;
  if (!v) {
    return __tie_expected_fail<R, E, Ts...>(v.error(), vs...);
  }
  return __tie_expected<R, S, E, Ts...>(S(v.value()), vs...);
}

template <typename F, typename E, typename T, typename... Ts>
std::expected<decltype(F(std::declval<std::tuple<const T &, const Ts &...>>())),
              E>
mapN(F f, std::expected<T, E> const &v, std::expected<Ts, E> const &...vs) {}

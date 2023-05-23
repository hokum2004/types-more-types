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

#include "__error_or.hpp"
#include "combiner.hpp"
#include "semigroup.hpp"

template <typename E, typename T, typename... Ts>
std::expected<std::tuple<const T &, const Ts &...>, E>
tie_expected(const std::expected<T, E> &v, const std::expected<Ts, E> &...vs) {
  using R = std::tuple<const T &, const Ts &...>;
  if ((v && ... && vs)) {
    return R(*v, *vs...);
  }

  return std::unexpected(
      Combiner<E>::combine(__error_or(v), __error_or(vs)...));
}

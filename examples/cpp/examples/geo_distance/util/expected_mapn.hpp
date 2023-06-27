#pragma once

#include <utility>
#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif
#include <functional>
#include <iterator>
#include <optional>
#include <tuple>
#include <type_traits>

#include "__error_or.hpp"
#include "combiner.hpp"
#include "semigroup.hpp"

template <typename F, typename E, typename T, typename... Ts>
std::expected<
    std::remove_cvref_t<std::invoke_result_t<F, const T &, const Ts &...>>, E>
mapn(F &&f, std::expected<T, E> const &v, std::expected<Ts, E> const &...vs) {
  if ((v && ... && vs)) {
    return std::invoke(std::forward<F>(f), *v, *vs...);
  }

  return std::unexpected(
      Combiner<E>::combine(__error_or(v), __error_or(vs)...));
}

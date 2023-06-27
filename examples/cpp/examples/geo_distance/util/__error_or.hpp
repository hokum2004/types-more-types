#pragma once

#ifdef __clang__
#include "expected.hpp"
#else
#include <expected>
#endif

#include "semigroup.hpp"

template <typename T, typename E>
const E &__error_or(const std::expected<T, E> &v) {
  return v ? Semigroup<E>::zero : v.error();
};


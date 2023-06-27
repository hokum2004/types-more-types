#pragma once

#include "semigroup.hpp"

template <typename T>
struct Combiner {
  static T combine(const T &);
  template <typename... Ts>
  static T combine(const T &, const Ts &...);
};

template <typename T>
T Combiner<T>::combine(const T &v) {
  return v;
}

template <typename T>
template <typename... Ts>
T Combiner<T>::combine(const T &v, const Ts &...vs) {
  return Semigroup<T>::op(v, combine(vs...));
}

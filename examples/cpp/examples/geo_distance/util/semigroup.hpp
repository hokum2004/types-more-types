#pragma once

template<typename T>
struct Semigroup {
  static T op(const T&, const T&);
  static const T zero;
};

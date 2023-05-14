#pragma once

template<typename T>
struct Combiner {
  static T combine(const T&, const T&);
};

#pragma once

#include "semigroup.hpp"

#include <string>

template <>
struct Semigroup<std::string> {
  static std::string op(const std::string &a, const std::string &b);
  static const std::string zero;
};

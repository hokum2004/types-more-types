#include "semigroup_string.hpp"

std::string Semigroup<std::string>::op(const std::string &a, const std::string &b) {
    if (a.empty())
      return b;
    if (b.empty())
      return a;
    return a + "; " + b;
}

const std::string Semigroup<std::string>::zero = std::string();

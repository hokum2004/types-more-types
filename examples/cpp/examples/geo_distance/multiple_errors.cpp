#include "multiple_errors.hpp"

const MultipleErrors Semigroup<MultipleErrors>::zero{"", {}};

std::ostream &operator<<(std::ostream &os, MultipleErrors const &errs) {
  os << errs.first;
  for (auto const &err : errs.rest) {
    os << "; " << err;
  }
  return os;
}


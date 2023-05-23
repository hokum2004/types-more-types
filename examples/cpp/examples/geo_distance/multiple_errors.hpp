#pragma once
#include "util/semigroup.hpp"

#include <string>
#include <vector>

struct MultipleErrors {
  std::string first;
  std::vector<std::string> rest;
};

template <>
struct Semigroup<MultipleErrors> {
  static MultipleErrors op(const MultipleErrors &e1, const MultipleErrors &e2) {
    if (e1.first.empty())
      return e2;
    if (e2.first.empty())
      return e1;

    std::vector<std::string> errs(e1.rest.begin(), e1.rest.end());
    errs.push_back(e2.first);
    std::copy(e2.rest.begin(), e2.rest.end(), std::back_inserter(errs));

    return MultipleErrors{e1.first, std::move(errs)};
  }

  static const MultipleErrors zero;
};


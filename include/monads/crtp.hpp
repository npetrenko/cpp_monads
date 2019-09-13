#pragma once

#include "type_traits.hpp"

template <class Derived>
class CRTPDerivedCaster {
public:
  template <class T>
  static constexpr auto&& Cast(T&& base) {
    using CopiedCRef = CopyCRef_t<From<Forward_t<T>>, Derived>;
    return static_cast<CopiedCRef>(base);
  }
};

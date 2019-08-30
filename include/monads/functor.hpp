#pragma once

#include <utility>
#include "crtp.hpp"
#include "mapping.hpp"

template <class Derived>
class Functor : private CRTPDerivedCaster<Derived> {
public:
    template <class Func>
    auto FMap(Func&& func) {
        return FMapImpl(*this, std::forward<Func>(func));
    }

    template <class Func>
    auto FMap(Func&& func) const {
        return FMapImpl(*this, std::forward<Func>(func));
    }

private:
    template <class ThisT, class Func>
    static auto FMapImpl(ThisT&& me, Func&& func) {
        using ValueType = typename CatTraits<Derived>::ValueType;
        static_assert(CanBeCalled_v<Func, ValueType>, "Func cannot be called on this ValueType");
        return Derived::FMapImpl(*Cast(&me), std::forward<Func>(func));
    }

    using CRTPDerivedCaster<Derived>::Cast;
};

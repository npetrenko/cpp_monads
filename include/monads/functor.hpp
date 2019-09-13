#pragma once

#include <utility>
#include "crtp.hpp"
#include "mapping.hpp"
#include "type_traits.hpp"

template <class Derived>
class Functor : private CRTPDerivedCaster<Derived> {
public:
    template <class Func>
    auto FMap(Func&& func) & {
        return FMapImpl(*this, std::forward<Func>(func));
    }

    template <class Func>
    auto FMap(Func&& func) const& {
        return FMapImpl(*this, std::forward<Func>(func));
    }

    template <class Func>
    auto FMap(Func&& func) && {
        return FMapImpl(std::move(*this), std::forward<Func>(func));
    }

private:
    template <class ThisT, class Func>
    static auto FMapImpl(ThisT&& me, Func&& func) {
        using ValueType = typename CatTraits<Derived>::ValueType;
        using CopiedCRef = CopyCRef_t<From<Forward_t<ThisT>>, ValueType>;

        static_assert(CanBeCalled_v<Func, CopiedCRef>,
                      "Func cannot be called on this cvrefed-ValueType");
        return Derived::FMapImpl(Cast(std::forward<ThisT>(me)), std::forward<Func>(func));
    }

    using CRTPDerivedCaster<Derived>::Cast;
};

#pragma once

#include <utility>
#include "crtp.hpp"
#include "type_traits.hpp"
#include "functor.hpp"
#include <exception>

template <class>
class Applicative;

/*
 * This struct has to be specialized with operator() implemented
 */
template <class Concrete>
struct Pure {
    template <class T, class Dummy = void>
    auto operator()(T&&) const {
        static_assert(!std::is_same_v<Dummy, void>,
                      "Pure has to be specialized for each Applicative functor");
    }
};

template <class Derived>
struct CatTraits<Applicative<Derived>> {
    using ValueType = typename CatTraits<Derived>::ValueType;
};

template <class Derived>
class Applicative : private CRTPDerivedCaster<Derived>, public Functor<Applicative<Derived>> {
    friend class Functor<Applicative<Derived>>;

public:
    template <class Other>
    auto operator|(Other&& arg) const& {
        return OpImpl(*this, std::forward<Other>(arg));
    }

    template <class Other>
    auto operator|(Other&& arg) & {
        return OpImpl(*this, std::forward<Other>(arg));
    }

    template <class Other>
    auto operator|(Other&& arg) && {
        return OpImpl(std::move(*this), std::forward<Other>(arg));
    }

private:
    template <class Me, class Other>
    static auto OpImpl(Me&& me, Other&& other) {
        using CleanOther = std::remove_cv_t<std::remove_reference_t<Other>>;
        static_assert(IsTemplateInstance_v<Applicative, CleanOther>,
                      "Cannot use operator| on non-applicative class");
        return Derived::OpImplOverride(Cast(std::forward<Me>(me)), std::forward<Other>(other));
    }

    template <class Me, class Func>
    static auto FMapImpl(Me&& me, Func&& func) {
        return Pure<Derived>()(std::forward<Func>(func)) | Cast(std::forward<Me>(me));
    }

    using CRTPDerivedCaster<Derived>::Cast;
};

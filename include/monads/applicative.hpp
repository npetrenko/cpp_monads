#pragma once

#include <utility>
#include "crtp.hpp"
#include "type_traits.hpp"
#include "functor.hpp"
#include <exception>

template <class>
class Applicative;

template <class Concrete>
struct Pure {
    template <class T>
    auto operator()(T&& value) const {
        (void)value;
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
    auto operator|(Other&& arg) const {
        return OpImpl(*this, std::forward<Other>(arg));
    }

    template <class Other>
    auto operator|(Other&& arg) {
        return OpImpl(*this, std::forward<Other>(arg));
    }

private:
    template <class Me, class Other>
    static auto OpImpl(Me&& me, Other&& other) {
        using CleanOther = std::remove_cv_t<std::remove_reference_t<Other>>;
        static_assert(IsTemplateSpecialization_v<Applicative, CleanOther>, "Cannot do that");
        return Derived::OperatorImpl(*Cast(&me), std::forward<Other>(other));
    }

    template <class Me, class Func>
    static auto FMapImpl(Me&& me, Func&& func) {
        return Pure<Derived>()(std::forward<Func>(func)) | *Cast(&me);
    }

    using CRTPDerivedCaster<Derived>::Cast;
};

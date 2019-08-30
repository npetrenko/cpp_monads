#include <gtest/gtest.h>
#include <monads/functor.hpp>
#include <monads/applicative.hpp>
#include <vector>
#include <optional>
#include <iostream>

TEST(Currying, Basic) {
    CurryingFunction func([](int a, int b) { return a + b; });
    static_assert(EmitsValue_v<decltype(func), int, int>, "Wrong");

    using Curried = decltype(func(1));
    static_assert(EmitsValue_v<Curried, int>, "Wrong");

    ASSERT_EQ(func(1)(2), 3);
    ASSERT_EQ(func(1, 2), 3);
}

template <class>
class VectorFunctor;

template <class T>
struct CatTraits<VectorFunctor<T>> {
    using ValueType = T;
};

template <class T>
class VectorFunctor : public Functor<VectorFunctor<T>>, public std::vector<T> {
    friend struct Functor<VectorFunctor<T>>;

public:
    VectorFunctor(std::vector<T> vec) : std::vector<T>(std::move(vec)) {
    }

private:
    template <class ThisT, class Func>
    static auto FMapImpl(ThisT&& me, Func&& func) {
        using Output = OutputType_t<Func, T>;
        std::vector<Output> output;
        output.reserve(me.size());
        std::transform(me.begin(), me.end(), std::back_inserter(output), std::forward<Func>(func));
        return VectorFunctor{std::move(output)};
    }
};

TEST(Functor, Basic) {
    VectorFunctor vec(std::vector<int>{1, 2, 3, 4});
    auto map = [](int i) { return i + 1; };
    std::vector expected{2, 3, 4, 5};
    ASSERT_EQ(vec.FMap(map), expected);
}

struct AbstractNothing {};

template <class T>
class MaybeApplicative;

template <class T>
MaybeApplicative<T> Nothing() {
    return {AbstractNothing()};
}

template <class T>
MaybeApplicative<std::remove_cv_t<std::remove_reference_t<T>>> Just(T&& value);

template <class T>
struct CatTraits<MaybeApplicative<T>> {
    using ValueType = T;
};

template <class T>
class MaybeApplicative : public Applicative<MaybeApplicative<T>>, private std::optional<T> {
private:
    using BaseT = std::optional<T>;

    template <class S>
    friend class Applicative;

public:
    using ValueType = T;
    explicit MaybeApplicative(T value) : BaseT(value) {
    }

    MaybeApplicative(AbstractNothing) : BaseT(std::nullopt) {
    }

    bool isJust() const {
        return this->has_value();
    }

    using BaseT::operator*;

private:
    template <class Me, class S>
    static auto OperatorImpl(Me&& me, S&& other) {
        auto call_impl = [&] { return (*std::forward<Me>(me))(*std::forward<S>(other)); };
        using ReturnType = decltype(call_impl());
        if (!other.isJust() || !me.isJust()) {
            return Nothing<ReturnType>();
        }
        return MaybeApplicative<ReturnType>(call_impl());
    }
};

template <class T>
struct Pure<MaybeApplicative<T>> {
    template <class S>
    auto operator()(S&& val) const {
        return Just(std::forward<S>(val));
    }
};

template <class T>
MaybeApplicative<std::remove_cv_t<std::remove_reference_t<T>>> Just(T&& value) {
    return MaybeApplicative{std::forward<T>(value)};
}

TEST(MaybeApplicative, IsApplicative) {
    auto func = Just(CurryingFunction([](int a, int b) { return a + b; }));

    MaybeApplicative maybe_value = func | Just(1) | Just(2);
    ASSERT_TRUE(maybe_value.isJust());
    ASSERT_EQ(*maybe_value, 3);

    MaybeApplicative maybe_novalue = func | Just(1) | Nothing<int>();
    ASSERT_FALSE(maybe_novalue.isJust());
}

TEST(MaybeApplicative, IsFunctor) {
    MaybeApplicative maybe_value = Just(1).FMap([](int a) { return a + 1; });
    ASSERT_TRUE(maybe_value.isJust());
    ASSERT_EQ(*maybe_value, 2);

    MaybeApplicative maybe_novalue =
        Nothing<int>().FMap([](auto) -> std::string { return "string"; });
    ASSERT_FALSE(maybe_novalue.isJust());
}

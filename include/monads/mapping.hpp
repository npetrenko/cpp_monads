#pragma once

#include <utility>
#include "type_traits.hpp"

template <class Func>
class CurryingFunction {
public:
    CurryingFunction(Func&& func) : func_(std::move(func)) {
    }

    CurryingFunction(const Func& func) : func_(func) {
    }

    template <class... Args>
    auto operator()(Args&&... args) & {
        return OpImpl(*this, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto operator()(Args&&... args) const& {
        return OpImpl(*this, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto operator()(Args&&... args) && {
        return OpImpl(std::move(*this), std::forward<Args>(args)...);
    }

private:
    template <class CF, class... Args>
    static auto OpImpl(CF&& cf, Args&&... args) {
        if constexpr (EmitsValue_v<Func, Forward_t<Args>...>) {
            return cf.func_(std::forward<Args>(args)...);
        } else {
            using CurrierT = Currier<Args...>;
            CurrierT currier{cf.func_, std::forward<Args>(args)...};
            return CurryingFunction<CurrierT>(std::move(currier));
        }
    }

    template <class... Args>
    struct Currier {
        using TupT = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;

        template <class... T>
        Currier(Func f, T&&... args) noexcept(noexcept(Func(std::move(f))) &&
                                              noexcept(TupT(std::forward<T>(args)...)))
            : args_copy(std::forward<T>(args)...), func(std::move(f)) {
        }

        TupT args_copy;
        Func func;

        template <class... NewArgs>
        auto operator()(NewArgs&&... new_args) const& {
            return OpImplIface(*this, std::forward<NewArgs>(new_args)...);
        }

        template <class... NewArgs>
        auto operator()(NewArgs&&... new_args) & {
            return OpImplIface(*this, std::forward<NewArgs>(new_args)...);
        }

        template <class... NewArgs>
        auto operator()(NewArgs&&... new_args) && {
            return OpImplIface(std::move(*this), std::forward<NewArgs>(new_args)...);
        }

    private:
        template <class Me, class... NewArgs>
        static auto OpImplIface(Me&& me, NewArgs&&... new_args) {
            return OpImpl(std::forward<Me>(me), std::make_index_sequence<sizeof...(Args)>(),
                          std::forward<NewArgs>(new_args)...);
        }

        template <class Me, size_t... ixs, class... NewArgs>
        static auto OpImpl(Me&& me, std::index_sequence<ixs...>, NewArgs&&... new_args) {
            using FuncForward = CopyCRef_t<From<Forward_t<Me>>, Func>;
            using TupForward = CopyCRef_t<From<Forward_t<Me>>, decltype(me.args_copy)>;

            return static_cast<FuncForward>(me.func)(
                std::get<ixs>(static_cast<TupForward>(me.args_copy))...,
                std::forward<NewArgs>(new_args)...);
        };
    };

    Func func_;
};

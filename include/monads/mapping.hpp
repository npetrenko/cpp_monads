#pragma once

#include <utility>
#include "type_traits.hpp"

template <class Func>
class CurryingFunction {
public:
    CurryingFunction(Func func) : func_(std::move(func)) {
    }

    template <class... Args>
    auto operator()(Args&&... args) {
        return OpImpl(*this, std::forward<Args>(args)...);
    }

    template <class... Args>
    auto operator()(Args&&... args) const {
        return OpImpl(*this, std::forward<Args>(args)...);
    }

private:
    template <class CF, class... Args>
    static auto OpImpl(CF&& cf, Args&&... args) {
        if constexpr (EmitsValue_v<Func, Args...>) {
            return cf.func_(std::forward<Args>(args)...);
        } else {
            auto lambda = [=](auto&&... new_args) {
                return cf.func_(args..., std::forward<decltype(new_args)>(new_args)...);
            };
            return CurryingFunction<decltype(lambda)>(std::move(lambda));
        }
    }

    Func func_;
};

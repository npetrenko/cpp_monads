#pragma once

template <class Derived>
class CRTPDerivedCaster {
public:
    template <class T>
    static constexpr Derived* Cast(T* ptr) {
        return static_cast<Derived*>(ptr);
    }

    template <class T>
    static const Derived* Cast(const T* ptr) {
        return static_cast<const Derived*>(ptr);
    }
};

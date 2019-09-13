#pragma once

#include <utility>
#include <type_traits>

template <template <class...> class Template, class Cls>
struct IsTemplateInstance {
    template <class... Args>
    static std::true_type Tester(Template<Args...>*);
    static std::false_type Tester(...);

    static constexpr bool value = decltype(Tester(std::declval<Cls*>()))::value;
};

template <template <class...> class Template, class Cls>
inline constexpr bool IsTemplateInstance_v = IsTemplateInstance<Template, Cls>::value;

template <class Func, class From>
struct CanBeCalled {
    template <class T, class = decltype(std::declval<T>()(std::declval<From>()))>
    static std::true_type Tester(T&& val);
    static std::false_type Tester(...);

    using type = decltype(Tester(std::declval<Func>()));
    static constexpr bool value = type::value;
};

template <class Func, class From>
inline constexpr bool CanBeCalled_v = CanBeCalled<Func, From>::value;

template <class Func, class From>
using OutputType_t = decltype(std::declval<Func>()(std::declval<From>()));

template <class Func, class... Args>
struct EmitsValue {
    template <class T, class = decltype(std::declval<T>()(std::declval<Args>()...))>
    static auto Tester(T* val) -> decltype((*val)(std::declval<Args>()...));
    static std::false_type Tester(...);

    static constexpr bool value =
        !std::is_same_v<decltype(Tester(std::declval<Func*>())), std::false_type>;
};

template <class Func, class... Args>
inline constexpr bool EmitsValue_v = EmitsValue<Func, Args...>::value;

template <class Cat>
struct CatTraits {};

template <class T>
struct From {};

template <class From, class To>
struct CopyCRef{};

template <class Fr, class To>
struct CopyCRef<From<Fr>, To> {
private:
  using Pure = std::remove_cv_t<std::remove_reference_t<To>>;
  using CQualifiedPure = std::conditional_t<std::is_const_v<std::remove_reference_t<Fr>>, const Pure, Pure>;
  using MaybeWithReference = std::conditional_t<std::is_reference_v<Fr>,
						std::conditional_t<std::is_rvalue_reference_v<Fr>, CQualifiedPure&&, CQualifiedPure&>,
						CQualifiedPure>;
public:
  using type = MaybeWithReference;
};

template <class From, class To>
using CopyCRef_t = typename CopyCRef<From, To>::type;

template <class T>
using Forward_t = decltype(std::forward<T>(std::declval<T>()));

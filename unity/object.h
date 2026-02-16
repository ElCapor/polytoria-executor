#ifndef OBJECT_H
#define OBJECT_H

#include <unity/unity.h>
#include <type_traits>
#include <nasec/meta.h>

template <typename Derived,
          nasec::meta::String ClassName,
          nasec::meta::String Assembly = Unity::AssemblyCSharp,
          nasec::meta::String Namespace = "*",
          nasec::meta::String Parent = "*">
struct Object {
    using is_ptoria_object = void;  // marker for SFINAE
};

// Extract parameters from an Object instantiation
template <typename>
struct ObjectParams;

template <typename Derived,
          nasec::meta::String ClassName,
          nasec::meta::String Assembly,
          nasec::meta::String Namespace,
          nasec::meta::String Parent>
struct ObjectParams<Object<Derived, ClassName, Assembly, Namespace, Parent>> {
    static constexpr auto _class    = ClassName;
    static constexpr auto _assembly  = Assembly;
    static constexpr auto _namespace = Namespace;
    static constexpr auto _parent    = Parent;
};

namespace detail {
    // Overload that matches the unique Object<T, ...> base of T
    template <typename T,
              nasec::meta::String ClassName,
              nasec::meta::String Assembly,
              nasec::meta::String Namespace,
              nasec::meta::String Parent>
    static ObjectParams<Object<T, ClassName, Assembly, Namespace, Parent>>
    Test(Object<T, ClassName, Assembly, Namespace, Parent>*);

    // Fallback
    static void Test(...);

    template <typename T>
    struct GetObjectParams {
        using type = decltype(Test((T*)nullptr));
        static_assert(!std::is_same_v<type, void>,
                      "Type T must have a base of type Object<T, ...> (direct or indirect)");
    };
}

template <typename T>
using ObjectParamsT = typename detail::GetObjectParams<T>::type;

template <typename T>
auto StaticClass() -> UnityClass* {
    using Params = ObjectParamsT<T>;
    return Unity::GetClass<Params::_class,
                           Params::_assembly,
                           Params::_namespace,
                           Params::_parent>();
}

namespace nasec {
    namespace meta {
        template <typename T, typename = void>
        struct is_object : std::false_type {};

        template <typename T>
        struct is_object<T, std::void_t<typename T::is_ptoria_object>> : std::true_type {};
    }
}

#endif /* OBJECT_H */

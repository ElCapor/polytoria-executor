#ifndef U_HPP
#define U_HPP

#include <unity/unity.hpp>

#define ASSEMBLY_CSHARP "Assembly-CSharp.dll"
#define ASSEMBLY_CSHARP_FIRSTPASS "Assembly-CSharp-firstpass.dll"
#define ASSEMBLY_MIRROR "Mirror.dll"

using U = UnityResolve;

template <class T> using UArray = UnityResolve::UnityType::Array<T>;
template <typename TKey, typename TValue> using UDictionary = UnityResolve::UnityType::Dictionary<TKey, TValue>;

using UK = UnityResolve::Class;
using US = UnityResolve::UnityType::String;
using UO = UnityResolve::UnityType::Object;
using UF = UnityResolve::Field;
using UM = UnityResolve::Method;
using UC = UnityResolve::UnityType::Component;
using UG = UnityResolve::UnityType::GameObject;

#endif /* U */

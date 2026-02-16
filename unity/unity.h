#ifndef UNITY_H
#define UNITY_H

#include <unity/unity_internal.h>
#include <nasec/assert.h>
#include <nasec/meta.h>

using UnityAssembly = UnityResolve::Assembly;
using UnityClass = UnityResolve::Class;
using UnityField = UnityResolve::Field;
using UnityMethod = UnityResolve::Method;
using UnityType = UnityResolve::UnityType;
using UnityString = UnityType::String;
using UnityObject = UnityType::Object;
using UnityGameObject = UnityType::GameObject;
using UnityComponent = UnityType::Component;
using UnityCursor = UnityType::Cursor;
using UnityCursorLockMode = UnityType::CursorLockMode;

template <typename T>
using UnityArray = UnityType::Array<T>;

template <typename T>
using UnityList = UnityType::List<T>;

template <typename TKey, typename TValue>
using UnityDictionary = UnityType::Dictionary<TKey, TValue>;

namespace Unity
{
    static constexpr nasec::meta::String AssemblyCSharp = "Assembly-CSharp.dll";
    static constexpr nasec::meta::String AssemblyFirstPass = "Assembly-CSharp-firstpass.dll";
    static constexpr nasec::meta::String AssemblyUnityEngine = "UnityEngine.CoreModule.dll";
    static constexpr nasec::meta::String Mirror = "Mirror.dll";

    void Init();
    void ThreadAttach();
    void ThreadDetach();

    /**
     * @brief Fast retrieval of assembly by name using compile-time string
     */
    template <nasec::meta::String Name>
    UnityAssembly *GetAssembly()
    {
        static UnityAssembly *assembly = nullptr;
        if (!assembly)
            assembly = UnityResolve::Get(Name.data);

        nasec::Assert(assembly != nullptr, "Failed to get assembly");
        return assembly;
    }

    /**
     * @brief Lightweight retrieval of class by name, namespace and parent using compile-time string
     * 
     * @tparam Name 
     * @tparam Assembly 
     * @tparam Namespace 
     * @tparam Parent 
     * @return UnityClass* 
     */
    template <nasec::meta::String Name, nasec::meta::String Assembly, nasec::meta::String Namespace = "*", nasec::meta::String Parent = "*">
    UnityClass *GetClass()
    {
        static UnityClass *klass = nullptr;
        if (!klass)
        {
            klass = GetAssembly<Assembly>()->Get(Name.data, Namespace.data, Parent.data);
        }

        nasec::Assert(klass != nullptr, "Failed to get class");
        return klass;
    }

    template <nasec::meta::String Name, nasec::meta::String... Types>
    auto GetMethod(UnityClass* klass) -> UnityMethod* {
        nasec::Assert(klass != nullptr, "Klass was nullptr");
        std::vector<std::string> types{ Types.data... };

        static UnityMethod* method = nullptr;
        if (!method) method = klass->Get<UnityMethod>(Name.data, types);
        nasec::Assert(method != nullptr, "Failed to get method");
        return method;
    }

    template <nasec::meta::String Name, nasec::meta::String... Types>
    auto GetField(UnityClass* klass) -> UnityField* {
        nasec::Assert(klass != nullptr, "Klass was nullptr");
        std::vector<std::string> types{ Types.data... };

        static UnityField* field = nullptr;
        if (!field) field = klass->Get<UnityField>(Name.data, types);
        return field;
    }

    template <typename T, nasec::meta::String Name, nasec::meta::String... Types>
    auto GetFieldValue(UnityClass* klass, void* obj) -> T {
        nasec::Assert(klass != nullptr, "Klass was nullptr");

        auto field = GetField<Name, Types...>(klass);
        nasec::Assert(obj != nullptr, "Object was nullptr");
        return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + field->offset); 
    }

    template <typename T, nasec::meta::String Name, nasec::meta::String... Types>
    auto SetFieldValue(UnityClass* klass, void* obj, T value) -> void {
        nasec::Assert(klass != nullptr, "Klass was nullptr");

        auto field = GetField<Name, Types...>(klass);
        nasec::Assert(obj != nullptr, "Object was nullptr");
        *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + field->offset) = value;
    }

    template <typename T, nasec::meta::String Name, nasec::meta::String... Types>
    auto GetStaticFieldValue(UnityClass* klass) -> T {
        static T value;

        nasec::Assert(klass != nullptr, "Klass was nullptr");
        auto field = GetField<Name, Types...>(klass);
        field->GetStaticValue(&value);
        // sometimes nullptr is a valid value, so we can't assert on it
        //nasec::Assert(value != nullptr, "Failed to get static field value");

        return value;
    }

    template <typename T, nasec::meta::String Name, nasec::meta::String... Types>
    auto SetStaticFieldValue(UnityClass* klass, T* value) -> void {
        nasec::Assert(klass != nullptr, "Klass was nullptr");
        auto field = GetField<Name, Types...>(klass);
        field->SetStaticValue(value);
    }


    template <typename T>
    T* Cast(void* obj) {
        return reinterpret_cast<T*>(obj);
    }

    template <typename T>
    UnityObject* CastToUnityObject(T* obj) {
        return reinterpret_cast<UnityObject*>(obj);
    }

}

#endif /* UNITY_H */

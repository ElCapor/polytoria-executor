#ifndef SINGLETON
#define SINGLETON

#include <unity/object.h>
#include <nasec/assert.h>

// singleton for object contraineed to the Object concept
template <typename T> 
struct Singleton
{
    static auto GetSingleton() -> T* 
    {
        static_assert(nasec::meta::is_object<T>::value, "T must be a Ptoria Object (derive from Object or Instance)");
        static T* instance = nullptr;
        if (!instance)
        {
            auto uf = StaticClass<T>()->Get<UnityField>("singleton");
            nasec::Assert(uf != nullptr, "Failed to get singleton field");
            uf->GetStaticValue(&instance);
            nasec::Assert(instance != nullptr, "Failed to get singleton instance");
        }
        return instance;
    };
};

#endif /* SINGLETON */

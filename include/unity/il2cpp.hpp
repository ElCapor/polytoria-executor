// Written by Nasec(@mrnasec) on 10/02/2026

/*  What's this?

	Basically a Helper header for exclusively Il2cpp
	It's capable of interacting with internal Il2cpp features, unlike mono/unity features inside UnityResolver
	
	One of the main jobs is to construct custom MethodInfo's that are linked with our C Side functions
	Which hasn't been done open source anywhere before! So are we the first ones? 0.o?

*/

#pragma once

#include <cstdint>
#include <vector>

namespace il2cpp
{
    typedef struct MethodInfo;
    typedef struct Il2CppReflectionMethod;
        
    // Use UnityResolver for those types as they are a part of both mono and il2cpp
    typedef void* Il2CppClass; 
    typedef void* Il2CppType;
    typedef void* Il2CppGenericContext;
    typedef void (*Il2CppMethodPointer)();
    typedef void (*InvokerMethod)(Il2CppMethodPointer, const MethodInfo*, void*, void**, void*);

    typedef union Il2CppRGCTXData
    {
        void* rgctxDataDummy;
        const MethodInfo* method;
        const Il2CppType* type;
        Il2CppClass* klass;
    } Il2CppRGCTXData;

    typedef struct Il2CppGenericMethod
    {
        const MethodInfo* methodDefinition;
        Il2CppGenericContext context;
    } Il2CppGenericMethod;

    typedef struct MethodInfo
    {
        Il2CppMethodPointer methodPointer;
        Il2CppMethodPointer virtualMethodPointer;
        InvokerMethod invoker_method;
        const char* name;
        Il2CppClass* klass;
        const Il2CppType* return_type;
        const Il2CppType** parameters;

        union
        {
            const Il2CppRGCTXData* rgctx_data; /* is_inflated is true and is_generic is false, i.e. a generic instance method */
            void* methodMetadataHandle;
        };

        /* note, when is_generic == true and is_inflated == true the method represents an uninflated generic method on an inflated type. */
        union
        {
            const Il2CppGenericMethod* genericMethod; /* is_inflated is true */
            void* genericContainerHandle; /* is_inflated is false and is_generic is true */
        };

        uint32_t token;
        uint16_t flags;
        uint16_t iflags;
        uint16_t slot;
        uint8_t parameters_count;
        uint8_t is_generic : 1; /* true if method is a generic method definition */
        uint8_t is_inflated : 1; /* true if declaring_type is a generic instance or if method is a generic instance*/
        uint8_t wrapper_type : 1; /* always zero (MONO_WRAPPER_NONE) needed for the debugger */
        uint8_t has_full_generic_sharing_signature : 1;
        uint8_t is_unmanaged_callers_only : 1;
    } MethodInfo;

    typedef struct Il2CppObject
    {
        union
        {
            Il2CppClass* klass;
            void** vtable;
        };
        MonitorData* monitor;
    } Il2CppObject;

    typedef struct Il2CppDelegate {
        Il2CppObject object;
        /* The compiled code of the target method */
        Il2CppMethodPointer method_ptr;
        /* The invoke code */
        Il2CppMethodPointer invoke_impl;
        Il2CppObject* target;
        const MethodInfo* method;

        // This is used in PlatformInvoke.cpp to store the native function pointer
        // IMPORTANT: It is assumed to NULL otherwise!  See PlatformInvoke::IsFakeDelegateMethodMarshaledFromNativeCode
        void* delegate_trampoline;

        // Used to store the mulicast_invoke_impl
        intptr_t extraArg;

        /* MONO:
         * If non-NULL, this points to a memory location which stores the address of
         * the compiled code of the method, or NULL if it is not yet compiled.
         * uint8_t **method_code;
         */
         // IL2CPP: Points to the "this" method pointer we use when calling invoke_impl
         // For closed delegates invoke_impl_this points to target and invoke_impl is method pointer so we just do a single indirect call
         // For all other delegates invoke_impl_this is points to it's owning delegate an invoke_impl is a delegate invoke stub
         // NOTE: This field is NOT VISIBLE to the GC because its not a managed field in the classlibs
         //       Our usages are safe becuase we either pointer to ourself or whats stored in the target field
        Il2CppObject* invoke_impl_this;

        void* interp_method;
        /* Interp method that is executed when invoking the delegate */
        void* interp_invoke_impl;
        Il2CppReflectionMethod* method_info;
        Il2CppReflectionMethod* original_method_info;
        Il2CppObject* data;

        bool method_is_virtual;
    };

    #define METHOD_IMPL_ATTRIBUTE_CODE_TYPE_MASK       0x0003
    #define METHOD_IMPL_ATTRIBUTE_IL                   0x0000
    #define METHOD_IMPL_ATTRIBUTE_NATIVE               0x0001
    #define METHOD_IMPL_ATTRIBUTE_OPTIL                0x0002
    #define METHOD_IMPL_ATTRIBUTE_RUNTIME              0x0003

    #define METHOD_IMPL_ATTRIBUTE_FORWARD_REF          0x0010
    #define METHOD_IMPL_ATTRIBUTE_PRESERVE_SIG         0x0080
    #define METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL        0x1000
    #define METHOD_IMPL_ATTRIBUTE_SYNCHRONIZED         0x0020
    #define METHOD_IMPL_ATTRIBUTE_NOINLINING           0x0008
    #define METHOD_IMPL_ATTRIBUTE_MAX_METHOD_IMPL_VAL  0xffff
    
    #define METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK        0x0007
    #define METHOD_ATTRIBUTE_COMPILER_CONTROLLED       0x0000
    #define METHOD_ATTRIBUTE_PRIVATE                   0x0001
    #define METHOD_ATTRIBUTE_FAM_AND_ASSEM             0x0002
    #define METHOD_ATTRIBUTE_ASSEM                     0x0003
    #define METHOD_ATTRIBUTE_FAMILY                    0x0004
    #define METHOD_ATTRIBUTE_FAM_OR_ASSEM              0x0005
    #define METHOD_ATTRIBUTE_PUBLIC                    0x0006
    
    #define METHOD_ATTRIBUTE_STATIC                    0x0010
    #define METHOD_ATTRIBUTE_FINAL                     0x0020
    #define METHOD_ATTRIBUTE_VIRTUAL                   0x0040
    #define METHOD_ATTRIBUTE_HIDE_BY_SIG               0x0080

    static MethodInfo* CreateCustomMethodInfo(const char* name, std::vector<Il2CppType*> params, Il2CppType* ret, Il2CppMethodPointer ptr)
    {
		MethodInfo* methodInfo = (MethodInfo*)calloc(1, sizeof(MethodInfo)); // Il2cpp uses their own memory pool, which we dont.

        methodInfo->klass = nullptr; // Since our function isn't a part of a class, we can set this to null
        methodInfo->flags = METHOD_ATTRIBUTE_PUBLIC | METHOD_ATTRIBUTE_STATIC;
		methodInfo->iflags = METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL | METHOD_IMPL_ATTRIBUTE_RUNTIME;
        methodInfo->name = name;
        methodInfo->slot = -1;
        methodInfo->return_type = ret;
		methodInfo->parameters_count = params.size();
        methodInfo->parameters = (const Il2CppType**)calloc(params.size(), sizeof(Il2CppType*));
        for (size_t i = 0; i < params.size(); i++)
        {
            methodInfo->parameters[i] = params[i];
        }

		methodInfo->methodPointer = ptr;
		methodInfo->virtualMethodPointer = ptr;

		return methodInfo;
    }

    __int64 __fastcall multicast_invoke_impl_2_params(__int64 a1, __int64 a2, __int64 a3)
    {
        unsigned __int64 v3; // rbx
        __int64 v6; // r14
        unsigned __int64 v7; // rdi
        __int64 result; // rax

        v3 = 0;
        v6 = *(uintptr_t*)(a1 + 120);
        v7 = *(uintptr_t*)(v6 + 24);
        if (!v7)
            return 0;
        do
        {
            result = (*(__int64(__fastcall**)(uintptr_t, __int64, __int64, uintptr_t))(*(uintptr_t*)(v6 + 8 * v3 + 32) + 24LL))(
                *(uintptr_t*)(*(uintptr_t*)(v6 + 8 * v3 + 32) + 64LL),
                a2,
                a3,
                *(uintptr_t*)(*(uintptr_t*)(v6 + 8 * v3 + 32) + 40LL));
            ++v3;
        } while (v3 < v7);
        return result;
    }

    static void BindDelegate(Il2CppDelegate* delegateFunc, MethodInfo* methodInfo) 
    {
        delegateFunc->method_ptr = methodInfo->virtualMethodPointer;
        delegateFunc->method = methodInfo;
        delegateFunc->target = nullptr;
        delegateFunc->invoke_impl_this = nullptr;
        delegateFunc->invoke_impl = delegateFunc->method_ptr;

        // Sadly we are forced to hardocode the multicast_invoke_impl of every parameter count
        if (methodInfo->parameters_count == 2)
        {
            delegateFunc->extraArg = (intptr_t)multicast_invoke_impl_2_params;
        }
        else
        {
            // add assert!
        }
    }
}
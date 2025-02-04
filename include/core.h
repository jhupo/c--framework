#ifndef __FRAMEWORK_CORE_H__
#define __FRAMEWORK_CORE_H__

#include <memory>
#include <string>
#include <iostream>

#if defined(FRANEWORK_LIBRARY)
    #if defined(_WIN32)
        #if defined(framework_EXPORTS)
            #define FRAMEWORK_EXPORT __declspec(dllexport)
        #else
            #define FRAMEWORK_EXPORT __declspec(dllimport)
        #endif
    #else
        #define FRAMEWORK_EXPORT __attribute__((visibility("default")))
    #endif
#else
    #define FRAMEWORK_EXPORT
#endif

#if defined(__GNUC__)
    #if __GNUC__ < 5
        #define FRAMEWORK_NOEXCEPT _NOEXCEPT
        #define FRAMEWORK_CONSTEXPR
    #else
        #define FRAMEWORK_NOEXCEPT noexcept
        #define FRAMEWORK_CONSTEXPR constexpr
    #endif
#elif defined(_MSC_VER) && (_MSC_VER < 1900)
    #define FRAMEWORK_NOEXCEPT _NOEXCEPT
    #define FRAMEWORK_CONSTEXPR
#else
    #define FRAMEWORK_NOEXCEPT noexcept
    #define FRAMEWORK_CONSTEXPR constexpr
#endif

#ifndef EXTERN_C
    #ifdef __cplusplus
        #define EXTERN_C extern "C"
    #else
        #define EXTERN_C extern
    #endif
#endif

#define FRAMEWORK_BEGIN_NAMESPACE namespace fw {
#define FRAMEWORK_END_NAMESPACE }

#define UNUSED(x) (void)(x)

#define DISABLE_COPY(Class)        \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

template <typename Class>
class SingletonPtr {
public:
    static std::shared_ptr<Class> inst() {
        static std::shared_ptr<Class> instance = std::make_shared<Class>();
        return instance;
    }
};

#endif
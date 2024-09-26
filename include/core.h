#ifndef __FRAMEWORK_CORE_H__
#define __FRAMEWORK_CORE_H__

#include <iostream>

#if defined(__GNUC__) 
#   if __GNUC__ >= 4
#       define DECL_EXPORT __attribute__ ((visibility ("default")))
#       define DECL_IMPORT __attribute__ ((visibility ("default")))
#   else
#       define DECL_EXPORT
#       define DECL_IMPORT
#   endif
#elif defined(_MSC_VER)
#   define DECL_EXPORT __declspec(dllexport)
#   define DECL_IMPORT __declspec(dllimport)
#else 
#   define DECL_EXPORT 
#   define DECL_IMPORT 
#endif


#if defined(__GNUC__) 
#   if __GNUC__ < 5
#       define FRAMEWORK_NOEXCEPT _NOEXCEPT
#       define FRAMEWORK_CONSTEXPR
#   else
#       define FRAMEWORK_NOEXCEPT noexcept
#       define FRAMEWORK_CONSTEXPR constexpr
#   endif
#elif defined(_MSC_VER) && (_MSC_VER < 1900)
#   define FRAMEWORK_NOEXCEPT _NOEXCEPT
#   define FRAMEWORK_CONSTEXPR 
#else 
#   define FRAMEWORK_NOEXCEPT noexcept
#   define FRAMEWORK_CONSTEXPR constexpr
#endif

#ifndef EXTERN_C
#  ifdef __cplusplus
#    define EXTERN_C extern "C"
#  else
#    define EXTERN_C extern
#  endif
#endif

#define BEGIN_NAMESPACE(x) namespace x {
#define END_NAMESPACE(x) }
#define USING_NAMESPACE(x) using namespace x;

#define UNUSED(x) (void)(x)

#define DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;

#define DECLARE_SHARED_PTR(Class) \
    public: typedef std::shared_ptr<Class> ptr; private:

#define DECLARE_CLASS_DETAIL(Class) \
    class Impl; Impl* d; \
    public: virtual ~Class() { delete d; } private:

#endif
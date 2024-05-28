#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#include <iostream>

#if defined _WIN32 || defined __CYGWIN__
#   define DECL_EXPORT __declspec(dllexport)
#   define DECL_IMPORT __declspec(dllimport)
#   define DECL_HIDE
#elif defined(__linux__) || defined(__unix__)
#   if __GNUC__ >= 4
#       define DECL_EXPORT __attribute__ ((visibility ("default")))
#       define DECL_IMPORT __attribute__ ((visibility ("default")))
#       define DECL_HIDE  __attribute__ ((visibility ("hidden")))
#   else
#       define DECL_EXPORT
#       define DECL_IMPORT
#       define DECL_HIDE
#   endif
#else
#error Platforms to be supported
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
    public:typedef std::shared_ptr<Class> ptr;private:

#endif

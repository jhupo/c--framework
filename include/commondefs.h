#ifndef __COMMONDEFS_H__
#define __COMMONDEFS_H__

#include <iostream>
#include <memory>

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


#if defined(FRAMEWORK_LIBRARY)
#  define FRAMEWORK_EXPORT DECL_EXPORT
#elif defined(FRAMEWORK_STATIC_LIBRARY)
#  define FRAMEWORK_EXPORT
#else
#  define FRAMEWORK_EXPORT DECL_IMPORT
#endif


#define UNUSED(x) (void)(x)

#define DISABLE_COPY(Class) \
    Class(const Class &) = delete;\
    Class &operator=(const Class &) = delete;

template <typename T> static inline T *GetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::element_type* GetPtrHelper(const Wrapper &p) { return p.get(); }

#define DECLARE_PRIVATE(Class) \
    inline Class##Private* data_func() { return reinterpret_cast<Class##Private *>(GetPtrHelper(d_ptr)); } \
    inline const Class##Private* data_func() const { return reinterpret_cast<const Class##Private *>(GetPtrHelper(d_ptr)); }

#define DECLARE_PUBLIC(Class)                                    \
    inline Class* self_func() { return static_cast<Class *>(s_ptr); } \
    inline const Class* self_func() const { return static_cast<const Class *>(s_ptr); }

#define DATA_PTR(Class) Class##Private * const d = data_func()
#define SELF_PTR(Class) Class * const s = self_func()

#define DECLARE_SHARED_PTR(Class)   \
    public: \
        typedef boost::shared_ptr<Class> ptr; \
    private:



#endif
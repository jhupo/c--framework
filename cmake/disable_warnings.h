#ifndef __COMMON_DISABLE_WARNINGS_H__
#define __COMMON_DISABLE_WARNINGS_H__

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 4251)
#   pragma warning(disable: 4275)
#   pragma warning(disable: 4503)
#   pragma warning(disable: 4996)
#   pragma warning(disable: 4800)
#   pragma warning(disable: 4267)
#   pragma warning(disable: 4244)
#   pragma warning(disable: 4838)
#   pragma warning(disable: 4018)
#else if defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wattributes"
#   pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#   pragma GCC diagnostic ignored "-Wreorder"
#   pragma GCC diagnostic ignored "-Wreturn-type"
#endif


#endif
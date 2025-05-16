#pragma once

#include <pxr/base/arch/export.h>
#include <pxr/pxr.h>

// clang-format off
#if defined _WIN32 || defined __CYGWIN__

  #ifdef TINKERUSD_EXPORT
    #ifdef __GNUC__
      #define TINKERUSD_PUBLIC __attribute__ ((dllexport))
    #else
      #define TINKERUSD_PUBLIC __declspec(dllexport)
    #endif
  #else
    #ifdef __GNUC__
      #define TINKERUSD_PUBLIC __attribute__ ((dllimport))
    #else
      #define TINKERUSD_PUBLIC __declspec(dllimport)
    #endif
  #endif
  #define TINKERUSD_LOCAL
#else
  #if __GNUC__ >= 4
    #define TINKERUSD_PUBLIC __attribute__ ((visibility ("default")))
    #define TINKERUSD_LOCAL  __attribute__ ((visibility ("hidden")))
#else
    #define TINKERUSD_PUBLIC
    #define TINKERUSD_LOCAL
#endif
#endif

#ifdef TINKERUSD_EXPORT
  #define TINKERUSD_TEMPLATE_CLASS(...) ARCH_EXPORT_TEMPLATE(class, __VA_ARGS__)
  #define TINKERUSD_TEMPLATE_STRUCT(...) ARCH_EXPORT_TEMPLATE(struct, __VA_ARGS__)
#else
  #define TINKERUSD_TEMPLATE_CLASS(...) ARCH_IMPORT_TEMPLATE(class, __VA_ARGS__)
  #define TINKERUSD_TEMPLATE_STRUCT(...) ARCH_IMPORT_TEMPLATE(struct, __VA_ARGS__)
#endif
// clang-format on
// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Config.h included before 'Defines' folder.
*/

#pragma once

#ifdef AE_COMPILER_MSVC
#  if _MSC_VER < 1900   // TODO
#   define and      &&
#   define or       ||
#   define not      !
#  endif
#endif

// mem leak check
#if defined(AE_COMPILER_MSVC) and defined(AE_ENABLE_MEMLEAK_CHECKS) and defined(AE_CFG_DEBUG)
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#   include <stdlib.h>
#   include <malloc.h>

    // call at exit
    // returns 'true' if no mem leaks
#   define AE_DUMP_MEMLEAKS()   (::_CrtDumpMemoryLeaks() != 1)
#else

#   undef  AE_ENABLE_MEMLEAK_CHECKS
#   define AE_DUMP_MEMLEAKS()   (true)
#endif

#if defined(AE_CFG_DEBUG) or defined(AE_CFG_DEVELOP) or defined(AE_CFG_PROFILE)
#   define AE_DBG_OR_DEV_OR_PROF
#endif

// Config
#ifndef AE_FAST_HASH
#   define AE_FAST_HASH     0
#endif

#ifndef AE_OPTIMIZE_IDS
# ifdef AE_DEBUG
#   define AE_OPTIMIZE_IDS  0
# else
#   define AE_OPTIMIZE_IDS  1
# endif
#endif


#if defined(AE_PLATFORM_LINUX)      or \
    defined(AE_PLATFORM_ANDROID)    or \
    defined(AE_PLATFORM_MACOS)      or \
    defined(AE_PLATFORM_IOS)        or \
    defined(AE_PLATFORM_EMSCRIPTEN)
#   define AE_PLATFORM_UNIX_BASED
#endif

#if defined(AE_PLATFORM_MACOS)      or \
    defined(AE_PLATFORM_IOS)
#   define AE_PLATFORM_APPLE
#endif

#if defined(AE_CPU_ARCH_ARM64)      or \
    defined(AE_CPU_ARCH_ARM32)
#   define AE_CPU_ARCH_ARM_BASED
#endif


#ifdef AE_COMPILER_MSVC
#   define AE_COMPILER_NAME         "MS Visual Studio"
#   define AE_COMPILER_VERSION      AE::Base::Version3{ _MSC_FULL_VER / 10000000, (_MSC_FULL_VER % 10000000) / 100000, _MSC_FULL_VER % 100000 } // (4) _MSC_BUILD 
                                    // 2017: 19.10 - 19.16
                                    // 2019: 19.20 - 19.29
                                    // 2022: 19.30 - ...
#endif
#ifdef AE_COMPILER_GCC
#   define AE_COMPILER_NAME         "GCC"
#   define AE_COMPILER_VERSION      AE::Base::Version3{ __GNUC_MINOR__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__ }
#endif
#ifdef AE_COMPILER_CLANG
#   define AE_COMPILER_NAME         "Clang"
#   define AE_COMPILER_VERSION      AE::Base::Version3{ __clang_major__, __clang_minor__, __clang_patchlevel__ }
#endif


#ifdef __INTELLISENSE__
#  define AE_PLATFORM_ANDROID
#endif


// tabs or spaces
#define AE_PRIVATE_USE_TABS         0

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Config.h included before 'Defines' folder.
*/

#pragma once

#ifdef AE_COMPILER_MSVC
#  if _MSC_VER < 1900	// TODO
#	define and		&&
#	define or		||
#	define not		!
#  endif
#endif

// mem leak check
#if defined(AE_COMPILER_MSVC) and defined(AE_ENABLE_MEMLEAK_CHECKS) and defined(AE_DEBUG)
#	define _CRTDBG_MAP_ALLOC
#	include <crtdbg.h>
#	include <stdlib.h>
#	include <malloc.h>

	// call at exit
	// returns 'true' if no mem leaks
#	define AE_DUMP_MEMLEAKS()	(::_CrtDumpMemoryLeaks() != 1)
#else

#	undef  AE_ENABLE_MEMLEAK_CHECKS
#	define AE_DUMP_MEMLEAKS()	(true)
#endif

// Config
#ifndef AE_FAST_HASH
#	define AE_FAST_HASH		0
#endif

#if defined(AE_DEBUG) or defined(AE_DEVELOP)
#	define AE_DBG_OR_DEV
#endif

#if defined(AE_DEBUG) or defined(AE_DEVELOP) or defined(AE_PROFILE)
#	define AE_DBG_OR_DEV_OR_PROF
#endif

#ifndef AE_OPTIMIZE_IDS
# ifdef AE_DBG_OR_DEV
#	define AE_OPTIMIZE_IDS	0
# else
#	define AE_OPTIMIZE_IDS	1
# endif
#endif


#if defined(AE_PLATFORM_LINUX)		or \
	defined(AE_PLATFORM_ANDROID)	or \
	defined(AE_PLATFORM_MACOS)		or \
	defined(AE_PLATFORM_IOS)
#	define AE_PLATFORM_UNIX_BASED
#endif

#if defined(AE_PLATFORM_MACOS)	or \
	defined(AE_PLATFORM_IOS)
#	define AE_PLATFORM_APPLE
#endif

#if defined(AE_CPU_ARCH_ARM64)	or \
	defined(AE_CPU_ARCH_ARM32)
#	define AE_CPU_ARCH_ARM_BASED
#endif

#ifdef __INTELLISENSE__
#  define AE_PLATFORM_ANDROID
#endif

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


#if defined(AE_DEBUG)
#  if defined(AE_DEVELOP) or defined(AE_PROFILE) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_DEVELOP)
#  if defined(AE_DEBUG) or defined(AE_PROFILE) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_PROFILE)
#  if defined(AE_DEBUG) or defined(AE_DEVELOP) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_RELEASE)
#  if defined(AE_DEBUG) or defined(AE_DEVELOP) or defined(AE_PROFILE)
#	error only one configuration must be enabled!
#  endif
#else
#	error unknown configuration!
#endif

#if defined(AE_PLATFORM_LINUX)		or \
	defined(AE_PLATFORM_ANDROID)	or \
	defined(AE_PLATFORM_MACOS)		or \
	defined(AE_PLATFORM_IOS)		or \
	defined(AE_PLATFORM_WINDOWS)	or \
	defined(AE_PLATFORM_EMSCRIPTEN)
#else
#	error unknown platform!
#endif

#if defined(AE_COMPILER_MSVC)	or \
	defined(AE_COMPILER_GCC)	or \
	defined(AE_COMPILER_CLANG)
#else
#	error unknown compiler!
#endif

#if defined(AE_CPU_ARCH_ARM64)	or \
	defined(AE_CPU_ARCH_ARM32)	or \
	defined(AE_CPU_ARCH_X64)	or \
	defined(AE_CPU_ARCH_X86)	or \
	defined(AE_CPU_ARCH_i686)
#else
#	error unknown CPU architecture!
#endif

#ifdef AE_PLATFORM_WINDOWS
#	if !defined(AE_WINDOWS_TARGET_VERSION)
#		error AE_WINDOWS_TARGET_VERSION must be defined!
#	else
#	 if !((AE_WINDOWS_TARGET_VERSION >= 6) && (AE_WINDOWS_TARGET_VERSION <= 10))
#		error AE_WINDOWS_TARGET_VERSION must be in range [6..10]
#	 endif
#	endif
#endif


// check features
#ifndef __cpp_threadsafe_static_init
#	error '__cpp_threadsafe_static_init' feature required
#endif


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_DEBUG
#	pragma detect_mismatch( "AE_DEBUG", "1" )
#  else
#	pragma detect_mismatch( "AE_DEBUG", "0" )
#  endif

#  ifdef AE_DEVELOP
#	pragma detect_mismatch( "AE_DEVELOP", "1" )
#  else
#	pragma detect_mismatch( "AE_DEVELOP", "0" )
#  endif

#  ifdef AE_PROFILE
#	pragma detect_mismatch( "AE_PROFILE", "1" )
#  else
#	pragma detect_mismatch( "AE_PROFILE", "0" )
#  endif

#  ifdef AE_RELEASE
#	pragma detect_mismatch( "AE_RELEASE", "1" )
#  else
#	pragma detect_mismatch( "AE_RELEASE", "0" )
#  endif

#  if defined(AE_FAST_HASH) && AE_FAST_HASH
#	pragma detect_mismatch( "AE_FAST_HASH", "1" )
#  else
#	pragma detect_mismatch( "AE_FAST_HASH", "0" )
#  endif

#  ifdef AE_CI_BUILD
#	pragma detect_mismatch( "AE_CI_BUILD", "1" )
#  else
#	pragma detect_mismatch( "AE_CI_BUILD", "0" )
#  endif

#  ifdef AE_ENABLE_MEMLEAK_CHECKS
#	pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "0" )
#  endif

#  if defined(AE_NO_EXCEPTIONS) && (AE_NO_EXCEPTIONS == 1)
#	pragma detect_mismatch( "AE_NO_EXCEPTIONS", "1" )
#  else
#	pragma detect_mismatch( "AE_NO_EXCEPTIONS", "0" )
#  endif


// platforms
#  ifdef AE_PLATFORM_WINDOWS
#	pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "0" )
#  endif

#  ifdef AE_PLATFORM_LINUX
#	pragma detect_mismatch( "AE_PLATFORM_LINUX", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_LINUX", "0" )
#  endif

#  ifdef AE_PLATFORM_ANDROID
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "0" )
#  endif

#  ifdef AE_PLATFORM_MACOS
#	pragma detect_mismatch( "AE_PLATFORM_MACOS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_MACOS", "0" )
#  endif

#  ifdef AE_PLATFORM_IOS
#	pragma detect_mismatch( "AE_PLATFORM_IOS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_IOS", "0" )
#  endif

#  ifdef AE_PLATFORM_EMSCRIPTEN
#	pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "0" )
#  endif

#  ifdef AE_PLATFORM_WINDOWS
#	if AE_WINDOWS_TARGET_VERSION == 10
#	 pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "10" )	// Win10
#	elif AE_WINDOWS_TARGET_VERSION == 8
#	 pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "8" )		// Win8
#	elif AE_WINDOWS_TARGET_VERSION == 7
#	 pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "7" )		// Win7
#	elif AE_WINDOWS_TARGET_VERSION == 6
#	 pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "6" )		// Vista
#	else
#	 pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "0" )
#	endif
#  else
#	pragma detect_mismatch( "AE_WINDOWS_TARGET_VERSION", "0" )
#  endif


// compilers
#  ifdef AE_COMPILER_MSVC
#	pragma detect_mismatch( "AE_COMPILER_MSVC", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_MSVC", "0" )
#  endif

#  ifdef AE_COMPILER_CLANG
#	pragma detect_mismatch( "AE_COMPILER_CLANG", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_CLANG", "0" )
#  endif

#  ifdef AE_COMPILER_GCC
#	pragma detect_mismatch( "AE_COMPILER_GCC", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_GCC", "0" )
#  endif


// architecture
#  ifdef AE_CPU_ARCH_ARM64
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_ARM32
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X86
#	pragma detect_mismatch( "AE_CPU_ARCH_X86", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_X86", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X64
#	pragma detect_mismatch( "AE_CPU_ARCH_X64", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_X64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_i686
#	pragma detect_mismatch( "AE_CPU_ARCH_i686", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_i686", "0" )
#  endif


#endif // AE_CPP_DETECT_MISMATCH


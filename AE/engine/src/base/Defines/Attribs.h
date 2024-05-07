// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// mark output and input-output function arguments //

// Previous content will be discarded.
// On error: returns empty object.
// On success: returns new object.
//
// OUT for reference - argument will be replaced by the new object.
// OUT for pointer   - memory pointed to will be overwritten.
//
#ifndef OUT
#	define OUT
#endif

// Previous content will be modified or added new content.
// On error: fallback to previous content is implementation defined, content may be partially modified.
//
// INOUT for reference - argument has read-write access.
// INOUT for pointer   - memory pointed to has read-write access.
//
#ifndef INOUT
#	define INOUT
#endif


// function suffix attribs
#define C_NE_OF		const	noexcept		override final
#define C_NE_OV		const	noexcept		override
#define __NE_OV				noexcept		override
#define __NE_OF				noexcept		override final
#define C_NE___		const	noexcept
#define M_NE___		mutable	noexcept
#define CrNE___		const&	noexcept
#define r_NE___		&		noexcept
#define rvNE___		&&		noexcept
#define __NE___				noexcept
#define CrTh___		const&	noexcept(false)
#define r_Th___		&		noexcept(false)
#define rvTh___		&&		noexcept(false)
#define __Th___				noexcept(false)
#define C_Th___		const	noexcept(false)
#define M_Th___		mutable	noexcept(false)
#define C_Th_OV		const	noexcept(false)	override
#define __Th_OV				noexcept(false)	override
#define C_Th_OF		const	noexcept(false)	override final
#define __Th_OF				noexcept(false)	override final


// function prefix attribs
/*
#define __Cx__				constexpr
#define	__Cx__				constexpr
#define St____		static
#define StCx__		static	constexpr
#define Fr____		friend
#define FrCx__		friend	constexpr
#define	St__In		static					inline
#define	St__FI		static					forceinline
#define __Cv__				consteval
*/

// no discard
#ifndef ND_
# ifdef AE_COMPILER_MSVC
#  if _MSC_VER >= 1917
#	define ND_					[[nodiscard]]
#  else
#	define ND_
#  endif
# endif // AE_COMPILER_MSVC

# ifdef AE_COMPILER_CLANG
#  if __has_feature( cxx_attributes )
#	define ND_					[[nodiscard]]
#  else
#	define ND_
#  endif
# endif // AE_COMPILER_CLANG

# ifdef AE_COMPILER_GCC
#  if __has_cpp_attribute( nodiscard )
#	define ND_					[[nodiscard]]
#  else
#	define ND_
#  endif
# endif // AE_COMPILER_GCC

#endif // ND_


// null pointer
#ifndef null
#	define null					nullptr
#endif


// force inline
#ifndef forceinline
# if defined(AE_CFG_DEBUG)
#	define forceinline			inline

# elif defined(AE_COMPILER_MSVC)
#	define forceinline			__forceinline

# elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	define forceinline			__inline__ __attribute__((__always_inline__))

# else
#	pragma warning ("'forceinline' is not supported")
#	define forceinline			inline
# endif
#endif


// no inline (for debugging)
#ifndef AE_NOINLINE
# if defined(AE_COMPILER_MSVC)
#	define AE_NOINLINE			__declspec(noinline)
# elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	define AE_NOINLINE			__attribute__((noinline))
# else
#	pragma warning ("'noinline' is not supported")
#	define AE_NOINLINE
# endif
#endif


// function name
#ifdef AE_COMPILER_MSVC
#	define AE_FUNCTION_NAME		__func__	//	local variable of type 'const char[]'

#elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	define AE_FUNCTION_NAME		__func__	//	local variable of type 'const char[]'

#else
#	define AE_FUNCTION_NAME		"unknown function"
#endif


// branch prediction optimization
#if __has_cpp_attribute( likely )
#	define AE_PRIVATE_HAS_CPP20_LIKELY
#endif

#ifdef AE_PRIVATE_HAS_CPP20_LIKELY
#	define if_likely( ... )		if ( __VA_ARGS__ ) [[likely]]
#	define if_unlikely( ... )	if ( __VA_ARGS__ ) [[unlikely]]

#elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	define if_likely( ... )		if ( __builtin_expect( !!(__VA_ARGS__), 1 ))
#	define if_unlikely( ... )	if ( __builtin_expect( !!(__VA_ARGS__), 0 ))
#else
	// not supported
#	define if_likely( ... )		if ( __VA_ARGS__ )
#	define if_unlikely( ... )	if ( __VA_ARGS__ )
#endif

#ifdef AE_PRIVATE_HAS_CPP20_LIKELY
#	define else_unlikely		else [[unlikely]]
#	define case_likely			[[likely]]		case
#	define case_unlikely		[[unlikely]]	case
#	define default_unlikely		[[unlikely]]	default
#	define for_likely( ... )	for ( __VA_ARGS__ )	[[likely]]
#	define for_unlikely( ... )	for ( __VA_ARGS__ ) [[unlikely]]
#else
	// not supported
#	define else_unlikely		else
#	define case_likely			case
#	define case_unlikely		case
#	define default_unlikely		default
#	define for_likely( ... )	for ( __VA_ARGS__ )
#	define for_unlikely( ... )	for ( __VA_ARGS__ )
#endif


// TODO
#if __has_cpp_attribute( fallthrough )
#	define FALLTHROUGH()		[[fallthrough]]
#else
#	define FALLTHROUGH()		{}
#endif


// field placement optimization
#if __has_cpp_attribute( no_unique_address )
#	define NO_UNIQUE_ADDRESS	[[no_unique_address]]
#else
#	define NO_UNIQUE_ADDRESS
#endif


// variable may be unused (C++17)
#if __has_cpp_attribute( maybe_unused  )
#	define MaybeUnused			[[maybe_unused ]]
#else
#	define MaybeUnused
#endif


// thiscall, cdecl
#ifdef AE_COMPILER_MSVC
#	define AE_CDECL				__cdecl
#	define AE_THISCALL			__thiscall

#elif defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_GCC)
#	define AE_CDECL				//__attribute__((cdecl))
#	define AE_THISCALL			//__attribute__((thiscall))
#endif


// native source location (C++20) instead of __FILE__, __LINE__
#ifdef __cpp_lib_source_location
#	define AE_HAS_SOURCE_LOCATION
#endif


// code vectorization
#ifdef AE_COMPILER_MSVC
#	define DONT_VECTORIZE		__pragma( loop( no_vector ))	// disable vectorization
#	define FORCE_VECTORIZE		__pragma( loop( ivdep ))		// ignore dependencies to enable vectorization
#	define UNROLL

#elif defined(AE_COMPILER_CLANG)
#	define DONT_VECTORIZE		_Pragma( "clang loop vectorize(disable) interleave(disable)" )
#	define FORCE_VECTORIZE		_Pragma( "clang loop vectorize(enable) interleave(enable)" )
#	define UNROLL				_Pragma( "clang loop unroll(full)" )

#else
#	define DONT_VECTORIZE
#	define FORCE_VECTORIZE
#	define UNROLL
#endif


// vtable
#ifdef AE_COMPILER_MSVC
#	define NO_VTABLE			__declspec( novtable )
#else
#	define NO_VTABLE
#endif


// intrinsic attribute
#if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL)
# if _MSC_VER >= 1935		// since VS 2022 17.5
#	define AE_INTRINSIC		[[msvc::intrinsic]]
# endif
#endif
#ifndef AE_INTRINSIC
#	define AE_INTRINSIC		forceinline
#endif


// force recursively inline all function call inside the block
#ifdef AE_COMPILER_MSVC
# if _MSC_VER > 1930		// since VS 2022
#	define AE_INLINE_ALL	[[msvc::flatten]]
# endif
#endif
#ifndef AE_INLINE_ALL
#	define AE_INLINE_ALL
#endif


// sanitizer
#ifdef AE_COMPILER_CLANG
#	define AE_ADDRESS_SANITIZER_ENABLED		__has_feature( address_sanitizer )
#	define AE_THREAD_SANITIZER_ENABLED		__has_feature( thread_sanitizer )
#	define AE_MEMORY_SANITIZER_ENABLED		__has_feature( memory_sanitizer )
#	define AE_DATA_FLOW_SANITIZER_ENABLED	__has_feature( dataflow_sanitizer )
#	define AE_SAFE_STACK_ENABLED			__has_feature( safe_stack )
#endif


// DLL import/export
#if not defined(AE_DLL_EXPORT) or not defined(AE_DLL_IMPORT)
# if defined(AE_COMPILER_MSVC)
#	define AE_DLL_EXPORT		__declspec( dllexport )
#	define AE_DLL_IMPORT		__declspec( dllimport )

# elif defined(AE_COMPILER_GCC) or defined(AE_COMPILER_CLANG)
#  ifdef AE_PLATFORM_WINDOWS
#	define AE_DLL_EXPORT		__attribute__( dllexport )
#	define AE_DLL_IMPORT		__attribute__( dllimport )
#  else
#	define AE_DLL_EXPORT		__attribute__((visibility("default")))
#	define AE_DLL_IMPORT		__attribute__((visibility("default")))
#  endif

# else
#	error define AE_DLL_EXPORT and AE_DLL_IMPORT for you compiler
# endif
#endif


// function argument name
#define NAMED_ARG( _name_, /*arg*/... )		(__VA_ARGS__)
//#define NAMED_ARG( _name_, /*arg*/... )	._name_ = (__VA_ARGS__)		// not supported yet


#if (defined(AE_CPU_ARCH_ARM32) and defined(__ARM_NEON__)) or defined(AE_CPU_ARCH_ARM64)
#	define AE_SIMD_NEON			1	// TODO: Neon32, Neon64, SVE, SVE2
//# define AE_SIMD_NEON_HALF

#	include <arm_neon.h>
#	ifdef __ARM_FEATURE_SVE
#	  include <arm_sve.h>
#	endif
	// TODO: arm64_neon.h
#endif

#if defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)
# ifndef __ARM_FP
#	error soft-FP is not supported
# endif
#endif

#if defined(AE_CPU_ARCH_X64) or defined(AE_CPU_ARCH_X86)
	// AVX
#  if AE_SIMD_AVX > 0
#	include <immintrin.h>
#  endif

	// AVX 512
#  if AE_SIMD_AVX >= 3
#	if defined(AE_COMPILER_MSVC)
#	  include <zmmintrin.h>			// included in 'immintrin.h'
#	elif defined(AE_COMPILER_CLANG)
#	  include <avx512fp16intrin.h>	// clang
#	else
#	  error include AVX512 header
#	endif
#  endif

	// SSE 4.2
#  if AE_SIMD_SSE > 0
#  include <nmmintrin.h>
#  endif

	// AES
#  if AE_SIMD_AES > 0
#	include <wmmintrin.h>
#  endif
#endif


#ifndef AE_SIMD_AVX
#	define AE_SIMD_AVX			0
#endif
#ifndef AE_SIMD_SSE
#	define AE_SIMD_SSE			0
#endif
#ifndef AE_SIMD_AES
#	define AE_SIMD_AES			0
#endif
#ifndef AE_SIMD_NEON
#	define AE_SIMD_NEON			0
#endif
#ifndef AE_SIMD_NEON_HALF
#	define AE_SIMD_NEON_HALF	0
#endif

#define AE_HAS_SIMD				(AE_SIMD_AVX | AE_SIMD_SSE | AE_SIMD_NEON)


// allow to use 'offsetof()' in 'static_assert()'
#ifndef AE_PLATFORM_EMSCRIPTEN
#	define AE_COMPILETIME_OFFSETOF
#endif


#define INTERNAL_LINKAGE( /* type  name */... )		namespace { static __VA_ARGS__ ; }


// Keep exactly the same type.
// 'auto' will deduce the value type instead of reference type.
// 'exact_t' will use reference type where it is possible.
//#define exact_t		decltype(auto)
/*
example of variable initialization:
	int i;
	int&& f();
	auto x3a = i;					// decltype(x3a) is int
	decltype(auto) x3d = i;			// decltype(x3d) is int
	auto x4a = (i);					// decltype(x4a) is int
	decltype(auto) x4d = (i);		// decltype(x4d) is int&
	auto x5a = f();					// decltype(x5a) is int
	decltype(auto) x5d = f();		// decltype(x5d) is int&&
	auto x6a = { 1, 2 };			// decltype(x6a) is std::initializer_list<int>
	decltype(auto) x6d = { 1, 2 };	// error, { 1, 2 } is not an expression
	auto *x7a = &i;					// decltype(x7a) is int*
	decltype(auto)*x7d = &i;		// error, declared type is not plain decltype(auto)
*/

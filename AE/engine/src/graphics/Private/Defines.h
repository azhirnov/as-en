// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_CFG_RELEASE) and not defined(AE_CI_BUILD_TEST)
#	define AE_DBG_GRAPHICS				0
#	define GFX_DBG_ONLY( ... )
#else
#	define AE_DBG_GRAPHICS				1
#	define GFX_DBG_ONLY( ... )			__VA_ARGS__
#endif


#ifdef AE_DEBUG
#	define AE_GRAPHICS_DBG_SYNC			1
#	define GRAPHICS_DBG_SYNC( ... )		__VA_ARGS__
#else
#	define AE_GRAPHICS_DBG_SYNC			0
#	define GRAPHICS_DBG_SYNC( ... )
#endif


// Should be used for validation errors which may cause crash or UB.
// Debug checks should use ASSERT() instead.
#ifndef AE_GRAPHICS_STRONG_VALIDATION
# error add Graphics module
#endif
#if AE_GRAPHICS_STRONG_VALIDATION
#	define GCTX_CHECK					CHECK_THROW
#	define GCTX_CHECK_MSG				CHECK_THROW_MSG
#	define AE_VALIDATE_GCTX				1
#	define GRES_CHECK					CHECK_ERR
#else
# if defined(AE_DEBUG) or defined(AE_CI_BUILD_TEST)
#	define GCTX_CHECK					CHECK
#	define GCTX_CHECK_MSG				CHECK_MSG
#	define AE_VALIDATE_GCTX				1
#	define GRES_CHECK					CHECK
# else
	// disable validation
#	define GCTX_CHECK( ... )			{}
#	define GCTX_CHECK_MSG( ... )		{}
#	define AE_VALIDATE_GCTX				0
#	define GRES_CHECK( ... )			{}
# endif
#endif


#ifdef AE_CI_BUILD_PERF
# if AE_DBG_GRAPHICS
#	error Graphics debugging is not compatible with CI performace tests
# endif
# if AE_GRAPHICS_DBG_SYNC
#	error Graphics sync debugging is not compatible with CI performace tests
# endif
# if AE_VALIDATE_GCTX
#	error Graphics context validation is not compatible with CI performace tests
# endif
#endif


#ifdef AE_CPP_DETECT_MISMATCH

#  if AE_DBG_GRAPHICS
#	pragma detect_mismatch( "AE_DBG_GRAPHICS", "1" )
#  else
#	pragma detect_mismatch( "AE_DBG_GRAPHICS", "0" )
#  endif

#  if AE_GRAPHICS_DBG_SYNC
#	pragma detect_mismatch( "AE_GRAPHICS_DBG_SYNC", "1" )
#  else
#	pragma detect_mismatch( "AE_GRAPHICS_DBG_SYNC", "0" )
#  endif

#  if AE_VALIDATE_GCTX
#	pragma detect_mismatch( "AE_VALIDATE_GCTX", "1" )
#  else
#	pragma detect_mismatch( "AE_VALIDATE_GCTX", "0" )
#  endif

#  if AE_GRAPHICS_STRONG_VALIDATION
#	pragma detect_mismatch( "AE_GRAPHICS_STRONG_VALIDATION", "1" )
#  else
#	pragma detect_mismatch( "AE_GRAPHICS_STRONG_VALIDATION", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

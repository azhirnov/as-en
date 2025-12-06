// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/OpenXRCommon.h"

namespace AE::App
{

	//
	// OpenXR Instance Functions
	//
	struct OpenXRInstanceFn
	{
	  #ifdef XR_NO_PROTOTYPES
		#define XRLOADER_STAGE_DECLFNPOINTER
		#include "openxr_fn_lib.h"
		#include "openxr_fn_inst.h"
		#undef  XRLOADER_STAGE_DECLFNPOINTER

		#define XRLOADER_STAGE_INLINEFN
		#include "openxr_fn_lib.h"
		#include "openxr_fn_inst.h"
		#undef  XRLOADER_STAGE_INLINEFN

		static PFN_xrGetInstanceProcAddr  _var_xrGetInstanceProcAddr;

		ND_ XRAPI_ATTR static XrResult xrGetInstanceProcAddr (XrInstance instance, const char* name, PFN_xrVoidFunction* function) __NE___ { return _var_xrGetInstanceProcAddr( instance, name, OUT function ); }
	  #endif

		ND_ static bool  _CheckXrErrors (XrResult errCode, const char* xrcall, const char* func, const AE::Base::SourceLoc &loc) __NE___;
	};
	StaticAssert( sizeof(OpenXRInstanceFn) == 1 );



	//
	// OpenXR Loader
	//
	struct OpenXRLoader final : Noninstanceable
	{
		ND_ static bool  Initialize (NtStringView libName = {})	__NE___;
			static void  Unload ()								__NE___;
		ND_ static bool  IsLoaded ()							__NE___;

		ND_ static bool  LoadInstance (XrInstance instance)		__NE___;
			static void  ResetInstance ()						__NE___;

		ND_ static bool  GetAndroidVMAndActivity (OUT void* &vm, OUT void* &activity) __NE___;
	};

} // AE::App


// check errors //

# ifdef AE_CFG_RELEASE
	#define XR_CHECK( ... )			{AE::Base::Unused(__VA_ARGS__);}

#	define AE_PRIVATE_XR_CHECK_R( _expr_, _msg_, _ret_ )										\
	{																							\
		const ::XrResult __xr_err__ = (_expr_);													\
		if_unlikely( __xr_err__ != XR_SUCCESS )													\
			return _ret_;																		\
	}

# else

#	define XR_CHECK( /* expr */... )															\
	{																							\
		const ::XrResult __xr_err__ = (__VA_ARGS__);											\
		AE::Base::Unused( AE::App::OpenXRInstanceFn::_CheckXrErrors( __xr_err__,				\
			AE_TOSTRING( __VA_ARGS__ ), AE_FUNCTION_NAME, AE::Base::SourceLoc::current() ));	\
	}

#	define AE_PRIVATE_XR_CHECK_R( _expr_, _msg_, _ret_ )										\
	{																							\
		const ::XrResult __xr_err__ = (_expr_);													\
		if_unlikely( not AE::App::OpenXRInstanceFn::_CheckXrErrors( __xr_err__,					\
							(_msg_), AE_FUNCTION_NAME, AE::Base::SourceLoc::current() ))		\
			return _ret_;																		\
	}

# endif // AE_CFG_RELEASE

# define AE_PRIVATE_XR_CHECK2_R( _func_, _ret_ )												\
		AE_PRIVATE_XR_CHECK_R( (_func_), AE_TOSTRING( _func_ ), (_ret_) )

# define XR_CHECK_ERR( /* expr, return_on_error */... )											\
		AE_PRIVATE_XR_CHECK2_R( AE_PRIVATE_GETARG_0( __VA_ARGS__, ),							\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default, ))

# define XR_CHECK_ERRV( _expr_ )																\
		AE_PRIVATE_XR_CHECK2_R( (_expr_), void() )

# define XR_ERR( _err_, /* msg, return_on_error */... )											\
		AE_PRIVATE_XR_CHECK_R(	(_err_), AE_PRIVATE_GETARG_0( __VA_ARGS__, ),					\
								AE_PRIVATE_GETARG_1( __VA_ARGS__, AE::Base::Default, ))

#endif // AE_ENABLE_OPENXR

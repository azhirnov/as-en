// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
	https://registry.khronos.org/OpenXR/specs/1.1/html/xrspec.html
	https://developer.android.com/develop/xr/openxr
	https://learn.microsoft.com/en-us/windows/mixed-reality/develop/native/openxr
	https://lf3-static.bytednsdoc.com/obj/eden-cn/sliheh7phbozbps/NativeSDK/3.0.0/NativeSdkSpec.html
*/

#pragma once

#ifdef AE_ENABLE_OPENXR

# define XR_NO_PROTOTYPES
# include "base/Defines/StdInclude.h"
# include "openxr/openxr.h"

# if XR_CURRENT_API_VERSION < XR_MAKE_VERSION(1, 1, 0)
#	error OpenXR 1.1 required
# endif

# include "pch/Threading.h"

namespace AE::App
{
	using namespace AE::Base;

	using Threading::Synchronized;

	#if AE_ENABLE_DATA_RACE_CHECK
	using Threading::RWDataRaceCheck;
	#endif

	class InputActionsOpenXR;
	class VRSessionOpenXR;
}

#endif // AE_ENABLE_OPENXR


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_ENABLE_OPENXR
#	pragma detect_mismatch( "AE_ENABLE_OPENXR", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_OPENXR", "0" )
#  endif

#  ifdef AE_ENABLE_PICO_OPENXR
#	pragma detect_mismatch( "AE_ENABLE_PICO_OPENXR", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_PICO_OPENXR", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

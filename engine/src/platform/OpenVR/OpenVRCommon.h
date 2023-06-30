// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_ENABLE_OPENVR
# include "openvr_capi.h"

namespace AE::App
{
    class InputActionsOpenVR;
    class OpenVRDevice;
}

#endif // AE_ENABLE_OPENVR


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_ENABLE_OPENVR
#   pragma detect_mismatch( "AE_ENABLE_OPENVR", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_OPENVR", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

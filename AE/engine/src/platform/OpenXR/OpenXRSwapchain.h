// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/OpenXRDevice.h"
# include "platform/Private/VRSurface.h"

namespace AE::App
{

	//
	// OpenXR Swapchain
	//

	class OpenXRSwapchain  //: public VRSurface
	{
	};


} // AE::App

#endif // AE_ENABLE_OPENXR

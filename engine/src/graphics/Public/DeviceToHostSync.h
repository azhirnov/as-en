// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	//
	// GPU to CPU Sync interface
	//

	class IDeviceToHostSync : public EnableRC< IDeviceToHostSync >
	{
	// interface
	public:
		// use only 0 timeout to avoid stalls
		ND_ virtual bool  Wait (nanoseconds timeout) = 0;
		ND_ virtual bool  IsComplete () = 0;
	};

	using DeviceToHostSyncPtr = RC< IDeviceToHostSync >;


} // AE::Graphics

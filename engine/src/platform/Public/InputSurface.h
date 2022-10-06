// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Input surface can be:
		- camera
		- video stream
*/

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{

	//
	// Input Surface
	//

	class IInputSurface
	{
	// interface
	public:
		virtual ~IInputSurface () {}

	};


} // AE::App

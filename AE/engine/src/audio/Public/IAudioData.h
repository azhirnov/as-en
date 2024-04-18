// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/Common.h"

namespace AE::Audio
{

	//
	// Audio Data interface
	//

	class IAudioData : public EnableRC<IAudioData>
	{
	// interface
	public:
		ND_ virtual bool					IsStream ()		__NE___	= 0;
		ND_ virtual RC<RStream>				Stream ()		__NE___ = 0;
		ND_ virtual AudioDataDesc const&	Description ()	C_NE___	= 0;
	};


} // AE::Audio

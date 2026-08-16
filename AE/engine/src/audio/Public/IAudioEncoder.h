// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "audio/Public/Common.h"

namespace AE::Audio
{
	struct AudioEncoderDesc
	{
	};


	//
	// Audio Encoder interface
	//

	class IAudioEncoder : public WStream
	{
	};


} // AE::Audio

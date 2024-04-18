// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioEncoder.h"

namespace AE::Audio
{

	//
	// BASS Audio Encoder
	//

	class AudioEncoderBASS : public IAudioEncoder
	{
	};


} // AE::Audio

#endif // AE_ENABLE_BASS

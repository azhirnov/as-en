// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioInput.h"

namespace AE::Audio
{

	//
	// BASS Audio Input
	//

	class AudioInputBASS final : public IAudioInput
	{
	// variables
	private:
		uint			_recChannelId	= 0;	// HRECORD
		uint			_deviceId		= 0;
		RC<WStream>		_stream;

		AudioInputDesc	_inDesc;


	// methods
	public:
		AudioInputBASS (const AudioInputDesc &desc, uint dev)	__NE___ : _deviceId{dev}, _inDesc{desc} {}
		~AudioInputBASS ()										__NE_OV;

		// IAudioInput //
		bool  Begin (RC<WStream>)								__NE_OV;
		bool  End (OUT RC<WStream> &, OUT AudioDataDesc &)		__NE_OV;
		bool  IsStarted ()										C_NE_OV	{ return bool{_stream} and _recChannelId != 0; }

		void  Resume ()											__NE_OV;
		void  Pause ()											__NE_OV;
		bool  IsRecording ()									C_NE_OV;
	};


} // AE::Audio

#endif // AE_ENABLE_BASS

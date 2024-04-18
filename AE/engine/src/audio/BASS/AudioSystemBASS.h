// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioSystem.h"
# include "audio/Public/IAudioEncoder.h"
# include "audio/Public/IAudioDecoder.h"

# include "audio/BASS/AudioInputBASS.h"
# include "audio/BASS/AudioOutputBASS.h"

namespace AE::Audio
{

	//
	// BASS Audio System
	//

	class AudioSystemBASS final : public IAudioSystem
	{
	// types
	private:
		struct StreamWrap;


	// variables
	private:
		bool			_initialized	= false;
		const uint		_maxChannels	= 8;


	// methods
	public:
		AudioSystemBASS ()														__NE___ {}
		~AudioSystemBASS ()														__NE___ {}


	// IAudioSystem //
		bool	Initialize (const Config &cfg)									__NE_OV;
		void	Deinitialize ()													__NE_OV;


		// Playback //
		void	Resume ()														__NE_OV;
		void	Pause ()														__NE_OV;
		bool	IsPlaying ()													C_NE_OV;

		float	Volume ()														C_NE_OV;
		void	SetVolume (float value)											__NE_OV;


		// 3D //
		void	SetListenerPosition (const Pos3D &value)						__NE_OV;
		Pos3D	ListenerPosition ()												C_NE_OV;

		void	SetListenerVelocity (const Vel3D &value)						__NE_OV;
		Vel3D	ListenerVelocity ()												C_NE_OV;

		void	Apply3D ()														__NE_OV;


		RC<IAudioData>		CreateData (RC<RStream>, ESoundFlags)				__NE_OV;
		RC<IAudioData>		CreateData (const void* data,
										Bytes dataSize,
										ESoundFlags flags)						__NE_OV;

		RC<IAudioInput>		CreateInput (const AudioInputDesc &)				__NE_OV;

		RC<IAudioOutput>	CreateOutput (RC<IAudioData>)						__NE_OV;

		RC<IAudioDecoder>	CreateDecoder (const AudioDecoderDesc &)			__NE_OV;
		RC<IAudioEncoder>	CreateEncoder (const AudioEncoderDesc &)			__NE_OV;

	private:
		ND_ RC<IAudioData>  _CreateStream (RC<RStream>, ESoundFlags)			__NE___;

		void  _PrintOutputDevices ()											C_Th___;
		void  _PrintInputDevices ()												C_Th___;

		friend IAudioSystem&  AE::AudioSystem ()								__NE___;
	};


} // AE::Audio

#endif // AE_ENABLE_BASS

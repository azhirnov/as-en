// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "audio/Public/IAudioOutput.h"
#include "audio/Public/IAudioInput.h"
#include "audio/Public/IAudioEncoder.h"
#include "audio/Public/IAudioDecoder.h"

namespace AE::Audio
{
	class IAudioData;
	class IAudioDecoder;
	class IAudioEncoder;


	//
	// Audio System interface
	//

	class IAudioSystem
	{
	// types
	public:
		struct Config
		{
			uint		updateThreads		= 1;
			Bytes		asyncFileBuffer		= 1_Mb;

			Config () __NE___ {}
		};

		class InstanceCtor {
		public:
			static void  Create ()		__NE___;
			static void  Destroy ()		__NE___;
		};


	// interface
	public:
		ND_ virtual bool	Initialize (const Config &cfg = Default)						__NE___	= 0;
			virtual void	Deinitialize ()													__NE___ = 0;

		// Playback //
			virtual void	Resume ()														__NE___ = 0;
			virtual void	Pause ()														__NE___ = 0;
		ND_ virtual bool	IsPlaying ()													C_NE___ = 0;

		ND_ virtual float	Volume ()														C_NE___ = 0;
			virtual void	SetVolume (float value)											__NE___ = 0;


		// 3D //
					void	SetListenerPosition (const float3 &value)						__NE___ { SetListenerPosition( Pos3D{value} ); }
			virtual void	SetListenerPosition (const Pos3D &value)						__NE___ = 0;
		ND_ virtual Pos3D	ListenerPosition ()												C_NE___	= 0;

					void	SetListenerVelocity (const float3 &value)						__NE___ { SetListenerVelocity( Vel3D{value} ); }
			virtual void	SetListenerVelocity (const Vel3D &value)						__NE___	= 0;
		ND_ virtual Vel3D	ListenerVelocity ()												C_NE___ = 0;

			virtual void	Apply3D ()														__NE___ = 0;


		// Create sound stream or sound buffer.
		//	'stream' or 'data' must contains sound data with known format (WAV, OGG, ...).
		//
		ND_ virtual RC<IAudioData>		CreateData (RC<RStream> stream,
													ESoundFlags flags)						__NE___ = 0;
		ND_ virtual RC<IAudioData>		CreateData (const void* data,
													Bytes dataSize,
													ESoundFlags flags)						__NE___ = 0;

		// Create audio recorder.
		//	Some implementations supports multiple simultaneous recording.
		//
		ND_ virtual RC<IAudioInput>		CreateInput (const AudioInputDesc &)				__NE___	= 0;


		// Create audio player or sound source.
		//
		ND_ virtual RC<IAudioOutput>	CreateOutput (RC<IAudioData>)						__NE___	= 0;


		// Create audio decoder.
		//
		ND_ virtual RC<IAudioDecoder>	CreateDecoder (const AudioDecoderDesc &)			__NE___ = 0;


		// Create audio encoder.
		//
		ND_ virtual RC<IAudioEncoder>	CreateEncoder (const AudioEncoderDesc &)			__NE___ = 0;
	};


} // AE::Audio

namespace AE
{
	ND_ Audio::IAudioSystem&  AudioSystem () __NE___;
}

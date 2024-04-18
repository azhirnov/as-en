// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BASS
# include "audio/Public/IAudioData.h"
# include "audio/Public/IAudioOutput.h"

namespace AE::Audio
{

	//
	// BASS Audio Output
	//

	class AudioOutputBASS final : public IAudioOutput
	{
	// variables
	private:
		uint			_channelId;	// HCHANNEL
		RC<IAudioData>	_data;		// just keep reference


	// methods
	public:
		AudioOutputBASS (uint id, RC<IAudioData> data)		__NE___ : _channelId{id}, _data{RVRef(data)} {}
		~AudioOutputBASS ()									__NE_OV;


		// IAudioOutput //
		void	Play ()										__NE_OV;
		void	Pause ()									__NE_OV;
		void	Stop ()										__NE_OV;
		bool	IsPlaying ()								__NE_OV;

		void	SetLooping (bool value)						__NE_OV;
		bool	IsLooping ()								__NE_OV;

		void	SetVolume (float value)						__NE_OV;
		float	Volume ()									__NE_OV;

		void	SetPlaybackLocation (Seconds value)			__NE_OV;
		Seconds	PlaybackLocation ()							__NE_OV;
		Seconds	Duration ()									__NE_OV;

		void	SetPosition (const Pos3D &value)			__NE_OV;
		Pos3D	Position ()									__NE_OV;

		void	SetVelocity (const Vel3D &value)			__NE_OV;
		Vel3D	Velocity ()									__NE_OV;

		void	SetFrequency (Freq value)					__NE_OV;
		Freq	Frequency ()								__NE_OV;
	};


} // AE::Audio

#endif // AE_ENABLE_BASS

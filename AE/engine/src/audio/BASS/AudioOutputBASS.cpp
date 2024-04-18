// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/AudioOutputBASS.h"
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{

/*
=================================================
	destructor
=================================================
*/
	AudioOutputBASS::~AudioOutputBASS () __NE___
	{
		Stop();
	}

/*
=================================================
	Play
=================================================
*/
	void  AudioOutputBASS::Play () __NE___
	{
		BASS_CHECK( bass.ChannelPlay( _channelId, FALSE ));
	}

/*
=================================================
	Pause
=================================================
*/
	void  AudioOutputBASS::Pause () __NE___
	{
		BASS_CHECK( bass.ChannelPause( _channelId ));
	}

/*
=================================================
	Stop
=================================================
*/
	void  AudioOutputBASS::Stop () __NE___
	{
		BASS_CHECK( bass.ChannelStop( _channelId ));
	}

/*
=================================================
	IsPlaying
=================================================
*/
	bool  AudioOutputBASS::IsPlaying () __NE___
	{
		return bass.ChannelIsActive( _channelId ) == BASS_ACTIVE_PLAYING;
	}

/*
=================================================
	SetLooping
=================================================
*/
	void  AudioOutputBASS::SetLooping (bool value) __NE___
	{
		BASS_CHECK( bass.ChannelFlags( _channelId, value ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP ) != UMax );
	}

/*
=================================================
	IsLooping
=================================================
*/
	bool  AudioOutputBASS::IsLooping () __NE___
	{
		return AllBits( bass.ChannelFlags( _channelId, 0, 0 ), BASS_SAMPLE_LOOP );
	}

/*
=================================================
	SetVolume
=================================================
*/
	void  AudioOutputBASS::SetVolume (float value) __NE___
	{
		BASS_CHECK( bass.ChannelSetAttribute( _channelId, BASS_ATTRIB_VOL, value ));
	}

/*
=================================================
	Volume
=================================================
*/
	float  AudioOutputBASS::Volume () __NE___
	{
		float	volume = 1.0f;
		BASS_CHECK( bass.ChannelGetAttribute( _channelId, BASS_ATTRIB_VOL, OUT &volume ));
		return volume;
	}

/*
=================================================
	SetPlaybackLocation
=================================================
*/
	void  AudioOutputBASS::SetPlaybackLocation (Seconds value) __NE___
	{
		QWORD	byte_pos = bass.ChannelSeconds2Bytes( _channelId, double(value.GetNonScaled()) );
		BASS_CHECK( bass.ChannelSetPosition( _channelId, byte_pos, BASS_POS_BYTE ));
	}

/*
=================================================
	PlaybackLocation
=================================================
*/
	Seconds  AudioOutputBASS::PlaybackLocation () __NE___
	{
		QWORD	byte_pos = bass.ChannelGetPosition( _channelId, BASS_POS_BYTE );
		return Seconds{ float( bass.ChannelBytes2Seconds( _channelId, byte_pos ))};
	}

/*
=================================================
	Duration
=================================================
*/
	Seconds  AudioOutputBASS::Duration () __NE___
	{
		QWORD	byte_len = bass.ChannelGetLength( _channelId, BASS_POS_BYTE );
		return Seconds{ float( bass.ChannelBytes2Seconds( _channelId, byte_len ))};
	}

/*
=================================================
	SetPosition
=================================================
*/
	void  AudioOutputBASS::SetPosition (const Pos3D &value) __NE___
	{
		BASS_3DVECTOR	pos = { value.x.GetNonScaled(),
								value.y.GetNonScaled(),
								value.z.GetNonScaled() };
		BASS_CHECK( bass.ChannelSet3DPosition( _channelId, &pos, null, null ));
	}

/*
=================================================
	Position
=================================================
*/
	Pos3D  AudioOutputBASS::Position () __NE___
	{
		BASS_3DVECTOR	pos = {};
		bass.ChannelGet3DPosition( _channelId, OUT &pos, null, null );
		return Pos3D{ pos.x, pos.y, pos.z };
	}

/*
=================================================
	SetVelocity
=================================================
*/
	void  AudioOutputBASS::SetVelocity (const Vel3D &value) __NE___
	{
		BASS_3DVECTOR	vel = { value.x.GetNonScaled(),
								value.y.GetNonScaled(),
								value.z.GetNonScaled() };
		bass.ChannelSet3DPosition( _channelId, null, null, &vel );
	}

/*
=================================================
	Velocity
=================================================
*/
	Vel3D  AudioOutputBASS::Velocity () __NE___
	{
		BASS_3DVECTOR	vel = {};
		bass.ChannelGet3DPosition( _channelId, null, null, &vel );
		return Vel3D{ vel.x, vel.y, vel.z };
	}

/*
=================================================
	SetFrequency
=================================================
*/
	void  AudioOutputBASS::SetFrequency (Freq value) __NE___
	{
		BASS_CHECK( bass.ChannelSetAttribute( _channelId, BASS_ATTRIB_FREQ, float(value.GetNonScaled()) ));
	}

/*
=================================================
	Frequency
=================================================
*/
	Freq  AudioOutputBASS::Frequency () __NE___
	{
		float	freq = 0.0f;
		BASS_CHECK( bass.ChannelGetAttribute( _channelId, BASS_ATTRIB_FREQ, OUT &freq ));
		return Freq{uint( freq + 0.5f )};
	}


} // AE::Audio

#endif // AE_ENABLE_BASS

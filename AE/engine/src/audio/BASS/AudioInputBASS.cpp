// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/AudioInputBASS.h"
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{
namespace
{
	static BOOL CALLBACK  RecordingCallback (HRECORD, const void* buffer, DWORD length, void* user) __NE___
	{
		auto&	stream = *Cast<WStream>( user );
		return stream.Write( buffer, Bytes{length} );
	}

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	destructor
=================================================
*/
	AudioInputBASS::~AudioInputBASS () __NE___
	{
		if ( _recChannelId != 0 )
			BASS_CHECK( bass.ChannelStop( _recChannelId ));
	}

/*
=================================================
	Begin
=================================================
*/
	bool  AudioInputBASS::Begin (RC<WStream> dstStream) __NE___
	{
		CHECK_ERR( not IsStarted() );

		// for current thread
		//BASS_CHECK_ERR( bass.RecordInit( _deviceId ));
		BASS_CHECK_ERR( bass.RecordSetDevice( _deviceId ));

		BASS_RECORDINFO		info = {};
		BASS_CHECK( bass.RecordGetInfo( OUT &info ));

		DWORD	freq = _inDesc.freq.GetNonScaled();
				freq = (freq == 0 ? info.freq : freq);

		DWORD	flags = 0; // BASS_RECORD_PAUSE
		switch_enum( _inDesc.sampleFormat )
		{
			case ESampleFormat::UInt8 :		flags |= BASS_SAMPLE_8BITS;			break;
			case ESampleFormat::UInt16 :										break;
			case ESampleFormat::Float32 :	flags |= BASS_SAMPLE_FLOAT;			break;
			case ESampleFormat::Unknown :
			default :						RETURN_ERR( "unsupported format" );	break;
		}
		switch_end

		HRECORD		channel = bass.RecordStart( freq, _inDesc.channels, flags, &RecordingCallback, dstStream.get() );

		DEBUG_ONLY( BASS_CheckError() );
		CHECK_ERR( channel != 0 );

		_recChannelId	= channel;
		_stream			= RVRef(dstStream);
		return true;
	}

/*
=================================================
	End
=================================================
*/
	bool  AudioInputBASS::End (OUT RC<WStream> &outStream, OUT AudioDataDesc &outDesc) __NE___
	{
		CHECK_ERR( IsStarted() );

		ChannelGetInfo( _recChannelId, OUT outDesc );
		ASSERT( outDesc.IsValid() );

		BASS_CHECK( bass.ChannelStop( _recChannelId ));
		_recChannelId = 0;

		outStream = RVRef(_stream);
		return true;
	}

/*
=================================================
	Resume
=================================================
*/
	void  AudioInputBASS::Resume () __NE___
	{
		ASSERT( IsStarted() );
		BASS_CHECK( bass.ChannelStart( _recChannelId ));
	}

/*
=================================================
	Pause
=================================================
*/
	void  AudioInputBASS::Pause () __NE___
	{
		ASSERT( IsStarted() );
		BASS_CHECK( bass.ChannelPause( _recChannelId ));
	}

/*
=================================================
	IsRecording
=================================================
*/
	bool  AudioInputBASS::IsRecording () C_NE___
	{
		ASSERT( IsStarted() );
		return bass.ChannelIsActive( _recChannelId ) == BASS_ACTIVE_PLAYING;
	}


} // AE::Audio

#endif // AE_ENABLE_BASS

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BASS
# include "audio/BASS/UtilsBASS.cpp.h"

namespace AE::Audio
{

/*
=================================================
	BASS_CheckError
=================================================
*/
	bool  BASS_CheckError () __NE___
	{
		int	err = bass.ErrorGetCode();
		if_likely( err == BASS_OK )
			return true;

	  #ifdef AE_ENABLE_LOGS
		String	msg = "BASS error: ";

		#define BASS_CASE_ERR( _code_ ) \
			case BASS_ ## _code_ :	msg += AE_TOSTRING( _code_ ); break;

		switch ( err )
		{
			BASS_CASE_ERR( ERROR_MEM )
			BASS_CASE_ERR( ERROR_FILEOPEN )
			BASS_CASE_ERR( ERROR_DRIVER )
			BASS_CASE_ERR( ERROR_BUFLOST )
			BASS_CASE_ERR( ERROR_HANDLE )
			BASS_CASE_ERR( ERROR_FORMAT )
			BASS_CASE_ERR( ERROR_POSITION )
			BASS_CASE_ERR( ERROR_INIT )
			BASS_CASE_ERR( ERROR_START )
			BASS_CASE_ERR( ERROR_SSL )
			BASS_CASE_ERR( ERROR_ALREADY )
			BASS_CASE_ERR( ERROR_NOTAUDIO )
			BASS_CASE_ERR( ERROR_NOCHAN )
			BASS_CASE_ERR( ERROR_ILLTYPE )
			BASS_CASE_ERR( ERROR_ILLPARAM )
			BASS_CASE_ERR( ERROR_NO3D )
			BASS_CASE_ERR( ERROR_NOEAX )
			BASS_CASE_ERR( ERROR_DEVICE )
			BASS_CASE_ERR( ERROR_NOPLAY )
			BASS_CASE_ERR( ERROR_FREQ )
			BASS_CASE_ERR( ERROR_NOTFILE )
			BASS_CASE_ERR( ERROR_NOHW )
			BASS_CASE_ERR( ERROR_EMPTY )
			BASS_CASE_ERR( ERROR_NONET )
			BASS_CASE_ERR( ERROR_CREATE )
			BASS_CASE_ERR( ERROR_NOFX )
			BASS_CASE_ERR( ERROR_NOTAVAIL )
			BASS_CASE_ERR( ERROR_DECODE )
			BASS_CASE_ERR( ERROR_DX )
			BASS_CASE_ERR( ERROR_TIMEOUT )
			BASS_CASE_ERR( ERROR_FILEFORM )
			BASS_CASE_ERR( ERROR_SPEAKER )
			BASS_CASE_ERR( ERROR_VERSION )
			BASS_CASE_ERR( ERROR_CODEC )
			BASS_CASE_ERR( ERROR_ENDED )
			BASS_CASE_ERR( ERROR_BUSY )
			BASS_CASE_ERR( ERROR_UNSTREAMABLE )
			BASS_CASE_ERR( ERROR_UNKNOWN )
			default : msg << "unknown (" << ToString( err ) << ')'; break;
		}
		AE_LOGI( msg );
	  #endif

		return false;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	BASS libraries
=================================================
*/
namespace
{
	static Library	bassLib;
	static Library	bassEncLib;
	static Library	bassEncOggLib;

} // namespace

/*
=================================================
	_Load
=================================================
*/
	bool  BASSFunctions::_Load () __NE___
	{
	  #ifdef AE_PLATFORM_WINDOWS
		const StringView	bass_name			= "bass.dll";
		const StringView	bass_enc_name		= "bassenc.dll";
		const StringView	bass_enc_ogg_name	= "bassenc_ogg.dll";
		const StringView	bass_flac_name		= "bassflac.dll";
		const StringView	bass_webm_name		= "basswebm.dll";
	  #else
		const StringView	bass_name			= "bass.so";
		const StringView	bass_enc_name		= "bassenc.so";
		const StringView	bass_enc_ogg_name	= "bassenc_ogg.so";
		const StringView	bass_flac_name		= "bassflac.so";
		const StringView	bass_webm_name		= "basswebm.so";
	  #endif

		bool	result = true;

		if ( bassLib.Load( bass_name ))
		{
			#define BASS_LOAD_FN( _name_ )	result &= bassLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
			BASS_FN( BASS_LOAD_FN )
			#undef BASS_LOAD_FN
		}

		if ( bassEncLib.Load( bass_enc_name ))
		{
			#define BASS_LOAD_FN( _name_ )	result &= bassEncLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
			BASS_ENC_FN( BASS_LOAD_FN )
			#undef BASS_LOAD_FN
		}

		/*if ( bassEncOggLib.Load( bass_enc_ogg_name ))
		{
			#define BASS_LOAD_FN( _name_ )	result &= bassEncOggLib.GetProcAddr( "BASS_" #_name_, OUT _name_ );
			BASS_ENC_OGG_FN( BASS_LOAD_FN )
			#undef BASS_LOAD_FN
		}*/

		return result;
	}

/*
=================================================
	_Unload
=================================================
*/
	void  BASSFunctions::_Unload () __NE___
	{
		#define BASS_UNLOAD_FN( _name_ )	_name_ = null;

		BASS_FN( BASS_UNLOAD_FN )
		BASS_ENC_FN( BASS_UNLOAD_FN )
		BASS_ENC_OGG_FN( BASS_UNLOAD_FN )

		#undef BASS_UNLOAD_FN

		bassEncOggLib.Unload();
		bassEncLib.Unload();
		bassLib.Unload();
	}
//-----------------------------------------------------------------------------


/*
=================================================
	Decode
=================================================
*
	bool  AudioDevice::Decode (RStream &stream, OUT RawSoundData &result) __NE___
	{
		ubyte	buffer [_BufferSize];
		return Decode( TempBuffer_t{ buffer, Bytes{_BufferSize} }, stream, OUT result );
	}

	bool  AudioDevice::Decode (const TempBuffer_t &buffer, RStream &stream, OUT RawSoundData &result) __NE___
	{
		CHECK_ERR( _initialized );
		CHECK_ERR( stream.IsOpen() );
		CHECK_ERR( buffer.ptr != null and buffer.size <= _LengthMask );

		BASS_FILEPROCS	file_procs	= { &StreamWrap::Close, &StreamWrap::Length, &StreamWrap::Read, &StreamWrap::Seek };
		HSTREAM			bass_stream	= bass.StreamCreateFileUser( STREAMFILE_NOBUFFER, BASS_STREAM_DECODE, &file_procs, &stream );
		CHECK_ERR( bass_stream != 0 );

		ON_DESTROY( [&bass_stream] { bass.StreamFree( bass_stream ); });

		BASS_CHANNELINFO	info = {};
		CHECK_ERR( bass.ChannelGetInfo( bass_stream, OUT &info ) == TRUE );

		float		bitrate = 0.0f;
		CHECK_ERR( bass.ChannelGetAttribute( bass_stream, BASS_ATTRIB_BITRATE, OUT &bitrate ) == TRUE );

		QWORD		length = bass.ChannelGetLength( bass_stream, BASS_POS_BYTE );
		CHECK_ERR( length != UMax );

		result.channels	= info.chans;
		result.freq		= Frequency_t(info.freq);
		result.duration	= secondsf{ bass.ChannelBytes2Seconds( bass_stream, length )};
		result.bitrate	= Bitrate_t{bitrate};
		result.format	= ESampleFormat::UInt16;
		result.buffer.resize( length );

		usize	offset = 0;
		for (; bass.ChannelIsActive( bass_stream );)
		{
			QWORD	pos  = bass.ChannelGetPosition( bass_stream, BASS_POS_BYTE );
			DWORD	size = bass.ChannelGetData( bass_stream, OUT buffer.ptr, (DWORD(buffer.size) & _LengthMask) | BASS_DATA_AVAILABLE );

			if ( size == UMax )
				BASS_CheckError();

			CHECK_ERR( offset == pos );
			CHECK_ERR( pos + size <= result.buffer.size() );

			MemCopy( OUT result.buffer.data() + Bytes{pos}, buffer.ptr, Bytes{size} );
			offset = pos + size;
		}

		CHECK_ERR( offset == result.buffer.size() );
		return true;
	}

/*
=================================================
	Encode
=================================================
*
	bool  AudioDevice::Encode (RStream &inStream, WStream &outStream, EAudioFormat outputFormat, EAudioQuality quality) __NE___
	{
		ubyte	buffer [_BufferSize];
		return Encode( TempBuffer_t{ buffer, Bytes{_BufferSize} }, inStream, outStream, outputFormat, quality );
	}

	bool  AudioDevice::Encode (const TempBuffer_t &buffer, RStream &inStream, WStream &outStream, EAudioFormat outputFormat, EAudioQuality quality) __NE___
	{
		CHECK_ERR( inStream.IsOpen() );
		CHECK_ERR( outStream.IsOpen() );
		CHECK_ERR( buffer.ptr != null and buffer.size <= _LengthMask );

		BASS_FILEPROCS	file_procs	= { &StreamWrap::Close, &StreamWrap::Length, &StreamWrap::Read, &StreamWrap::Seek };
		HSTREAM			bass_stream	= bass.StreamCreateFileUser( STREAMFILE_NOBUFFER, BASS_STREAM_DECODE, &file_procs, &inStream );
		HENCODE			bass_enc	= 0;
		CHECK_ERR( bass_stream != 0 );

		ON_DESTROY( [&bass_stream] { bass.StreamFree( bass_stream ); });

		float	bitrate = 0.0f;
		CHECK_ERR( bass.ChannelGetAttribute( bass_stream, BASS_ATTRIB_BITRATE, OUT &bitrate ) == TRUE );

		//float	freq = 0.0f;
		//bass.ChannelGetAttribute( bass_stream, BASS_ATTRIB_FREQ, OUT &freq );

		switch_enum( outputFormat )
		{
			case EAudioFormat::OGG :
			{
				int		qual = 3;
				switch ( quality )
				{
					case EAudioQuality::Highest :	qual = 10;	break;
					case EAudioQuality::High :		qual = 7;	break;
					case EAudioQuality::Medium :	qual = 5;	break;
					case EAudioQuality::Low :		qual = 3;	break;
					case EAudioQuality::Lowest :	qual = -1;	break;
				}

				String	options;
				options << " --bitrate "s << ToString(uint(bitrate));					// in Kb/s
				options << " --max-bitrate "s << ToString(Max( 320u, uint(bitrate) ));	// in Kb/s
				options << " --quality " << ToString(qual);								// in range -1..10
				//options << " --resample " << ToString(uint(freq));					// in Hz
				AE_LOG_DBG( "BASS OGG encoder options: "s << options );

				bass_enc = bass.Encode_OGG_Start( bass_stream, options.c_str(), 0, StreamWrap::Encode, &outStream );
				CHECK_ERR( bass_enc != 0 );
				break;
			}
			case EAudioFormat::RAW :
			default :
				RETURN_ERR( "unknown output format" );
		}
		switch_end

		for (; bass.ChannelIsActive( bass_stream );)
		{
			DWORD	err = bass.ChannelGetData( bass_stream, OUT buffer.ptr, (DWORD(buffer.size) & _LengthMask) | BASS_DATA_AVAILABLE );

			if ( err == UMax )
				BASS_CheckError();

			CHECK_ERR( bass.Encode_IsActive( bass_stream ));
		}

		CHECK_ERR( bass.Encode_Stop( bass_stream ));

		return true;
	}
*/

} // AE::Audio

#endif // AE_ENABLE_BASS

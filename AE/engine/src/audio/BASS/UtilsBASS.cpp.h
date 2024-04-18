// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#define NOBASSOVERLOADS

#ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4668)
#	include "bass.h"
#	include "bassenc.h"
#	include "bassenc_ogg.h"
#	pragma warning (pop)
#else
#	include "bass.h"
#	include "bassenc.h"
#	include "bassenc_ogg.h"
#endif

#include "base/Defines/Undef.h"
#include "audio/Public/IAudioData.h"

#define BASS_CHECK( __VA_ARGS__ )									\
	{																\
		BOOL	ok = (__VA_ARGS__);									\
		DEBUG_ONLY( if ( not ok ) {									\
			AE::Audio::BASS_CheckError();							\
			ASSERT_MSG( false, AE_TOSTRING( __VA_ARGS__ ));			\
		})															\
		Unused( ok );												\
	}

#define BASS_CHECK_ERR( __VA_ARGS__ )								\
	{																\
		BOOL	ok = (__VA_ARGS__);									\
		DEBUG_ONLY( if ( not ok )  AE::Audio::BASS_CheckError() );	\
		CHECK_ERR_MSG( ok, AE_TOSTRING( __VA_ARGS__ ));				\
	}

namespace AE::Audio
{
	bool  BASS_CheckError () __NE___;

	struct BASSFunctions
	{
	public:
		#define BASS_FN( _builder_ )\
			_builder_( GetVersion )\
			_builder_( Init )\
			_builder_( Free )\
			_builder_( SetConfig )\
			_builder_( StreamFree )\
			_builder_( RecordFree )\
			_builder_( RecordInit )\
			_builder_( RecordSetDevice )\
			_builder_( RecordGetInfo )\
			_builder_( RecordStart )\
			_builder_( RecordGetDevice )\
			_builder_( RecordGetDeviceInfo )\
			_builder_( ChannelFree )\
			_builder_( ChannelBytes2Seconds )\
			_builder_( ChannelSeconds2Bytes )\
			_builder_( ChannelStart )\
			_builder_( ChannelStop )\
			_builder_( ChannelPause )\
			_builder_( ChannelPlay )\
			_builder_( ChannelIsActive )\
			_builder_( ChannelFlags )\
			_builder_( ChannelSetAttribute )\
			_builder_( ChannelGetAttribute )\
			_builder_( ChannelSetPosition )\
			_builder_( ChannelGetPosition )\
			_builder_( ChannelGetLength )\
			_builder_( ChannelSet3DPosition )\
			_builder_( ChannelGet3DPosition )\
			_builder_( ChannelGetInfo )\
			_builder_( GetVolume )\
			_builder_( SetVolume )\
			_builder_( Start )\
			_builder_( Pause )\
			_builder_( IsStarted )\
			_builder_( Set3DPosition )\
			_builder_( Get3DPosition )\
			_builder_( Apply3D )\
			_builder_( StreamCreateFileUser )\
			_builder_( GetDeviceInfo )\
			_builder_( ErrorGetCode )\
			_builder_( SampleGetChannel )\
			_builder_( SampleGetInfo )\
			_builder_( SampleLoad )\
			_builder_( SampleFree )

		#define BASS_ENC_FN( _builder_ )\
			_builder_( Encode_GetVersion )

		#define BASS_ENC_OGG_FN( _builder_ )\
			_builder_( Encode_OGG_GetVersion )


		#define BASS_DECL_FN( _name_ )	static inline decltype(&BASS_##_name_)  _name_ = null;
		BASS_FN( BASS_DECL_FN )
		BASS_ENC_FN( BASS_DECL_FN )
		BASS_ENC_OGG_FN( BASS_DECL_FN )
		#undef BASS_DECL_FN

	private:
		friend class AudioSystemBASS;
		ND_ static bool  _Load ()	__NE___;
			static void  _Unload ()	__NE___;
	};
	static BASSFunctions	bass;


/*
=================================================
	SampleGetInfo
=================================================
*/
	inline void  SampleGetInfo (DWORD handle, OUT AudioDataDesc &dst) __NE___
	{
		BASS_SAMPLE	src = {};
		BASS_CHECK( bass.SampleGetInfo( handle, OUT &src ));

		dst				= Default;
		dst.duration	= Seconds{ float( bass.ChannelBytes2Seconds( handle, src.length ))};
		dst.freq		= Freq{ src.freq };
		dst.size		= Bytes{ src.length };
		dst.channels	= CheckCast<ubyte>( src.chans );

		switch ( src.origres )
		{
			case 0 :	dst.sampleFormat = Default;					break;
			case 8 :	dst.sampleFormat = ESampleFormat::UInt8;	ASSERT( AllBits( src.flags, BASS_SAMPLE_8BITS ));	break;
			case 16 :	dst.sampleFormat = ESampleFormat::UInt16;	break;
			case 32 :	dst.sampleFormat = ESampleFormat::Float32;	ASSERT( AllBits( src.flags, BASS_SAMPLE_FLOAT ));	break;
			default :	DBG_WARNING( "unsupported format" );		break;
		}

		if ( dst.sampleFormat == Default )
		{
			if ( AllBits( src.flags, BASS_SAMPLE_8BITS ))	dst.sampleFormat = ESampleFormat::UInt8;	else
			if ( AllBits( src.flags, BASS_SAMPLE_FLOAT ))	dst.sampleFormat = ESampleFormat::Float32;	else
															dst.sampleFormat = ESampleFormat::UInt16;
		}
	}

/*
=================================================
	ChannelGetInfo
=================================================
*/
	inline void  ChannelGetInfo (DWORD handle, OUT AudioDataDesc &dst) __NE___
	{
		BASS_CHANNELINFO	src = {};
		BASS_CHECK( bass.ChannelGetInfo( handle, OUT &src ));

		dst				= Default;
		dst.freq		= Freq{ src.freq };
		dst.channels	= CheckCast<ubyte>( src.chans );

		switch ( src.origres )
		{
			case 0 :	dst.sampleFormat = Default;					break;
			case 8 :	dst.sampleFormat = ESampleFormat::UInt8;	ASSERT( AllBits( src.flags, BASS_SAMPLE_8BITS ));	break;
			case 16 :	dst.sampleFormat = ESampleFormat::UInt16;	break;
			case 32 :	dst.sampleFormat = ESampleFormat::Float32;	ASSERT( AllBits( src.flags, BASS_SAMPLE_FLOAT ));	break;
			default :	DBG_WARNING( "unsupported format" );		break;
		}

		if ( dst.sampleFormat == Default )
		{
			if ( AllBits( src.flags, BASS_SAMPLE_8BITS ))	dst.sampleFormat = ESampleFormat::UInt8;	else
			if ( AllBits( src.flags, BASS_SAMPLE_FLOAT ))	dst.sampleFormat = ESampleFormat::Float32;	else
															dst.sampleFormat = ESampleFormat::UInt16;
		}
	}


} // AE::Audio

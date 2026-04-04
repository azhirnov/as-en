// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;


#ifdef AE_ENABLE_FFMPEG

namespace
{
	using Allocator_t		= LinearAllocator<>;


	static const auto		c_Codec		= EVideoCodec::H265;
	static const StringView	c_CodecName	= "hevc";


	class VideoWStream final : public WStream
	{
		friend class VideoRStream;

	private:
		Array<ubyte>	_data;
		Bytes			_pos;
		Bytes			_lastPos;


	public:
		VideoWStream () __NE___ {}

		bool			IsOpen ()				C_NE_OV	{ return true; }
		ESourceType		GetSourceType ()		C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::RandomAccess; }
		ReqAlign		DirectAccessAlign ()	C_NE_OV	{ return Default; }
		Bytes			Position ()				C_NE_OV	{ return _pos; }
		bool			IsChanged ()			__NE___	{ bool changed = _pos != _lastPos;  _lastPos = _pos;  return changed; }

		bool  SeekFwd (Bytes offset)  __NE_OV
		{
			if_unlikely( (_pos + offset) > _data.size() )
				return false;

			_pos += offset;
			return true;
		}

		bool  UpdateAt (Bytes pos)  __NE_OV
		{
			if_unlikely( pos > _data.size() )
				return false;

			_pos = pos;
			return true;
		}

		Bytes  WriteSeq (const void* buffer, Bytes size)  __NE_OV
		{
			if_unlikely( _pos + size > _data.size() )
			{
				_data.resize( usize{_pos + size} );
			}

			ASSERT( _pos + size <= _data.size() );
			MemCopy( OUT _data.data() + _pos, buffer, size );

			_pos += size;
			return size;
		}

		void  Flush ()  __NE_OV
		{
			ASSERT_MSG( false, "unused" );
		}
	};


	class VideoRStream final : public RStream
	{
	private:
		VideoWStream &		_wstream;
		Bytes				_pos;
		Bytes				_reqCapacity;

	public:
		VideoRStream (VideoWStream &wstream) __NE___ : _wstream{wstream}
		{}

		bool			IsOpen ()				C_NE_OV	{ return true; }
		ESourceType		GetSourceType ()		C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::RandomAccess; }
		ReqAlign		DirectAccessAlign ()	C_NE_OV	{ return Default; }
		PosAndSize		PositionAndSize ()		C_NE_OV	{ return { _pos, _Size() }; }

		bool  HasEnoughData ()  __NE___
		{
			return _pos + _Size() >= _reqCapacity;
		}

		bool  SeekFwd (Bytes offset)  __NE_OV
		{
			Bytes	pos = Min( offset + _pos, _Size() );
			ASSERT( _pos == pos );
			return _pos == pos;
		}

		bool  SeekSet (Bytes newPos)  __NE_OV
		{
			_pos = Min( newPos, _Size() );
			ASSERT( _pos == newPos );
			return _pos == newPos;
		}

		Bytes  ReadSeq (OUT void* buffer, const Bytes reqSize)  __NE_OV
		{
			Bytes	size = Min( reqSize, _Size() - _pos );
			if ( size != reqSize )
				_reqCapacity = _pos + reqSize;
			else
				_reqCapacity = 0_b;

			MemCopy( OUT buffer, _DataPtr() + _pos, size );
			_pos += size;

			return size;
		}

	private:
		Bytes			_Size ()		C_NE___ { return Bytes{_wstream._data.size()}; }
		const void*		_DataPtr ()		C_NE___	{ return _wstream._data.data(); }
	};


	static void  FFmpegStream_Test1 ()
	{
		const uint	fps				= 40;
		const uint	frame_count		= fps * 10;
		const auto	format			= EPixelFormat::RGBA8_UNorm;
		const bool	is_bitstream	= true;

		auto	wstream = MakeRC<VideoWStream>();
		auto	rstream = MakeRC<VideoRStream>( *wstream );

		// encode & decode
		{
			auto	enc = VideoFactory::CreateFFmpegEncoder();
			TEST( enc );
			{
				IVideoEncoder::Config	cfg;
				cfg.srcFormat	= format;
				cfg.dstFormat	= EVideoFormat::YUV420P;
				cfg.colorPreset	= EColorPreset::JPEG_BT709;
				cfg.srcDim		= uint2{320, 320};
				cfg.dstDim		= cfg.srcDim;
				cfg.codec		= c_Codec;
				cfg.filter		= Video::EFilter::Bilinear;
				cfg.quality		= 0.5f;
				cfg.framerate	= FrameRate{ fps };
				cfg.bitrate		= BitrateKib{ fps/2 };
				cfg.flags		= is_bitstream ? EEncoderFlags::Bitstream : EEncoderFlags::Fragmented;

				TEST( enc->Begin( cfg, wstream ));
			}

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	enc_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, format, EImageAspect::Color };

			ImageMemView	dec_view;
			Allocator_t		alloc;

			IVideoDecoder::FrameInfo	fi;


			RC<IVideoDecoder>	dec;
			const auto	InitDecoder = [&] ()
			{{
				// at begin will try read ~12KiB
				if ( rstream->Size() < 4_KiB )
					return;

				dec = VideoFactory::CreateFFmpegDecoder();
				TEST( dec );
				{
					IVideoDecoder::Config	cfg;
					cfg.dstFormat	= format;

					TEST( dec->Begin( cfg, rstream ));
				}

				TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT dec_view, alloc ));
			}};

			const auto	Decode = [&] (INOUT uint &i)
			{{
				if ( not rstream->HasEnoughData() )
					return;

				if ( not dec )
				{
					InitDecoder();
					if ( not dec )
						return;
				}

				// decode
				auto err = dec->GetVideoFrame( INOUT dec_view, OUT fi );

				if ( err == EResult::EndOfFile )
				{
					TEST( not rstream->HasEnoughData() );
					return;
				}

				TEST( err == EResult::OK );
				TEST( dec_view.Format() == format );

				/*if ( not is_bitstream )
				{
					auto	tp = Seconds{ double(i) / fps };
					TEST( BitEqual( fi.timestamp, tp ));
					TEST_Eq( fi.frameIdx, i );
				}*/
				++i;
			}};


			uint	read_fid = 0;
			for (uint write_fid = 0; write_fid < frame_count; ++write_fid)
			{
				const RGBA8u	col	{RainbowWrap( write_fid * 0.2f )};
				for (auto& c : pixels) { c = col; }

				TEST( enc->AddFrame( enc_view, True{} ));

				if ( not wstream->IsChanged() )
					continue;

				Decode( INOUT read_fid );
			}
			TEST( enc->End() );
			TEST( wstream->IsChanged() );


			for (; read_fid < frame_count;)
			{
				Decode( INOUT read_fid );
			}

			auto	err = dec->GetVideoFrame( INOUT dec_view, OUT fi );
			TEST( err == EResult::EndOfFile );
			TEST( dec->End() );

			TEST( wstream->Position() == rstream->Position() );
		}
	}
}


extern void  Test_FFmpegStream ()
{
	FFmpegStream_Test1();

	TEST_PASSED();
}

#else

extern void  Test_FFmpegStream ()
{}

#endif // AE_ENABLE_FFMPEG

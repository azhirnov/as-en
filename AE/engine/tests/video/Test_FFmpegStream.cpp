// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;


#ifdef AE_ENABLE_FFMPEG

namespace
{
	using Allocator_t		= LinearAllocator<>;


	class VideoWStream final : public WStream
	{
	private:
		Array<ubyte>	_data;
		Bytes			_dataOffset;
		Bytes			_pos;
		Bytes			_lastPos;


	public:
		VideoWStream ()							__NE___ {}

		bool			IsOpen ()				C_NE_OV	{ return true; }
		ESourceType		GetSourceType ()		C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::RandomAccess; }
		ReqAlign		DirectAccessAlign ()	C_NE_OV	{ return Default; }
		Bytes			Position ()				C_NE_OV	{ return _pos; }
		Bytes			Delta ()				C_NE___	{ return _pos - _lastPos; }

		Bytes			ActualSize ()			C_NE___	{ return _dataOffset + _data.size(); }
		Range<Bytes>	Available ()			C_NE___	{ return Range{ _dataOffset, ActualSize() }; }

		ArrayView<ubyte>	Data (Bytes offset)	C_NE___
		{
			CHECK( offset >= _dataOffset );
			offset -= _dataOffset;
			return ArrayView<ubyte>{ _data.data() + offset, _data.size() - usize{offset} };
		}

		bool  IsChanged ()						__NE___
		{
			bool changed = Delta() != 0;
			_lastPos = _pos;
			return changed;
		}

		bool  SeekFwd (Bytes offset)			__NE_OV
		{
			CHECK_ERR( Available().ContainsOrEqual( _pos + offset ));

			_pos += offset;
			return true;
		}

		bool  UpdateAt (Bytes pos)				__NE_OV
		{
			CHECK_ERR( Available().ContainsOrEqual( pos ));

			_pos = pos;
			return true;
		}

		Bytes  WriteSeq (const void* buffer, Bytes size)  __NE_OV
		{
			if_unlikely( _pos + size > ActualSize() )
			{
				_data.resize( usize{_pos + size - _dataOffset} );
			}

			ASSERT( _pos + size <= ActualSize() );
			ASSERT( _pos >= _dataOffset );

			MemCopy( OUT _data.data() + _pos - _dataOffset, buffer, size );

			_pos += size;
			return size;
		}

		void  Flush ()  __NE_OV
		{
			ASSERT_MSG( false, "unused" );
		}

		void  FreeData (Bytes pos)
		{
			CHECK_ERRV( pos >= _dataOffset );
			CHECK_ERRV( pos <= _pos );

			_data.erase( _data.begin(), _data.begin() + usize{pos - _dataOffset} );
			_dataOffset = pos;

			ASSERT( ActualSize() >= _pos );
		}
	};


	class VideoRStream final : public RStream
	{
	private:
		VideoWStream &		_wstream;
		Bytes				_pos;
		Bytes				_reqCapacity;
		bool				_decodingStarted	= false;

	public:
		VideoRStream (VideoWStream &wstream)	__NE___ : _wstream{wstream}
		{}

		bool			IsOpen ()				C_NE_OV	{ return true; }
		ESourceType		GetSourceType ()		C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::RandomAccess; }
		ReqAlign		DirectAccessAlign ()	C_NE_OV	{ return Default; }

		// ffmpeg Read() wrapper will return EOF until decoder is not created.
		// It used to avoid hang in decoder initialization.
		PosAndSize		PositionAndSize ()		C_NE_OV	{ return { _pos, _decodingStarted ? UMax : ActualSize() }; }

		Bytes			ActualSize ()			C_NE___	{ return _wstream.ActualSize(); }
		Range<Bytes>	Available ()			C_NE___	{ return _wstream.Available(); }

		void			StartDecoding ()		__NE___	{ _decodingStarted = true; }

		bool			HasEnoughData ()		C_NE___	{ return ActualSize() >= _reqCapacity; }

		bool  SeekFwd (Bytes)  __NE_OV
		{
			ASSERT_MSG( false, "unused" );
			return false;
		}

		bool  SeekSet (Bytes newPos)  __NE_OV
		{
			CHECK_ERR( not _decodingStarted );
			CHECK_ERR( Available().ContainsOrEqual( newPos ));

			_pos = newPos;
			return true;
		}

		Bytes  ReadSeq (OUT void* buffer, const Bytes reqSize)  __NE_OV
		{
			Bytes	size = Min( reqSize, ActualSize() - _pos );
			if ( size != reqSize )
				_reqCapacity = _pos + reqSize;
			else
				_reqCapacity = 0_b;

			auto arr = _wstream.Data( _pos );
			CHECK( arr.size() >= size );

			MemCopy( OUT buffer, arr.data(), size );
			_pos += size;

			return size;
		}

		void  FreeConsumedData ()
		{
			_wstream.FreeData( _pos );
		}
	};


	static void  FFmpegStream_Test1 ()
	{
		const uint	dim				= 640;
		const uint	fps				= 40;
		const uint	frame_count		= fps * 10;
		const auto	format			= EPixelFormat::RGBA8_UNorm;
		const auto	codec			= EVideoCodec::H264;		// TODO: H265 low latency
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
				cfg.srcDim		= uint2{dim, dim};
				cfg.dstDim		= cfg.srcDim;
				cfg.codec		= codec;
				cfg.filter		= Video::EFilter::Bilinear;
				cfg.quality		= 0.5f;
				cfg.framerate	= FrameRate{ fps };
				cfg.bitrate		= BitrateKib{ fps/2 * dim/10 };
				cfg.flags		= is_bitstream ?
									EEncoderFlags::Bitstream | EEncoderFlags::LowLatency :
									EEncoderFlags::Fragmented;
				cfg.hwAccelerated = EHwAcceleration::Disable;	// required for LL
				cfg.ioBufferSize = 4_KiB;

				TEST( enc->Begin( cfg, wstream ));
			}

			Array<RGBA8u>	pixels;	pixels.resize( dim * dim );
			ImageMemView	enc_view{ pixels.data(), ArraySizeOf(pixels), uint3{}, uint3{dim, dim, 1}, 0_b, 0_b, format, EImageAspect::Color };

			ImageMemView	dec_view;
			Allocator_t		alloc;

			IVideoDecoder::FrameInfo	fi;
			uint						max_decoded_frames			= 0;
			uint						last_wfile_update_in_frame	= 0;

			RC<IVideoDecoder>	dec;
			const auto	InitDecoder = [&] ()
			{{
				if ( not rstream->HasEnoughData() )
					return;

				auto	temp = VideoFactory::CreateFFmpegDecoder();
				TEST( temp );
				{
					IVideoDecoder::Config	cfg;
					cfg.dstFormat	= format;

					if ( not temp->Begin( cfg, rstream ))
					{
						TEST( rstream->SeekSet( 0_b ));
						return;
					}

					dec = temp;
					rstream->StartDecoding();
				}

				TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT dec_view, alloc ));
			}};

			const auto	Decode = [&] (INOUT uint &i)
			{{
				if ( not dec )
				{
					InitDecoder();
					if ( not dec )
						return;
				}

				const uint	prev_i = i;

				// decode
				for (;;)
				{
					auto err = dec->GetVideoFrame( INOUT dec_view, OUT fi );

					if ( err == EResult::NeedMoreData )
						break;

					if ( err == EResult::EndOfFile )
					{
						TEST( not rstream->HasEnoughData() );
						break;
					}

					TEST( err == EResult::OK );
					TEST( dec_view.Format() == format );
					TEST_Eq( i, fi.frameIdx );

					rstream->FreeConsumedData();
					++i;
				}

				uint	di = i - prev_i;
				max_decoded_frames = Max( max_decoded_frames, di );
			}};


			uint	read_fid = 0;
			for (uint write_fid = 0; write_fid < frame_count; ++write_fid)
			{
				const RGBA8u	col	{RainbowWrap( write_fid * 0.2f )};
				for (auto& c : pixels) { c = col; }

				TEST( enc->AddFrame( enc_view, True{} ));

				AE_LOGI( "Stream size delta: "s << ToString( wstream->Delta() ) << ", write: " << ToString( write_fid ) <<
						 ", read: " << ToString( read_fid ) << ", delay " << ToString( write_fid - read_fid ));

				if ( not wstream->IsChanged() )
					continue;

				AE_LOGI( "  Flush encoder stream, delay "s << ToString( write_fid - last_wfile_update_in_frame ));
				last_wfile_update_in_frame = write_fid;

				Decode( INOUT read_fid );
			}
			TEST( enc->End() );

			// for low latency should be near to 1
			CHECK_Eq( max_decoded_frames, 1 );

			for (; read_fid < frame_count;)
			{
				uint	last_fid = read_fid;
				Decode( INOUT read_fid );

				if ( last_fid == read_fid )
				{
					AE_LOGW( "No progress after "s << ToString(read_fid) << " frames" );
					break;
				}
			}

			AE_LOGI( "Write frames "s << ToString(frame_count) << ", read frames " << ToString(read_fid) );

			auto	err = dec->GetVideoFrame( INOUT dec_view, OUT fi );
			TEST( AnyEqual( err, EResult::NeedMoreData, EResult::EndOfFile ));
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

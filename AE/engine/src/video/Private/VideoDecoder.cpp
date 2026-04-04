// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Public/VideoDecoder.h"
#include "video/Private/EnumToString.cpp.h"

namespace AE::Video
{
	using namespace AE::Graphics;

namespace
{
	static constexpr auto	c_FFmpegMinPixAlign = 16_b;
}

/*
=================================================
	Properties::ToString
=================================================
*/
#ifdef AE_ENABLE_LOGS
	String  IVideoDecoder::Properties::ToString () C_Th___
	{
		String	str;
		if ( videoStream.IsValid() )
		{
			str << "\n  codecName:     " << StringView{videoStream.codecName}
				<< "\n  index:   . . . " << Base::ToString( videoStream.index )
				<< "\n  type:          " << Base::ToString( videoStream.type )
				<< "\n  codec:   . . . " << Base::ToString( videoStream.codec )
				<< "\n  pixFormat:     " << Base::ToString( videoStream.pixFormat )
				<< "\n  videoFormat:   " << Base::ToString( videoStream.videoFormat )
				<< "\n  colorPreset:   " << Base::ToString( videoStream.colorPreset )
				<< "\n  frameCount:  . " << Base::ToString( videoStream.frameCount )
				<< "\n  duration:      " << Base::ToString_HMS( videoStream.duration.GetScaled() )
				<< "\n  avgFrameRate:  " << Base::ToString( videoStream.avgFrameRate.ToFloat<float>() ) << " (" << Base::ToString( videoStream.avgFrameRate ) << ')'
				<< "\n  minFrameRate:  " << Base::ToString( videoStream.minFrameRate.ToFloat<float>() ) << " (" << Base::ToString( videoStream.minFrameRate ) << ')'
				<< "\n  bitrate:   . . " << Base::ToString( videoStream.bitrate.GetScaled() )
				<< "\n  dimension:     " << Base::ToString( videoStream.dimension.x ) << 'x' << Base::ToString( videoStream.dimension.y )
				<< "\n  xChromaOffset: " << Base::ToString( videoStream.xChromaOffset )
				<< "\n  yChromaOffset: " << Base::ToString( videoStream.yChromaOffset )
				<< "\n  ycbcrRange:  . " << Base::ToString( videoStream.ycbcrRange )
				<< "\n--------";
		}

		// TODO: audio stream

		return str;
	}
#endif
/*
=================================================
	Properties::GetStream
=================================================
*/
	IVideoDecoder::StreamInfo const*  IVideoDecoder::Properties::GetStream (const int idx) C_NE___
	{
		if ( videoStream.index == idx )
			return &videoStream;

		for (auto& s : audioStreams)
		{
			if_unlikely( s.index == idx )
				return &s;
		}
		return null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	PrintFileProperties
=================================================
*/
	String  IVideoDecoder::PrintFileProperties (const Path &filename, const CodecConfig &cfg) C_Th___
	{
		return GetFileProperties( filename, cfg ).ToString();
	}

/*
=================================================
	PrintFileProperties
=================================================
*/
	String  IVideoDecoder::PrintFileProperties (RC<RStream> stream, const CodecConfig &cfg) C_Th___
	{
		return GetFileProperties( RVRef(stream), cfg ).ToString();
	}

/*
=================================================
	AllocMemView
=================================================
*/
	bool  IVideoDecoder::AllocMemView (OUT ImageMemView		&memView,
									   EPixelFormat			format,
									   const uint2			&originDim,
									   IAllocator			&allocator,
									   Bytes				minAlign) __NE___
	{
		minAlign = Max( minAlign, c_FFmpegMinPixAlign );

		auto&	fmt_info	= EPixelFormat_GetInfo( format );
		CHECK_ERR( fmt_info.PlaneCount() <= 1 );

		Bytes	row_pitch	= ImageUtils::RowSize( originDim.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
		Bytes	img_size	= row_pitch * originDim.y;
		void*	data		= allocator.Allocate( SizeAndAlign{ img_size, minAlign });
		CHECK_ERR( data != null );

		memView = ImageMemView{ data, img_size, uint3{}, uint3{originDim, 1}, row_pitch, img_size, format, EImageAspect::Color };
		return true;
	}

/*
=================================================
	AllocMemView
=================================================
*/
	bool  IVideoDecoder::AllocMemView (OUT ImagePlanesMemView	&memViewArr,
									   const EPixelFormat		format,
									   const uint2				&originDim,
									   IAllocator				&allocator,
									   Bytes					minAlign) __NE___
	{
		const auto	AllocPlane	= [&] (EImageAspect aspect) -> bool
		{{
			EPixelFormat	plane_fmt	= format;
			POTVec2			plane_scale;

			if ( aspect != EImageAspect::Color ) {
				CHECK_ERR( EPixelFormat_GetPlaneInfo( format, aspect, OUT plane_fmt, OUT plane_scale ));
				CHECK_ERR( All( IsMultipleOf( originDim, plane_scale )));
			}

			const uint2		dim			= originDim / plane_scale;
			auto&			plane_info	= EPixelFormat_GetInfo( plane_fmt );
			const Bytes		row_pitch	= ImageUtils::RowSize( dim.x, plane_info.bitsPerBlock, plane_info.TexBlockDim() );
			const Bytes		img_size	= row_pitch * dim.y;
			const Bytes		align		= Max( minAlign, c_FFmpegMinPixAlign, plane_info.BytesPerBlock() );
			void*			data		= allocator.Allocate( SizeAndAlign{ img_size, align });
			CHECK_ERR( data != null );

			memViewArr.push_back( ImageMemView{ data, img_size, uint3{}, uint3{dim, 1},
												row_pitch, img_size, plane_fmt, aspect });
			return true;
		}};

		const auto&	fmt_info	= EPixelFormat_GetInfo( format );
		const uint	plane_count	= Max( 1u, fmt_info.PlaneCount() );
		const bool	multiplanar	= fmt_info.IsMultiPlanar();
		bool		ok			= true;

		CHECK_ERR( plane_count >= 1 and plane_count <= 3 );
		CHECK_ERR( (not multiplanar) == (plane_count == 1) );

		memViewArr.clear();

		for (uint plane = 0; ok and plane < plane_count; ++plane)
		{
			const auto	aspect = multiplanar ? EImageAspect_Plane( plane ) : EImageAspect::Color;
			ok = AllocPlane( aspect );
		}

		if_unlikely( not ok )
		{
			for (auto& view : memViewArr)
			{
				auto&	part = view.Parts().front();
				allocator.Deallocate( part.ptr, part.size );
			}
			memViewArr.clear();
		}
		return ok;
	}

/*
=================================================
	DeallocMemView
=================================================
*/
	void  IVideoDecoder::DeallocMemView (INOUT ImageMemView &memView, IAllocator &allocator) __NE___
	{
		for (auto& part : memView.Parts()) {
			allocator.Deallocate( part.ptr, part.size );
		}
	}

	void  IVideoDecoder::DeallocMemView (INOUT ImagePlanesMemView &memView, IAllocator &allocator) __NE___
	{
		for (auto& plane : memView) {
			DeallocMemView( plane, allocator );
		}
	}

} // AE::Video

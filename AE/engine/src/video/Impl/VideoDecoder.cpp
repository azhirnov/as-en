// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Impl/EnumToString.cpp.h"
#include "video/Public/VideoDecoder.h"

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
	String  IVideoDecoder::Properties::ToString () C_Th___
	{
		String	str;
		for (auto& info : streams)
		{
			str << "\n  codecName:    " << StringView{info.codecName}
				<< "\n  index:  . . . " << Base::ToString( info.index )
				<< "\n  type:         " << Base::ToString( info.type )
				<< "\n  codec:  . . . " << Base::ToString( info.codec )
				<< "\n  pixFormat:    " << Base::ToString( info.pixFormat )
				<< "\n  videoFormat:  " << Base::ToString( info.videoFormat )
				<< "\n  colorPreset:  " << Base::ToString( info.colorPreset )
				<< "\n  frameCount: . " << Base::ToString( info.frameCount )
				<< "\n  duration:     " << Base::ToString_HMS( info.duration.GetScaled() )
				<< "\n  avgFrameRate: " << Base::ToString( info.avgFrameRate )
				<< "\n  minFrameRate: " << Base::ToString( info.minFrameRate )
				<< "\n  bitrate:  . . " << Base::ToString( info.bitrate.GetScaled() )
				<< "\n  dimension:    " << Base::ToString( info.dimension )
				<< "\n--------";
		}
		return str;
	}

/*
=================================================
	Properties::GetStream
=================================================
*/
	IVideoDecoder::StreamInfo const*  IVideoDecoder::Properties::GetStream (const int idx) C_NE___
	{
		for (auto& s : streams)
		{
			if_unlikely( s.index == idx )
				return &s;
		}
		return null;
	}

/*
=================================================
	AllocMemView
=================================================
*/
	bool  IVideoDecoder::AllocMemView (const Config			&cfg,
									   OUT ImageMemView		&memView,
									   IAllocator			&allocator,
									   Bytes				minAlign) __NE___
	{
		minAlign = Max( minAlign, c_FFmpegMinPixAlign );

		auto&	fmt_info	= EPixelFormat_GetInfo( cfg.dstFormat );
		CHECK_ERR( fmt_info.PlaneCount() <= 1 );

		Bytes	row_pitch	= ImageUtils::RowSize( cfg.dstDim.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
		Bytes	img_size	= row_pitch * cfg.dstDim.y;
		void*	data		= allocator.Allocate( SizeAndAlign{ img_size, minAlign });
		CHECK_ERR( data != null );

		memView = ImageMemView{ data, img_size, uint3{}, uint3{cfg.dstDim, 1}, row_pitch, img_size, cfg.dstFormat, EImageAspect::Color };
		return true;
	}

/*
=================================================
	AllocMemView
=================================================
*/
	bool  IVideoDecoder::AllocMemView (const Config			&cfg,
									   OUT ImageMemViewArr	&memViewArr,
									   IAllocator			&allocator,
									   Bytes				minAlign) __NE___
	{
		const auto	AllocPlane	= [&] (EImageAspect aspect) -> bool
		{{
			EPixelFormat	plane_fmt	= cfg.dstFormat;
			uint2			plane_scale	{1,1};

			if ( aspect != EImageAspect::Color ) {
				CHECK_ERR( EPixelFormat_GetPlaneInfo( cfg.dstFormat, aspect, OUT plane_fmt, OUT plane_scale ));
				CHECK_ERR( All( IsMultipleOf( cfg.dstDim, plane_scale )));
			}

			const uint2		dim			= cfg.dstDim / plane_scale;
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

		const auto&	fmt_info	= EPixelFormat_GetInfo( cfg.dstFormat );
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


} // AE::Video

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphics.pch.h"

namespace
{
	static void PixelFormat_Test1 ()
	{
		for (EPixelFormat fmt = EPixelFormat(0); fmt < EPixelFormat::_Count; fmt = EPixelFormat(uint(fmt) + 1))
		{
			auto&	info = EPixelFormat_GetInfo( fmt );
			TEST( info.format == fmt );
			TEST( EPixelFormat_IsYcbcr( fmt ) == info.IsMultiPlanar() );

			const bool	comp =	EPixelFormat_IsBC( fmt )	or
								EPixelFormat_IsETC( fmt )	or
								EPixelFormat_IsEAC( fmt )	or
								EPixelFormat_IsASTC( fmt );
			TEST( info.IsCompressed() == comp );
		}
	}


	static void PixelFormat_Test2 ()
	{
		auto	a1 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_RGB8_UNorm, true );
		TEST( a1 == EPixelFormat::RGB8_UNorm );

		auto	a2 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_RGB8_UNorm, false );
		TEST( a2 == EPixelFormat::RGBA8_UNorm );

		auto	a3 = EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_RGBA_4x4, false );
		TEST( a3 == EPixelFormat::RGBA8_UNorm );
		TEST( a3 == EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_RGBA_4x4, true ));

		auto	a4 = EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_sRGB8_A8_6x6, false );
		TEST( a4 == EPixelFormat::sRGB8_A8 );
		TEST( a4 == EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_sRGB8_A8_6x6, false ));

		auto	a5 = EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_RGBA16F_12x12, false );
		TEST( a5 == EPixelFormat::RGBA16F );
		TEST( a5 == EPixelFormat_ToNoncompressed( EPixelFormat::ASTC_RGBA16F_12x12, false ));

		auto	a6 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_sRGB8, true );
		TEST( a6 == EPixelFormat::sRGB8 );

		auto	a7 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_sRGB8, false );
		TEST( a7 == EPixelFormat::sRGB8_A8 );

		auto	a8 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_RGB8_A1_UNorm, false );
		TEST( a8 == EPixelFormat::RGBA8_UNorm );
		TEST( a8 == EPixelFormat_ToNoncompressed( EPixelFormat::BC1_RGB8_A1_UNorm, true ));

		auto	a9 = EPixelFormat_ToNoncompressed( EPixelFormat::BC1_sRGB8_A1, false );
		TEST( a9 == EPixelFormat::sRGB8_A8 );
		TEST( a9 == EPixelFormat_ToNoncompressed( EPixelFormat::BC1_sRGB8_A1, true ));

		auto	a10 = EPixelFormat_ToNoncompressed( EPixelFormat::BC2_RGBA8_UNorm, false );
		TEST( a10 == EPixelFormat::RGBA8_UNorm );
		TEST( a10 == EPixelFormat_ToNoncompressed( EPixelFormat::BC2_RGBA8_UNorm, true ));

		auto	a11 = EPixelFormat_ToNoncompressed( EPixelFormat::BC2_sRGB8, true );
		TEST( a11 == EPixelFormat::sRGB8 );

		auto	a12 = EPixelFormat_ToNoncompressed( EPixelFormat::BC2_sRGB8, false );
		TEST( a12 == EPixelFormat::sRGB8_A8 );

		auto	a13 = EPixelFormat_ToNoncompressed( EPixelFormat::BC3_RGBA8_UNorm, true );
		TEST( a13 == EPixelFormat::RGBA8_UNorm );
		TEST( a13 == EPixelFormat_ToNoncompressed( EPixelFormat::BC3_RGBA8_UNorm, false ));

		auto	a14 = EPixelFormat_ToNoncompressed( EPixelFormat::BC3_sRGB8, true );
		TEST( a14 == EPixelFormat::sRGB8 );

		auto	a15 = EPixelFormat_ToNoncompressed( EPixelFormat::BC3_sRGB8, false );
		TEST( a15 == EPixelFormat::sRGB8_A8 );

		auto	a16 = EPixelFormat_ToNoncompressed( EPixelFormat::BC4_R8_SNorm, false );
		TEST( a16 == EPixelFormat::R8_SNorm );
		TEST( a16 == EPixelFormat_ToNoncompressed( EPixelFormat::BC4_R8_SNorm, true ));

		auto	a17 = EPixelFormat_ToNoncompressed( EPixelFormat::BC4_R8_UNorm, false );
		TEST( a17 == EPixelFormat::R8_UNorm );
		TEST( a17 == EPixelFormat_ToNoncompressed( EPixelFormat::BC4_R8_UNorm, true ));

		auto	a18 = EPixelFormat_ToNoncompressed( EPixelFormat::BC5_RG8_SNorm, false );
		TEST( a18 == EPixelFormat::RG8_SNorm );
		TEST( a18 == EPixelFormat_ToNoncompressed( EPixelFormat::BC5_RG8_SNorm, true ));

		auto	a19 = EPixelFormat_ToNoncompressed( EPixelFormat::BC5_RG8_UNorm, false );
		TEST( a19 == EPixelFormat::RG8_UNorm );
		TEST( a19 == EPixelFormat_ToNoncompressed( EPixelFormat::BC5_RG8_UNorm, true ));

		auto	a20 = EPixelFormat_ToNoncompressed( EPixelFormat::BC6H_RGB16F, true );
		TEST( a20 == EPixelFormat::RGB16F );

		auto	a21 = EPixelFormat_ToNoncompressed( EPixelFormat::BC6H_RGB16F, false );
		TEST( a21 == EPixelFormat::RGBA16F );

		auto	a22 = EPixelFormat_ToNoncompressed( EPixelFormat::BC6H_RGB16UF, true );
		TEST( a22 == EPixelFormat::RGB16F );

		auto	a23 = EPixelFormat_ToNoncompressed( EPixelFormat::BC6H_RGB16UF, false );
		TEST( a23 == EPixelFormat::RGBA16F );

		auto	a24 = EPixelFormat_ToNoncompressed( EPixelFormat::BC7_RGBA8_UNorm, false );
		TEST( a24 == EPixelFormat::RGBA8_UNorm );
		TEST( a24 == EPixelFormat_ToNoncompressed( EPixelFormat::BC7_RGBA8_UNorm, true ));

		auto	a25 = EPixelFormat_ToNoncompressed( EPixelFormat::BC7_sRGB8_A8, false );
		TEST( a25 == EPixelFormat::sRGB8_A8 );
		TEST( a25 == EPixelFormat_ToNoncompressed( EPixelFormat::BC7_sRGB8_A8, true ));

		auto	a26 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGB8_UNorm, true );
		TEST( a26 == EPixelFormat::RGB8_UNorm );

		auto	a27 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGB8_UNorm, false );
		TEST( a27 == EPixelFormat::RGBA8_UNorm );

		auto	a28 = EPixelFormat_ToNoncompressed( EPixelFormat::ECT2_sRGB8, true );
		TEST( a28 == EPixelFormat::sRGB8 );

		auto	a29 = EPixelFormat_ToNoncompressed( EPixelFormat::ECT2_sRGB8, false );
		TEST( a29 == EPixelFormat::sRGB8_A8 );

		auto	a30 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGB8_A1_UNorm, false );
		TEST( a30 == EPixelFormat::RGBA8_UNorm );
		TEST( a30 == EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGB8_A1_UNorm, true ));

		auto	a31 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_sRGB8_A1, false );
		TEST( a31 == EPixelFormat::sRGB8_A8 );
		TEST( a31 == EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_sRGB8_A1, true ));

		auto	a32 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGBA8_UNorm, false );
		TEST( a32 == EPixelFormat::RGBA8_UNorm );
		TEST( a32 == EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_RGBA8_UNorm, true ));

		auto	a33 = EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_sRGB8_A8, false );
		TEST( a33 == EPixelFormat::sRGB8_A8 );
		TEST( a33 == EPixelFormat_ToNoncompressed( EPixelFormat::ETC2_sRGB8_A8, true ));

		auto	a34 = EPixelFormat_ToNoncompressed( EPixelFormat::EAC_R11_SNorm, false );
		TEST( a34 == EPixelFormat::R16_SNorm );
		TEST( a34 == EPixelFormat_ToNoncompressed( EPixelFormat::EAC_R11_SNorm, true ));

		auto	a35 = EPixelFormat_ToNoncompressed( EPixelFormat::EAC_R11_UNorm, false );
		TEST( a35 == EPixelFormat::R16_UNorm );
		TEST( a35 == EPixelFormat_ToNoncompressed( EPixelFormat::EAC_R11_UNorm, true ));

		auto	a36 = EPixelFormat_ToNoncompressed( EPixelFormat::EAC_RG11_SNorm, false );
		TEST( a36 == EPixelFormat::RG16_SNorm );
		TEST( a36 == EPixelFormat_ToNoncompressed( EPixelFormat::EAC_RG11_SNorm, true ));

		auto	a37 = EPixelFormat_ToNoncompressed( EPixelFormat::EAC_RG11_UNorm, false );
		TEST( a37 == EPixelFormat::RG16_UNorm );
		TEST( a37 == EPixelFormat_ToNoncompressed( EPixelFormat::EAC_RG11_UNorm, true ));
	}


	static void PixelFormat_Test3 ()
	{
		auto	a1 = EPixelFormat_ToRGBA( EPixelFormat::RGB8_UNorm );
		TEST( a1 == EPixelFormat::RGBA8_UNorm );

		auto	a2 = EPixelFormat_ToRGBA( EPixelFormat::RGB16U );
		TEST( a2 == EPixelFormat::RGBA16U );

		auto	a3 = EPixelFormat_ToRGBA( EPixelFormat::RG16F );
		TEST( a3 == EPixelFormat::RGBA16F );

		auto	a4 = EPixelFormat_ToRGBA( EPixelFormat::EAC_RG11_SNorm );
		TEST( a4 == Default );


		auto	b1 = EPixelFormat_ToBC( EPixelFormat::RGBA8_UNorm );
		TEST( b1 == EPixelFormat::BC7_RGBA8_UNorm );

		auto	b2 = EPixelFormat_ToBC( EPixelFormat::sRGB8_A8 );
		TEST( b2 == EPixelFormat::BC7_sRGB8_A8 );


		auto	c1 = EPixelFormat_ToETC_EAC( EPixelFormat::RGBA8_UNorm );
		TEST( c1 == EPixelFormat::ETC2_RGBA8_UNorm );

		auto	c2 = EPixelFormat_ToETC_EAC( EPixelFormat::R16_UNorm );
		TEST( c2 == EPixelFormat::EAC_R11_UNorm );

		auto	c3 = EPixelFormat_ToETC_EAC( EPixelFormat::RG16_UNorm );
		TEST( c3 == EPixelFormat::EAC_RG11_UNorm );


		auto	d1 = EPixelFormat_ToASTC( EPixelFormat::RGBA8_UNorm, {4,4} );
		TEST( d1 == EPixelFormat::ASTC_RGBA_4x4 );

		auto	d2 = EPixelFormat_ToASTC( EPixelFormat::RGBA8_UNorm, {8,8} );
		TEST( d2 == EPixelFormat::ASTC_RGBA_8x8 );

		auto	d3 = EPixelFormat_ToASTC( EPixelFormat::sRGB8_A8, {10,10} );
		TEST( d3 == EPixelFormat::ASTC_sRGB8_A8_10x10 );

		auto	d4 = EPixelFormat_ToASTC( EPixelFormat::RGBA16F, {12,12} );
		TEST( d4 == EPixelFormat::ASTC_RGBA16F_12x12 );
	}


	static void VertexType_Test1 ()
	{
		HashSet<EVertexType>	unique;
		EnumSet<EVertexType>	bits;

		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )										\
			StaticAssert( EVertexType::_name_ < EVertexType::_Count );						\
			TEST( unique.insert( EVertexType::_name_ ).second );							\
			TEST( (bits & EnumSet<EVertexType>{}.insert( EVertexType::_name_ )).None() );	\
			bits.insert( EVertexType::_name_ );

		AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT );
		#undef AE_VERTEXTYPE_VISIT
	}
}


extern void UnitTest_PixelFormat ()
{
	PixelFormat_Test1();
	PixelFormat_Test2();
	PixelFormat_Test3();

	VertexType_Test1();

	TEST_PASSED();
}

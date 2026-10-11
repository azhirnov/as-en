// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"
#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

using namespace AE::PipelineCompiler;


/*
=================================================
	_CompileDecodeAllETC
=================================================
*/
void  Executor::_CompileDecodeAllETC () __Th___
{
	AE_LOGI( "Compile ETC Decoder" );
	CHECK_THROW( ObjectStorage::Instance() );

	_CompileDecodeETC_RGB8();
	_CompileDecodeETC_RGB8A1();
	_CompileDecodeETC_RGBA8();
	_CompileDecodeEAC_R11U();
	_CompileDecodeEAC_R11S();
	_CompileDecodeEAC_RG11U();
	_CompileDecodeEAC_RG11S();

	_CompileHwDecodeETC();
}

/*
=================================================
	_CompileETCDecode
=================================================
*/
void  Executor::_CompileETCDecode (ECompFormat compFmt, StringView decodeFnName) __Th___
{
	auto			block_size	= EPixelFormat_GetInfo( ToPixFmt( compFmt )).BytesPerBlock();
	EPixelFormat	block_fmt	= Default;

	String	src =
"#define USE_LOW_PRECISION	0\n"
"#define AE_LICENSE_MIT\n"
"#include \"Compression/DecodeETC.glsl\"\n\n"

"void  Main ()\n"
"{\n"
"	int2	coord	= int2(gl.GlobalInvocationID.xy);\n"
"	int2	dim		= gl.image.GetSize( d_OutputImg );\n"
"	if ( AllLess( coord, dim ))\n"
"	{\n";

	if ( block_size == 8 ){
		src << "		uint2	block	= gl.image.Load( d_InputBlocks, coord / 4 ).rg;\n";
		block_fmt = EPixelFormat::RG32U;
	}else
	if ( block_size == 16 ){
		src << "		uint4	block	= gl.image.Load( d_InputBlocks, coord / 4 );\n";
		block_fmt = EPixelFormat::RGBA32U;
	}else
		CHECK_THROW( false, "unsupported block size" );

	src <<
"		float4	color	= float4(" <<  decodeFnName << "( block, coord & 3 ));\n"
"		gl.image.Store( d_OutputImg, coord, color );\n"
"	}\n"
"}";

	_CompileCS( src, "SwDec."s << ToString(compFmt),
		[&](AnyTypeRef dsLayoutRef)
		{
			auto	ds_layout = dsLayoutRef.As<DescriptorSetLayoutPtr>();
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_InputBlocks", ArraySize{}, EImageType::Dim2D, block_fmt,		EAccessType::Coherent, EResourceState::ShaderStorage_Read );
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_OutputImg",   ArraySize{}, EImageType::Dim2D, _intermPixFmt,	EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		});
}

/*
=================================================
	_CompileDecodeETC_*
=================================================
*/
void  Executor::_CompileDecodeETC_RGB8 ()
{
	_CompileETCDecode( ECompFormat::ETC2_RGB8_UNorm, "DecodeETC2" );
}

void  Executor::_CompileDecodeETC_RGB8A1 ()
{
	_CompileETCDecode( ECompFormat::ETC2_RGB8_A1_UNorm, "DecodeETC2A1" );
}

void  Executor::_CompileDecodeETC_RGBA8 ()
{
	_CompileETCDecode( ECompFormat::ETC2_RGBA8_UNorm, "DecodeETC2_EAC" );
}

void  Executor::_CompileDecodeEAC_R11U ()
{
	_CompileETCDecode( ECompFormat::EAC_R11_UNorm, "DecodeEAC_Ru" );
}

void  Executor::_CompileDecodeEAC_R11S ()
{
	_CompileETCDecode( ECompFormat::EAC_R11_SNorm, "DecodeEAC_Rs" );
}

void  Executor::_CompileDecodeEAC_RG11U ()
{
	_CompileETCDecode( ECompFormat::EAC_RG11_UNorm, "DecodeEAC_RGu" );
}

void  Executor::_CompileDecodeEAC_RG11S ()
{
	_CompileETCDecode( ECompFormat::EAC_RG11_SNorm, "DecodeEAC_RGs" );
}

/*
=================================================
	_CompileHwDecodeETC
=================================================
*/
void  Executor::_CompileHwDecodeETC ()
{
	if ( not SupportsETC() )
		return;

	const char	src[] = R"(
#include "Math.glsl"

void  Main ()
{
	int2	coord	= int2(gl.GlobalInvocationID.xy);
	int2	dim		= gl.image.GetSize( d_OutputImg );
	if ( AllLess( coord, dim ))
	{
		float4	col	= gl.texture.Fetch( d_InputTex, coord, 0 );
		gl.image.Store( d_OutputImg, coord, col );
	}
}
)";

	_CompileCS( src, "HwDecETC",
		[this](AnyTypeRef dsLayoutRef)
		{
			auto	ds_layout = dsLayoutRef.As<DescriptorSetLayoutPtr>();
			ds_layout->AddSampledImage( EShaderStages::Compute, "d_InputTex",  ArraySize{}, EImageType::Dim2D | EImageType::Float, EResourceState::ShaderSample );
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_OutputImg", ArraySize{}, EImageType::Dim2D, _intermPixFmt, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		});
}
//-----------------------------------------------------------------------------



/*
=================================================
	_CompileEncodeAllETC
=================================================
*/
void  Executor::_CompileEncodeAllETC () __Th___
{
	AE_LOGI( "Compile ETC Encoder" );

	_CompileEncodeETC_RGB8();
	_CompileEncodeEAC_R11U();
}

/*
=================================================
	_CompileETCEncode
=================================================
*/
void  Executor::_CompileETCEncode (ECompFormat compFmt, StringView decodeFnName) __Th___
{
	auto			block_size	= EPixelFormat_GetInfo( ToPixFmt( compFmt )).BytesPerBlock();
	EPixelFormat	block_fmt	= Default;
	StringView		pix_type, swizzle, block_type, block_to_u4;

	if ( block_size == 8 ){
		block_type	= "uint2";
		block_to_u4	= "uint4(block,0,0)";
		block_fmt	= EPixelFormat::RG32U;
	}else
	if ( block_size == 16 ){
		block_type	= "uint4";
		block_to_u4	= "block";
		block_fmt	= EPixelFormat::RGBA32U;
	}else
		CHECK_THROW( false, "unsupported block size" );

	switch ( compFmt )
	{
		case ECompFormat::ETC2_RGB8_UNorm :		pix_type = "float3";	swizzle = ".rgb";	break;
		case ECompFormat::ETC2_RGB8_A1_UNorm :
		case ECompFormat::ETC2_RGBA8_UNorm :	pix_type = "float4";						break;
		case ECompFormat::EAC_R11_UNorm :
		case ECompFormat::EAC_R11_SNorm :		pix_type = "float";		swizzle = ".r";		break;
		case ECompFormat::EAC_RG11_UNorm :
		case ECompFormat::EAC_RG11_SNorm :		pix_type = "float2";	swizzle = ".rg";	break;
		default :								CHECK_THROW( false, "unsupported format" );
	}

	String	src =
"#define USE_LOW_PRECISION	0\n"
"#define AE_LICENSE_MIT\n"
"#include \"Compression/EncodeETC.glsl\"\n\n"

"void  Main ()\n"
"{\n"
"	int2	coord = int2(gl.GlobalInvocationID.xy);\n"
"	"s << pix_type << "	values[16];\n"
"	for (int y = 0; y < 4; ++y)\n"
"	for (int x = 0; x < 4; ++x) {\n"
"		values[x + y*4] = gl.image.Load( d_InputImg, coord*4 + int2(x,y) )" << swizzle << ";\n"
"	}\n"
"	" << block_type << "	block = " << decodeFnName << "( values );\n"
"	gl.image.Store( d_OutputBlocks, coord, " << block_to_u4 << ");\n"
"}";

	_CompileCS( src, "SwEnc."s << ToString(compFmt),
		[&](AnyTypeRef dsLayoutRef)
		{
			auto	ds_layout = dsLayoutRef.As<DescriptorSetLayoutPtr>();
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_OutputBlocks", ArraySize{}, EImageType::Dim2D, block_fmt,		EAccessType::Coherent, EResourceState::ShaderStorage_Write );
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_InputImg",	  ArraySize{}, EImageType::Dim2D, _intermPixFmt,	EAccessType::Coherent, EResourceState::ShaderStorage_Read );
		});
}

/*
=================================================
	_CompileEncodeETC_*
=================================================
*/
void  Executor::_CompileEncodeETC_RGB8 () __Th___
{
	_CompileETCEncode( ECompFormat::ETC2_RGB8_UNorm, "EncodeETC2" );
}

void  Executor::_CompileEncodeEAC_R11U () __Th___
{
	_CompileETCEncode( ECompFormat::EAC_R11_UNorm,	"EncodeEAC_Ru" );
	_CompileETCEncode( ECompFormat::EAC_RG11_UNorm,	"EncodeEAC_RGu" );
}

/*
=================================================
	_GetModeETC2
=================================================
*/
static int  ETC2Branch (uint lo, bool has1BitAlpha)
{
    const uint s0 =  lo        & 0xFFu;
    const uint s1 = (lo >>  8) & 0xFFu;
    const uint s2 = (lo >> 16) & 0xFFu;
    const uint s3 = (lo >> 24) & 0xFFu;

    const bool diffBit = (s3 & 2u) != 0u;
    if ( not has1BitAlpha and not diffBit )
        return 0;

    auto delta3 = [](uint32_t v) -> int {
        int d = int(v & 7u);
        return d >= 4 ? d - 8 : d;
    };

    const int R1 = int(s0 >> 3) + delta3(s0);
    const int G1 = int(s1 >> 3) + delta3(s1);
    const int B1 = int(s2 >> 3) + delta3(s2);

    if ( R1 < 0 or R1 > 31 ) return 1;
    if ( G1 < 0 or G1 > 31 ) return 2;
    if ( B1 < 0 or B1 > 31 ) return 3;
    return 4;
}

int  Executor::_GetModeETC2 (const RGBA32u &block)
{
	return ETC2Branch( block.r, false );
}

int  Executor::_GetModeETC2A1 (const RGBA32u &block)
{
	return ETC2Branch( block.r, true );
}

int  Executor::_GetModeETC2_EAC (const RGBA32u &block)
{
	return ETC2Branch( block.b, false );
}

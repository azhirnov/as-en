// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"
#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

using namespace AE::PipelineCompiler;


/*
=================================================
	_CompileDecodeAllBC
=================================================
*/
void  Executor::_CompileDecodeAllBC () __Th___
{
	AE_LOGI( "Compile BC Decoder" );
	CHECK_THROW( ObjectStorage::Instance() );

	_CompileDecodeBC1();
	_CompileDecodeBC4S();
	_CompileDecodeBC4U();
	_CompileDecodeBC5S();
	_CompileDecodeBC5U();
	_CompileDecodeBC6S();
	_CompileDecodeBC6U();
	_CompileDecodeBC7();
	_CompileHwDecodeBC();
}

/*
=================================================
	_CompileBCDecode
=================================================
*/
void  Executor::_CompileBCDecode (EPixelFormat compFmt, StringView decodeFnName) __Th___
{
	auto			block_size	= EPixelFormat_GetInfo( compFmt ).BytesPerBlock();
	EPixelFormat	block_fmt	= Default;

	String	src =
"#define USE_LOW_PRECISION	0\n"
"#define AE_LICENSE_MIT\n"
"#include \"Compression/DecodeBC.glsl\"\n\n"

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
	_CompileDecodeBC*
=================================================
*/
void  Executor::_CompileDecodeBC1 () __Th___
{
	_CompileBCDecode( EPixelFormat::BC1_RGB8_UNorm, "DecodeBC1" );
}

void  Executor::_CompileDecodeBC4S () __Th___
{
	_CompileBCDecode( EPixelFormat::BC4_R8_SNorm, "DecodeBC4S" );
}

void  Executor::_CompileDecodeBC4U () __Th___
{
	_CompileBCDecode( EPixelFormat::BC4_R8_UNorm, "DecodeBC4U" );
}

void  Executor::_CompileDecodeBC5S () __Th___
{
	_CompileBCDecode( EPixelFormat::BC5_RG8_SNorm, "DecodeBC5S" );
}

void  Executor::_CompileDecodeBC5U () __Th___
{
	_CompileBCDecode( EPixelFormat::BC5_RG8_UNorm, "DecodeBC5U" );
}

void  Executor::_CompileDecodeBC6S () __Th___
{
	_CompileBCDecode( EPixelFormat::BC6H_RGB16F, "DecodeBC6_SF16" );
}

void  Executor::_CompileDecodeBC6U () __Th___
{
	_CompileBCDecode( EPixelFormat::BC6H_RGB16UF, "DecodeBC6_UF16" );
}

void  Executor::_CompileDecodeBC7 () __Th___
{
	_CompileBCDecode( EPixelFormat::BC7_RGBA8_UNorm, "DecodeBC7" );
}

/*
=================================================
	_CompileHwDecodeBC
=================================================
*/
void  Executor::_CompileHwDecodeBC () __Th___
{
	if ( not SupportsBC() )
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

	_CompileCS( src, "HwDecBC",
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
	_CompileEncodeAllBC
=================================================
*/
void  Executor::_CompileEncodeAllBC () __Th___
{
//	AE_LOGI( "Compile BC Encoder" );
}

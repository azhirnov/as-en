// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"
#include "res_pack/pipeline_compiler/ScriptObjects/ObjectStorage.h"

using namespace AE::PipelineCompiler;


/*
=================================================
	_CompileDecodeAllASTC
=================================================
*/
void  Executor::_CompileDecodeAllASTC () __Th___
{
	AE_LOGI( "Compile ASTC Decoder" );
	CHECK_THROW( ObjectStorage::Instance() );

	_CompileDecodeASTC();

	_CompileHwDecodeASTC();
}

/*
=================================================
	_CompileASTCDecode
=================================================
*/
void  Executor::_CompileASTCDecode (ECompFormat compFmt, StringView decodeFnName) __Th___
{
	auto&			fmt_info	= EPixelFormat_GetInfo( ToPixFmt( compFmt ));
	auto			block_size	= fmt_info.BytesPerBlock();
	EPixelFormat	block_fmt	= Default;

	String	src =
"#define USE_LOW_PRECISION	0\n"
"#define AE_LICENSE_MIT\n"
"#include \"Compression/DecodeASTC.glsl\"\n\n"

"void  Main ()\n"
"{\n"
"	int2	coord	= int2(gl.GlobalInvocationID.xy);\n"
"	int2	dim		= gl.image.GetSize( d_OutputImg );\n"
"	if ( AllLess( coord, dim ))\n"
"	{\n"
"		const int2	block_dim   = int2("s << ToString(fmt_info.blockDim.x) << "," << ToString(fmt_info.blockDim.y) << ");\n" <<
"		int2		block_coord = coord / block_dim;\n"
"		int2		local_coord = coord % block_dim;\n";

	if ( block_size == 8 ){
		src << "		uint2	block	= gl.image.Load( d_InputBlocks, block_coord ).rg;\n";
		block_fmt = EPixelFormat::RG32U;
	}else
	if ( block_size == 16 ){
		src << "		uint4	block	= gl.image.Load( d_InputBlocks, block_coord );\n";
		block_fmt = EPixelFormat::RGBA32U;
	}else
		CHECK_THROW( false, "unsupported block size" );

	src <<
"		float4	color	= float4(" <<  decodeFnName << "( block, local_coord ));\n"
"		gl.image.Store( d_OutputImg, coord, color );\n"
"	}\n"
"}";

	_CompileCS( src, "SwDec."s << ToString(compFmt),
		[&](AnyTypeRef dsLayoutRef)
		{
			auto	ds_layout = dsLayoutRef.As<DescriptorSetLayoutPtr>();
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_InputBlocks", ArraySize{}, EImageType::Dim2D, block_fmt,		EAccessType::Coherent, EResourceState::ShaderStorage_Read );
			ds_layout->AddStorageImage( EShaderStages::Compute, "d_OutputImg",   ArraySize{}, EImageType::Dim2D, _intermPixFmt,	EAccessType::Coherent, EResourceState::ShaderStorage_Write );

			ds_layout->AddUniformTexelBuffer( EShaderStages::Compute, "LUTRemainingBitsToEndpointQuant", ArraySize{}, EImageType::UInt, EResourceState::ShaderSample );
			ds_layout->AddUniformTexelBuffer( EShaderStages::Compute, "LUTEndpointUnquantize",			 ArraySize{}, EImageType::UInt, EResourceState::ShaderSample );
			ds_layout->AddUniformTexelBuffer( EShaderStages::Compute, "LUTWeightQuantizer",				 ArraySize{}, EImageType::UInt, EResourceState::ShaderSample );
			ds_layout->AddUniformTexelBuffer( EShaderStages::Compute, "LUTWeightUnquantize",			 ArraySize{}, EImageType::UInt, EResourceState::ShaderSample );
			ds_layout->AddUniformTexelBuffer( EShaderStages::Compute, "LUTTritQuintDecode",				 ArraySize{}, EImageType::UInt, EResourceState::ShaderSample );
			ds_layout->AddSampledImage( EShaderStages::Compute,		  "LUTPartitionTable",				 ArraySize{}, EImageType::UInt | EImageType::Dim2D, EResourceState::ShaderSample );
		});
}

/*
=================================================
	_CompileDecodeASTC_*
=================================================
*/
void  Executor::_CompileDecodeASTC ()
{
	_CompileASTCDecode( ECompFormat::ASTC_RGBA8_4x4, "DecodeASTC_4x4" );
	_CompileASTCDecode( ECompFormat::ASTC_RGBA8_8x8, "DecodeASTC_8x8" );
}

/*
=================================================
	_CompileHwDecodeASTC
=================================================
*/
void  Executor::_CompileHwDecodeASTC ()
{
	if ( not SupportsASTC() )
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

	_CompileCS( src, "HwDecASTC",
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
	_CompileEncodeAllASTC
=================================================
*/
void  Executor::_CompileEncodeAllASTC () __Th___
{
	AE_LOGI( "Compile ASTC Encoder" );

	_CompileEncodeASTC4x4();
}

/*
=================================================
	_CompileASTCEncode
=================================================
*/
void  Executor::_CompileASTCEncode (ECompFormat compFmt, StringView decodeFnName) __Th___
{
	auto&			fmt_info	= EPixelFormat_GetInfo( ToPixFmt( compFmt ));
	auto			block_size	= fmt_info.BytesPerBlock();
	EPixelFormat	block_fmt	= Default;
	StringView		pix_type	= "float4", swizzle, block_type, block_to_u4;

	if ( block_size == 16 ){
		block_type	= "uint4";
		block_to_u4	= "block";
		block_fmt	= EPixelFormat::RGBA32U;
	}else
		CHECK_THROW( false, "unsupported block size" );

	String	src =
"#define USE_LOW_PRECISION	0\n"
"#define AE_LICENSE_MIT\n"
"#include \"Compression/EncodeASTC.glsl\"\n\n"

"void  Main ()\n"
"{\n"
"	int2		coord		= int2(gl.GlobalInvocationID.xy);\n"
"	const int2	block_dim	= int2("s << ToString(fmt_info.blockDim.x) << "," << ToString(fmt_info.blockDim.y) << ");\n"

"	" << pix_type << "	values[block_dim.x * block_dim.y];\n"
"	for (int y = 0; y < block_dim.y; ++y)\n"
"	for (int x = 0; x < block_dim.x; ++x) {\n"
"		values[x + y * block_dim.x] = gl.image.Load( d_InputImg, coord * block_dim + int2(x,y) )" << swizzle << ";\n"
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
	_CompileEncodeASTC*
=================================================
*/
void  Executor::_CompileEncodeASTC4x4 () __Th___
{
	_CompileASTCEncode( ECompFormat::ASTC_RGBA8_4x4, "EncodeASTC4x4" );
}

/*
=================================================
	_GetModeASTC*
=================================================
*/
int  Executor::_GetModeASTC4x4 (const RGBA32u &block)
{
	return _GetModeASTC( block, uint2{4,4} );
}

int  Executor::_GetModeASTC (const RGBA32u &block, uint2 blockSize)
{
	const uint32_t mode = block.r & 0x7FFu;
	const uint32_t bw   = blockSize.x;
	const uint32_t bh   = blockSize.y;

	// Void-extent: bits[8:0] = 111111100
	// bits[10:9] = 10 LDR, 11 HDR; 00/01 reserved
	if ((mode & 0x1FFu) == 0x1FCu)
		return ((mode >> 9) >= 2u) ? int(mode) : -1;

	uint32_t N = 0, M = 0;
	uint32_t D = (mode >> 10) & 1u;
	uint32_t R = ((mode >> 2) & 3u) << 1 | ((mode >> 4) & 1u); // R2 R1 | R0
	const uint32_t A = (mode >> 5) & 3u;
	const uint32_t B = (mode >> 7) & 3u;

	if ((mode & 3u) != 0u)
	{
		switch (mode & 3u)
		{
		case 1: N = B + 4; M = A + 2; break;	// 4..7 × 2..5
		case 2: N = B + 8; M = A + 2; break;	// 8..11 × 2..5
		case 3: N = A + 2; M = B + 8; break;	// 2..5 × 8..11
		}
	}
	else if (B != 3u)
	{
		N = A + 6; M = B + 6;					// 6..9 × 6..9
	}
	else
	{
		// bits[8:7]=11, bits[1:0]=00: 12-wide / 12-tall, no dual-plane
		D = 0;
		if (((mode >> 10) & 1u) == 0u)
			{ N = 12;     M = A + 2; }			// 12 × 2..5
		else
			{ N = A + 2;  M = 12;    }			// 2..5 × 12
	}

	if (R < 2u)									// R = 0,1 reserved
		return -1;

	if (N < 2u || M < 2u || N > bw || M > bh)
		return -1;

	const uint32_t nweights = N * M * (D + 1u);
	if (nweights < 2u || nweights > 64u)
		return -1;

	return int(mode);
}

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  DSLayout_Test1 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "ubuf" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "uvec4 u;"
				 "ivec4 i;" );

		ScriptSamplerPtr	samp{ new ScriptSampler{ "DefSampler" }};
		samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
		samp->SetAnisotropy( 8.f );

		DescriptorSetLayoutPtr	dsl{ new DescriptorSetLayout{ "PerDraw" }};
		dsl->SetUsage( uint(EDescSetUsage::UpdateTemplate) );
		dsl->AddUniformBuffer( uint(EShaderStages::Vertex), "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform );
		dsl->AddStorageBuffer( uint(EShaderStages::Vertex | EShaderStages::Fragment), "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW );
		dsl->AddUniformTexelBuffer( uint(EShaderStages::Fragment), "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl->AddStorageImage( uint(EShaderStages::Fragment), "storageImage", ArraySize{1}, EImageType::Float | EImageType::Img2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl->AddCombinedImage( uint(EShaderStages::Fragment), "colorTex", ArraySize{1}, EImageType::Float | EImageType::Img2D, EResourceState::ShaderSample );
		dsl->AddImmutableSampler( uint(EShaderStages::Fragment), "imtblSampler", "DefSampler" );
		TEST( dsl->_Build() );
		
		PipelineLayout::UniqueTypes_t	unique_types;

		String	hdr = "\n", src;
		dsl->ToGLSL( EShaderStages::Vertex | EShaderStages::Fragment, 1, INOUT hdr, INOUT src, INOUT unique_types );
		src = hdr + src;

		const String	ref = R"(
#if SH_VERT
  // state: ShaderUniform | PreRasterizationShaders
  // size: 32 b
  layout(set=1, binding=0, std140) uniform constBufType {
	layout(offset=0, align=16) uvec4  u;
	layout(offset=16, align=16) ivec4  i;
  } constBuf;
#endif
#if SH_VERT | SH_FRAG
  // state: ShaderStorage_RW | PreRasterizationShaders | FragmentShader
  // static size: 32 b, array stride: 0 b
  layout(set=1, binding=1, std430) coherent buffer storageBufType {
	layout(offset=0, align=16) uvec4  u;
	layout(offset=16, align=16) ivec4  i;
  } storageBuf [2];
#endif
#if SH_FRAG
  // state: ShaderSample | FragmentShader
  layout(set=1, binding=2) uniform uisamplerBuffer texBuffer;
  // state: ShaderStorage_Write | FragmentShader
  layout(set=1, binding=3, rgba8) coherent uniform image2D storageImage;
  // state: ShaderSample | FragmentShader
  layout(set=1, binding=4) uniform sampler2D colorTex;
  // immutable sampler
  layout(set=1, binding=5) uniform sampler imtblSampler;
#endif
)";
		TEST( src == ref );
	}
}


extern void  UnitTest_DSLayout_GLSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.target			= ECompilationTarget::Vulkan;
	obj.pplnStorage		= &ppln;
	obj.metalCompiler	= MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler	= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );
	
	try {
		DSLayout_Test1();
	} catch(...) {
		TEST( false );
	}
	
	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}

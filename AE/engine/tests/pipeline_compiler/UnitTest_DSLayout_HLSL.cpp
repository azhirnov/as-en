// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  DSLayout_Test1 ()
	{
		ShaderStructTypePtr	st = ShaderStructType::Create( "ubuf" );
		st->Set( EStructLayout::Compatible_Std140,
				 "uvec4 u;"
				 "ivec4 i;" );

		ScriptSamplerPtr	samp = ScriptSampler::Create( "DefSampler" );
		samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
		samp->SetAnisotropy( 8.f );

		DescriptorSetLayoutPtr	dsl = DescriptorSetLayout::Create( "PerDraw" );
		dsl->SetUsage( EDescSetUsage::UpdateTemplate );
		dsl->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
		dsl->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
		dsl->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Dim2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl->AddCombinedImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EResourceState::ShaderSample );
		dsl->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );
		TEST( dsl->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	hdr = "\n", src;
		dsl->ToHLSL( EShaderStages::Vertex | EShaderStages::Fragment, 1, INOUT hdr, INOUT src, INOUT unique_types );
		src = hdr + src;

		const String	ref = R"(
// size: 32, align: 16
struct ubuf
{
	vector<uint32_t,4>  u;  // offset: 0, align: 16, size: 16
	vector<int32_t,4>   i;  // offset: 16, align: 16, size: 16
};

#if SH_VERT
  // state: ShaderUniform | VertexProcessingShaders
  // size: 32 B
  [[vk::binding(0, 1)]] ConstantBuffer< ubuf >  constBuf : register(b0, space1);
#endif
#if SH_VERT | SH_FRAG
  // state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader
  // static size: 32 B, array stride: 0 B
  [[vk::binding(1, 1)]] RWStructuredBuffer< ubuf >  storageBuf [2] : register(u0, space1);
#endif
#if SH_FRAG
  // state: ShaderSample | FragmentShader
  [[vk::binding(2, 1)]] Buffer< vector<uint32_t,4> >  texBuffer : register(t0, space1);
  // state: ShaderStorage_Write | FragmentShader
  [[vk::binding(3, 1)]] [format("rgba8")] RWTexture2D< vector<half,4> >  storageImage : register(u2, space1);
  // state: ShaderSample | FragmentShader
  [[vk::binding(4, 1)]] Texture2D< vector<float,4> >  colorTex : register(t1, space1);
  // immutable sampler
  [[vk::binding(5, 1)]] SamplerState  imtblSampler : register(s0, space1);
#endif
)";
		TEST( src == ref );
	}
}


extern void  UnitTest_DSLayout_HLSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.defaultFeatureSet	= "DefaultFS";
	obj.target				= ECompilationTarget::Vulkan;
	obj.pplnStorage			= &ppln;
	obj.spirvCompiler		= MakeUnique<SpirvCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

	#ifdef AE_METAL_TOOLS
		obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	#endif
	#ifdef AE_ENABLE_SLANG
		obj.slangCompiler = MakeUnique<SLangCompiler>( ArrayView<Path>{} );
	#endif

	ScriptFeatureSetPtr	fs = ScriptFeatureSet::Create( "DefaultFS" );
	fs->fs.Init( FeatureSet::EFeature::RequireTrue );
	fs->fs.storageImageFormats.insert( EPixelFormat::RGBA8_UNorm );
	fs->fs.perPipeline.maxUniformBuffers = 8;
	fs->fs.perPipeline.maxStorageBuffers = 8;
	fs->fs.perPipeline.maxStorageImages = 8;
	fs->fs.perPipeline.maxSampledImages = 8;
	fs->fs.perPipeline.maxSamplers = 8;
	fs->fs.perPipeline.maxTotalResources = 1024;
	fs->fs.perStage.maxUniformBuffers = 8;
	fs->fs.perStage.maxStorageBuffers = 8;
	fs->fs.perStage.maxStorageImages = 8;
	fs->fs.perStage.maxSampledImages = 8;
	fs->fs.perStage.maxSamplers = 8;
	fs->fs.perStage.maxTotalResources = 1024;

	try {
		DSLayout_Test1();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}

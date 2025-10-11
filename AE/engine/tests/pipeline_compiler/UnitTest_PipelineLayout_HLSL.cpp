// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  PipelineLayout_Test1 ()
	{
		ShaderStructTypePtr	st{ new ShaderStructType{ "ubuf" }};
		st->Set( EStructLayout::Compatible_Std140,
				 "uvec4 u;"
				 "ivec4 i;" );

		ScriptSamplerPtr	samp{ new ScriptSampler{ "DefSampler" }};
		samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
		samp->SetAnisotropy( 8.f );
		
		DescriptorSetLayoutPtr	dsl0{ new DescriptorSetLayout{ "PerDraw" }};
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf2", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, True{"dynamic"} );
		dsl0->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
		dsl0->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl0->AddStorageTexelBuffer( EShaderStages::Fragment, "texStorage", ArraySize{1}, EImageType::Float | EImageType::Buffer, EPixelFormat::RGBA32F, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
		dsl0->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl0->AddSampledImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EResourceState::ShaderSample );
		dsl0->AddCombinedImage( EShaderStages::Fragment, "sampledTex", ArraySize{1}, EImageType::Float | EImageType::DimCubeArray, EResourceState::ShaderSample );
		dsl0->AddCombinedImage_ImmutableSampler( EShaderStages::Fragment, "sampledTex2", EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample, {"DefSampler"s} );
		dsl0->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );

		DescriptorSetLayoutPtr	dsl1{ new DescriptorSetLayout{ "Material" }};
		dsl1->AddSampledImage( EShaderStages::Fragment, "diffuseTex", ArraySize{1}, EImageType::Float | EImageType::Dim2DArray, EResourceState::ShaderSample );
		dsl1->AddSampledImage( EShaderStages::Fragment, "noiseTex", ArraySize{1}, EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample );
		
		DescriptorSetLayoutPtr	dsl2{ new DescriptorSetLayout{ "PerPass" }};
		dsl2->AddSubpassInput( EShaderStages::Fragment, "inputTex", 0, EImageType::Float | EImageType::Dim2DMS, EResourceState::InputColorAttachment );
		dsl2->AddRayTracingScene( EShaderStages::Fragment, "rtScene", ArraySize{1} );

		PipelineLayoutPtr	ppln_layout{ new PipelineLayout{ "Layout1" }};
		ppln_layout->AddDSLayout( 0, "PerDraw" );
		ppln_layout->AddDSLayout( 2, "Material" );
		ppln_layout->AddDSLayout( 3, "PerPass" );
		TEST( ppln_layout->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	src = '\n' + ppln_layout->ToHLSL( EShaderStages::Fragment, INOUT unique_types );

		const String	ref = R"#(
// size: 32, align: 16
struct ubuf
{
	vector<uint32_t,4>  u;  // offset: 0, align: 16, size: 16
	vector<int32_t,4>   i;  // offset: 16, align: 16, size: 16
};

//---------------------
// ds[0], name: 'PerDraw', type: 'PerDraw'
  // state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader
  // static size: 32 B, array stride: 0 B
  [[vk::binding(2, 0)]] RWStructuredBuffer< ubuf >  storageBuf [2] : register(u0, space0);
  // state: ShaderSample | FragmentShader
  [[vk::binding(3, 0)]] Buffer< vector<uint32_t,4> >  texBuffer : register(t0, space0);
  // state: ShaderStorage_RW | FragmentShader
  [[vk::binding(4, 0)]] RWBuffer< vector<float,4> >  texStorage : register(u2, space0);
  // state: ShaderStorage_Write | FragmentShader
  [[vk::binding(5, 0)]] [format("rgba8")] RWTexture2D< vector<float,4> >  storageImage : register(u3, space0);
  // state: ShaderSample | FragmentShader
  [[vk::binding(6, 0)]] Texture2D< vector<float,4> >  colorTex : register(t1, space0);
  // state: ShaderSample | FragmentShader
  [[vk::binding(7, 0)]] TextureCubeArray< vector<float,4> >  sampledTex : register(t2, space0);
  // state: ShaderSample | FragmentShader, immutable sampler
  [[vk::binding(8, 0)]] Texture3D< vector<float,4> >  sampledTex2 : register(t3, space0);
  // immutable sampler
  [[vk::binding(9, 0)]] SamplerState  imtblSampler : register(s0, space0);
//---------------------

//---------------------
// ds[2], name: 'Material', type: 'Material'
  // state: ShaderSample | FragmentShader
  [[vk::binding(0, 2)]] Texture2DArray< vector<float,4> >  diffuseTex : register(t0, space2);
  // state: ShaderSample | FragmentShader
  [[vk::binding(1, 2)]] Texture3D< vector<float,4> >  noiseTex : register(t1, space2);
//---------------------

//---------------------
// ds[3], name: 'PerPass', type: 'PerPass'
  // state: InputColorAttachment | FragmentShader
  [[vk::binding(0, 3)]] Texture2DMS< vector<float,4> >  inputTex : register(t0, space3, InputAttachmentIndex(0));
  [[vk::binding(1, 3)]] RaytracingAccelerationStructure  rtScene : register(t1, space3);
//---------------------

)#";
		TEST( src == ref );
	}
}


extern void  UnitTest_PipelineLayout_HLSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.defaultFeatureSet	= "DefaultFS";
	obj.target				= ECompilationTarget::Vulkan;
	obj.pplnStorage			= &ppln;
	obj.spirvCompiler		= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

	#ifdef AE_METAL_TOOLS
		obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	#endif
	#ifdef AE_ENABLE_SLANG
		obj.slangCompiler = MakeUnique<SLangCompiler>( ArrayView<Path>{} );
	#endif
		
	ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ "DefaultFS" }};
	fs->fs.Init( FeatureSet::EFeature::RequireTrue );
	fs->fs.storageImageFormats.insert( EPixelFormat::RGBA8_UNorm );
	fs->fs.storageImageFormats.insert( EPixelFormat::RGBA32F );
	fs->fs.perPipeline.maxUniformBuffers = 8;
	fs->fs.perPipeline.maxStorageBuffers = 8;
	fs->fs.perPipeline.maxStorageImages = 8;
	fs->fs.perPipeline.maxSampledImages = 8;
	fs->fs.perPipeline.maxSamplers = 8;
	fs->fs.perPipeline.maxAccelStructures = 8;
	fs->fs.perPipeline.maxInputAttachments = 2;
	fs->fs.perPipeline.maxTotalResources = 1024;
	fs->fs.perStage.maxUniformBuffers = 8;
	fs->fs.perStage.maxStorageBuffers = 8;
	fs->fs.perStage.maxStorageImages = 8;
	fs->fs.perStage.maxSampledImages = 8;
	fs->fs.perStage.maxSamplers = 8;
	fs->fs.perStage.maxTotalResources = 1024;
	fs->fs.perStage.maxAccelStructures = 2;
	fs->fs.perStage.maxInputAttachments = 2;
	fs->fs.perPipeline_maxUniformBuffersDynamic = 2;
	fs->fs.perPipeline_maxTotalBuffersDynamic = 2;

	fs->fs.rayQuery = FeatureSet::EFeature::RequireTrue;

	try {
		PipelineLayout_Test1();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}

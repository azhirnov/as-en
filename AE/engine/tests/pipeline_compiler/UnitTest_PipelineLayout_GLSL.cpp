// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	static void  PipelineLayout_Test1 ()
	{
		ShaderStructTypePtr	st = ShaderStructType::Create( "ubuf" );
		st->Set( EStructLayout::Compatible_Std140,
				 "uvec4 u;"
				 "ivec4 i;" );

		ScriptSamplerPtr	samp = ScriptSampler::Create( "DefSampler" );
		samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
		samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
		samp->SetAnisotropy( 8.f );

		DescriptorSetLayoutPtr	dsl0 = DescriptorSetLayout::Create( "PerDraw" );
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{"static"} );
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf2", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, True{"dynamic"} );
		dsl0->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
		dsl0->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl0->AddStorageTexelBuffer( EShaderStages::Fragment, "texStorage", ArraySize{1}, EImageType::Float | EImageType::Buffer, EPixelFormat::RGBA32F, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
		dsl0->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl0->AddSampledImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EResourceState::ShaderSample );
		dsl0->AddCombinedImage( EShaderStages::Fragment, "sampledTex", ArraySize{1}, EImageType::Float | EImageType::DimCubeArray, EResourceState::ShaderSample );
		dsl0->AddCombinedImage_ImmutableSampler( EShaderStages::Fragment, "sampledTex2", EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample, {"DefSampler"s} );
		dsl0->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );

		DescriptorSetLayoutPtr	dsl1 = DescriptorSetLayout::Create( "Material" );
		dsl1->AddSampledImage( EShaderStages::Fragment, "diffuseTex", ArraySize{1}, EImageType::Float | EImageType::Dim2DArray, EResourceState::ShaderSample );
		dsl1->AddSampledImage( EShaderStages::Fragment, "noiseTex", ArraySize{1}, EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample );

		DescriptorSetLayoutPtr	dsl2 = DescriptorSetLayout::Create( "PerPass" );
		dsl2->AddSubpassInput( EShaderStages::Fragment, "inputTex", 0, EImageType::Float | EImageType::Dim2DMS, EResourceState::InputColorAttachment );
		dsl2->AddRayTracingScene( EShaderStages::Fragment, "rtScene", ArraySize{1} );

		PipelineLayoutPtr	ppln_layout = PipelineLayout::Create( "Layout1" );
		ppln_layout->AddDSLayout( 0, "PerDraw" );
		ppln_layout->AddDSLayout( 2, "Material" );
		ppln_layout->AddDSLayout( 3, "PerPass" );
		TEST( ppln_layout->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	src = '\n' + ppln_layout->ToGLSL( EShaderStages::Fragment, INOUT unique_types );

		const String	ref = R"#(
//---------------------
// ds[0], name: 'PerDraw', type: 'PerDraw'
  // state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader
  // static size: 32 B, array stride: 0 B
  layout(set=0, binding=2, std430) coherent buffer AE_Type_ubuf_storageBuf {
	layout(offset=0, align=16)   uvec4  u;  // size: 16
	layout(offset=16, align=16)  ivec4  i;  // size: 16
  } storageBuf [2];
  // state: ShaderSample | FragmentShader
  layout(set=0, binding=3) uniform usamplerBuffer texBuffer;
  // state: ShaderStorage_RW | FragmentShader
  layout(set=0, binding=4) coherent uniform imageBuffer texStorage;
  // state: ShaderStorage_Write | FragmentShader
  layout(set=0, binding=5, rgba8) coherent writeonly uniform image2D storageImage;
  // state: ShaderSample | FragmentShader
  layout(set=0, binding=6) uniform texture2D colorTex;
  // state: ShaderSample | FragmentShader
  layout(set=0, binding=7) uniform samplerCubeArray sampledTex;
  // state: ShaderSample | FragmentShader, immutable sampler
  layout(set=0, binding=8) uniform sampler3D sampledTex2;
  // immutable sampler
  layout(set=0, binding=9) uniform sampler imtblSampler;
//---------------------

//---------------------
// ds[2], name: 'Material', type: 'Material'
  // state: ShaderSample | FragmentShader
  layout(set=2, binding=0) uniform texture2DArray diffuseTex;
  // state: ShaderSample | FragmentShader
  layout(set=2, binding=1) uniform texture3D noiseTex;
//---------------------

//---------------------
// ds[3], name: 'PerPass', type: 'PerPass'
  // state: InputColorAttachment | FragmentShader
  layout(set=3, binding=0, input_attachment_index=0) uniform subpassInputMS inputTex;
  layout(set=3, binding=1) uniform accelerationStructureEXT rtScene;
//---------------------

#define DESCRIPTOR_StorageBuffer_storageBuf
#define DESCRIPTOR_UniformTexelBuffer_texBuffer
#define DESCRIPTOR_StorageTexelBuffer_texStorage
#define DESCRIPTOR_StorageImage_storageImage
#define DESCRIPTOR_SampledImage_colorTex
#define DESCRIPTOR_CombinedImage_sampledTex
#define DESCRIPTOR_CombinedImage_ImmutableSampler_sampledTex2
#define DESCRIPTOR_ImmutableSampler_imtblSampler
#define DESCRIPTOR_SampledImage_diffuseTex
#define DESCRIPTOR_SampledImage_noiseTex
#define DESCRIPTOR_SubpassInput_inputTex
#define DESCRIPTOR_RayTracingScene_rtScene
)#";
		TEST( src == ref );
	}


	static void  PipelineLayout_Test2 ()
	{
		const ArraySize	unsized_arr {0};

		ShaderStructTypePtr	st = ShaderStructType::Create( "sbuf" );
		st->Set( EStructLayout::Compatible_Std430,
				 "uvec4 u;"
				 "ivec4 i;" );

		DescriptorSetLayoutPtr	dsl0 = DescriptorSetLayout::Create( "PerDraw2" );
		dsl0->AddStorageBuffer( EShaderStages::Vertex, "constBuf",  unsized_arr, "sbuf", EAccessType::Coherent, EResourceState::ShaderStorage_Read, False{"static"} );
		//dsl0->AddStorageBuffer( EShaderStages::Vertex, "constBuf2", unsized_arr, "sbuf", EAccessType::Coherent, EResourceState::ShaderStorage_Read, True{"dynamic"} );
		dsl0->SetUsage( EDescSetUsage::DescriptorHeap );

		DescriptorSetLayoutPtr	dsl1 = DescriptorSetLayout::Create( "Material2" );
		dsl1->AddSampledImage( EShaderStages::Fragment, "diffuseTex", unsized_arr, EImageType::Float | EImageType::Dim2DArray, EResourceState::ShaderSample );
		dsl1->AddSampledImage( EShaderStages::Fragment, "noiseTex", unsized_arr, EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample );
		dsl1->SetUsage( EDescSetUsage::DescriptorHeap );

		DescriptorSetLayoutPtr	dsl2 = DescriptorSetLayout::Create( "PerPass2" );
		//dsl2->AddSubpassInput( EShaderStages::Fragment, "inputTex", 0, EImageType::Float | EImageType::Dim2DMS, EResourceState::InputColorAttachment );
		dsl2->AddRayTracingScene( EShaderStages::Fragment, "rtScene", unsized_arr );
		dsl2->SetUsage( EDescSetUsage::DescriptorHeap );

		PipelineLayoutPtr	ppln_layout = PipelineLayout::Create( "Layout2" );
		ppln_layout->AddDSLayout( 0, "PerDraw2" );
		ppln_layout->AddDSLayout( 2, "Material2" );
		ppln_layout->AddDSLayout( 3, "PerPass2" );
		TEST( ppln_layout->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	src = '\n' + ppln_layout->ToGLSL( EShaderStages::Fragment, INOUT unique_types );

		const String	ref = R"#(
//---------------------
// ds[0], name: 'PerDraw2', type: 'PerDraw2'
//---------------------

//---------------------
// ds[2], name: 'Material2', type: 'Material2'
  // state: ShaderSample | FragmentShader
  layout(descriptor_heap) uniform texture2DArray diffuseTex [];
  // state: ShaderSample | FragmentShader
  layout(descriptor_heap) uniform texture3D noiseTex [];
//---------------------

//---------------------
// ds[3], name: 'PerPass2', type: 'PerPass2'
  layout(descriptor_heap) uniform accelerationStructureEXT rtScene [];
//---------------------

#define DESCRIPTOR_SampledImage_diffuseTex
#define DESCRIPTOR_SampledImage_noiseTex
#define DESCRIPTOR_RayTracingScene_rtScene
)#";
		TEST( src == ref );
	}
}


extern void  UnitTest_PipelineLayout_GLSL ()
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

	ScriptFeatureSetPtr	fs = ScriptFeatureSet::Create( "DefaultFS" );
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
	fs->fs.perDescSet_maxTotalResources = 512;
	fs->fs.perStage.maxUniformBuffers = 8;
	fs->fs.perStage.maxStorageBuffers = 8;
	fs->fs.perStage.maxStorageImages = 8;
	fs->fs.perStage.maxSampledImages = 8;
	fs->fs.perStage.maxSamplers = 8;
	fs->fs.perStage_maxTotalResources = 1024;
	fs->fs.perStage.maxAccelStructures = 2;
	fs->fs.perStage.maxInputAttachments = 2;
	fs->fs.perPipeline_maxUniformBuffersDynamic = 2;
	fs->fs.perPipeline_maxStorageBuffersDynamic = 2;
	fs->fs.perPipeline_maxTotalBuffersDynamic = 2;

	fs->fs.rayQuery = FeatureSet::EFeature::RequireTrue;

	try {
		PipelineLayout_Test1();
		PipelineLayout_Test2();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}

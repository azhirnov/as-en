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
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf2", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, True{"dynamic"} );
		dsl0->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
		dsl0->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl0->AddStorageTexelBuffer( EShaderStages::Fragment, "texStorage", ArraySize{1}, EImageType::Float | EImageType::Buffer, EPixelFormat::RGBA32F, EAccessType::Coherent, EResourceState::ShaderStorage_RW );
		dsl0->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl0->AddSampledImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Dim2D, EResourceState::ShaderSample );
	//	dsl0->AddCombinedImage( EShaderStages::Fragment, "sampledTex", ArraySize{1}, EImageType::Float | EImageType::DimCubeArray, EResourceState::ShaderSample );
		dsl0->AddCombinedImage_ImmutableSampler( EShaderStages::Fragment, "sampledTex2", EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample, {"DefSampler"s} );
		dsl0->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );

		DescriptorSetLayoutPtr	dsl1 = DescriptorSetLayout::Create( "Material" );
		dsl1->AddSampledImage( EShaderStages::Fragment, "diffuseTex", ArraySize{1}, EImageType::Float | EImageType::Dim2DArray, EResourceState::ShaderSample );
		dsl1->AddSampledImage( EShaderStages::Fragment, "noiseTex", ArraySize{1}, EImageType::Float | EImageType::Dim3D, EResourceState::ShaderSample );

		DescriptorSetLayoutPtr	dsl2 = DescriptorSetLayout::Create( "PerPass" );
	//	dsl2->AddSubpassInput( EShaderStages::Fragment, "inputTex", 0, EImageType::Float | EImageType::Dim2DMS, EResourceState::InputColorAttachment );		// TODO
		dsl2->AddRayTracingScene( EShaderStages::Fragment, "rtScene", ArraySize{1} );

		PipelineLayoutPtr	ppln_layout = PipelineLayout::Create( "Layout1" );
		ppln_layout->AddDSLayout( 0, "PerDraw" );
		ppln_layout->AddDSLayout( 2, "Material" );
		ppln_layout->AddDSLayout( 3, "PerPass" );
		TEST( ppln_layout->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	src = "\n";
		ppln_layout->ToMSL( EShaderStages::Fragment, INOUT unique_types, INOUT src, INOUT src );

		const String	ref = R"#(
struct ubuf
{
	uint4  u;  // offset: 0, align: 16, size: 16
	int4   i;  // offset: 16, align: 16, size: 16
};
static_assert( sizeof(ubuf) == 32, "size mismatch" );

constexpr sampler imtblSampler (
  coord::normalized,
  s_address::clamp_to_edge,
  t_address::repeat,
  r_address::mirrored_repeat,
  border_color::transparent_black,
  mag_filter::linear,
  min_filter::linear,
  mip_filter::nearest,
  lod_clamp(-1000.00, 1000.00),
  max_anisotropy(8)
);
  /* state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader */
  /* static size: 32 B, array stride: 0 B */
  device ubuf storageBuf [[buffer(4)]] [2],
  /* state: ShaderSample | FragmentShader */
  texture_buffer< uint, access::read > texBuffer [[texture(0)]],
  /* state: ShaderStorage_RW | FragmentShader */
  texture_buffer< float, access::read_write > texStorage [[texture(1)]],
  /* state: ShaderStorage_Write | FragmentShader */
  texture2d< float, access::write > storageImage [[texture(2)]],
  /* state: ShaderSample | FragmentShader */
  texture2d< float, access::sample > colorTex [[texture(3)]],
  /* state: ShaderSample | FragmentShader */
  texture3d< float, access::sample > sampledTex2 [[texture(4)]],
  /* state: ShaderSample | FragmentShader */
  texture2d_array< float, access::sample > diffuseTex [[texture(5)]],
  /* state: ShaderSample | FragmentShader */
  texture3d< float, access::sample > noiseTex [[texture(6)]],
  instance_acceleration_structure rtScene [[buffer(2)]],
#define DESCRIPTOR_StorageBuffer_storageBuf
#define DESCRIPTOR_UniformTexelBuffer_texBuffer
#define DESCRIPTOR_StorageTexelBuffer_texStorage
#define DESCRIPTOR_StorageImage_storageImage
#define DESCRIPTOR_SampledImage_colorTex
#define DESCRIPTOR_CombinedImage_ImmutableSampler_sampledTex2
#define DESCRIPTOR_ImmutableSampler_imtblSampler
#define DESCRIPTOR_SampledImage_diffuseTex
#define DESCRIPTOR_SampledImage_noiseTex
#define DESCRIPTOR_RayTracingScene_rtScene
)#";
		TEST( src  == ref );
	}
}


extern void  UnitTest_PipelineLayout_MSL ()
{
#ifdef AE_METAL_TOOLS
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.defaultFeatureSet	= "DefaultFS";
	obj.target				= ECompilationTarget::Metal_Mac;
	obj.pplnStorage			= &ppln;
	obj.metalCompiler		= MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler		= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

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
	fs->fs.perPipeline_maxTotalBuffersDynamic = 2;

	fs->fs.rayQuery = FeatureSet::EFeature::RequireTrue;

	try {
		PipelineLayout_Test1();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
#endif
}

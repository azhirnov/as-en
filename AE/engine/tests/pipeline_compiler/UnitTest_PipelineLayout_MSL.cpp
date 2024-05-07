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
		//dsl0->AllowPartiallyUpdate();
		dsl0->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
		dsl0->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
		dsl0->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl0->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Float | EImageType::Img2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl0->AddSampledImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Img2D, EResourceState::ShaderSample );
		dsl0->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );
		//TEST( dsl0->_Build() );

		DescriptorSetLayoutPtr	dsl1{ new DescriptorSetLayout{ "Material" }};
		dsl1->AddSampledImage( EShaderStages::Fragment, "diffuseTex", ArraySize{1}, EImageType::Float | EImageType::Img2DArray, EResourceState::ShaderSample );
		dsl1->AddSampledImage( EShaderStages::Fragment, "noiseTex", ArraySize{1}, EImageType::Float | EImageType::Img3D, EResourceState::ShaderSample );
		//TEST( dsl0->_Build() );

		PipelineLayoutPtr	ppln_layout{ new PipelineLayout{ "Layout1" }};
		ppln_layout->AddDSLayout( 0, "PerDraw" );
		ppln_layout->AddDSLayout( 2, "Material" );
		TEST( ppln_layout->Build() );

		PipelineLayout::UniqueTypes_t	unique_types;

		String	src = "\n";
		ppln_layout->ToMSL( EShaderStages::Fragment, INOUT unique_types, INOUT src, INOUT src );

		const String	ref = R"#(
struct ubuf
{
	uint4  u;  // offset: 0
	int4  i;  // offset: 16
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
  /* static size: 32 b, array stride: 0 b */
  device ubuf storageBuf [[buffer(3)]] [2],
  /* state: ShaderSample | FragmentShader */
  texture_buffer< uint, access::read > texBuffer [[texture(0)]],
  /* state: ShaderStorage_Write | FragmentShader */
  texture2d< float, access::write > storageImage [[texture(1)]],
  /* state: ShaderSample | FragmentShader */
  texture2d< float, access::sample > colorTex [[texture(2)]],
  /* state: ShaderSample | FragmentShader */
  texture2d_array< float, access::sample > diffuseTex [[texture(3)]],
  /* state: ShaderSample | FragmentShader */
  texture3d< float, access::sample > noiseTex [[texture(4)]],
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

	ScriptFeatureSetPtr	fs {new ScriptFeatureSet{ "DefaultFS" }};
	fs->fs.SetAll( FeatureSet::EFeature::RequireTrue );
	fs->fs.storageImageFormats.insert( EPixelFormat::RGBA8_UNorm );
	fs->fs.perDescrSet.maxUniformBuffers = 8;
	fs->fs.perDescrSet.maxStorageBuffers = 8;
	fs->fs.perDescrSet.maxStorageImages = 8;
	fs->fs.perDescrSet.maxSampledImages = 8;
	fs->fs.perDescrSet.maxSamplers = 8;
	fs->fs.perDescrSet.maxTotalResources = 1024;
	fs->fs.perStage.maxUniformBuffers = 8;
	fs->fs.perStage.maxStorageBuffers = 8;
	fs->fs.perStage.maxStorageImages = 8;
	fs->fs.perStage.maxSampledImages = 8;
	fs->fs.perStage.maxSamplers = 8;
	fs->fs.perStage.maxTotalResources = 1024;

	try {
		PipelineLayout_Test1();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
#endif
}

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

		String	src = ppln_layout->ToGLSL( EShaderStages::Fragment, INOUT unique_types );

		const String	ref = R"#(//---------------------
// ds[0], name: 'PerDraw', type: 'PerDraw'
  // state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader
  // static size: 32 b, array stride: 0 b
  layout(set=0, binding=1, std430) coherent buffer AE_Type_ubuf {
	layout(offset=0, align=16) uvec4  u;
	layout(offset=16, align=16) ivec4  i;
  } storageBuf [2];
  // state: ShaderSample | FragmentShader
  layout(set=0, binding=2) uniform usamplerBuffer texBuffer;
  // state: ShaderStorage_Write | FragmentShader
  layout(set=0, binding=3, rgba8) coherent uniform image2D storageImage;
  // state: ShaderSample | FragmentShader
  layout(set=0, binding=4) uniform texture2D colorTex;
  // immutable sampler
  layout(set=0, binding=5) uniform sampler imtblSampler;
//---------------------

//---------------------
// ds[2], name: 'Material', type: 'Material'
  // state: ShaderSample | FragmentShader
  layout(set=2, binding=0) uniform texture2DArray diffuseTex;
  // state: ShaderSample | FragmentShader
  layout(set=2, binding=1) uniform texture3D noiseTex;
//---------------------

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
}

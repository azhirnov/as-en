// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  DSLayoutToMSL (EDescSetUsage usage, String dsName, OUT String &types, OUT String &decl)
	{
		static bool	once = true;
		if ( once )
		{
			once = false;
			ShaderStructTypePtr	st{ new ShaderStructType{ "ubuf" }};
			st->Set( EStructLayout::Compatible_Std140,
					 "uvec4 u;"
					 "ivec4 i;" );

			ScriptSamplerPtr	samp{ new ScriptSampler{ "DefSampler" }};
			samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
			samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
			samp->SetAnisotropy( 8.f );
		}

		DescriptorSetLayoutPtr	dsl{ new DescriptorSetLayout{ dsName }};
		dsl->SetUsage( uint(usage) );
		dsl->AddUniformBuffer( uint(EShaderStages::Vertex), "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform );
		dsl->AddStorageBuffer( uint(EShaderStages::Vertex | EShaderStages::Fragment), "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW );
		dsl->AddUniformTexelBuffer( uint(EShaderStages::Fragment), "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
		dsl->AddStorageImage( uint(EShaderStages::Fragment), "storageImage", ArraySize{1}, EImageType::Float | EImageType::Img2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
		dsl->AddSampledImage( uint(EShaderStages::Fragment), "colorTex", ArraySize{1}, EImageType::Float | EImageType::Img2D, EResourceState::ShaderSample );
		dsl->AddImmutableSampler( uint(EShaderStages::Fragment), "imtblSampler", "DefSampler" );
		TEST( dsl->_Build() );
		
		types = "\n";
		decl  = "\n";

		PipelineLayout::UniqueTypes_t		unique_types;
		DescriptorSetLayout::MSLBindings	bindings;
		dsl->ToMSL( EShaderStages::Fragment, INOUT bindings, INOUT types, INOUT decl, INOUT unique_types );
	}


	static void  DSLayout_Test1 ()
	{
		String types, decl;
		DSLayoutToMSL( EDescSetUsage::AllowPartialyUpdate, "PerDraw", OUT types, OUT decl );

		const String	ref_types = R"#(
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
  lod_clamp(-1000.000000, 1000.000000),
  max_anisotropy(8)
);
)#";
		const String	ref_decl  = R"#(
  /* state: ShaderStorage_RW | PreRasterizationShaders | FragmentShader */\
  /* static size: 32 b, array stride: 0 b */\
  device ubuf storageBuf [[buffer(0)]] [2],\
  /* state: ShaderSample | FragmentShader */\
  texture_buffer< uint, access::read > texBuffer [[texture(0)]],\
  /* state: ShaderStorage_Write | FragmentShader */\
  texture2d< float, access::write > storageImage [[texture(1)]],\
  /* state: ShaderSample | FragmentShader */\
  texture2d< float, access::sample > colorTex [[texture(2)]],\
)#";
		TEST( types == ref_types );
		TEST( decl  == ref_decl );
	}
	

	static void  DSLayout_Test2 ()
	{
		String types, decl;
		DSLayoutToMSL( EDescSetUsage::ArgumentBuffer, "Material", OUT types, OUT decl );

		const String	ref_types = R"#(
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
  lod_clamp(-1000.000000, 1000.000000),
  max_anisotropy(8)
);
struct ArgBufMaterialType
{
  /* state: ShaderUniform | PreRasterizationShaders */
  /* size: 32 b */
  ubuf constBuf [[id(0)]] ;
  /* state: ShaderStorage_RW | PreRasterizationShaders | FragmentShader */
  /* static size: 32 b, array stride: 0 b */
  device ubuf storageBuf [[id(2)]] [2];
  /* state: ShaderSample | FragmentShader */
  texture_buffer< uint, access::read > texBuffer [[id(4)]];
  /* state: ShaderStorage_Write | FragmentShader */
  texture2d< float, access::write > storageImage [[id(5)]];
  /* state: ShaderSample | FragmentShader */
  texture2d< float, access::sample > colorTex [[id(6)]];
};

)#";
		const String	ref_decl  = R"#(
  constant ArgBufMaterialType& descSet0 [[buffer(0)]],\
)#";
		TEST( types == ref_types );
		TEST( decl  == ref_decl );
	}
}


extern void  UnitTest_DSLayout_MSL ()
{
	ObjectStorage	obj;
	PipelineStorage	ppln;
	obj.target			= ECompilationTarget::Metal_Mac;
	obj.pplnStorage		= &ppln;
	obj.metalCompiler	= MakeUnique<MetalCompiler>( ArrayView<Path>{} );
	obj.spirvCompiler	= MakeUnique<SpirvCompiler>( Array<Path>{} );
	obj.spirvCompiler->SetDefaultResourceLimits();
	ObjectStorage::SetInstance( &obj );

	try {
		DSLayout_Test1();
		DSLayout_Test2();
	} catch(...) {
		TEST( false );
	}

	ObjectStorage::SetInstance( null );
	TEST_PASSED();
}

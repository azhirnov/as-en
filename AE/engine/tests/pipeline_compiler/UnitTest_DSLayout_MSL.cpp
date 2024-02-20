// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  DSLayoutToMSL (EDescSetUsage usage, String dsName, OUT String &types, OUT String &decl)
    {
        static bool once = true;
        if ( once )
        {
            once = false;
            ShaderStructTypePtr st{ new ShaderStructType{ "ubuf" }};
            st->Set( EStructLayout::Compatible_Std140,
                     "uvec4 u;"
                     "ivec4 i;" );

            ScriptSamplerPtr    samp{ new ScriptSampler{ "DefSampler" }};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
            samp->SetAnisotropy( 8.f );
        }

        DescriptorSetLayoutPtr  dsl{ new DescriptorSetLayout{ dsName }};
        dsl->SetUsage( usage );
        dsl->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
        dsl->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
        dsl->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
        dsl->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Float | EImageType::Img2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
        dsl->AddSampledImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Img2D, EResourceState::ShaderSample );
        dsl->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );
        TEST( dsl->Build() );

        types = "\n";
        decl  = "\n";

        PipelineLayout::UniqueTypes_t       unique_types;
        DescriptorSetLayout::MSLBindings    bindings;
        dsl->ToMSL( EShaderStages::Fragment, INOUT bindings, INOUT types, INOUT decl, INOUT unique_types );

      #if not AE_PRIVATE_USE_TABS
        types = Parser::TabsToSpaces( types );
        decl  = Parser::TabsToSpaces( decl );
      #endif
    }


    static void  DSLayout_Test1 ()
    {
        String types, decl;
        DSLayoutToMSL( EDescSetUsage::AllowPartialyUpdate, "PerDraw", OUT types, OUT decl );

        const String    ref_types = R"#(
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
)#";
        const String    ref_decl  = R"#(
  /* state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader */
  /* static size: 32 b, array stride: 0 b */
  device ubuf storageBuf [[buffer(3)]] [2],
  /* state: ShaderSample | FragmentShader */
  texture_buffer< uint, access::read > texBuffer [[texture(0)]],
  /* state: ShaderStorage_Write | FragmentShader */
  texture2d< float, access::write > storageImage [[texture(1)]],
  /* state: ShaderSample | FragmentShader */
  texture2d< float, access::sample > colorTex [[texture(2)]],
)#";
        TEST( types == ref_types );
        TEST( decl  == ref_decl );
    }


    static void  DSLayout_Test2 ()
    {
        String types, decl;
        DSLayoutToMSL( EDescSetUsage::ArgumentBuffer, "Material", OUT types, OUT decl );

        const String    ref_types = R"#(
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
struct ArgBufMaterialType
{
  /* state: ShaderUniform | VertexProcessingShaders */
  /* size: 32 b */
  ubuf constBuf [[id(0)]] ;
  /* state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader */
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
        const String    ref_decl  = R"#(
  constant ArgBufMaterialType& descSet0 [[buffer(0)]],
)#";
        TEST( types == ref_types );
        TEST( decl  == ref_decl );
    }
}


extern void  UnitTest_DSLayout_MSL ()
{
#ifdef AE_METAL_TOOLS
    ObjectStorage   obj;
    PipelineStorage ppln;
    obj.defaultFeatureSet   = "DefaultFS";
    obj.target              = ECompilationTarget::Metal_Mac;
    obj.pplnStorage         = &ppln;
    obj.metalCompiler       = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
    obj.spirvCompiler       = MakeUnique<SpirvCompiler>( Array<Path>{} );
    obj.spirvCompiler->SetDefaultResourceLimits();
    ObjectStorage::SetInstance( &obj );

    ScriptFeatureSetPtr fs {new ScriptFeatureSet{ "DefaultFS" }};
    fs->fs.SetAll( EFeature::RequireTrue );
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
        DSLayout_Test1();
        DSLayout_Test2();
    } catch(...) {
        TEST( false );
    }

    ObjectStorage::SetInstance( null );
    TEST_PASSED();
#endif
}

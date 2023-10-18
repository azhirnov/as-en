// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
    static void  DSLayout_Test1 ()
    {
        ShaderStructTypePtr st{ new ShaderStructType{ "ubuf" }};
        st->Set( EStructLayout::Compatible_Std140,
                 "uvec4 u;"
                 "ivec4 i;" );

        ScriptSamplerPtr    samp{ new ScriptSampler{ "DefSampler" }};
        samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
        samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::Repeat, EAddressMode::MirrorRepeat );
        samp->SetAnisotropy( 8.f );

        DescriptorSetLayoutPtr  dsl{ new DescriptorSetLayout{ "PerDraw" }};
        dsl->SetUsage( EDescSetUsage::UpdateTemplate );
        dsl->AddUniformBuffer( EShaderStages::Vertex, "constBuf", ArraySize{1}, "ubuf", EResourceState::ShaderUniform, False{} );
        dsl->AddStorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "storageBuf", ArraySize{2}, "ubuf", EAccessType::Coherent, EResourceState::ShaderStorage_RW, False{} );
        dsl->AddUniformTexelBuffer( EShaderStages::Fragment, "texBuffer", ArraySize{1}, EImageType::UInt | EImageType::Buffer, EResourceState::ShaderSample );
        dsl->AddStorageImage( EShaderStages::Fragment, "storageImage", ArraySize{1}, EImageType::Img2D, EPixelFormat::RGBA8_UNorm, EAccessType::Coherent, EResourceState::ShaderStorage_Write );
        dsl->AddCombinedImage( EShaderStages::Fragment, "colorTex", ArraySize{1}, EImageType::Float | EImageType::Img2D, EResourceState::ShaderSample );
        dsl->AddImmutableSampler( EShaderStages::Fragment, "imtblSampler", "DefSampler" );
        TEST( dsl->Build() );

        PipelineLayout::UniqueTypes_t   unique_types;

        String  hdr = "\n", src;
        dsl->ToGLSL( EShaderStages::Vertex | EShaderStages::Fragment, 1, INOUT hdr, INOUT src, INOUT unique_types );
        src = hdr + src;

      #if not AE_PRIVATE_USE_TABS
        src = Parser::TabsToSpaces( src );
      #endif

        const String    ref = R"(
#if SH_VERT
  // state: ShaderUniform | VertexProcessingShaders
  // size: 32 b
  layout(set=1, binding=0, std140) uniform AE_Type_ubuf {
    layout(offset=0, align=16) uvec4  u;
    layout(offset=16, align=16) ivec4  i;
  } constBuf;
#endif
#if SH_VERT | SH_FRAG
  // state: ShaderStorage_RW | VertexProcessingShaders | FragmentShader
  // static size: 32 b, array stride: 0 b
  layout(set=1, binding=1, std430) coherent buffer AE_Type_ubuf {
    layout(offset=0, align=16) uvec4  u;
    layout(offset=16, align=16) ivec4  i;
  } storageBuf [2];
#endif
#if SH_FRAG
  // state: ShaderSample | FragmentShader
  layout(set=1, binding=2) uniform usamplerBuffer texBuffer;
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
    ObjectStorage   obj;
    PipelineStorage ppln;
    obj.defaultFeatureSet   = "DefaultFS";
    obj.target              = ECompilationTarget::Vulkan;
    obj.pplnStorage         = &ppln;
    obj.spirvCompiler       = MakeUnique<SpirvCompiler>( Array<Path>{} );
    obj.spirvCompiler->SetDefaultResourceLimits();
    ObjectStorage::SetInstance( &obj );

    #ifdef AE_METAL_TOOLS
        obj.metalCompiler = MakeUnique<MetalCompiler>( ArrayView<Path>{} );
    #endif

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
    } catch(...) {
        TEST( false );
    }

    ObjectStorage::SetInstance( null );
    TEST_PASSED();
}

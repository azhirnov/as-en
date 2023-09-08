// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void  VertexBuffers ()
{
    // vertex components
    {
        RC<ShaderStructType>    st1 = ShaderStructType( "Posf3" );
        st1.Set( "packed_float3     Position;" );
        st1.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<ShaderStructType>    st2 = ShaderStructType( "Normf3" );
        st2.Set( "packed_float3     Normal;" );
        st2.AddUsage( ShaderStructTypeUsage::VertexLayout );

        RC<ShaderStructType>    st3 = ShaderStructType( "UVf2" );
        st3.Set( "packed_float2     Texcoord;" );
        st3.AddUsage( ShaderStructTypeUsage::VertexLayout );
    }

    // vertex buffers
    {
        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{Posf3, Normf3, UVf2}" );
        vb.Add( "Position",     "Posf3" );
        vb.Add( "Normals",      "Normf3" );
        vb.Add( "UVs",          "UVf2" );
    }{
        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{Posf3, Normf3}" );
        vb.Add( "Position",     "Posf3" );
        vb.Add( "Normals",      "Normf3" );
    }{
        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{Posf3}" );
        vb.Add( "Position",     "Posf3" );
    }
}


void  InitPipelineLayout ()
{
    // samplers
    {
        RC<Sampler>     samp = Sampler( "model.Sampler" );
        samp.Filter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
        samp.AddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        samp.Anisotropy( 16.0f );
    }

    // descriptor set
    {
        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "model.mtr.ds" );
        ds.StorageBuffer( EShaderStages::Vertex,    "un_Nodes",     "ModelNodeArray",       EResourceState::ShaderStorage_Read );
    //  ds.StorageBuffer( EShaderStages::Vertex,    "un_Meshes",    "ModelMeshArray",       EResourceState::ShaderStorage_Read );
    //  ds.StorageBuffer( EShaderStages::Vertex,    "un_Materials", "ModelMaterialArray",   EResourceState::ShaderStorage_Read );
        ds.SampledImage( EShaderStages::Fragment,   "un_AlbedoMaps", ArraySize(128), EImageType::FImage2D );
        ds.ImtblSampler( EShaderStages::Fragment,   "un_AlbedoSampler", "model.Sampler" );
    }

    // pipeline layout
    {
        RC<PipelineLayout>      pl = PipelineLayout( "model.pl" );
        pl.DSLayout( "pass",     0, "pass.ds" );
        pl.DSLayout( "material", 1, "model.mtr.ds" );
        pl.PushConst( "pc", "model.pc", EShader::Vertex );
    }
}


void  BufferTypes ()
{
    // push constant
    {
        RC<ShaderStructType>    st = ShaderStructType( "model.pc" );
        st.Set( EStructLayout::Compatible_Std140,
                "uint               nodeIdx;" );
    }

    // scene node
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelNode" );
        st.Set( EStructLayout::Compatible_Std430,
                "float4x4           transform;" +
                "float3x3           normalMat;" +
                "uint               meshIdx;" +
                "uint               materialIdx;" );
        st.AddUsage( ShaderStructTypeUsage::BufferLayout );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelNodeArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelNode          data [];" );
    }

    // material
    {
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterial" );
        st.Set( EStructLayout::Compatible_Std430,
                "uint               albedoMap;" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "ModelMaterialArray" );
        st.Set( EStructLayout::Compatible_Std430,
                "ModelMaterial      data [];" );
    }
}


void  ASmain ()
{
    VertexBuffers();
    BufferTypes();
    InitPipelineLayout();
}

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "simple3d.io" );
        st.Set( "float3     normal;" +
                "float3     texcoord;" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "simple3d.ub" );
        st.Set( "float4x4  mvp;" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "simple3d.ds0" );
        ds.SampledImage( EShaderStages::Fragment, "un_ColorTexture", ArraySize(1), EImageType::FImageCube );
        ds.ImtblSampler( EShaderStages::Fragment, "un_ColorTexture_sampler", "LinearRepeat"  );
        ds.UniformBuffer( EShaderStages::Vertex, "drawUB", ArraySize(1), "simple3d.ub" );   // TODO: dynamic
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "simple3d.pl" );
        pl.DSLayout( 0, "simple3d.ds0" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "CubeVertex" );
        st.Set( "packed_float3      Position;" +
                "packed_float3      Normal;" +
                "packed_float3      Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{CubeVertex}" );
        vb.Add( "Position", st );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "SphericalCubeVertex" );
        st.Set( "packed_float4      Position;" +
                "packed_float4      Texcoord;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{SphericalCubeVertex}" );
        vb.Add( "Position", st );
    }

    {
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "simple3d.draw1" );
        ppln.AddFeatureSet( "MinimalFS" );
        ppln.SetLayout( "simple3d.pl" );
        ppln.SetVertexInput( "VB{CubeVertex}" );
        ppln.SetFragmentOutputFromRenderTech( "Scene3D.RTech", "Main" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "simple3d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.Define( "CUBE" );
            vs.file     = "simple3d.glsl";      // file:///<path>/samples/demo/_data/shaders/simple3d.glsl
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "simple3d.glsl";      // file:///<path>/samples/demo/_data/shaders/simple3d.glsl
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "simple3d.draw1" );
            spec.AddToRenderTech( "Scene3D.RTech", "Main" );
            spec.SetViewportCount( 1 );

            RenderState rs;

            rs.depth.test                   = true;
            rs.depth.write                  = true;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Front;

            spec.SetRenderState( rs );
        }
    }{
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "simple3d.draw2" );
        ppln.AddFeatureSet( "MinimalFS" );
        ppln.SetLayout( "simple3d.pl" );
        ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
        ppln.SetFragmentOutputFromRenderTech( "Scene3D.RTech", "Main" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "simple3d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.Define( "SPHERICAL_CUBE" );
            vs.file     = "simple3d.glsl";      // file:///<path>/samples/demo/_data/shaders/simple3d.glsl
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "simple3d.glsl";      // file:///<path>/samples/demo/_data/shaders/simple3d.glsl
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "simple3d.draw2" );
            spec.AddToRenderTech( "Scene3D.RTech", "Main" );
            spec.SetViewportCount( 1 );

            RenderState rs;

            rs.depth.test                   = true;
            rs.depth.write                  = true;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Front;

            spec.SetRenderState( rs );
        }
    }
}

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#include <pipeline_compiler.as>

void ASmain ()
{
    {
        RC<ShaderStructType>    st = ShaderStructType( "camera3d.io" );
        st.Set( EStructLayout::InternalIO,
                "float3     normal;" +
                "float3     texcoord;" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "camera3d.ub" );
        st.Set( "float4x4  mvp;" );

        RC<DescriptorSetLayout> ds = DescriptorSetLayout( "camera3d.ds0" );
        ds.SampledImage( EShaderStages::Fragment, "un_ColorTexture", EImageType::FImageCube );
        ds.ImtblSampler( EShaderStages::Fragment, "un_ColorTexture_sampler", "LinearRepeat"  );
        ds.UniformBufferDynamic( EShaderStages::Vertex, "drawUB", "camera3d.ub" );
    }{
        RC<PipelineLayout>      pl = PipelineLayout( "camera3d.pl" );
        pl.DSLayout( 0, "camera3d.ds0" );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "CubeVertex" );
        st.Set( EStructLayout::InternalIO,
                "packed_short_norm4     Position;" +
                "packed_short_norm4     Texcoord;" +
                "packed_short_norm4     Normal;" +
                "packed_short_norm4     Tangent;" +
                "packed_short_norm4     BiTangent;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{CubeVertex}" );
        vb.Add( "Position", st );
    }{
        RC<ShaderStructType>    st = ShaderStructType( "SphericalCubeVertex" );
        st.Set( EStructLayout::InternalIO,
                "packed_short_norm4     Position;" +
                "packed_short_norm4     Texcoord;" +
                "packed_short_norm4     Tangent;" +
                "packed_short_norm4     BiTangent;" );

        RC<VertexBufferInput>   vb = VertexBufferInput( "VB{SphericalCubeVertex}" );
        vb.Add( "Position", st );
    }

    {
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "camera3d.draw1" );
        ppln.SetLayout( "camera3d.pl" );
        ppln.SetVertexInput( "VB{CubeVertex}" );
        ppln.SetFragmentOutputFromRenderTech( "Scene3D.RTech", "Main" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "camera3d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.Define( "CUBE" );
            vs.file     = "camera3d.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/camera3d.glsl)
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "camera3d.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/camera3d.glsl)
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "camera3d.draw1" );
            spec.AddToRenderTech( "Scene3D.RTech", "Main" );

            RenderState rs;

            rs.depth.test                   = true;
            rs.depth.write                  = true;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = true;
            rs.rasterization.cullMode       = ECullMode::Back;

            spec.SetRenderState( rs );
        }
    }{
        RC<GraphicsPipeline>    ppln = GraphicsPipeline( "camera3d.draw2" );
        ppln.SetLayout( "camera3d.pl" );
        ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
        ppln.SetFragmentOutputFromRenderTech( "Scene3D.RTech", "Main" );
        ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "camera3d.io" );

        {
            RC<Shader>  vs = Shader();
            vs.Define( "SPHERICAL_CUBE" );
            vs.file     = "camera3d.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/camera3d.glsl)
            vs.options  = EShaderOpt::Optimize;
            ppln.SetVertexShader( vs );
        }
        {
            RC<Shader>  fs = Shader();
            fs.file     = "camera3d.glsl";      // [src](https://github.com/azhirnov/as-en/blob/dev/AE/samples/demo/_data/shaders/camera3d.glsl)
            fs.options  = EShaderOpt::Optimize;
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "camera3d.draw2" );
            spec.AddToRenderTech( "Scene3D.RTech", "Main" );

            RenderState rs;

            rs.depth.test                   = true;
            rs.depth.write                  = true;

            rs.inputAssembly.topology       = EPrimitive::TriangleList;

            rs.rasterization.frontFaceCCW   = false;
            rs.rasterization.cullMode       = ECullMode::Back;

            spec.SetRenderState( rs );
        }
    }
}

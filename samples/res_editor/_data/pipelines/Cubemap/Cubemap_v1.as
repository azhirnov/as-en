// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <pipeline_compiler>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        {
            RC<ShaderStructType>    st = ShaderStructType( "cubemap.io" );
            st.Set( "float3     normal;" +
                    "float3     texcoord;" );
        }{
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "cubemap.mtr.ds" );
            ds.CombinedImage( EShaderStages::Fragment,  "un_CubeMap",   EImageType::FImageCube,  Sampler_LinearMipmapRepeat );
            ds.UniformBuffer( EShaderStages::Vertex,    "un_PerObject", ArraySize(1),            "SphericalCubeMaterialUB" );
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "cubemap.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "cubemap.mtr.ds" );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "cubemap.draw1" );
            ppln.SetLayout( "cubemap.pl" );
            ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
            ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "cubemap.io" );

            {
                RC<Shader>  vs = Shader();
                vs.LoadSelf();
                ppln.SetVertexShader( vs );
            }
            {
                RC<Shader>  fs = Shader();
                fs.LoadSelf();
                ppln.SetFragmentShader( fs );
            }

            // specialization
            {
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "cubemap.draw1" );
                spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

                RenderState rs;

                rs.depth.test                   = true;
                rs.depth.write                  = true;

                rs.inputAssembly.topology       = EPrimitive::TriangleList;

                rs.rasterization.frontFaceCCW   = true;
                rs.rasterization.cullMode       = ECullMode::None;

                spec.SetRenderState( rs );
            }
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    void Main ()
    {
        gl.Position     = un_PerPass.camera.viewProj * (un_PerObject.transform * float4(in_Position.xyz, 1.0));
        Out.texcoord    = in_Texcoord.xyz;
        Out.normal      = in_Position.xyz;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Math.glsl"

    void Main ()
    {
        out_Color = gl.texture.Sample( un_CubeMap, In.texcoord );
    }

#endif
//-----------------------------------------------------------------------------

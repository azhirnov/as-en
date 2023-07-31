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
            RC<ShaderStructType>    st = ShaderStructType( "planet3.io.vs-fs" );
            st.Set( "float3     normal;" +
                    "float3     texcoord;" );
        }{
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "planet3.mtr.ds" );
            ds.CombinedImage( EShaderStages::Fragment,  "un_HeightMap",     EImageType::FImageCube,  Sampler_LinearRepeat );
            ds.CombinedImage( EShaderStages::Fragment,  "un_NormalMap",     EImageType::FImageCube,  Sampler_LinearRepeat );
            ds.CombinedImage( EShaderStages::Fragment,  "un_AlbedoMap",     EImageType::FImageCube,  Sampler_LinearRepeat );
            ds.CombinedImage( EShaderStages::Fragment,  "un_EmissionMap",   EImageType::FImageCube,  Sampler_LinearRepeat );
            ds.UniformBuffer( EShaderStages::Vertex,    "un_PerObject",     ArraySize(1),            "SphericalCubeMaterialUB" );
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "planet3.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "planet3.mtr.ds" );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "planet3.draw" );
            ppln.SetLayout( "planet3.pl" );
            ppln.SetVertexInput( "VB{SphericalCubeVertex}" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
            ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "planet3.io.vs-fs" );

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
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "planet3.draw" );
                spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

                RenderState rs;

                rs.depth.test                   = true;
                rs.depth.write                  = true;

                rs.inputAssembly.topology       = EPrimitive::TriangleList;

                rs.rasterization.frontFaceCCW   = true;
                rs.rasterization.cullMode       = ECullMode::Back;

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
        const float3    light_dir   = float3( 0.f, 0.f, -1.f );
        const float3    norm        = gl.texture.Sample( un_NormalMap, In.texcoord ).xyz;
        const float3    color       = gl.texture.Sample( un_AlbedoMap, In.texcoord ).xyz;
        const float     light       = Dot( norm, light_dir );

        out_Color = float4( color * light, 1.0 );
    }

#endif
//-----------------------------------------------------------------------------

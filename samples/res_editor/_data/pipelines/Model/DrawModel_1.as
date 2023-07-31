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
            RC<ShaderStructType>    st = ShaderStructType( "model1.io.vs-fs" );
            st.Set( "float3     normal;" +
                    "float2     texcoord;" +
                    "uint       materialIdx;" );
        }{
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "model1.mtr.ds" );
        //  ds.StorageBuffer( EShaderStages::Vertex,    "un_Nodes",     ArraySize(1),   "ModelNodeArray",       EAccessType::Coherent,  EResourceState::ShaderStorage_Read );
        //  ds.StorageBuffer( EShaderStages::Vertex,    "un_Meshes",    ArraySize(1),   "ModelMeshArray",       EAccessType::Coherent,  EResourceState::ShaderStorage_Read );
        //  ds.StorageBuffer( EShaderStages::Vertex,    "un_Materials", ArraySize(1),   "ModelMaterialArray",   EAccessType::Coherent,  EResourceState::ShaderStorage_Read );
            ds.SampledImage( EShaderStages::Fragment,   "un_AlbedoMaps", ArraySize(128), EImageType::FImage2D );
            ds.ImtblSampler( EShaderStages::Fragment,   "un_AlbedoSampler", "LinearMipmapRepeat" );
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "model1.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "model1.mtr.ds" );
            pl.PushConst( "pc", "ModelNode", EShader::Vertex );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "model1.draw" );
            ppln.SetLayout( "model1.pl" );
            ppln.SetVertexInput( "VB{Posf3, Normf3, UVf2}" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
            ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "model1.io.vs-fs" );

            {
                RC<Shader>  vs = Shader();
                vs.LoadSelf();
                ppln.SetVertexShader( vs );
            }{
                RC<Shader>  fs = Shader();
                fs.LoadSelf();
                ppln.SetFragmentShader( fs );
            }

            // specialization
            {
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "model1.draw" );
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
#if 0

    void Main ()
    {
        ModelNode   node    = un_Nodes .data[ pc.nodeIdx ];
        ModelMesh   mesh    = un_Meshes.data[ node.meshIdx ];

        uint        v_idx   = mesh.indices  .data[ gl.VertexIndex ];
        float3      pos     = float_packed3_cast( mesh.positions.data[ v_idx ]);
        float3      norm    = float_packed3_cast( mesh.normals  .data[ v_idx ]);
        float2      uv      = float_packed2_cast( mesh.texcoords.data[ v_idx ]);

        gl.Position     = un_PerPass.camera.viewProj * (node.transform * float4(pos, 1.0));
        Out.texcoord    = uv;
        Out.normal      = norm;
        Out.materialIdx = node.materialIdx;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

    void Main ()
    {
        gl.Position     = un_PerPass.camera.viewProj * (pc.transform * float4(in_Position, 1.0));
        Out.texcoord    = in_Texcoord;
        Out.normal      = in_Normal;
        Out.materialIdx = pc.materialIdx;
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Math.glsl"

    void Main ()
    {
        const float3    light_dir   = float3( 0.f, 0.f, -1.f );
        const float     light       = Dot( In.normal, light_dir );
        float4          albedo      = gl.texture.Sample( gl::CombinedTex2D{ un_AlbedoMaps, un_AlbedoSampler }, In.texcoord );

        out_Color = albedo * light;
    }

#endif
//-----------------------------------------------------------------------------

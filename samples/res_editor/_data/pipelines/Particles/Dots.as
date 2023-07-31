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
            RC<ShaderStructType>    st = ShaderStructType( "particles.io.vs_gs" );
            st.Set( "float4     color;" +
                    "float      size;" );
        }{
            RC<ShaderStructType>    st = ShaderStructType( "particles.io.gs_fs" );
            st.Set( "float2     uv;" +
                    "float4     color;" );
        }{
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "particles.mtr.ds" );
            ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", ArraySize(1), "UnifiedGeometryMaterialUB" );
            ds.StorageBuffer( EShaderStages::Vertex, "un_Particles", ArraySize(1), "ParticleArray", EAccessType::Coherent, EResourceState::ShaderStorage_Read );
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "particles.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "particles.mtr.ds" );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "particles.draw1" );
            ppln.SetLayout( "particles.pl" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
            ppln.SetShaderIO( EShader::Vertex,   EShader::Geometry, "particles.io.vs_gs" );
            ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "particles.io.gs_fs" );

            {
                RC<Shader>  vs = Shader();
                vs.LoadSelf();
                ppln.SetVertexShader( vs );
            }{
                RC<Shader>  gs = Shader();
                gs.LoadSelf();
                ppln.SetGeometryShader( gs );
            }{
                RC<Shader>  fs = Shader();
                fs.LoadSelf();
                ppln.SetFragmentShader( fs );
            }

            // specialization
            {
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "particles.draw1" );
                spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

                RenderState rs;

                RenderState_ColorBuffer cb;
                cb.srcBlendFactor   .set( EBlendFactor::One );
                cb.dstBlendFactor   .set( EBlendFactor::One );
                cb.blendOp          .set( EBlendOp::Add );
                cb.blend            = true;
                rs.color.SetColorBuffer( 0, cb );

                rs.depth.test                   = false;
                rs.depth.write                  = false;

                rs.inputAssembly.topology       = EPrimitive::Point;
                rs.rasterization.cullMode       = ECullMode::None;

                spec.SetRenderState( rs );
            }
        }
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
    #include "Math.glsl"

    void Main ()
    {
        Particle    p   = un_Particles.elements[gl.VertexIndex];
        gl.Position     = un_PerPass.camera.view * (un_PerObject.transform * float4(p.position_size.xyz, 1.0));
        Out.color       = unpackUnorm4x8( floatBitsToUint( p.velocity_color.w ));
        Out.size        = p.position_size.w * 4.0 / Max( un_PerPass.resolution.x, un_PerPass.resolution.y );
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
    #include "Math.glsl"

    layout (points) in;
    layout (triangle_strip, max_vertices = 4) out;

    void Main ()
    {
        const float4    pos     = gl_in[0].gl_Position;
        const float     size    = In[0].size;
        const float4    color   = In[0].color;

        // a: left-bottom
        float2  va  = pos.xy + float2(-0.5, -0.5) * size;
        gl.Position = un_PerPass.camera.proj * float4(va, pos.zw);
        Out.uv      = float2(0.0, 0.0);
        Out.color   = color;
        gl.EmitVertex();

        // b: left-top
        float2  vb  = pos.xy + float2(-0.5, 0.5) * size;
        gl.Position = un_PerPass.camera.proj * float4(vb, pos.zw);
        Out.uv      = float2(0.0, 1.0);
        Out.color   = color;
        gl.EmitVertex();

        // d: right-bottom
        float2  vd  = pos.xy + float2(0.5, -0.5) * size;
        gl.Position = un_PerPass.camera.proj * float4(vd, pos.zw);
        Out.uv      = float2(1.0, 0.0);
        Out.color   = color;
        gl.EmitVertex();

        // c: right-top
        float2  vc  = pos.xy + float2(0.5, 0.5) * size;
        gl.Position = un_PerPass.camera.proj * float4(vc, pos.zw);
        Out.uv      = float2(1.0, 1.0);
        Out.color   = color;
        gl.EmitVertex();

        gl.EndPrimitive();
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Math.glsl"

    void Main ()
    {
        out_Color = In.color * (1.0 - Distance( ToSNorm(In.uv), float2(0.0) ));
    }

#endif
//-----------------------------------------------------------------------------

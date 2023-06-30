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
            st.Set( "float4     startPos;" +
                    "float4     endPos;" +
                    "float4     color;" +
                    "float      size;" );
        }{
            RC<ShaderStructType>    st = ShaderStructType( "particles.io.gs_fs" );
            st.Set( "float2     uv;" +
                    "float4     color;" );
        }{
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "particles.mtr.ds" );
            ds.UniformBuffer( EShaderStages::Vertex, "mtrUB", ArraySize(1), "UnifiedGeometryMaterialUB" );
            ds.StorageBuffer( EShaderStages::Vertex, "un_Particles", ArraySize(1), "ParticleArray", EAccessType::Coherent, EResourceState::ShaderStorage_Read );
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "particles.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "particles.mtr.ds" );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "particles.draw1" );
            ppln.AddFeatureSet( "Default" );
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
                spec.AddToRenderTech( "rtech", "main" );
                spec.SetViewportCount( 1 );

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
        float4x4    mv  = passUB.camera.view * mtrUB.transform;

        Out.endPos      = mv * float4(p.position_size.xyz, 1.0);
        Out.color       = unpackUnorm4x8( floatBitsToUint( p.velocity_color.w ));
        Out.size        = p.position_size.w * 2.0 / Max( passUB.resolution.x, passUB.resolution.y );

        float3  vel     = Normalize(p.velocity_color.xyz) * Min( Length(p.velocity_color.xyz), Out.size * 25.0 );
        Out.startPos    = mv * float4(p.position_size.xyz - vel * 0.5, 1.0);
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
    #include "Math.glsl"

    layout (points) in;
    layout (triangle_strip, max_vertices = 4) out;

    // check if point is inside oriented rectangle
    bool IsPointInside (in float2 a, in float2 b, in float2 c, in float2 m)
    {
        float2  ab      = b - a;
        float2  bc      = c - b;
        float2  am      = m - a;
        float2  bm      = m - b;

        float   ab_am   = Dot( ab, am );
        float   ab_ab   = Dot( ab, ab );
        float   bc_bm   = Dot( bc, bm );
        float   bc_bc   = Dot( bc, bc );

        return  ab_am >= 0.0f  and bc_bm >= 0.0f and
                ab_am <= ab_ab and bc_bm <= bc_bc;
    }

    void Main ()
    {
        //        _ _                       //
        //      /\ / \          \ /         //
        //     /  s  /           s          //
        //    /  / \/           / \         //
        //   /\ /  /         \ /            //
        //  \  e  /           e             //
        //   \/_\/           / \            //

        const float4    start   = In[0].startPos;
        const float4    end     = In[0].endPos;
        const float2    dir     = Normalize( end.xy - start.xy );
        const float2    norm    = float2( -dir.y, dir.x );

        const float     size    = In[0].size * 0.5;     // rotated rectangle size
        const float     side    = size * 0.95;          // size with error
        const float4    color   = In[0].color;

        const float2    rect_a  = start.xy + norm * size;
        const float2    rect_b  = start.xy - norm * size;
        const float2    rect_c  = end.xy - norm * size;

        const float4    points[] = float4[](
            start + float4( norm + dir, 0.0, 0.0) * side,
            start + float4( norm - dir, 0.0, 0.0) * side,
            start + float4(-norm - dir, 0.0, 0.0) * side,
            start + float4(-norm + dir, 0.0, 0.0) * side,
            end   + float4( norm + dir, 0.0, 0.0) * side,
            end   + float4( norm - dir, 0.0, 0.0) * side,
            end   + float4(-norm - dir, 0.0, 0.0) * side,
            end   + float4(-norm + dir, 0.0, 0.0) * side
        );

        const float2    uv_coords[] = float2[](
            float2(0.0, 1.0),
            float2(0.0, 0.0),
            float2(1.0, 1.0),
            float2(1.0, 0.0)
        );

        // find external points (must be 4 points)
        for (int i = 0, j = 0; i < points.length() and j < 4; ++i)
        {
            if ( not IsPointInside( rect_a, rect_b, rect_c, points[i].xy ))
            {
                gl.Position  = passUB.camera.proj * points[i];
                Out.uv       = uv_coords[j];
                Out.color    = color;

                gl.EmitVertex();
                ++j;
            }
        }

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

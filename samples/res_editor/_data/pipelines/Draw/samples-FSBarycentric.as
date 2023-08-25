// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Use fragment barycentric extension to draw wireframe.
*/
#ifdef __INTELLISENSE__
#   include <pipeline_compiler.as>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        {
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "mtr.ds" );
            ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject",    ArraySize(1), "UnifiedGeometryMaterialUB" );
            ds.StorageBuffer( EShaderStages::Vertex, "un_Cube",         ArraySize(1), "CubeSBlock", EAccessType::Coherent, EResourceState::ShaderStorage_Read );    // external
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "mtr.ds" );
        }

        {
            RC<GraphicsPipeline>    ppln = GraphicsPipeline( "tmpl" );
            ppln.SetLayout( "pl" );
            ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

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
                RC<GraphicsPipelineSpec>    spec = ppln.AddSpecialization( "spec" );
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
        uint    idx     = un_Cube.indices[ gl.VertexIndex ];
        gl.Position     = un_PerPass.camera.viewProj * (un_PerObject.transform * float4(un_Cube.positions[idx], 1.0));
    }

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
    #include "Math.glsl"

    void Main ()
    {
    #if 0
        // without derivatives
        const float     closest_edge    = Min( gl.BaryCoord.x, Min( gl.BaryCoord.y, gl.BaryCoord.z ));
        const float     wireframe       = SmoothStep( 0, 0.01, closest_edge );

    #else
        const float     thickness       = 1.5;  // pixels
        const float     falloff         = 6.0;  // pixels

        const float3    dx_barycoord    = dFdx( gl.BaryCoord );
        const float3    dy_barycoord    = dFdy( gl.BaryCoord );
        const float3    d_barycoord     = Sqrt( dx_barycoord * dx_barycoord + dy_barycoord * dy_barycoord );
        const float3    d_thickness     = d_barycoord * thickness;
        const float3    d_falloff       = d_barycoord * falloff;

        const float3    remap           = SmoothStep( d_thickness, d_thickness + d_falloff, gl.BaryCoord);
        const float     wireframe       = Min( remap.x, Min( remap.y, remap.z ));

    #endif
        out_Color = float4( wireframe );
    }

#endif
//-----------------------------------------------------------------------------

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <pipeline_compiler>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        // Shader IO
        {
            RC<ShaderStructType>    st = ShaderStructType( "meshtofrag.io" );
            st.Set( EStructLayout::InternalIO,
                    "float4         color;" );
        }

        // pipeline layout
        {
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "mesh_sh.ds" );
            ds.UniformBuffer( EShaderStages::Mesh, "un_PerObject",  ArraySize(1), "UnifiedGeometryMaterialUB" );
            ds.StorageBuffer( EShaderStages::Mesh, "un_Cube",       ArraySize(1), "mesh.CubeUBlock", EAccessType::Coherent, EResourceState::ShaderStorage_Read );   // external
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "mesh_sh.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "mesh_sh.ds" );
        }


        RC<MeshPipeline>    ppln = MeshPipeline( "mesh_sh.draw1" );
        ppln.SetLayout( "mesh_sh.pl" );
        ppln.SetShaderIO( EShader::Mesh, EShader::Fragment, "meshtofrag.io" );
        ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

        {
            RC<Shader>  ms = Shader();
            ms.version  = EShaderVersion::SPIRV_1_4;
            ms.LoadSelf();
            ms.MeshLocalSize( 24 );
            ms.MeshOutput( 24, 12, EPrimitive::TriangleList );
            ppln.SetMeshShader( ms );
        }{
            RC<Shader>  fs = Shader();
            fs.version  = EShaderVersion::SPIRV_1_4;
            fs.LoadSelf();
            ppln.SetFragmentShader( fs );
        }

        // specialization
        {
            RC<MeshPipelineSpec>    spec = ppln.AddSpecialization( "mesh_shader" );
            spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

            RenderState     rs;

            rs.depth.test                   = true;
            rs.depth.write                  = true;

            rs.rasterization.frontFaceCCW   = false;
            rs.rasterization.cullMode       = ECullMode::None;

            spec.SetRenderState( rs );
        }
    }

#endif
//-----------------------------------------------------------------------------

#ifdef SH_MESH
    #include "Frustum.glsl"

    ND_ bool  IsVisible (float3 center, float radius) {
        return Frustum_TestSphere( un_PerPass.camera.frustum, center, radius );
    }

    void Main ()
    {
        const uint      I       = gl.LocalInvocationID.x;
        const float     radius  = un_Cube.sphereRadius;
        const float3    vpos    = (un_PerObject.transform * float4(0.0, 0.0, 0.0, 1.0)).xyz;

        gl.MeshVertices[I].gl_Position  = un_PerPass.camera.viewProj * (un_PerObject.transform * float4( un_Cube.positions[I], 1.0 ));
        Out[I].color                    = float4( ToUNorm( un_Cube.normals[I] ), 1.0 );

        if ( I < 12 )
            gl.PrimitiveTriangleIndices[I] = un_Cube.indices[I];

        if ( I == 0 )
        {
            if ( IsVisible( vpos, radius ))
                gl.SetMeshOutputs( 24, 12 );
            else
                gl.SetMeshOutputs( 0, 0 );
        }
    }

#endif
//-----------------------------------------------------------------------------

#ifdef SH_FRAG

    void Main ()
    {
        out_Color = In.color;
    }
#endif

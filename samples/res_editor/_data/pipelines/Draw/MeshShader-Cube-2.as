// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#   include <pipeline_compiler>
#   include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

    void ASmain ()
    {
        const uint  mesh_per_thread = 32;

        // Shader IO
        {
            RC<ShaderStructType>    st = ShaderStructType( "MeshPayload" );
            st.Set( EStructLayout::InternalIO,
                    "float3         pos;" +
                    "float          scale;" +
                    "float          lod;" );
        }{
            RC<ShaderStructType>    st = ShaderStructType( "tasktomesh.io" );
            st.Set( EStructLayout::InternalIO,
                    "MeshPayload    payload [" + ToString(mesh_per_thread) + "];" );
        }{
            RC<ShaderStructType>    st = ShaderStructType( "meshtofrag.io" );
            st.Set( EStructLayout::InternalIO,
                    "float4         color;" );
        }

        // pipeline layout
        {
            RC<DescriptorSetLayout> ds = DescriptorSetLayout( "mesh_sh.ds" );
            ds.UniformBuffer( EShaderStages::MeshTask | EShaderStages::Mesh, "un_PerObject", ArraySize(1), "UnifiedGeometryMaterialUB" );
            ds.StorageBuffer( EShaderStages::MeshTask | EShaderStages::Mesh, "un_Cube",      ArraySize(1), "mesh.CubeUBlock", EAccessType::Coherent, EResourceState::ShaderStorage_Read );  // external
            ds.StorageBuffer( EShaderStages::MeshTask,                       "un_DrawTasks", ArraySize(1), "mesh.DrawTask", EAccessType::Coherent, EResourceState::ShaderStorage_Read );  // external
        }{
            RC<PipelineLayout>      pl = PipelineLayout( "mesh_sh.pl" );
            pl.DSLayout( "pass",     0, "pass.ds" );
            pl.DSLayout( "material", 1, "mesh_sh.ds" );
        }


        RC<MeshPipeline>    ppln = MeshPipeline( "mesh_sh.draw1" );
        ppln.SetLayout( "mesh_sh.pl" );
        ppln.SetShaderIO( EShader::MeshTask, EShader::Mesh,     "tasktomesh.io" );
        ppln.SetShaderIO( EShader::Mesh,     EShader::Fragment, "meshtofrag.io" );
        ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

        {
            RC<Shader>  ts = Shader();
            ts.version  = EShaderVersion::SPIRV_1_4;
            ts.LoadSelf();
            ts.MeshLocalSize( mesh_per_thread );
            ppln.SetTaskShader( ts );
        }{
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

#ifdef SH_MESH_TASK
    #include "Frustum.glsl"

    shared uint  s_TaskCount;

    ND_ bool  IsVisible (float3 center, float radius) {
        return Frustum_TestSphere( un_PerPass.camera.frustum, center, radius );
    }

    void Main ()
    {
        const uint      I           = gl.LocalInvocationID.x;
        const uint      gid         = gl.GlobalInvocationID.x;

        // initialize shared
        {
            if ( I == 0 )
                s_TaskCount = 0;

            gl.memoryBarrier.Shared();
            gl.WorkgroupBarrier();
        }

        const float2    task_pos    = un_DrawTasks.tasks[gid].xy;
        const float     scale       = un_DrawTasks.tasks[gid].z;
        const float     time_offset = un_DrawTasks.tasks[gid].w;
        const float     radius      = un_Cube.sphereRadius * scale;
        const float3    pos         = float3(task_pos.x, Sin( un_PerPass.time + time_offset ), task_pos.y );
        const float3    vpos        = (un_PerObject.transform * float4(pos, 1.0)).xyz;

        if ( IsVisible( vpos, radius ))
        {
            const uint  idx         = gl.AtomicAdd( INOUT s_TaskCount, 1 );

            Out.payload[idx].pos    = pos;
            Out.payload[idx].scale  = scale;
            Out.payload[idx].lod    = 0.f;
        }

        gl.WorkgroupBarrier();
        gl.memoryBarrier.Shared();

        if ( I == 0 )
            gl.EmitMeshTasks( s_TaskCount, 1, 1 );
    }

#endif
//-----------------------------------------------------------------------------

#ifdef SH_MESH
    #include "Math.glsl"

    void Main ()
    {
        const uint      gid     = gl.WorkGroupID.x;
        const uint      I       = gl.LocalInvocationID.x;

        const float     scale   = In.payload[gid].scale;
        const float4    pos     = float4( In.payload[gid].pos + un_Cube.positions[I] * scale, 1.f );

        gl.MeshVertices[I].gl_Position  = un_PerPass.camera.viewProj * (un_PerObject.transform * pos);
        Out[I].color                    = float4( ToUNorm( un_Cube.normals[I] ), 1.0 );

        if ( I < 12 )
            gl.PrimitiveTriangleIndices[I] = un_Cube.indices[I];

        gl.SetMeshOutputs( AE_maxVertices, AE_maxPrimitives );
    }

#endif
//-----------------------------------------------------------------------------

#ifdef SH_FRAG

    void Main ()
    {
        out_Color = In.color;
    }

#endif
//-----------------------------------------------------------------------------

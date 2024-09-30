// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

*/
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// max number of mesh groups per single task shader thread
		const uint	task_local_size = 32;

		const uint	cube_vert_count	= 24;
		const uint	cube_prim_count	= 12;

		// Shader IO
		{
			RC<ShaderStructType>	st = ShaderStructType( "MeshPayload" );
			st.Set( EStructLayout::InternalIO,
					"float3			pos;" +
					"float			scale;" +
					"float			lod;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "task-mesh.io" );
			st.Set( EStructLayout::InternalIO,
					"MeshPayload	payload [" + ToString(task_local_size) + "];" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "mesh-frag.io" );
			st.Set( EStructLayout::InternalIO,
					"float4			color;" );
		}

		// pipeline layout
		{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::MeshTask | EShaderStages::Mesh, "un_PerObject", "UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::MeshTask | EShaderStages::Mesh, "un_Cube",		 "CubeSBlock",    EResourceState::ShaderStorage_Read );  // external
			ds.StorageBuffer( EShaderStages::MeshTask,						 "un_DrawTasks", "mesh.DrawTask", EResourceState::ShaderStorage_Read );  // external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}


		RC<MeshPipeline>	ppln = MeshPipeline( "tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetShaderIO( EShader::MeshTask, EShader::Mesh,		"task-mesh.io" );
		ppln.SetShaderIO( EShader::Mesh,	 EShader::Fragment,	"mesh-frag.io" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

		{
			RC<Shader>	ts = Shader();
			ts.version	= EShaderVersion::SPIRV_1_4;
			ts.LoadSelf();
			ts.MeshLocalSize( task_local_size );
			ppln.SetTaskShader( ts );
		}{
			RC<Shader>	ms = Shader();
			ms.version	= EShaderVersion::SPIRV_1_4;
			ms.LoadSelf();
			ms.MeshLocalSize( cube_vert_count );
			ms.MeshOutput( cube_vert_count, cube_prim_count, EPrimitive::TriangleList );
			ppln.SetMeshShader( ms );
		}{
			RC<Shader>	fs = Shader();
			fs.version	= EShaderVersion::SPIRV_1_4;
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<MeshPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState		rs;

			rs.depth.test					= true;
			rs.depth.write					= true;

			rs.rasterization.frontFaceCCW	= false;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------

#ifdef SH_MESH_TASK
	#include "Frustum.glsl"
	#include "Transform.glsl"

	WGShared uint  s_TaskCount;

	ND_ bool  IsVisible (float3 center, float radius) {
		return Frustum_TestSphere( un_PerPass.camera.frustum, center, radius );
	}

	void Main ()
	{
		const uint		I			= gl.LocalInvocationID.x;
		const uint		gid			= gl.GlobalInvocationID.x;

		// initialize shared
		{
			if ( I == 0 )
				s_TaskCount = 0;

			gl.memoryBarrier.Shared();
			gl.WorkgroupBarrier();
		}

		const float2	task_pos	= un_DrawTasks.tasks[gid].xy;
		const float		scale		= un_DrawTasks.tasks[gid].z;
		const float		time_offset	= un_DrawTasks.tasks[gid].w;
		const float		radius		= un_Cube.sphereRadius * scale;
		const float3	pos			= float3(task_pos.x, Sin( un_PerPass.time + time_offset ), task_pos.y );
		const float3	vpos		= LocalPosToWorldSpace( pos ).xyz;

		if ( IsVisible( vpos, radius ))
		{
			const uint	idx			= gl.AtomicAdd( INOUT s_TaskCount, 1 );

			Out.payload[idx].pos	= pos;
			Out.payload[idx].scale	= scale;
			Out.payload[idx].lod	= 0.f;
		}

		gl.WorkgroupBarrier();
		gl.memoryBarrier.Shared();

		if ( I == 0 )
			gl.EmitMeshTasks( s_TaskCount, 1, 1 );
	}

#endif
//-----------------------------------------------------------------------------

#ifdef SH_MESH
	#include "Transform.glsl"

	void Main ()
	{
		const uint		gid		= gl.WorkGroupID.x;
		const uint		I		= gl.LocalInvocationID.x;

		const float		scale	= In.payload[gid].scale;
		const float4	pos		= float4( In.payload[gid].pos + un_Cube.positions[I] * scale, 1.f );

		gl.MeshVertices[I].gl_Position	= LocalPosToClipSpace( pos );
		Out[I].color					= float4( ToUNorm( un_Cube.normals[I] ), 1.0 );

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

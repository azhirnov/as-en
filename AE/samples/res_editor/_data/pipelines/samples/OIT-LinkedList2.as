// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"int	instanceId;" +
					"float	viewDepth;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex,	"un_Geometry",	"GeometrySBlock",	EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Vertex,	"un_DrawTasks",	"DrawTask",			EResourceState::ShaderStorage_Read );	// external
			ds.StorageBuffer( EShaderStages::Fragment,	"un_Storage",	"IntrsPoint_Array",	EResourceState::ShaderStorage_RW );		// external
			ds.StorageBuffer( EShaderStages::Fragment,	"un_Count",		"CountSBlock",		EResourceState::ShaderStorage_RW );		// external
			ds.StorageImage(  EShaderStages::Fragment,	"un_ABuffer",	EImageType::UInt_2D, EPixelFormat::R32U, EAccessType::Restrict, EResourceState::ShaderStorage_RW );	// external
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		{
			RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
			ppln.SetLayout( "pl" );
			ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

			{
				RC<Shader>	vs = Shader();
				vs.LoadSelf();
				ppln.SetVertexShader( vs );
			}{
				RC<Shader>	fs = Shader();
				fs.LoadSelf();
				ppln.SetFragmentShader( fs );
			}

			// specialization
			{
				RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec" );
				spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

				RenderState	rs;

				rs.depth.test					= true;
				rs.depth.write					= false;

				rs.inputAssembly.topology		= EPrimitive::TriangleList;

				rs.rasterization.frontFaceCCW	= true;
				rs.rasterization.cullMode		= ECullMode::None;

				spec.SetRenderState( rs );
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Transform.glsl"

	void Main ()
	{
		const float3	sphere_pos	= un_DrawTasks.tasks[ gl.InstanceIndex ][0].xyz;
		const float		scale		= un_DrawTasks.tasks[ gl.InstanceIndex ][0].w;
		const float4	color		= un_DrawTasks.tasks[ gl.InstanceIndex ][1];

		const uint		idx			= gl.VertexIndex;
		const float3	vert_pos	= sphere_pos + (un_Geometry.positions[idx] * scale);
		const float4	world_pos	= LocalPosToWorldSpace( vert_pos );

		gl.Position		= WorldPosToClipSpace( world_pos );
		Out.instanceId	= gl.InstanceIndex;
		Out.viewDepth	= WorldPosToViewSpace( world_pos ).z;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	ND_ uint  Pack (uint instId, bool front)	{ return (uint(front) << 31) | (instId & 0xFFFF); }

	void Main ()
	{
		uint	st_pos	= gl.AtomicAdd( INOUT un_Count.counter, 1 );

		if ( st_pos < un_Storage.elements.length() )
		{
			uint	old_pos = gl.image.AtomicExchange( un_ABuffer, int2(gl.FragCoord.xy), st_pos );

			IntrsPoint	ip;
			ip.depth	= In.viewDepth;
			ip.objId	= Pack( In.instanceId, gl.FrontFacing );
			ip.next		= old_pos;

			un_Storage.elements[st_pos] = ip;
		}
	}

#endif
//-----------------------------------------------------------------------------

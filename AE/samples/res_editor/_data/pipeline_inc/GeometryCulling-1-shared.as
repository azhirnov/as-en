// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name, string pass)
	{
		if ( not HasPipelineLayout( "pl" ))
		{
			{
				RC<ShaderStructType>	st = ShaderStructType( "io" );
				st.Set( EStructLayout::InternalIO,
						"mediump float4		color;" +
						"mediump float3		worldPos;" );
			}{
				RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
				ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Vertex, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
				ds.StorageBuffer( EShaderStages::Vertex, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );
			}{
				RC<PipelineLayout>		pl = PipelineLayout( "pl" );
				pl.DSLayout( "pass",	 0, "pass.ds" );
				pl.DSLayout( "material", 1, "mtr.ds" );
			}
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+".t" );

		if ( pass == "pass-0" and HasDescriptorSetLayout( "subpass-0.ds" ))
		{
			RC<PipelineLayout>	pl = PipelineLayout( "pl-0" );
			pl.DSLayout( "pass",	 0, "subpass-0.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			ppln.SetLayout( "pl-0" );
		}else
		if ( pass == "pass-1" and HasDescriptorSetLayout( "subpass-1.ds" ))
		{
			RC<PipelineLayout>	pl = PipelineLayout( "pl-1" );
			pl.DSLayout( "pass",	 0, "subpass-1.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			ppln.SetLayout( "pl-1" );
		}else
			ppln.SetLayout( "pl" );

		ppln.SetFragmentOutputFromRenderTech( "rtech", pass );

		if ( name != "DepthPrePass" )
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( name == "DepthPrePass" ) vs.Define( "DEPTH_PRE_PASS" );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( name == "DepthLateTest" ) fs.Define( "LATE_ZS" );
			if ( name == "EarlyZDiscard" ) fs.Define( "DISCARD" );
			if ( name == "DepthPrePass" )  fs.Define( "DEPTH_PRE_PASS" );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", pass );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			
			if ( name != "WithoutDepthTest" ) {
				rs.depth.test				= true;
				rs.depth.write				= true;
			}

			if ( name == "DepthEqual" ) {
				rs.depth.write				= false;
				rs.depth.compareOp			= ECompareOp::Equal;
			}

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}
	
	void  CreatePipeline (string name)
	{
		CreatePipeline( name, "main" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Math.glsl"

	void Main ()
	{
		uint	obj_id;
		#ifdef iRemapIdx
			obj_id = un_RemapIdx.elements[ gl.InstanceIndex ].newIndex;
		#else
			obj_id = gl.InstanceIndex;
		#endif

		ObjectTransform	obj	= un_Transform.elements[ obj_id ];
		float3			pos = un_Geometry.positions[ gl.VertexIndex ];

		pos *= obj.scale;
		pos += obj.position;
		pos -= un_PerPass.camera.pos;

		gl.Position = un_PerPass.camera.viewProj * float4(pos, 1.0);

		#ifndef DEPTH_PRE_PASS
			Out.color		= unpackUnorm4x8( obj.color );
			Out.worldPos	= pos;
		#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"


	void Main ()
	{
	#ifdef DEPTH_PRE_PASS
	#else
		float3	norm	= ComputeNormalInWS_dxdy( In.worldPos );
		float	ndl		= Clamp( Dot( norm, Normalize(iLight) ), 0.1, 1.0 );

		out_Color = In.color * ndl * 0.5;

		#ifdef DISCARD
			// dosn't change depth, so earlyZS is still enabled
			if ( out_Color.a < 0.001 )
				gl.Discard;
		#endif
		#ifdef LATE_ZS
			gl.FragDepth = gl.FragCoord.z + out_Color.r * 0.00001;
		#endif
	#endif
	}

#endif
//-----------------------------------------------------------------------------

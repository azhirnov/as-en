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
						"mediump float3		worldPos;" +
						"mediump float2		uv;" +
						"uint				objId;" );
			}{
				RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
				ds.UniformBuffer( EShaderStages::Vertex, "un_PerObject", "UnifiedGeometryMaterialUB" );
				ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "un_Geometry",  "GeometryData",			EResourceState::ShaderStorage_Read );	// external
				ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "un_Transform", "ObjectTransform_Array",	EResourceState::ShaderStorage_Read );	// external
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

		if ( name != "DepthPrePass" and name != "VisibilityBuffer1Pass2" )
			ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			if ( name == "DepthPrePass" )			vs.Define( "DEPTH_PRE_PASS" );
			if ( name == "VisibilityBuffer1Pass2" )	vs.Define( "VIS_BUF_2" );
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( name == "DepthLateTest" )			fs.Define( "LATE_ZS" );
			if ( name == "EarlyZDiscard" )			fs.Define( "DISCARD" );
			if ( name == "DepthPrePass" )			fs.Define( "DEPTH_PRE_PASS" );
			if ( name == "VisibilityBuffer1Pass1" )	{ fs.Define( "VIS_BUF1_1" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			if ( name == "VisibilityBuffer1Pass2" )	{ fs.Define( "VIS_BUF1_2" );  if ( pass != "main" ) fs.Define( "VISBUF_SUBPASS" ); }
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name );
			spec.AddToRenderTech( "rtech", pass );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			if ( name != "WithoutDepthTest" and name != "VisibilityBuffer1Pass2" ) {
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
	#include "CodeTemplates.glsl"

	void Main ()
	{
	#ifdef VIS_BUF_2
		// full screen triangle
		gl.Position = FullscreenTrianglePos();
		return;

	#else

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
			Out.objId		= obj_id;
		#endif
	#endif
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#ifndef LATE_ZS
		layout(early_fragment_tests) in;
	#endif

	#include "Hash.glsl"
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"

	#ifdef VIS_BUF1_2
		float3  LocalPosToWorldSpace (ObjectTransform obj, uint vertId)
		{
			float3	pos = un_Geometry.positions[ vertId ];
			pos *= obj.scale;
			pos += obj.position;
			pos -= un_PerPass.camera.pos;
			return pos;
		}

		float4  WorldPosToClipSpace (float3 pos) {
			return un_PerPass.camera.viewProj * float4(pos, 1.0);
		}
	#endif

	#ifdef VISBUF_SUBPASS
		#define SubpassLoad( _tex_ )	gl.subpass.Load( in_##_tex_ )
	#else
		#define SubpassLoad( _tex_ )	gl.texture.Fetch( un_##_tex_, int2(gl.FragCoord.xy), 0 )
	#endif


	void Main ()
	{
	#ifdef DEPTH_PRE_PASS
		// nothing
		return;

	#elif defined(VIS_BUF1_1)
		out_VisBuf.rg = uint2( gl.PrimitiveID, In.objId );
		return;

	#else
		#ifdef VIS_BUF1_2
			const uint2			primId_objId = SubpassLoad( VisBuf ).rg;

			if ( primId_objId.y == 0xFFFF )
				gl.Discard;

			ObjectTransform		obj			= un_Transform.elements[ primId_objId.y ];
			const uint			primId		= primId_objId.x * 3;
			const uint3			idx			= uint3( un_Geometry.indices[primId], un_Geometry.indices[primId+1], un_Geometry.indices[primId+2] );
			float3				wpos0		= LocalPosToWorldSpace( obj, idx.x );
			float3				wpos1		= LocalPosToWorldSpace( obj, idx.y );
			float3				wpos2		= LocalPosToWorldSpace( obj, idx.z );
			BarycentricDeriv	deriv		= CalcFullBary( WorldPosToClipSpace( wpos0 ),
															WorldPosToClipSpace( wpos1 ),
															WorldPosToClipSpace( wpos2 ),
															ToSNorm( gl.FragCoord.xy * un_PerPass.invResolution.xy ),
															2.0 * un_PerPass.invResolution.xy );

			GradientInterpolationResults uv_res = Interpolate2DWithDeriv( deriv, un_Geometry.uvs[idx.x], un_Geometry.uvs[idx.y], un_Geometry.uvs[idx.z] );

			DerivativesOutput	wp_deriv	= Cal3DDeriv( deriv, wpos0, wpos1, wpos2 );
			const float3		norm		= Normalize( Cross( wp_deriv.db_dx, wp_deriv.db_dy ));	// ComputeNormalInWS_dxdy
			const float4		color		= unpackUnorm4x8( obj.color ) + float4(uv_res.dx, uv_res.dy) * 10.0;

		#else
			const float4		color		= In.color;
			const float3		norm		= ComputeNormalInWS_dxdy( In.worldPos );
		#endif

		float	ndl	= Clamp( Dot( norm, Normalize(iLight) ), 0.1, 1.0 );

		out_Color = color * ndl * 0.5;

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

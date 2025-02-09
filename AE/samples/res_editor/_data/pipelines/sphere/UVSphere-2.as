// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (bool dbgGS, string suffix)
	{
		{
			RC<PipelineLayout>		pl = PipelineLayout( "pl"+suffix );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			if ( dbgGS ) pl.AddDebugDSLayout( EShaderOpt::Trace, EShaderStages::Geometry );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl"+suffix );
		ppln.SetLayout( "pl"+suffix );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex,   EShader::Geometry, "vs-gs.io" );
		ppln.SetShaderIO( EShader::Geometry, EShader::Fragment, "gs-fs.io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	gs = Shader();
			if ( dbgGS ) gs.options = EShaderOpt::Trace;
			gs.LoadSelf();
			ppln.SetGeometryShader( gs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( "spec"+suffix );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;
			rs.inputAssembly.topology		= EPrimitive::Point;
			rs.rasterization.frontFaceCCW	= false;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "vs-gs.io" );
			st.Set( EStructLayout::InternalIO,
					"float4		color;" );
		}{
			RC<ShaderStructType>	st = ShaderStructType( "gs-fs.io" );
			st.Set( EStructLayout::InternalIO,
					"float4		color;" +
					"float2		uv;" +
					"float3		tangent;" +
					"float3		bitangent;" +
					"float3		center;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
		}

		CreatePipeline( false, "" );
		CreatePipeline( true,  ".gs-dbg" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "Color.glsl"
	#include "Hash.glsl"

	void Main ()
	{
		gl.Position = un_Dots.elements[ gl.VertexIndex ].pos_r;
		Out.color	= float4( HEHash11( gl.VertexIndex ));
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_GEOM
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "Quaternion.glsl"

	layout (points, invocations = 6) in;
	layout (triangle_strip, max_vertices = 4) out;


	float2  ProceduralQuadUV (uint idx)		{ return float2( (idx>>1)&1, idx&1 ); }

	float3  ProjectToSphere (const float2 snormCoord, ECubeFace face)
	{
		switch ( iProj )
		{
			case 0 :	return CM_IdentitySC_Forward(	snormCoord, face );
			case 1 :	return CM_TangentialSC_Forward(	snormCoord, face );
			case 2 :	return CM_EverittSC_Forward(	snormCoord, face );
			case 3 :	return CM_5thPolySC_Forward(	snormCoord, face );
			case 4 :	return CM_COBE_SC_Forward(		snormCoord, face );
			case 5 :	return CM_ArvoSC_Forward(		snormCoord, face );
		}
		return float3(0.0);
	}

	float2  Distortion2D (float2 n)
	{
		switch ( iProj )
		{
			case 0 :	return n;
			case 1 :	return CM_TangentialSC_Inverse( n );
			case 2 :	return CM_EverittSC_Inverse( n );
			case 3 :	return CM_5thPolySC_Inverse( n );
			case 4 :	return CM_COBE_SC_Inverse( n );
			case 5 :	return CM_ArvoSC_Inverse( n );
		}
	}

	float3  ProjectToCubemap (float3 dir, float3 tangent, float3 bitangent, float2 offset, const ECubeFace targetFace)
	{
	#if 1
		dir	= Normalize( dir + tangent * offset.x + bitangent * offset.y );
		dir = CM_InverseRotation( targetFace, dir );
	#else
		dir = QMul( QRotation_ChordOver2R( offset.x, tangent   ), dir );
		dir = QMul( QRotation_ChordOver2R( offset.y, bitangent ), dir );
		dir	= Normalize( dir );
		dir = CM_InverseRotation( targetFace, dir );
	#endif

		return dir;
	}

	void Main ()
	{
		dbg_EnableTraceRecording( gl.InvocationID == iDbgFace );

		const float3	dir		= gl_in[0].gl_Position.xyz;
		const float		radius	= iRadius;	//0.04; //gl_in[0].gl_Position.w;
		const ECubeFace	face	= gl.InvocationID;

	#if 1
		float3	tangent, bitangent;
		Ray_GetPerpendicular( dir, OUT tangent, OUT bitangent );
	#else
		float3	tangent		= CM_RotateVec( float3(0.0, 1.0, 0.0), targetFace );
		float3	bitangent	= CM_RotateVec( float3(0.0, 0.0, 1.0), targetFace );
	#endif

		const float4	color	= Rainbow( DHash12( float2( In[0].color.r, float(face) )));
		float3			proj_2d	[4];

		for (uint i = 0; i < 4; ++i)
		{
			float2	offset	= ToSNorm( ProceduralQuadUV( i )) * radius;
			proj_2d[i]		= ProjectToCubemap( dir, tangent, bitangent, offset, face );
		}

		const float	min_z	= iMinZ;
		if ( All4( proj_2d[0].z < min_z, proj_2d[1].z < min_z, proj_2d[2].z < min_z, proj_2d[3].z < min_z ))
			return;

		for (uint i = 0; i < 4; ++i)
		{
			gl.Layer		= face;
			gl.Position		= un_CBuf.proj * float4( Distortion2D( proj_2d[i].xy / proj_2d[i].z ), 1.0, 1.0 );
			Out.color		= color;
			Out.uv			= ProceduralQuadUV( i );
			Out.tangent		= tangent;
			Out.bitangent	= bitangent;
			Out.center		= dir;

			gl.EmitVertex();
		}
		gl.EndPrimitive();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "CubeMap.glsl"
	#include "CodeTemplates.glsl"

	float3  ProjectToSphere (const float2 snormCoord, ECubeFace face)
	{
		switch ( iProj )
		{
			case 0 :	return CM_IdentitySC_Forward(	snormCoord, face );
			case 1 :	return CM_TangentialSC_Forward(	snormCoord, face );
			case 2 :	return CM_EverittSC_Forward(	snormCoord, face );
			case 3 :	return CM_5thPolySC_Forward(	snormCoord, face );
			case 4 :	return CM_COBE_SC_Forward(		snormCoord, face );
			case 5 :	return CM_ArvoSC_Forward(		snormCoord, face );
		}
		return float3(0.0);
	}

	void Main ()
	{
		float2	uv	= gl.FragCoord.xy / un_PerPass.resolution.xy;
		float3	pos	= ProjectToSphere( ToSNorm(uv), ECubeFace(gl.Layer) );

		switch ( iView )
		{
			case 0 :
				out_Color = In.color;	break;

			case 1 :
			//	out_Color = In.color * Saturate( Length( ToSNorm( In.uv )));	break;
				out_Color = In.color * Saturate( Distance( In.center, pos ));	break;

			case 2 :
				out_Color = float4(In.uv, 0.0, 1.0);	break;

			case 3 :
				out_Color = float4(FSBarycentricWireframe( 1.0, 1.0 ).x);	break;
		}
	}

#endif
//-----------------------------------------------------------------------------

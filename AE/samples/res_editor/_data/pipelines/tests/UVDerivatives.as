// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string name, bool dbgFS)
	{
		EShaderOpt	dbg_mode = EShaderOpt::Trace;

		string	ppln_name = name;
		if ( dbgFS ) ppln_name += ".dbg_fs";

		{
			RC<PipelineLayout>		pl = PipelineLayout( "pl."+ppln_name );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
			if ( dbgFS ) pl.AddDebugDSLayout( 2, dbg_mode, EShaderStages::Fragment );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "t."+ppln_name );
		ppln.SetLayout( "pl."+ppln_name );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );
		ppln.SetShaderIO( EShader::Vertex, EShader::Fragment, "io" );

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			if ( dbgFS )	fs.options = dbg_mode;
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( ppln_name );
			spec.AddToRenderTech( "rtech", "main" );  // in ScriptSceneGraphicsPass

			RenderState	rs;

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::Back;

			spec.SetRenderState( rs );
		}
	}

	void ASmain ()
	{
		{
			RC<ShaderStructType>	st = ShaderStructType( "io" );
			st.Set( EStructLayout::InternalIO,
					"float2		uv;" );
		}{
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "un_VBuffer", "VBuffer", EResourceState::ShaderStorage_Read );
		}

		CreatePipeline( "ppln", false );
		CreatePipeline( "ppln", true );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		const uint		idx		= iShape*3 + gl.VertexIndex;
		const float4x4	proj	= un_VBuffer.projection[iProj];

		gl.Position = proj * float4( un_VBuffer.vertices[idx] + iCameraPos, 1.f );
		Out.uv		= un_VBuffer.uvs[idx];
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Matrix.glsl"
	#include "InvocationID.glsl"
	#include "../3party_shaders/VisibilityBuffer.glsl"

	int3  GetGlobalSize () {
		return int3( un_PerPass.resolution.xy, 1 );
	}

	float3  GetGlobalSizeRcp () {
		return float3( un_PerPass.invResolution.xy, 1.0 );
	}

	float4  LocalToClipSpace (float3 v) {
		return un_VBuffer.projection[iProj] * float4(v + iCameraPos, 1.0);
	}

	float3  LocalToWorldSpace (float3 v) {
		return v + iCameraPos;
	}

	float3  CalcCurrentPos ()
	{
		return float3( GetGlobalCoordSNorm().xy, gl.FragCoord.z );
	}


	void  Main ()
	{
		// transform triangle to clip space
		float4	p0		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+0] );
		float4	p1		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+1] );
		float4	p2		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+2] );
		float3	ip		= CalcCurrentPos();

		float2	uv0		= un_VBuffer.uvs[iShape*3+0];
		float2	uv1		= un_VBuffer.uvs[iShape*3+1];
		float2	uv2		= un_VBuffer.uvs[iShape*3+2];

		float2	ref_uv	= In.uv;
		float2	ref_dx	= gl.dFdx( In.uv );
		float2	ref_dy	= gl.dFdy( In.uv );
		float	ref_lod	= Log2( Max( Length(ref_dx), Length(ref_dy) ));

		// v1 from 'The Forge'
		BarycentricDeriv	deriv1	= CalcFullBary( p0, p1, p2, GetGlobalCoordSNorm().xy, 2.0 * un_PerPass.invResolution );
		GradientInterpolationResults uv1_res = Interpolate2DWithDeriv( deriv1, uv0, uv1, uv2 );

		// v2 from 'The Forge'
		// only for perspective projection
		BarycentricDeriv	deriv2	= CalcRayBary( LocalToWorldSpace( un_VBuffer.vertices[iShape*3+0] ),
												   LocalToWorldSpace( un_VBuffer.vertices[iShape*3+1] ),
												   LocalToWorldSpace( un_VBuffer.vertices[iShape*3+2] ),
												   float3(GetGlobalCoordSNorm().xy, 0.0), float3(0.0),
												   f4x4_Identity(), MatInverse(un_VBuffer.projection[iProj]),
												   2.0 * un_PerPass.invResolution );
		GradientInterpolationResults uv2_res = Interpolate2DWithDeriv( deriv2, uv0, uv1, uv2 );

		float	scale	= Exp10( float(iScale) );

		float2	a, b;
		switch ( iMode )
		{
			case 0 :	a = ref_uv;		b = uv1_res.interp;		break;
			case 1 :	a = ref_dx;		b = uv1_res.dx;			break;
			case 2 :	a = ref_dy;		b = uv1_res.dy;			break;

			case 3 :	a = ref_uv;		b = uv2_res.interp;		break;
			case 4 :	a = ref_dx;		b = uv2_res.dx;			break;
			case 5 :	a = ref_dy;		b = uv2_res.dy;			break;

		}
		switch ( iCmp )
		{
			case 0 :	out_Color = float4(Abs(a) * scale, 0.0, 1.0);		break;
			case 1 :	out_Color = float4(Abs(b) * scale, 0.0, 1.0);		break;
			case 2 :	out_Color = float4(Abs( a - b ) * scale, 0.0, 1.0);	break;
			case 3 :	out_Color = float4(Distance( a, b ) * scale);		break;
		}
	}

#endif
//-----------------------------------------------------------------------------

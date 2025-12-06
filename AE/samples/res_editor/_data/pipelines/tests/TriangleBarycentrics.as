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
			RC<DescriptorSetLayout>	ds = DescriptorSetLayout( "mtr.ds" );
			ds.UniformBuffer( EShaderStages::Vertex,	"un_PerObject",	"UnifiedGeometryMaterialUB" );
			ds.StorageBuffer( EShaderStages::Vertex | EShaderStages::Fragment, "un_VBuffer", "VBuffer", EResourceState::ShaderStorage_Read );
		}{
			RC<PipelineLayout>		pl = PipelineLayout( "pl" );
			pl.DSLayout( "pass",	 0, "pass.ds" );
			pl.DSLayout( "material", 1, "mtr.ds" );
		}

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( "tmpl" );
		ppln.SetLayout( "pl" );
		ppln.SetFragmentOutputFromRenderTech( "rtech", "main" );

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

			rs.inputAssembly.topology		= EPrimitive::TriangleList;

			rs.rasterization.frontFaceCCW	= true;
			rs.rasterization.cullMode		= ECullMode::None;

			spec.SetRenderState( rs );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT

	void Main ()
	{
		const uint		idx		= iShape*3 + gl.VertexIndex;
		const float4x4	proj	= un_VBuffer.projection[iProj];

		gl.Position = proj * float4( un_VBuffer.vertices[idx] + iCameraPos, 1.f );
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


	float3  CalcBarycentrics (float4 p0, float4 p1, float4 p2, float3 p)
	{
		float3	v0		= p1.xyz - p0.xyz;
		float3	v1		= p2.xyz - p0.xyz;
		float3	v2		= p  - p0.xyz;

		// determinant
		float	d00		= Dot( v0, v0 );
		float	d01		= Dot( v0, v1 );
		float	d11		= Dot( v1, v1 );
		float	d20		= Dot( v2, v0 );
		float	d21		= Dot( v2, v1 );
		float	denom	= d00 * d11 - d01 * d01;

		float3	bar;
		bar.y = (d11 * d20 - d01 * d21) / denom;
		bar.z = (d00 * d21 - d01 * d20) / denom;
		bar.x = 1.0 - bar.y - bar.z;
		return bar;
	}


	void  Main ()
	{
		// transform triangle to clip space
		float4	p0		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+0] );
		float4	p1		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+1] );
		float4	p2		= LocalToClipSpace( un_VBuffer.vertices[iShape*3+2] );
		float3	ip		= CalcCurrentPos();

	  #ifdef AE_fragment_shader_barycentric
		float3	ref_bar	= gl.BaryCoord;
	  #endif

		// TODO: works only for ortho projection
		float3	bar1	= CalcBarycentrics( p0, p1, p2, ip );

		// v1 from 'The Forge'
		float3	bar2	= CalcFullBary( p0, p1, p2, GetGlobalCoordSNorm().xy, 2.0 * un_PerPass.invResolution ).m_lambda;

		// v2 from 'The Forge'
		// only for perspective projection
		float3	bar3	= CalcRayBary( LocalToWorldSpace( un_VBuffer.vertices[iShape*3+0] ),
									   LocalToWorldSpace( un_VBuffer.vertices[iShape*3+1] ),
									   LocalToWorldSpace( un_VBuffer.vertices[iShape*3+2] ),
									   float3(GetGlobalCoordSNorm().xy, 0.0), float3(0.0),
									   f4x4_Identity(), MatInverse(un_VBuffer.projection[iProj]),
									   2.0 * un_PerPass.invResolution ).m_lambda;

		float	scale	= Exp10( float(iScale) );

		float3	bar;
		switch ( iMode )
		{
			case 0 :	bar = ref_bar;	break;
			case 1 :	bar = bar1;		break;
			case 2 :	bar = bar2;		break;
			case 3 :	bar = bar3;		break;
		}
		switch ( iCmp )
		{
			case 0 :	out_Color = float4(bar, 1.0);							break;
			case 1 :	out_Color = float4(Abs( ref_bar - bar ) * scale, 1.0);	break;
			case 2 :	out_Color = float4(Distance( ref_bar, bar ) * scale);	break;
		}
	}

#endif
//-----------------------------------------------------------------------------

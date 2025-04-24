// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Quad decals with correct texture coords.

	Calculate UV for projected quads and apply distortion correction for UV.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6), MipmapLevel(~0) );	cubemap.Name( "Cubemap tex" );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		RC<DynamicInt>	proj_type = DynamicInt();
		Slider( proj_type, "iProj",  0, 5,  1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.Constant(	"iProj",		proj_type );
			pass.Slider(	"iRadius",		0.01f,			0.4f,			0.1f );
			pass.Slider(	"iOffset",		float2(-0.5),	float2(0.5),	float2(0.0) );
			pass.Slider(	"iLod",			1,				8,				4 );
			pass.Slider(	"iOrient",		float3(-180.f),	float3(180.f),	float3(0.f) );
			pass.Slider(	"iMode",		0,				2,				2 );
			pass.Slider(	"iUVFix",		0,				1 );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( OrbitalCamera() );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iScale",		0.25f,			1.1f,		1.1f );
			pass.Slider( "iFov",		0.f,			90.f,		35.f );
			pass.Constant( "iProj",		proj_type );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	float3  Project (float3 n)
	{
		switch ( iProj )
		{
			case 0 :	n = CM_IdentitySC_Inverse( n );		break;
			case 1 :	n = CM_TangentialSC_Inverse( n );	break;
			case 2 :	n = CM_EverittSC_Inverse( n );		break;
			case 3 :	n = CM_5thPolySC_Inverse( n );		break;
			case 4 :	n = CM_COBE_SC_Inverse( n );		break;
			case 5 :	n = CM_ArvoSC_Inverse( n );			break;
		}
		return CM_IdentitySC_Forward( n.xy, ECubeFace(n.z) );
	}

	void  Main ()
	{
		float2	uv		= GetGlobalCoordSNormCorrected2() * iScale;
		float4	norm	= UVtoSphereNormal( uv, ToRad(iFov) );
		float3	uvw		= norm.xyz;

		uvw = float3x3(un_PerPass.camera.view) * uvw;
		uvw = Project( uvw );

		out_Color.rgb = gl.texture.Sample( un_CubeMap, uvw ).rgb * SmoothStep( norm.w, 0.0, 0.01 );
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "SDF.glsl"
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"


	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	float3  ProjectToSphere (const float2 snormCoord, ECubeFace face)
	{
		float3	n;
		switch ( iProj )
		{
			case 0 :	n = CM_IdentitySC_Forward(	snormCoord, face );  break;
			case 1 :	n = CM_TangentialSC_Forward(snormCoord, face );  break;
			case 2 :	n = CM_EverittSC_Forward(	snormCoord, face );  break;
			case 3 :	n = CM_5thPolySC_Forward(	snormCoord, face );  break;
			case 4 :	n = CM_COBE_SC_Forward(		snormCoord, face );  break;
			case 5 :	n = CM_ArvoSC_Forward(		snormCoord, face );  break;
		}
		return n;
	}

	float3  ProjectToCubeFace (float3 dir)
	{
		switch ( iProj )
		{
			case 0 :	dir = CM_IdentitySC_Inverse( dir );  break;
			case 1 :	dir = CM_TangentialSC_Inverse( dir );  break;
			case 2 :	dir = CM_EverittSC_Inverse( dir );  break;
			case 3 :	dir = CM_5thPolySC_Inverse( dir );  break;
			case 4 :	dir = CM_COBE_SC_Inverse( dir );  break;
			case 5 :	dir = CM_ArvoSC_Inverse( dir );  break;
		}
		return dir;
	}

	void  CirclesLayer (float2 ncoord, const float lod, out float3 uv_d, out float3 pos_face)
	{
		float3	n		= ProjectToSphere( ncoord, FaceIdx() );	// == normal
		float	radius	= Min( iRadius, 1.0 / (lod * 3.8) );
		float	max_off	= Max( 0.0, 0.45 - radius*3.0 );
		float2	offset	= (0.5 - Fract(lod)) + Clamp( iOffset, -max_off, +max_off );

		n = QMul( QRotationY(ToRad( iOrient.x )), n );
		n = QMul( QRotationX(ToRad( iOrient.y )), n );
		n = QMul( QRotationZ(ToRad( iOrient.z )), n );
		n = Normalize( n );

		float3	uv_face		= ProjectToCubeFace( n );
		float2	pos_on_face	= Floor( lod * uv_face.xy + Fract(lod) ) + offset;
		float3	center		= ProjectToSphere( pos_on_face / lod, ECubeFace(uv_face.z) );

		float3	tangent		= CM_Tangent( center, ECubeFace(uv_face.z) );
		float3	bitangent	= CM_Bitangent( center, ECubeFace(uv_face.z) );

		uv_d.x = Dot( n, tangent );
		uv_d.y = Dot( n, bitangent );
		uv_d.z = Distance( n, center );

		// correction
		if ( iUVFix == 1 ) {
			uv_d.xy = Normalize( uv_d.xy ) * uv_d.z / radius;
		}else{
			uv_d.xy /= radius;
		}

		uv_d.z -= radius;

		pos_face = float3( pos_on_face, uv_face.z );
	}


	void  Main ()
	{
		const float		lod		= iLod * 0.5;
		const float2	size	= float2(GetGlobalSize().xy);
		const float2	ncoord	= GetGlobalCoordSNorm().xy;
		float4			color	= float4(0.0);

		color.b = AA_QuadGrid( size * ncoord, lod/size, 12.5 ) * 0.6;

		float3	uv_d, pos_face;
		CirclesLayer( ncoord, lod, OUT uv_d, OUT pos_face );

		// isolines for distance
		if ( iMode == 0 )
		{
			color.rgb = SDF_Isolines2( uv_d.z * 100.0, 0.2 );
		}

		// isolines for distance from UV, which may be broken without UV correction
		if ( iMode == 1 )
		{
			uv_d.z  = Length( uv_d.xy ) - 1.0;
			color.rgb = SDF_Isolines2( uv_d.z * 5.0, 0.1 );
		}

		if ( iMode == 2 )
		{
			uv_d.xy = ToUNorm( uv_d.xy );
			if ( IsUNorm( uv_d.xy ))
			{
				color.rg = uv_d.xy;
				color.b  = 0.0;
				color.rg *= AA_QuadGrid( uv_d.xy*10.0, float2(1.0), 0.1 );
			}
		}

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------

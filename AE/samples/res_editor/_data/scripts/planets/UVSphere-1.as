// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Procedural sphere with approximation for perspective distortion.
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
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );
			pass.Constant( "iProj",		proj_type );
			pass.Slider( "iRadius",		0.f,	0.1f,	0.06f );
			pass.Slider( "iMode",		0,		1 );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( OrbitalCamera() );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",	cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iScale",		0.25f,			1.1f,		1.1f );
			pass.Slider( "iFov",		0.f,			90.f,		60.f );
			pass.Slider( "iRadius",		0.0f,			0.9f,		0.5f );	// used to check circle distortion
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

		// screen space circle
		{
			float	d  = SDF2_Circle( uv, 0.23 * iRadius );
					d  = SDF_OpSub( d, SDF2_Circle( uv, 0.2 * iRadius ));

			float	a  = SmoothStep( -d*4.0/iRadius, 0.02, 0.025 );
			float	b  = SmoothStep( -d*2.5/iRadius, 0.02, 0.025 );

			out_Color.rgb *= (1.0 - a);
			out_Color.rgb = Lerp( out_Color.rgb, float3(0.8, 0.0, 1.0), b );
		}

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"


	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	float3  ProjectToSphere (const float2 snormCoord)
	{
		switch ( iProj )
		{
			case 0 :	return CM_IdentitySC_Forward(	snormCoord, FaceIdx() );
			case 1 :	return CM_TangentialSC_Forward(	snormCoord, FaceIdx() );
			case 2 :	return CM_EverittSC_Forward(	snormCoord, FaceIdx() );
			case 3 :	return CM_5thPolySC_Forward(	snormCoord, FaceIdx() );
			case 4 :	return CM_COBE_SC_Forward(		snormCoord, FaceIdx() );
			case 5 :	return CM_ArvoSC_Forward(		snormCoord, FaceIdx() );
		}
		return float3(0.0);
	}

	void  Main ()
	{
		const float		lod				= 4.0;
		const float2	size			= float2(GetGlobalSize().xy);
		const float2	ncoord			= GetGlobalCoordSNorm().xy;
		const float3	pos_on_sphere	= ProjectToSphere( ncoord );	// == normal
		float4			color			= float4(0.0);

		color.b = AA_QuadGrid( size * ncoord, lod/size, 12.5 ) * 0.6;

		const float2	scale	= float2(lod);
		float			dist	= float_max;

		for (int y = -1; y <= 1; ++y)
		for (int x = -1; x <= 1; ++x)
		{
			const float2	pos_on_face	= Floor( scale * ncoord ) + float2(x,y) + 0.5;
			const float3	obj_pos		= ProjectToSphere( pos_on_face / scale );
			const float		d			= Distance( pos_on_sphere, obj_pos ) - iRadius;

			dist = Min( dist, d );
		}

		if ( iMode == 0 )
		{
			color.g = Saturate( 1.0 - SmoothStep( dist, -0.005, 0.005 ));
			color.b *= (1.0 - color.g);
		}

		if ( iMode == 1 )
		{
			color.rgb = SDF_Isolines( -dist * 100.0 );
		}

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------

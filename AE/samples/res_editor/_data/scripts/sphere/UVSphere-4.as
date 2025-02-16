// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Random distribution for circles on sphere.
	Single layer has not intersections between circles.
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
			//pass.Slider(	"iOrient",		float3(-180.f),	float3(180.f),	float3(0.f) );
			//pass.Slider(	"iOffset",		float2(-2.0),	float2(1.0),	float2(0.0) );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( OrbitalCamera() );
			pass.Output( "out_Color",		rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",		cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iScale",			0.25f,			1.1f,		1.1f );
			pass.Slider( "iFov",			0.f,			90.f,		35.f );
			pass.Constant( "iProj",			proj_type );
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
	#include "Normal.glsl"
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
			case 0 :	dir = CM_IdentitySC_Inverse(	dir );  break;
			case 1 :	dir = CM_TangentialSC_Inverse(	dir );  break;
			case 2 :	dir = CM_EverittSC_Inverse(		dir );  break;
			case 3 :	dir = CM_5thPolySC_Inverse(		dir );  break;
			case 4 :	dir = CM_COBE_SC_Inverse(		dir );  break;
			case 5 :	dir = CM_ArvoSC_Inverse(		dir );  break;
		}
		return dir;
	}


	float3  RandomRotation (float3 seed, float3 n)
	{
		float3	angle = float3(0.0);

		#if 1
			angle = ToSNorm( DHash33( seed * 200.0 ) );
			angle *= float_Pi;
		#else
			angle = ToRad( iOrient );
		#endif

		n = QMul( QRotationX( angle.x ), n );
		n = QMul( QRotationY( angle.y ), n );
		n = QMul( QRotationZ( angle.z ), n );

		return Normalize( n );
	}


	float2  RandomOffset (float3 seed)
	{
		#if 1
			return ToSNorm( DHash23( seed * 111.0 ));
		#else
			return iOffset;
		#endif
	}


	float4  CirclesLayer (float3 n, const float lod, const float radius, const float offsetScale, const float probability)
	{
		n = RandomRotation( float3(lod, radius, offsetScale), n );

		float3	uv_face		= ProjectToCubeFace( n );
		float2	pos_on_face	= Floor( lod * uv_face.xy + Fract(lod) );
		float3	seed		= float3( pos_on_face, uv_face.z );

		if ( DHash13( seed * 22.0 + 77.0 ) > probability )
			return float4(0.0);

				pos_on_face	+= (0.5 - Fract(lod)) + offsetScale * RandomOffset( seed );
				pos_on_face = Clamp( pos_on_face / lod, float2(-1.1), float2(1.1) );

		float3	center		= ProjectToSphere( pos_on_face, ECubeFace(uv_face.z) );

		float3	tangent		= CM_Tangent( center, ECubeFace(uv_face.z) );
		float3	bitangent	= CM_Bitangent( center, ECubeFace(uv_face.z) );

		float3	uv_d;
		uv_d.x = Dot( n, tangent );
		uv_d.y = Dot( n, bitangent );
		uv_d.z = DistanceOnSphere( n, center );

		// correction
		uv_d.xy = Normalize( uv_d.xy ) * uv_d.z / radius;

		float4	col = Rainbow( DHash13( seed ));

		col *= 1.0 - SmoothStep( uv_d.z - radius, 0.0, 0.01 );
		return col;
	}


	float4  Blend (float4 lhs, float4 rhs)
	{
		return lhs * (1.0 - rhs.a) + rhs * rhs.a;
	}

	float4  CirclesLayers (float3 pos)
	{
		float	prob = 0.5;
		float4	col  = float4(0.f);
	//	col = Blend( col, CirclesLayer( pos, 0.5, 0.5,   0.175,		prob ));
		col = Blend( col, CirclesLayer( pos, 0.5, 0.25,  0.325,		prob ));
	//	col = Blend( col, CirclesLayer( pos, 1.0, 0.25,  0.115,		prob ));
		col = Blend( col, CirclesLayer( pos, 1.0, 0.125, 0.285,		prob ));
	//	col = Blend( col, CirclesLayer( pos, 2.0, 0.06,  0.275,		prob ));
		col = Blend( col, CirclesLayer( pos, 2.0, 0.03,  0.39,		prob ));
	//	col = Blend( col, CirclesLayer( pos, 3.0, 0.03,  0.31,		prob ));
	//	col = Blend( col, CirclesLayer( pos, 4.0, 0.015, 0.36,		prob ));
		col = Blend( col, CirclesLayer( pos, 4.0, 0.008, 0.4,		prob ));
		col = Blend( col, CirclesLayer( pos, 4.0, 0.007, 0.39,		prob ));
		col = Blend( col, CirclesLayer( pos, 4.0, 0.006, 0.395,		prob ));
		return col;
	}


	void  Main ()
	{
		const float2	ncoord	= GetGlobalCoordSNorm().xy;
		float3			pos		= ProjectToSphere( ncoord, FaceIdx() );
		float4			color	= CirclesLayers( pos );

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------

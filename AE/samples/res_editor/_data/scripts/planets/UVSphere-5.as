// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Line decals on sphere.

	Performance:
		11.5ns per thread with iCount=4 on Mali G57
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
		RC<Image>	cubemap			= Image( EPixelFormat::RGBA8_UNorm, uint2(1024), ImageLayer(6), MipmapLevel(~0) );
		RC<Image>	cubemap_view	= cubemap.CreateView( EImage::Cube );

		RC<DynamicInt>	proj_type = DynamicInt();
		Slider( proj_type, "iProj",  0, 5,  1 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );
			pass.Constant(	"iProj",		proj_type );
			pass.Slider(	"iMode",		0,		1,		0 );
			pass.Slider(	"iCount",		1,		100,	4 );
			pass.Slider(	"iHScale",		0.1f,	100.f,	26.43f );
			pass.Slider(	"iHBias",		0.f,	100.f,	15.46f );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Set( OrbitalCamera() );
			pass.Output( "out_Color",	rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",	cubemap_view,	Sampler_LinearRepeat );
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


	void  LineLayer (inout float4 color, inout float4 data, const float2 ncoord, const float seed)
	{
		const float		seed2	= seed * iHScale + iHBias;
		const float3	plane_n	= float3(0.0, 0.0, 1.0);
		const float		plane_d	= 0.0;

		float3	n = ProjectToSphere( ncoord, ECubeFace(gl.WorkGroupID.z) );	// == normal

		// random rotation
		{
			Quat	q;
			q.data.x = HEHash11( floatBitsToUint( seed2 ));
			q.data.y = HEHash11( floatBitsToUint( seed2 + iHBias + 0.111*iHScale ));
			q.data.z = HEHash11( floatBitsToUint( seed2 + iHBias + 0.222*iHScale ));
			q.data.w = 1.0;
			q.data.xyz = ToSNorm( q.data.xyz );

			q = QNormalize( q );
			n = Normalize( QMul( q, n ));
		}

		float2 uv;
		uv.x = ASin( SDF_Plane( n, plane_n, plane_d )) * float_InvPi;	// [-0.5, +0.5]

		float2	plane_uv = Plane_ProjectPoint( n, plane_n );
		uv.y = ATan( plane_uv.y, plane_uv.x ) * float_InvPi;			// [-1, +1]

		float2	scale	= float2(8.0, 1.1) * (DHash21( seed2*0.531 ) * float2(2.0,1.0) + float2(1.0,1.0));
		float2	uv2		= uv * scale;

		if ( ! IsSNorm( uv2 ))
			return;

		if ( iMode == 0 )
		{
			color = float4( ToUNorm(uv2) * AA_QuadGrid( uv*20.0, float2(1.0), 0.1 ), 0.0, 1.0 );
		}

		if ( iMode == 1 )
		{
			float	d = SDF2_Line( uv * scale.y, float2(0.0, -0.92), float2(0.0, 0.92) ) - 0.01;
			color.rgb = SDF_Isolines( -d * 200.0 );
		}
	}


	void  Main ()
	{
		const float2	size	= float2(GetGlobalSize().xy);
		const float2	ncoord	= GetGlobalCoordSNorm().xy;
		float4			color	= float4(0.0);
		float4			data	= float4(float_max);

		color.b = AA_QuadGrid( size * ncoord, 4.0/size, 12.5 ) * 0.6;

		for (uint i = 0; i < iCount; ++i) {
			LineLayer( INOUT color, INOUT data, ncoord, float(i+1)*1.62342 );
		}

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
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
		RC<Image>	noise			= Image( EImageType::Float_2D, "shadertoy/RGBANoiseSmall.png" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut(	"un_OutImage",	cubemap_view );
			pass.ArgIn(		"un_Noise",		noise,		Sampler_LinearRepeat );
			pass.Slider( "iRndMode",	0,		1,		1 );
			pass.Slider( "iHash",		0,		5 );
			pass.Slider( "iCount",		1,		400,	100 );
			pass.Slider( "iHScale",		0.1f,	100.f,	1.f );
			pass.Slider( "iHBias",		0.f,	100.f,	0.f );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( cubemap_view.Dimension2_Layers() );

			GenMipmaps( cubemap_view );
		}{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",		rt,				RGBA32f(0.0) );
			pass.ArgIn(  "un_CubeMap",		cubemap_view,	Sampler_LinearMipmapRepeat );
			pass.Slider( "iRotation",		float2(-180.f, -90.f),	float2(180.f, 90.f),	float2(0.f) );
			pass.Slider( "iScale",			0.25f,					1.1f,					1.1f );
			pass.Slider( "iFov",			0.f,					90.f,					0.f );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "CubeMap.glsl"
	#include "Geometry.glsl"
	#include "Quaternion.glsl"
	#include "InvocationID.glsl"

	float3  Project (float3 n)
	{
		n = CM_EverittSC_Inverse( n );
		return CM_IdentitySC_Forward( n.xy, ECubeFace(n.z) );
	}

	void  Main ()
	{
		float2	uv		= GetGlobalCoordSNormCorrected2() * iScale;
		float4	norm	= UVtoSphereNormal( uv, ToRad(iFov) );
		float3	uvw		= norm.xyz;

		uvw = QMul( QRotationY(ToRad( iRotation.x )), uvw );
		uvw = QMul( QRotationX(ToRad( iRotation.y )), uvw );
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

	float3  ProjectToSphere (const float2 snormCoord, ECubeFace face) {
		return CM_EverittSC_Forward( snormCoord, face );
	}

	float3  ProjectToCubeFace (float3 dir) {
		return CM_EverittSC_Inverse( dir );
	}


	float3  RandomRotation (const float seed, float3 n)
	{
		float	seed2 = seed * iHScale + iHBias;

		// random angle to quaternion
		if ( iRndMode == 0 )
		{
			float3	angle = float3(0.0);

			switch ( iHash )
			{
				case 0 :	angle = DHash31( seed2 );  break;
				case 1 :	angle = DHash31( seed2 )*0.8 + DHash31( seed * 54.23 )*0.2;  break;

				case 2 :
					angle.x = HEHash11( floatBitsToUint( seed2 ));
					angle.y = HEHash11( floatBitsToUint( seed2 + iHBias + 0.111*iHScale ));
					angle.z = HEHash11( floatBitsToUint( seed2 + iHBias + 0.222*iHScale ));
					break;

				case 3 :
					angle.x = Hash_Uniform( (seed * iHScale) + float2(iHBias, 0.0),		0.11 );
					angle.y = Hash_Uniform( (seed * iHScale) + float2(0.0, -iHBias),	0.22 );
					angle.z = Hash_Uniform( (seed * iHScale) + float2(iHBias, iHBias),	0.33 );
					break;

				case 4 : {
					float2	uv = float2(seed, seed+4.5)*0.0162982 * iHScale + iHBias;
					angle = gl.texture.Sample( un_Noise, uv ).rgb;
					break;
				}

				case 5 : {
					float2	uv;
					uv.x = HEHash11( floatBitsToUint( seed2 ));
					uv.y = HEHash11( floatBitsToUint( seed2 + iHBias + 0.111*iHScale ));
					angle = gl.texture.Sample( un_Noise, uv ).rgb;
					break;
				}
			}

			angle = ToSNorm( angle ) * float_Pi;
			n = QMul( QRotationX( angle.x ), n );
			n = QMul( QRotationY( angle.y ), n );
			n = QMul( QRotationZ( angle.z ), n );
		}

		// random quaternion normalization
		if ( iRndMode == 1 )
		{
			Quat	q;

			switch ( iHash )
			{
				case 0 :	q.data.xyz = DHash31( seed2 );  break;
				case 1 :	q.data.xyz = DHash31( seed2 )*0.8 + DHash31( seed * 54.23 )*0.2;  break;

				case 2 :
					q.data.x = HEHash11( floatBitsToUint( seed2 ));
					q.data.y = HEHash11( floatBitsToUint( seed2 + iHBias + 0.111*iHScale ));
					q.data.z = HEHash11( floatBitsToUint( seed2 + iHBias + 0.222*iHScale ));
					break;

				case 3 :
					q.data.x = Hash_Uniform( (seed * iHScale) + float2(iHBias, 0.0),	0.11 );
					q.data.y = Hash_Uniform( (seed * iHScale) + float2(0.0, -iHBias),	0.22 );
					q.data.z = Hash_Uniform( (seed * iHScale) + float2(iHBias, iHBias),	0.33 );
					break;

				case 4 : {
					float2	uv = float2(seed, seed+4.5)*0.0162982 * iHScale + iHBias;
					q.data.xyz = gl.texture.Sample( un_Noise, uv ).rgb;
					break;
				}

				case 5 : {
					float2	uv;
					uv.x = HEHash11( floatBitsToUint( seed2 ));
					uv.y = HEHash11( floatBitsToUint( seed2 + iHBias + 0.111*iHScale ));
					q.data.xyz = gl.texture.Sample( un_Noise, uv ).rgb;
					break;
				}
			}

			q.data.w = 1.0;
			q.data.xyz = ToSNorm( q.data.xyz );

			q = QNormalize( q );
			n = QMul( q, n );
		}

		return Normalize( n );
	}


	float4  CirclesLayer (float3 n, const float lod, const float radius, float seed1)
	{
		n = RandomRotation( seed1, n );

		float3	uv_face		= ProjectToCubeFace( n );
		float2	pos_on_face	= Floor( lod * uv_face.xy + Fract(lod) );
		float3	seed		= float3( pos_on_face, uv_face.z );

				pos_on_face	+= (0.5 - Fract(lod));
				pos_on_face = Clamp( pos_on_face / lod, float2(-1.1), float2(1.1) );

		float3	center		= ProjectToSphere( pos_on_face, ECubeFace(uv_face.z) );

		float	d	= DistanceOnSphere( n, center );
		float4	col	= Rainbow( DHash13( seed * 111.0 ));

		col *= 1.0 - SmoothStep( d - radius, 0.0, 0.01 );
		return col;
	}


	float4  Blend (float4 lhs, float4 rhs)
	{
		return lhs * (1.0 - rhs.a) + rhs * rhs.a;
	}

	float4  CirclesLayers (float3 pos)
	{
		float	prob = 0.5;
		float4	col  = float4(0.2f);

		for (int i = 0; i < iCount; ++i) {
			col = Blend( col, CirclesLayer( pos, 0.5, 0.02, float(i) ));
		}
		return col;
	}


	void  Main ()
	{
		const float2	size	= float2(GetGlobalSize().xy);
		const float2	ncoord	= GetGlobalCoordSNorm().xy;
		float3			pos		= ProjectToSphere( ncoord, FaceIdx() );
		float4			color	= CirclesLayers( pos );

		gl.image.Store( un_OutImage, GetGlobalCoord(), color );
	}

#endif
//-----------------------------------------------------------------------------

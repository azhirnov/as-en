// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
#	define GEN_NOISE
#	define GEN_TURB
#	define APPLY_SPLINE
#	define GEN_NORMAL
#	define sTILE		8	// sphere tile size
#endif
//-----------------------------------------------------------------------------
#if defined(SH_COMPUTE) && defined(sTILE)
	#include "GlobalIndex.glsl"

	ND_ int3  GlobalThreadID ()
	{
		int2	size	= GetGlobalSize().xy;
		int3	pos		= GetGlobalCoord();

		pos.xy += size * IndexToVec2( int(un_PerPass.frame), sTILE );
		return pos;
	}

	ND_ float2  GlobalThreadSNorm (int2 pos)
	{
		int2	size = GetGlobalSize().xy * sTILE;
		return UIndexToSNormRound( pos, size );
	}

	ND_ float2  GlobalThreadSNorm () {
		return GlobalThreadSNorm( GlobalThreadID().xy );
	}

	ND_ int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "Noise.glsl"
	#include "CubeMap.glsl"

	#undef GEN_NOISE
	#include "tools/TerrainNoise.glsl"	// add CombineNoise()

	#define CastUNorm	c = ToSNorm( n )
	#define CastSNorm	c = n

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )

	FBM_NOISE_A1_Hash( IQNoise,			float2, uv )
	FBM_NOISE_A1_Hash( WarleyNoise,		float3, seedScaleBias_offsetScale )
	FBM_NOISE_A1_Hash( VoronoiContour,	float3, seedScaleBias_offsetScale )
	FBM_NOISE_A2_Hash( VoronoiContour3,	float3, seedScaleBias_offsetScale, float3, hashScaleBiasOff )


	void  Main ()
	{
		const int3		tid		= GlobalThreadID();
		float4			noise	= gl.image.Load( un_Noise, tid );
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		const float3	uv		= float3( iParams.xy, 0.f );
		const float		vcos	= 0.75;	// scale for voronoi cell offset
		const float3	uv2		= float3( uv.xy, vcos );
	  #ifdef vTILE
		float3			pos		= GlobalThreadSNorm();
	  #else
		float3			pos		= CM_TangentialSC_Forward( GlobalThreadSNorm(), FaceIdx() );
	  #endif
						pos		= (pos * iPScale) + (iPBias * iPScale * 0.25);
		float			n		= 0.f;
		float			c		= 0.f;

		if ( iIsDistortion == 1 )
		{
			pos += noise.xyz;
			noise = noise.wwww;
		}

		#if iNoise_max != 16
		#	error iNoise max value must be 16
		#endif
		switch ( iNoise )
		{
			case 0 :	n = GradientNoise( pos );										CastSNorm;	break;
			case 1 :	n = ValueNoise( pos );											CastSNorm;	break;
			case 2 :	n = PerlinNoise( pos );											CastSNorm;	break;
			case 3 :	n = SimplexNoise( pos * 0.5 );									CastSNorm;	break;
			case 4 :	n = IQNoise( pos * 2.0, uv.xy );								CastSNorm;	break;
			case 5 :	n = Voronoi( pos, uv2 ).minDist;								CastUNorm;	break;
			case 6 :	n = VoronoiContour( pos, uv2 );									CastUNorm;	break;
			case 7 :	n = WarleyNoise( pos, uv2 );									CastUNorm;	break;
			case 8 :	n = VoronoiContour3( pos, float3(1.0,0.0,vcos), uv.xzy );		CastSNorm;	break;

			// FBM
			case 9 :	n = GradientNoiseFBM( pos, lac, pers, iOctaves );				CastSNorm;	break;
			case 10 :	n = ValueNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 11 :	n = PerlinNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 12 :	n = SimplexNoiseFBM( pos * 0.5, lac, pers, iOctaves );			CastSNorm;	break;
			case 13 :	n = WarleyNoiseFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 14 :	n = IQNoiseFBM( pos * 2.0, uv.xy, lac, pers, iOctaves );		CastUNorm;	break;
			case 15 :	n = VoronoiContourFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 16 :	n = VoronoiContour3FBM( pos, float3(1.0,0.0,vcos), uv.xzy, lac, pers, iOctaves );	CastSNorm;	break;
		}
		c = c * iVScaleBias.x + iVScaleBias.y;
		noise = CombineNoise( iOp, noise, float4(c) );
		gl.image.Store( un_Noise, tid, noise );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TURB
	#include "Noise.glsl"
	#include "CubeMap.glsl"

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )

	TURBULENCE_Hash( GradientNoise )
	TURBULENCE_Hash( ValueNoise )
	TURBULENCE_Hash( PerlinNoise )
	TURBULENCE_Hash( SimplexNoise )
	TURBULENCE_Hash( IQNoise )
	TURBULENCE_Hash( WarleyNoise )
	TURBULENCE_Hash( VoronoiContour )

	TURBULENCE_FBM_Hash( GradientNoiseFBM )
	TURBULENCE_FBM_Hash( ValueNoiseFBM )
	TURBULENCE_FBM_Hash( PerlinNoiseFBM )
	TURBULENCE_FBM_Hash( SimplexNoiseFBM )
	TURBULENCE_FBM_Hash( IQNoiseFBM )
	TURBULENCE_FBM_Hash( WarleyNoiseFBM )
	TURBULENCE_FBM_Hash( VoronoiContourFBM )


	void  Main ()
	{
		const int3		tid		= GlobalThreadID();
		float			src		= gl.image.Load( un_Noise, tid ).r;
		float3			dpos	= float3(0.0);
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		const float2	uv		= iParams.xy;
	  #ifdef vTILE
		float3			pos		= GlobalThreadSNorm();
	  #else
		float3			pos		= CM_TangentialSC_Forward( GlobalThreadSNorm(), FaceIdx() );
	  #endif
						pos		= (pos * iPScale) + (iPBias * iPScale * 0.25);

		#if iNoise_max != 14
		#	error iNoise max value must be 14
		#endif
		switch ( iNoise )
		{
			case 0 :	break;
			case 1 :	dpos = Turbulence_GradientNoise( pos );									break;
			case 2 :	dpos = Turbulence_ValueNoise( pos );									break;
			case 3 :	dpos = Turbulence_PerlinNoise( pos );									break;
			case 4 :	dpos = Turbulence_SimplexNoise( pos * 0.5 );							break;
			case 5 :	dpos = Turbulence_IQNoise( pos * 2.0 );									break;
			case 6 :	dpos = Turbulence_WarleyNoise( pos );									break;
			case 7 :	dpos = Turbulence_VoronoiContour( pos );								break;
			case 8 :	dpos = Turbulence_GradientNoiseFBM( pos, lac, pers, iOctaves );			break;
			case 9 :	dpos = Turbulence_ValueNoiseFBM( pos, lac, pers, iOctaves );			break;
			case 10 :	dpos = Turbulence_PerlinNoiseFBM( pos, lac, pers, iOctaves );			break;
			case 11 :	dpos = Turbulence_SimplexNoiseFBM( pos * 0.5, lac, pers, iOctaves );	break;
			case 12 :	dpos = Turbulence_IQNoiseFBM( pos * 2.0, lac, pers, iOctaves );			break;
			case 13 :	dpos = Turbulence_WarleyNoiseFBM( pos, lac, pers, iOctaves );			break;
			case 14 :	dpos = Turbulence_VoronoiContourFBM( pos, lac, pers, iOctaves );		break;
		}

		float4	noise;
		switch ( iOp )
		{
			case 0 :
				noise = float4( dpos * iDScale, src );
				break;

			case 1 :
				src = Max( ToUNorm(src), 0.0 ) * iDScale;
				noise = float4( dpos * src, 0.0 );
				break;
		}

		noise = Clamp( noise, -1.0, 1.0 );
		gl.image.Store( un_Noise, tid, noise );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef APPLY_SPLINE
	#include "tools/SplineHelper.glsl"

	void  Main ()
	{
		const int3	tid		= GlobalThreadID();
		float4		noise	= ToUNorm( gl.image.Load( un_Noise, tid ));

		noise.x = ApplySpline( noise.x, iMode, iA, iB ).x;
		noise.y = ApplySpline( noise.y, iMode, iA, iB ).x;
		noise.z = ApplySpline( noise.z, iMode, iA, iB ).x;
		noise.w = ApplySpline( noise.w, iMode, iA, iB ).x;

		noise = Saturate( noise ) * iScale + iBias;

		gl.image.Store( un_Noise, tid, ToSNorm( noise ));

		if ( GetGlobalIndex() == 0 )
		{
			un_Params.A		= iA;
			un_Params.B		= iB;
			un_Params.Mode	= iMode;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NORMAL
	#include "Normal.glsl"
	#include "CubeMap.glsl"

	float2	invSize;

	float3  GetPosition (int2 coord, int2 offset)
	{
		float2	ncoord	= ToSNorm( (float2(coord + offset) + 0.5) * invSize );
		float3	pos		= CM_TangentialSC_Forward( ncoord, FaceIdx() );
		float3	uvw		= CM_IdentitySC_Forward( ncoord, FaceIdx() );
		float	h		= gl.texture.SampleLod( un_HeightMap, uvw, 0.0 ).r;
		pos *= (1.0 + h * iHeight);
		return pos;
	}

	void  Main ()
	{
		invSize = 1.0 / float2(gl.texture.GetSize( un_HeightMap, 0 ).xy);

		const int3	tid	= GlobalThreadID();
		float3		norm;
		SmoothNormal3x3i( norm, GetPosition, tid.xy );

		gl.image.Store( un_NormalMap, tid, float4(-norm, 0.0) );
	}

#endif
//-----------------------------------------------------------------------------

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define GEN_NOISE
#	define GEN_TURB
#	define APPLY_SPLINE
#	define GEN_NORMAL
#	define sTILE		8	// sphere tile size
#endif
//-----------------------------------------------------------------------------
#if defined(SH_COMPUTE) && defined(sTILE)
	#include "InvocationID.glsl"

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
	#include "CubeMap.glsl"

	#undef GEN_NOISE
	#define NOISE_UTILS
	#include "tools/TerrainNoise.glsl"	// add CombineNoise()

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( Voronoi )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )
	FBM_NOISE_Hash( VoronoiContourSparse )


	void  Main ()
	{
		const int3		tid		= GlobalThreadID();
		float4			noise	= gl.image.Load( un_Noise, tid );
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
	  #ifdef vTILE
		float3			pos		= GlobalThreadSNorm();
	  #else
		float3			pos		= CM_TangentialSC_Forward( GlobalThreadSNorm(), FaceIdx() );
	  #endif
						pos		= (pos * iPScale) + (iPBias * iPScale * 0.25);
		float			n		= 0.f;

		NoiseParams		params;
		params.seedScale	= float3(iParams.x);
		params.seedBias		= float3(iParams.y);
		params.custom		= float4(-float_max);

		FBMParams		fbm = CreateFBMParams( lac, pers, iOctaves );

		if ( iIsDistortion == 1 )
		{
			pos += noise.xyz;
			noise = noise.wwww;
		}

		#if iNoise_max != 17
		#	error iNoise max value must be 17
		#endif
		switch ( iNoise )
		{
			case 0 :								n = GradientNoise( pos, params );			break;
			case 1 :								n = ValueNoise( pos, params );				break;
			case 2 :								n = PerlinNoise( pos, params );				break;
			case 3 :								n = SimplexNoise( pos * 0.5, params );		break;
			case 4 :	SetupIQWavelet( params );	n = IQNoise( pos * 2.0, params );			n = ToSNorm( n );	break;

			case 5 :	SetupVoronoi( params );		n = Voronoi( pos, params );					n = ToSNorm( n );	break;
			case 6 :	SetupVoronoi( params );		n = VoronoiContour( pos, params );			n = ToSNorm( n );	break;
			case 7 :	SetupIQWavelet( params );	n = WarleyNoise( pos, params );				n = ToSNorm( n );	break;
			case 8 :	SetupVCSparse( params );	n = VoronoiContourSparse( pos, params );	break;

			// FBM
			case 9 :								n = GradientNoiseFBM( pos, params, fbm );		break;
			case 10 :								n = ValueNoiseFBM( pos, params, fbm );			break;
			case 11 :								n = PerlinNoiseFBM( pos, params, fbm );			break;
			case 12 :								n = SimplexNoiseFBM( pos * 0.5, params, fbm );	break;
			case 13 :	SetupIQWavelet( params );	n = IQNoiseFBM( pos * 2.0, params, fbm );		n = ToSNorm( n );	break;

			case 14 :	SetupVoronoi( params );		n = VoronoiFBM( pos, params, fbm );				n = ToSNorm( n );	break;
			case 15 :	SetupIQWavelet( params );	n = WarleyNoiseFBM( pos, params, fbm );			n = ToSNorm( n );	break;
			case 16 :	SetupVoronoi( params );		n = VoronoiContourFBM( pos, params, fbm );		n = ToSNorm( n );	break;
			case 17 :	SetupVCSparse( params );	n = VoronoiContourSparseFBM( pos, params, fbm ); break;
		}

		n = n * iVScaleBias.x + iVScaleBias.y;
		noise = CombineNoise( iOp, noise, float4(n) );
		gl.image.Store( un_Noise, tid, noise );		// store SNorm
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TURB
	#include "CubeMap.glsl"

	#undef GEN_NOISE
	#define NOISE_UTILS
	#include "tools/TerrainNoise.glsl"	// add Setup*()

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( Voronoi )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )

	TURBULENCE_Hash( GradientNoise )
	TURBULENCE_Hash( ValueNoise )
	TURBULENCE_Hash( PerlinNoise )
	TURBULENCE_Hash( SimplexNoise )
	TURBULENCE_Hash( IQNoise )
	TURBULENCE_Hash( Voronoi )
	TURBULENCE_Hash( WarleyNoise )
	TURBULENCE_Hash( VoronoiContour )

	TURBULENCE_FBM_Hash( GradientNoiseFBM )
	TURBULENCE_FBM_Hash( ValueNoiseFBM )
	TURBULENCE_FBM_Hash( PerlinNoiseFBM )
	TURBULENCE_FBM_Hash( SimplexNoiseFBM )
	TURBULENCE_FBM_Hash( IQNoiseFBM )
	TURBULENCE_FBM_Hash( VoronoiFBM )
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

		NoiseParams		params;
		params.seedScale	= float3(iParams.x);
		params.seedBias		= float3(iParams.y);
		params.custom		= float4(-float_max);

		FBMParams		fbm = CreateFBMParams( lac, pers, iOctaves );

		#if iNoise_max != 16
		#	error iNoise max value must be 16
		#endif
		switch ( iNoise )
		{
			case 0 :	break;
			case 1 :								dpos = Turbulence_GradientNoise( pos, params );					break;
			case 2 :								dpos = Turbulence_ValueNoise( pos, params );					break;
			case 3 :								dpos = Turbulence_PerlinNoise( pos, params );					break;
			case 4 :								dpos = Turbulence_SimplexNoise( pos * 0.5, params );			break;
			case 5 :	SetupIQWavelet( params );	dpos = Turbulence_IQNoise( pos * 2.0, params );					break;

			// Voronoi
			case 6 :	SetupVoronoi( params );		dpos = Turbulence_WarleyNoise( pos, params );					break;
			case 7 :	SetupVoronoi( params );		dpos = Turbulence_Voronoi( pos, params );						break;
			case 8 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiContour( pos, params );				break;

			// FBM
			case 9 :								dpos = Turbulence_GradientNoiseFBM( pos, params, fbm );			break;
			case 10 :								dpos = Turbulence_ValueNoiseFBM( pos, params, fbm );			break;
			case 11 :								dpos = Turbulence_PerlinNoiseFBM( pos, params, fbm );			break;
			case 12 :								dpos = Turbulence_SimplexNoiseFBM( pos * 0.5, params, fbm );	break;
			case 13 :	SetupIQWavelet( params );	dpos = Turbulence_IQNoiseFBM( pos * 2.0, params, fbm );			break;

			// Voronoi FBM
			case 14 :	SetupVoronoi( params );		dpos = Turbulence_WarleyNoiseFBM( pos, params, fbm );			break;
			case 15 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiFBM( pos, params, fbm );				break;
			case 16 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiContourFBM( pos, params, fbm );		break;
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

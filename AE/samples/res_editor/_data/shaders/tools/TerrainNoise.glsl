// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#	define GEN_NOISE
#	define GEN_TURB
#	define APPLY_SPLINE
#	define GEN_NORMAL
#	define VIEW_SPLINE
#	define VIEW_1D
#	define VIEW_2D
#	define VIEW_2D_VEC3
#	define tTILE			8	// terrain tile size
#endif
//-----------------------------------------------------------------------------
#if defined(SH_COMPUTE) && defined(tTILE)
	#include "InvocationID.glsl"

	ND_ int2  GlobalThreadID ()
	{
		int2	size	= GetGlobalSize().xy;
		int2	pos		= GetGlobalCoord().xy;

		pos += size * IndexToVec2( int(un_PerPass.frame), tTILE );
		return pos;
	}

	ND_ float3  GlobalThreadSNorm ()
	{
		int2	size	= GetGlobalSize().xy * tTILE;
		int2	pos		= GlobalThreadID();
		return float3( UIndexToSNormRound( pos, size ), 0.f );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(GEN_NOISE) or defined(GEN_TURB)
#	define NOISE_UTILS
#endif
//-----------------------------------------------------------------------------
#ifdef NOISE_UTILS
	#include "Noise.glsl"

	ND_ float4  CombineNoise (const int2 op, float4 lhs, float4 rhs)
	{
		// 'rhs' op
		switch ( op.x )
		{
			case 0 :												break;
			case 1 :	rhs	= -rhs;									break;
			case 2 :	rhs	= Abs(rhs);								break;
			case 3 :	rhs	= GreaterF( rhs, float4(0.0) );			break;
			case 4 :	rhs	= LessF( rhs, 0.0 );					break;
			case 5 :	rhs = Clamp( rhs, 0.0, 1.0 );				break;
			case 6 :	rhs = Clamp( rhs + 1.0, 0.0, 1.0 );			break;
			case 7 :	rhs = Clamp( -rhs, 0.0, 1.0 );				break;
		}
		// combine op
		switch ( op.y )
		{
			case 0 :	lhs = rhs;										break;
			case 1 :	lhs = lhs + rhs;								break;
			case 2 :	lhs = lhs * rhs;								break;
			case 3 :	lhs = ToSNorm( ToUNorm(lhs) * ToUNorm(rhs) );	break;
			case 4 :	lhs = Min( lhs, rhs );							break;
			case 5 :	lhs = Max( lhs, rhs );							break;
		}
		return Clamp( lhs, -1.0, 1.0 );
	}

	void  SetupIQWavelet (inout NoiseParams params)
	{
		params.seedScale	= float3(1.0);
		params.seedBias		= float3(0.0);
		params.custom.xy	= iParams.xy;
	}

	// Voronoi, Warley
	void  SetupVoronoi (inout NoiseParams params)
	{
		params.custom.x = 0.9;
	}

	// Voronoi Contour Sparse
	void  SetupVCSparse (inout NoiseParams params)
	{
		SetupVoronoi( params );
		params.custom.yzw = float3(1.0, 0.0,  0.75);
	}

	// Voronoi Circles
	void  SetupVCircles (inout NoiseParams params)
	{
		SetupVoronoi( params );
		params.custom.y = 0.5;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE

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
		const int2		tid		= GlobalThreadID();
		float4			noise	= gl.image.Load( un_Noise, tid );
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		float3			pos		= (GlobalThreadSNorm() * iPScale) + (iPBias * iPScale * float3(0.25, 0.25, 0.01));
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

		#if iNoise_max != 22
		#	error iNoise max value must be 22
		#endif
		switch ( iNoise )
		{
			case 0 :								n = GradientNoise( pos, params );				break;
			case 1 :								n = ValueNoise( pos, params );					break;
			case 2 :								n = PerlinNoise( pos, params );					break;
			case 3 :								n = SimplexNoise( pos * 0.5, params );			break;
			case 4 :	SetupIQWavelet( params );	n = IQNoise( pos * 2.0, params );				n = ToSNorm( n );	break;
			case 5 :	SetupIQWavelet( params );	n = WaveletNoise( pos.xy * 0.25, params );		break;

			// Voronoi
			case 6 :	SetupVoronoi( params );		n = Voronoi( pos.xy, params );					n = ToSNorm( n );	break;
			case 7 :	SetupVoronoi( params );		n = Voronoi( pos, params );						n = ToSNorm( n );	break;
			case 8 :	SetupVoronoi( params );		n = VoronoiContour( pos.xy, params );			n = ToSNorm( n );	break;
			case 9 :	SetupVoronoi( params );		n = VoronoiContour( pos, params );				n = ToSNorm( n );	break;
			case 10 :	SetupVCSparse( params );	n = VoronoiContourSparse( pos.xy, params );		break;
			case 11 :	SetupVCSparse( params );	n = VoronoiContourSparse( pos, params );		break;
			case 12 :	SetupVCircles( params );	n = VoronoiCircles( pos.xy, params );			n = ToSNorm( n );	break;
			case 13 :	SetupVoronoi( params );		n = WarleyNoise( pos, params );					n = ToSNorm( n );	break;

			// FBM
			case 14 :								n = GradientNoiseFBM( pos, params, fbm );		break;
			case 15 :								n = ValueNoiseFBM( pos, params, fbm );			break;
			case 16 :								n = PerlinNoiseFBM( pos, params, fbm );			break;
			case 17 :								n = SimplexNoiseFBM( pos * 0.5, params, fbm );	break;
			case 18 :	SetupIQWavelet( params );	n = IQNoiseFBM( pos * 2.0, params, fbm );		n = ToSNorm( n );	break;

			// Voronoi FBM
			case 19 :	SetupVoronoi( params );		n = VoronoiFBM( pos, params, fbm );				n = ToSNorm( n );	break;
			case 20 :	SetupVoronoi( params );		n = WarleyNoiseFBM( pos, params, fbm );			n = ToSNorm( n );	break;
			case 21 :	SetupVoronoi( params );		n = VoronoiContourFBM( pos, params, fbm );		n = ToSNorm( n );	break;
			case 22 :	SetupVCSparse( params );	n = VoronoiContourSparseFBM( pos, params, fbm );break;
		}

		n = n * iVScaleBias.x + iVScaleBias.y;
		noise = CombineNoise( iOp, noise, float4(n) );
		gl.image.Store( un_Noise, tid, noise );		// store SNorm
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TURB

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
		const int2		tid		= GlobalThreadID();
		float			src		= gl.image.Load( un_Noise, tid ).r;
		float3			dpos	= float3(0.0);
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		const float3	pos		= (GlobalThreadSNorm() * iPScale) + (iPBias * iPScale * float3(0.25, 0.25, 0.01));

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
			case 5 :	SetupIQWavelet( params );	dpos = Turbulence_IQNoise( pos * 2.0, params ) - 0.5;			break;

			// Voronoi
			case 6 :	SetupVoronoi( params );		dpos = Turbulence_WarleyNoise( pos, params ) - 0.7;				break;
			case 7 :	SetupVoronoi( params );		dpos = Turbulence_Voronoi( pos, params ) - 0.25;				break;
			case 8 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiContour( pos, params ) - 0.1;			break;

			// FBM
			case 9 :								dpos = Turbulence_GradientNoiseFBM( pos, params, fbm );			break;
			case 10 :								dpos = Turbulence_ValueNoiseFBM( pos, params, fbm );			break;
			case 11 :								dpos = Turbulence_PerlinNoiseFBM( pos, params, fbm );			break;
			case 12 :								dpos = Turbulence_SimplexNoiseFBM( pos * 0.5, params, fbm );	break;
			case 13 :	SetupIQWavelet( params );	dpos = Turbulence_IQNoiseFBM( pos * 2.0, params, fbm ) - 0.5;	break;

			// Voronoi FBM
			case 14 :	SetupVoronoi( params );		dpos = Turbulence_WarleyNoiseFBM( pos, params, fbm ) - 0.7;		break;
			case 15 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiFBM( pos, params, fbm ) - 0.25;		break;
			case 16 :	SetupVoronoi( params );		dpos = Turbulence_VoronoiContourFBM( pos, params, fbm ) - 0.1;	break;
		}

		dpos += iDOffset;

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
		const int2	tid		= GlobalThreadID();
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
#ifdef VIEW_SPLINE
	#include "SDF.glsl"
	#include "InvocationID.glsl"
	#include "tools/SplineHelper.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
				uv.y = 1.0 - uv.y;

		float	y0	= ApplySpline( uv.x, un_Params.Mode, un_Params.A, un_Params.B ).x;

		float	x1	= GetGlobalCoordUNorm( int3(1) ).x;
		float	y1	= ApplySpline( x1, un_Params.Mode, un_Params.A, un_Params.B ).x;
		float	d	= SDF2_Line( uv, float2(uv.x,y0), float2(x1,y1) );

		out_Color = float4(0.0);
		if ( d < 0.002f )
		{
			out_Color.rgb = float3(1.0) * TriangleWave( uv.x * 50.0 ) * 1.5;
			out_Color.a = 1.0;
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_1D
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	ND_ float2  Noise (const int dx)
	{
		float	x	= GetGlobalCoordUNorm( int3(dx) ).x;
		float2	uv	= Saturate( float2( x, iYOffset ));

		uv = (float2(iTilePos) + uv) / float2(iTilePos_max+1);

	  #ifdef iLayer
		float	noise = gl.texture.Sample( un_Noise, float3(uv, iLayer) ).r;
	  #else
		float	noise = gl.texture.Sample( un_Noise, uv ).r;
	  #endif

		return float2( x, 1.0 - noise );
	}

	void  Main ()
	{
		float2	pos	= GetGlobalCoordUNorm().xy;		pos.y = pos.y * 1.05 - 0.025;
		float2	p0	= Noise( 0 );
		float2	p1	= Noise( 1 );
		float	d	= SDF2_Line( pos, p0, p1 );
		float	n	= p0.y;

		out_Color = pos.y > 0.5 ? float4(0.15, 0.15, 0.2, 1.0) : float4(0.2, 0.15, 0.15, 1.0);

		if ( d < 0.001f )
			out_Color = Rainbow( n );

		out_Color.rgb *= AA_QuadGrid( ToSNorm(pos) * un_PerPass.resolution.xy, float2(1.0/200.0), 3.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_2D
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy,
												  float2(gl.texture.GetSize( un_Noise, 0 )) );
	  #ifdef iLayer
		float	noise = gl.texture.Sample( un_Noise, float3(uv, iLayer) ).r;
	  #else
		float	noise = gl.texture.Sample( un_Noise, uv ).r;
	  #endif

		if ( IsUNorm( uv ))
			out_Color = Rainbow( ToUNorm( -noise ));
		else
			out_Color = float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_2D_VEC3
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy,
												  float2(gl.texture.GetSize( un_Noise, 0 )) );
	  #ifdef iLayer
		float3	noise = gl.texture.Sample( un_Noise, float3(uv, iLayer) ).rgb;
	  #else
		float3	noise = gl.texture.Sample( un_Noise, uv ).rgb;
	  #endif

		if ( IsUNorm( uv ))
			out_Color = float4( ToUNorm(noise), 1.0 );
		else
			out_Color = float4(0.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NORMAL
	#include "Normal.glsl"
	#include "CubeMap.glsl"

	float2	invSize;

	float3  GetPosition (int2 coord, int2 offset)
	{
		float3	pos;
		pos.xz = ToSNorm( float2(coord + offset) * invSize );
		pos.y  = gl.image.Load( un_HeightMap, coord + offset ).r * iHeight;
		return pos;
	}

	void  Main ()
	{
		invSize = iSize / float2(gl.image.GetSize( un_HeightMap )-1);

		const int2	tid	= GlobalThreadID();
		float3		norm;
		SmoothNormal3x3i( norm, GetPosition, tid );

		gl.image.Store( un_NormalMap, tid, float4(norm, 0.0) );
	}

#endif
//-----------------------------------------------------------------------------

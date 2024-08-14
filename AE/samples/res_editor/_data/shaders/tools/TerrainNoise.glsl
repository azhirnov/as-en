// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
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
	#include "GlobalIndex.glsl"

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
#ifdef SH_COMPUTE

	ND_ float4  CombineNoise (const int2 op, float4 lhs, float4 rhs)
	{
		// 'rhs' op
		switch ( op.x )
		{
			case 0 :												break;
			case 1 :	rhs	= -rhs;									break;
			case 2 :	rhs	= Abs(rhs);								break;
			case 3 :	rhs	= GreaterEqualFp( rhs, float4(0.0) );	break;
			case 4 :	rhs	= LessFp( rhs, float4(0.0) );			break;
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

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "Noise.glsl"

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
		const int2		tid		= GlobalThreadID();
		float4			noise	= gl.image.Load( un_Noise, tid );
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		const float3	uv		= float3( iParams.xy, 0.f );
		const float		vcos	= 0.75;	// scale for voronoi cell offset
		const float3	uv2		= float3( uv.xy, vcos );
		float3			pos		= (GlobalThreadSNorm() * iPScale) + (iPBias * iPScale * float3(0.25, 0.25, 0.01));
		float			n		= 0.f;
		float			c		= 0.f;

		if ( iIsDistortion == 1 )
		{
			pos += noise.xyz;
			noise = noise.wwww;
		}

		#if iNoise_max != 21
		#	error iNoise max value must be 21
		#endif
		switch ( iNoise )
		{
			case 0 :	n = GradientNoise( pos );										CastSNorm;	break;
			case 1 :	n = ValueNoise( pos );											CastSNorm;	break;
			case 2 :	n = PerlinNoise( pos );											CastSNorm;	break;
			case 3 :	n = SimplexNoise( pos * 0.5 );									CastSNorm;	break;
			case 4 :	n = IQNoise( pos * 2.0, uv.xy );								CastSNorm;	break;
			case 5 :	n = WaveletNoise( pos.xy * 0.25, uv.xy );						CastSNorm;	break;
			case 6 :	n = Voronoi( pos.xy, uv2 ).minDist;								CastUNorm;	break;
			case 7 :	n = Voronoi( pos, uv2 ).minDist;								CastUNorm;	break;
			case 8 :	n = VoronoiContour2( pos.xy, uv2 ).minDist;						CastUNorm;	break;
			case 9 :	n = VoronoiContour2( pos, uv2 ).minDist;						CastUNorm;	break;
			case 10 :	n = VoronoiContour3( pos.xy, float3(1.0,0.0,vcos), uv.xzy );	CastSNorm;	break;
			case 11 :	n = VoronoiContour3( pos, float3(1.0,0.0,vcos), uv.xzy );		CastSNorm;	break;
			case 12 :	n = VoronoiCircles( pos.xy, 0.5f, uv2 );						CastUNorm;	break;
			case 13 :	n = WarleyNoise( pos, uv2 );									CastUNorm;	break;

			// FBM
			case 14 :	n = GradientNoiseFBM( pos, lac, pers, iOctaves );				CastSNorm;	break;
			case 15 :	n = ValueNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 16 :	n = PerlinNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 17 :	n = SimplexNoiseFBM( pos * 0.5, lac, pers, iOctaves );			CastSNorm;	break;
			case 18 :	n = WarleyNoiseFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 19 :	n = IQNoiseFBM( pos * 2.0, uv.xy, lac, pers, iOctaves );		CastUNorm;	break;
			case 20 :	n = VoronoiContourFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 21 :	n = VoronoiContour3FBM( pos, float3(1.0,0.0,vcos), uv.xzy, lac, pers, iOctaves );	CastSNorm;	break;
		}

		c = c * iVScaleBias.x + iVScaleBias.y;
		noise = CombineNoise( iOp, noise, float4(c) );
		gl.image.Store( un_Noise, tid, noise );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TURB
	#include "Noise.glsl"

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
		const int2		tid		= GlobalThreadID();
		float			src		= gl.image.Load( un_Noise, tid ).r;
		float3			dpos	= float3(0.0);
		const float		lac		= Clamp( iParams.z, 0.1, 10.0 );
		const float		pers	= Clamp( iParams.w, 0.1, 2.0 );
		const float2	uv		= iParams.xy;
		const float3	pos		= (GlobalThreadSNorm() * iPScale) + (iPBias * iPScale * float3(0.25, 0.25, 0.01));

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
	#include "Geometry.glsl"
	#include "GlobalIndex.glsl"
	#include "tools/SplineHelper.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
				uv.y = 1.0 - uv.y;

		float	y0	= ApplySpline( uv.x, un_Params.Mode, un_Params.A, un_Params.B ).x;

		float	x1	= GetGlobalCoordUNorm( int3(1) ).x;
		float	y1	= ApplySpline( x1, un_Params.Mode, un_Params.A, un_Params.B ).x;
		float	d	= Line_MinDistance( float2(uv.x,y0), float2(x1,y1), uv );

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
	#include "Geometry.glsl"
	#include "GlobalIndex.glsl"

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

		return float2( x, 1.0 - ToUNorm(noise) );
	}

	void  Main ()
	{
		float2	pos	= GetGlobalCoordUNorm().xy;		pos.y = pos.y * 1.05 - 0.025;
		float2	p0	= Noise( 0 );
		float2	p1	= Noise( 1 );
		float	d	= Line_MinDistance( p0, p1, pos );
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
	#include "GlobalIndex.glsl"

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
	#include "GlobalIndex.glsl"

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

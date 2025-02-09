// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 100.f );
			camera.FovY( 50.f );

			const float	s = 0.8f;
			camera.ForwardBackwardScale( s );
			camera.UpDownScale( s );
			camera.SideMovementScale( s );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::None );
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iType",		0,					5,				3 );
			pass.Slider( "iMode",		0,					5,				1 );
			pass.Slider( "iFalloff",	0,					7,				1 );
			pass.Slider( "iOffset",		float2(-1.0),		float2(1.0),	float2(0.0) );
			pass.Slider( "iThick",		float2(0.0,1.0),	float2(8.0),	float2(0.1, 2.0) );
			pass.Slider( "iScale",		0.1,				10.0,			2.0 );
			pass.Slider( "iRotate",		0.0,				90.0,			0.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Ray.glsl"
	#include "Easing.glsl"
	#include "Geometry.glsl"
	#include "Intersectors.glsl"
	#include "GlobalIndex.glsl"


	float  ApplyFalloff (float x)
	{
		switch ( iFalloff )
		{
			case 0 :	return Cbrt( x );
			case 1 :	return Sqrt( x );	// best falloff
			case 2 :	return x;			// linear
			case 3 :	return HermiteEaseInOut( x );
			case 4 :	return QuadraticEaseInOut( x );
			case 5 :	return CubicEaseInOut( x );
			case 6 :	return x*x;
			case 7 :	return x*x*x;
		}
	}

	float2  Thickness ()
	{
		float2	thick = iThick;
		switch ( iMode )
		{
			case 0 :	thick = float2(0.1, 1.5);	break;
			case 1 :	thick = float2(0.0, 2.0);	break;	// best for 3D
			case 2 :	thick = float2(0.0, 2.75);	break;	// best for 2D
			case 3 :	thick = float2(0.5, 3.0);	break;
			case 4 :	thick = float2(0.9, 4.0);	break;
		}
		return thick;
	}


	void Main ()
	{
		const float		scale2d = 100.0;
		const float2	thick	= Thickness();

		// 3D
		float2	pos;
		bool	isec;
		{
			Ray	ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / un_PerPass.resolution.xy );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			pos = ray.pos.xz * iScale;

			ray = Ray_From( un_PerPass.camera.invViewProj, float3(0.0), un_PerPass.camera.clipPlanes.x, float2(0.5) );

			pos += Normalize( ray.dir.xz ) * un_PerPass.time * 0.2;
		}

		out_Color = float4(0.25);


		switch ( iType )
		{
			// 2D grid
			case 0 :
			{
				pos		= Floor( gl.FragCoord.xy ) + iOffset;
				pos		= SDF_Rotate2D( pos, ToRad(iRotate) );
				pos.x	+= TriangleWave( un_PerPass.time * 0.25 ) * 2.0;

				float	dist	= AA_QuadGrid( pos, float2(1.0/scale2d), thick );

				out_Color = float4( ApplyFalloff( dist ));
				break;
			}

			// 2D grid non-uniform
			case 1 :
			{
				pos		= Floor( gl.FragCoord.xy ) + iOffset;
				pos		= SDF_Rotate2D( pos, ToRad(iRotate) );
				pos.x	+= TriangleWave( un_PerPass.time * 0.25 ) * 2.0;

				float	dist	= AA_QuadGrid_dxdy( (pos + float2(Abs(pos.y)*0.1, 0.0)) / scale2d, thick ).x;

				out_Color = float4( ApplyFalloff( dist ));
				break;
			}

			// 2D circles + radial lines
			case 2 :
			{
						pos		= GetGlobalCoordSF().xy + iOffset;
				float	dist	= Length( pos );
						pos		= Normalize( pos );
				float	md		= AA_Lines( dist, 1.0/scale2d, thick );
						md		*= AA_RadialLines_dxdy( pos, 16.0, thick ).x;

				out_Color = float4( ApplyFalloff( md ));
				break;
			}

			// 3D grid, using derivatives
			case 3 :
			{
				float2	df = AA_QuadGrid_dxdy( pos, thick );

				if ( isec )
				{
					float	fog = SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 );
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, fog );
				}
				break;
			}

			// 3D subdiv grid, using derivatives
			case 4 :
			{
				float2	df = AA_QuadGridSubDiv_dxdy( pos, thick );

				if ( isec )
				{
					float	fog = SmoothStep( df.y, 5.5, 6.5 );
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, fog );
				}
				break;
			}

			// 3D circles + radial lines, using derivatives
			case 5 :
			{
				float2	df1		= AA_Circles_dxdy( pos, thick );
				float2	df2		= AA_RadialLines_dxdy( pos, 48.0, thick );
				float2	df		= float2( df1.x * df2.x, Max( df1.y, df2.y ));

				if ( isec )
				{
					float	fog = SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 );
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, fog );
				}
				break;
			}

			// 3D subdiv circles + radial lines, using derivatives
		/*	case 6 :
			{
				float2	df1		= AA_CirclesSubDiv_dxdy( pos, thick );
				float2	df2		= AA_RadialLinesSubDiv_dxdy( pos, 48.0, thick );
				float2	df		= float2( df1.x * df2.x, Max( df1.y, df2.y ));

				if ( isec )
				{
					float	fog = SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 );
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, fog );
				}
				break;
			}*/
		}
	}

#endif
//-----------------------------------------------------------------------------

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
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
			pass.Slider( "iType",		0,					3,				1 );
			pass.Slider( "iMode",		0,					5,				2 );
			pass.Slider( "iFalloff",	0,					7,				1 );
			pass.Slider( "iOffset",		float2(-1.0),		float2(1.0),	float2(0.0) );
			pass.Slider( "iThick",		float2(0.0,1.0),	float2(8.0),	float2(0.1, 2.0) );
			pass.Slider( "iScale",		0.1,				10.0,			1.0 );
			pass.Slider( "iRotate",		0.0,				90.0,			0.0 );

			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
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
			case 1 :	thick = float2(0.1, 2.0);	break;
			case 2 :	thick = float2(0.0, 2.75);	break;	// best
			case 3 :	thick = float2(0.5, 3.0);	break;
			case 4 :	thick = float2(0.9, 4.0);	break;
		}
		return thick;
	}


	void Main ()
	{
		const float		scale2d = 100.0;

		switch ( iType )
		{
			// 2D grid
			case 0 :
			{
				float2	pos		= Floor( gl.FragCoord.xy ) + iOffset;
						pos		= SDF_Rotate2D( pos, ToRad(iRotate) );
						pos.x	+= TriangleWave( un_PerPass.time * 0.25 ) * 2.0;

				float	dist	= AA_QuadGrid( pos, float2(1.0/scale2d), Thickness() );

				// use derivatives for non uniform space
				//float	dist	= AA_QuadGrid_dxdy( (pos + float2(Abs(pos.y)*0.1, 0.0)) / scale2d, Thickness() ).x;

				out_Color = float4( ApplyFalloff( dist ));
				break;
			}

			// 2D circles
			case 1 :
			{
				float2	pos		= GetGlobalCoordFloat().xy + iOffset;
				float	dist	= Length( pos );
						pos		= Normalize( pos );
				float	md		= AA_Lines( dist, 1.0/scale2d, Thickness() );
						md		*= AA_RadialLines_dxdy( pos, 16.0, Thickness() ).x;

				out_Color = float4( ApplyFalloff( md ));
				break;
			}

			// 3D grid + derivatives
			case 2 :
			{
				Ray	ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / un_PerPass.resolution.xy );

				out_Color = float4(0.25);

				float	t;
				bool	isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

				Ray_SetLength( ray, t );

				float2	thick	= Thickness();
				float2	pos		= ray.pos.xz * iScale;
				float2	df		= AA_QuadGrid_dxdy( pos, thick );

				if ( isec )
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 ));
				break;
			}

			// 3D circles + derivatives
			case 3 :
			{
				Ray	ray = Ray_From( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, gl.FragCoord.xy / un_PerPass.resolution.xy );

				out_Color = float4(0.25);

				float	t;
				bool	isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

				Ray_SetLength( ray, t );

				float2	thick	= Thickness();
				float2	pos		= ray.pos.xz * iScale;
				float2	df1		= AA_Circles_dxdy( pos, thick );
				float2	df2		= AA_RadialLines_dxdy( pos, 32.0, thick );
				float2	df		= float2( df1.x * df2.x, Max( df1.y, df2.y ));

				if ( isec )
					out_Color = Lerp( float4(ApplyFalloff( df.x )), out_Color, SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 ));
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------

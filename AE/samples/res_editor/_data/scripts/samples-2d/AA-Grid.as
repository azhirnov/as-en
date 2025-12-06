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
			RC<Postprocess>		pass = Postprocess();
			pass.Set( camera );
			pass.Output( "out_Color",	rt );
			pass.Slider( "iType",		0,					5,				3 );
			pass.Slider( "iMode",		0,					5,				1 );
			pass.Slider( "iFalloff",	0,					7,				1 );
			pass.Slider( "iOffset",		float2(-1.0),		float2(1.0),	float2(0.0) );
			pass.Slider( "iThick",		float2(0.0,1.0),	float2(8.0),	float2(0.1, 2.0) );
			pass.Slider( "iScale",		0.1,				10.0,			2.0 );
			pass.Slider( "iRotate",		0.0,				90.0,			0.0 );
			pass.Slider( "iInvColor",	0,					1 );
			pass.Slider( "iRadialLines",16,					64,				16 );
			pass.Slider( "iSubDiv",		float3(0.5),		float3(4.0),	float3(1.0) );
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
	#include "InvocationID.glsl"


	float  ApplyFalloff (float x)
	{
		switch ( iFalloff )
		{
			case 0 :	return Cbrt( x );
			case 1 :	return Sqrt( x );	// best falloff
			case 2 :	return x;			// linear,			best for iInvColor
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
			case 0 :	thick = float2(0.1,  1.5);	break;
			case 1 :	thick = float2(0.0,  2.0);	break;	// best for 3D
			case 2 :	thick = float2(0.0,  2.75);	break;	// best for 2D
			case 3 :	thick = float2(0.5,  3.0);	break;
			case 4 :	thick = float2(0.75, 4.0);	break;	// best for 3D with iInvColor
		}
		return thick;
	}


	float4  InvertColor (float x)
	{
		return float4(0.0, 1.0, 0.0, 0.0) * (1.0 - x);
	}


	void Main ()
	{
		const float		scale2d = 100.0;
		const float2	thick	= Thickness();

		// 3D
		float2	pos;
		bool	isec;
		{
			Ray	ray = Ray_Perspective( un_PerPass.camera.invViewProj, un_PerPass.camera.pos, un_PerPass.camera.clipPlanes.x, GetGlobalCoordUNorm().xy );

			float	t;
			isec = Plane_Ray_Intersect( ray, float3(0.0, 1.0, 0.0), float3(0.0, 1.0, 0.0), OUT t );

			Ray_SetLength( ray, t );

			pos = ray.pos.xz * iScale;

			ray = Ray_Perspective( un_PerPass.camera.invViewProj, float3(0.0), un_PerPass.camera.clipPlanes.x, float2(0.5) );

			if ( iType < 5 )
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

				float	dist = AA_QuadGrid( pos, float2(1.0/scale2d), thick );

				out_Color = float4( ApplyFalloff( dist ));

				if ( iInvColor == 1 )
					out_Color = InvertColor( out_Color.x );
				break;
			}

			// 2D grid non-uniform
			case 1 :
			{
				pos		= Floor( gl.FragCoord.xy ) + iOffset;
				pos		= SDF_Rotate2D( pos, ToRad(iRotate) );
				pos.x	+= TriangleWave( un_PerPass.time * 0.25 ) * 2.0;

				float	dist = AA_QuadGrid_dxdy( (pos + float2(Abs(pos.y)*0.1, 0.0)) / scale2d, thick ).x;

				out_Color = float4( ApplyFalloff( dist ));

				if ( iInvColor == 1 )
					out_Color = InvertColor( out_Color.x );
				break;
			}

			// 2D circles + radial lines
			case 2 :
			{
						pos		= GetGlobalCoordSF().xy + iOffset;
						pos.x	+= TriangleWave( un_PerPass.time * 0.25 ) * 2.0;
				float	dist	= Length( pos );
						pos		= Normalize( pos );
				float	md		= AA_Lines( dist, 1.0/scale2d, thick );
						md		*= AA_RadialLines_dxdy( pos, float(iRadialLines), thick ).x;

				out_Color = float4( ApplyFalloff( md ));

				if ( iInvColor == 1 )
					out_Color = InvertColor( out_Color.x );
				break;
			}

			// 3D grid, using derivatives
			case 3 :
			{
				float2	df = AA_QuadGrid_dxdy( pos, thick );

				if ( isec )
				{
					float4	grid_col = float4(ApplyFalloff( df.x ));
					if ( iInvColor == 1 )
						grid_col = InvertColor( grid_col.x );

					float	fog = SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 );
					out_Color = Lerp( grid_col, out_Color, fog );
				}
				break;
			}

			// 3D subdiv grid, using derivatives
			case 4 :
			{
				float2	df = AA_QuadGridSubDiv_dxdy( pos, float3(60.0, 0.6, 0.1) * iSubDiv, thick ).xy;

				if ( isec )
				{
					float4	grid_col = float4(ApplyFalloff( df.x ));
					if ( iInvColor == 1 )
						grid_col = InvertColor( grid_col.x );

					float	fog = SmoothStep( df.y / iSubDiv.x, 5.5, 6.5 );
					out_Color = Lerp( grid_col, out_Color, fog );
				}
				break;
			}

			// 3D circles + radial lines, using derivatives
			case 5 :
			{
				float2	df1		= AA_Circles_dxdy( pos, thick );
				float2	df2		= AA_RadialLines_dxdy( pos, float(iRadialLines), thick );
				float2	df		= float2( df1.x * df2.x, df1.y );

				if ( isec )
				{
					float4	grid_col = float4(ApplyFalloff( df.x ));
					if ( iInvColor == 1 )
						grid_col = InvertColor( grid_col.x );

					float	fog = SmoothStep( df.y, Max( 0.1, 0.3 - thick.x*0.1 ), 0.7 );
					out_Color = Lerp( grid_col, out_Color, fog );
				}
				break;
			}

			// 3D subdiv circles + radial lines, using derivatives
			/*case 6 :
			{
				float2	df1		= AA_CirclesSubDiv_dxdy( pos, float3(60.0, 0.6, 0.1) * iSubDiv, thick ).xy;
				float2	df2		= AA_RadialLinesSubDiv_dxdy( pos, float(iRadialLines), float3(60.0, 0.6, 0.1) * iSubDiv, thick ).xy;
				float2	df		= float2( df1.x * df2.x, Max( df1.y, df2.y ));

				if ( isec )
				{
					float4	grid_col = float4(ApplyFalloff( df.x ));
					if ( iInvColor == 1 )
						grid_col = InvertColor( grid_col.x );

					float	fog = SmoothStep( df.y / iSubDiv.x, 5.5, 6.5 );
					out_Color = Lerp( grid_col, out_Color, fog );
				}
				break;
			}*/
		}
	}

#endif
//-----------------------------------------------------------------------------

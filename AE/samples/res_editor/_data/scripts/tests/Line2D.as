// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	test geometry functions for 2D line
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PERPENDICULAR
#	define QUADRILATERAL
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>	mode	= DynamicUInt();
		RC<DynamicUInt>	anim	= DynamicUInt();

		Slider( mode,	"Mode",			0,	2,	0 );
		Slider( anim,	"Animation",	0,	1,	1 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "PERPENDICULAR=0" );
			pass.Output( "out_Color",		rt );
			pass.Constant( "iAnimation",	anim );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PERPENDICULAR=1" );
			pass.Output( "out_Color",		rt );
			pass.Constant( "iAnimation",	anim );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "QUADRILATERAL" );
			pass.Output( "out_Color",		rt );
			pass.Constant( "iAnimation",	anim );
			pass.EnableIfEqual( mode, 2 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PERPENDICULAR
	#include "SDF.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"

	bool  Line_Perpendicular2 (const float2 pos, const float2 begin, const float2 end, out float2 pointOnLine)
	{
		Line2d	line = Line_Create( begin, end );

	#if PERPENDICULAR == 1
		// same as 'Line_Perpendicular()'
		pointOnLine = Line_ProjectPoint( line, pos );
		return Line_PointInside( line, pointOnLine );
	#else
		float2	norm = LeftVector( end - begin );
		return	Line_RayIntersection( line, Line_Create( pos, pos + norm ), pointOnLine ) and
				Line_PointInside( line, pointOnLine );
	#endif
	}


	void  Main ()
	{
		float	uv_scale	= 2.0;
		float	line_width	= 0.01;
		float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;
		Line2d	line		= Line_Create( float2(-0.8, 0.8), float2(0.7, -0.8) );

		if ( iAnimation == 1 )
		{
			float2	dir	= SinCos( un_PerPass.time * 0.1 );
			line.begin	= dir;
			line.end	= -dir;
		}

		float	dist0		= SDF2_Line( uv, line.begin, line.end ) - line_width * 2.0;

		float	dist2		= float_max;
		float2	uv2			= float2(float_max);
		float2	point_on_line;

		if ( un_PerPass.mouse.z > 0.0 )
		{
			uv2 = MapPixCoordToSNormCorrected( Saturate(un_PerPass.mouse.xy) * un_PerPass.resolution.xy, float2(GetGlobalSize().xy) ) * uv_scale;

			if ( Line_Perpendicular2( uv2, line.begin, line.end, OUT point_on_line ))
				dist2 = SDF2_Line( uv, uv2, point_on_line ) - line_width;
		}
		else
		{
			for (float y = -1.0; y < 1.5; y += 2.0)
			for (float x = -uv_scale; x <= uv_scale; x += uv_scale/10.0)
			{
				float2	uv3 = float2(x, y);
				float	d	= float_max;
				float2	p;

				if ( Line_Perpendicular2( uv3, line.begin, line.end, OUT p ))
				{
					d = SDF2_Line( uv, uv3, p ) - line_width;
					if ( d < dist2 )
					{
						dist2			= d;
						uv2				= uv3;
						point_on_line	= p;
					}
				}
			}
		}

		float2	point	= Line_ProjectPoint( line, uv2 );
		float	dist3	= SDF2_Circle( uv - point, line_width );
		bool	side	= Line_PointOnLeftSide( line, uv2 );

		if ( dist3 < 0.0 )
			out_Color = float4(0.0, 0.0, 1.0, 1.0);
		else
		if ( dist0 < 0.0 and dist0 < Max( dist2, dist3 ))
			out_Color = float4(1.0, 0.0, 0.0, 1.0);
		else
		if ( dist2 < 0.0 and dist2 < dist3 )
			out_Color = side ? float4(0.0, 1.0, 0.0, 1.0) : float4(0.7, 0.8, 0.0, 1.0);
		else
			out_Color = float4(0.0, 0.0, 0.0, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef QUADRILATERAL
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float	uv_scale	= 4.0;
		float	line_width	= 0.02;
		float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		float2	v0	= float2(-0.8,   0.8);
		float2	v3	= float2(-0.85, -0.7);

		float2	v1	= float2( 0.5,   0.2);
		float2	v2	= float2( 0.6,  -0.3);

		if ( iAnimation == 1 )
		{
			float2	dir1 = SinCos( un_PerPass.time * 0.1 );
			float2	dir2 = SinCos( un_PerPass.time * 0.1 + float_Pi * 0.3 );

			v0 = float2(  dir1.x * 0.4 - 0.8,  Abs(dir1.y) );
			v3 = float2( -dir1.x * 0.4 - 0.8, -Abs(dir1.y) );

			v1 = float2(  dir2.x * 0.3 + 0.8,  Abs(dir2.y) );
			v2 = float2( -dir2.x * 0.3 + 0.8, -Abs(dir2.y) );
		}

		float	d0	= SDF2_Circle( uv - v0, line_width );
		float	d1	= SDF2_Circle( uv - v1, line_width );
		float	d2	= SDF2_Circle( uv - v2, line_width );
		float	d3	= SDF2_Circle( uv - v3, line_width );

		if ( d0 < 0.0 ) { out_Color = float4(1.0, 0.0, 0.0, 1.0);  return; }	// red
		if ( d1 < 0.0 ) { out_Color = float4(0.9, 0.9, 0.0, 1.0);  return; }	// yellow
		if ( d2 < 0.0 ) { out_Color = float4(0.0, 1.0, 0.0, 1.0);  return; }	// green
		if ( d3 < 0.0 ) { out_Color = float4(0.0, 0.4, 1.0, 1.0);  return; }	// blue

		if ( Quadrilateral_PointInside( v0, v1, v2, v3, uv ))
			out_Color = float4(0.0, 1.0, 0.0, 1.0);
		else
			out_Color = float4(0.0, 0.0, 0.0, 1.0);
	}

#endif
//-----------------------------------------------------------------------------

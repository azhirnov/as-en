// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	test geometry functions for rectangle
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INTERSECT_DIR
#	define INTERSECT_RECT
#	define CLIP_RECT
#	define JOIN_RECT
#	define CLIP_LINE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>	mode	= DynamicUInt();

		Slider( mode,	"Mode",		0,	3,	0 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "INTERSECT_DIR" );
			pass.Output( "out_Color",		rt );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "INTERSECT_RECT" );
			pass.Output( "out_Color",		rt );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CLIP_RECT" );
			pass.Output( "out_Color",		rt );
			pass.EnableIfEqual( mode, 2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "JOIN_RECT" );
			pass.Output( "out_Color",		rt );
			pass.EnableIfEqual( mode, 3 );
		/*}{
			RC<Postprocess>		pass = Postprocess( "", "CLIP_LINE" );
			pass.Output( "out_Color",		rt );
			pass.EnableIfEqual( mode, 4 );*/
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INTERSECT_DIR
	#include "SDF.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float		rect_size	= 1.0;
		const float		rect_border	= 0.013;
		const float		line_width	= 0.02;
		const float		edge_width	= 0.02;
		const float		point_size	= 0.02;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.2);

		float2	dir		= SinCos( un_PerPass.time * 0.4 );
		float2	point	= Rect_Intersection( dir );
		uint	edge	= Rect_DirToEdge( dir );

		// draw rectangle
		{
			float	d  = SDF2_Rect( uv, float2(rect_size + rect_border) );
					d  = SDF_OpSub( d, SDF2_Rect( uv, float2(rect_size - rect_border) ));
					d /= rect_size;
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.1, 0.0, 0.8), f );
		}

		// draw rectangle edge
		{
			Line2d	line = Rect_Edge( Rect_Create( float2(-rect_size), float2(rect_size) ), edge );

			float	d	= SDF2_Line( uv, line.begin, line.end ) - edge_width;
			float	f	= SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.4, 0.0, 1.0), f );
		}

		// draw ray
		{
			float	d	= SDF2_Line( uv, float2(0.0), dir * 2.0 ) - line_width;
			float	f	= SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 1.0, 0.0), f );
		}

		// draw intersection point
		{
			float	d	= SDF2_Circle( SDF_Move( uv, point ), point_size );
			float	f	= SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(1.0, 0.0, 0.0), f );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INTERSECT_RECT
	#include "SDF.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float		rect_border	= 0.013;
		const float2	mpos		= un_PerPass.mouse.z > 0.f ?
										ToSNorm( un_PerPass.mouse.xy ) * 1.5 :
										SinCos( un_PerPass.time * 0.5 ) * 0.9;
		const Rect		rect_a		= Rect_FromCenterSize( float2(0.0), float2(1.1) );
		const Rect		rect_b		= Rect_FromCenterSize( mpos, float2(0.5) );
		const bool		intersects	= Rect_Intersects( rect_a, rect_b );
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.2);

		// draw rectangle
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_a ));
			float2	hsize	= Rect_HalfSize( rect_a );

			float	d  = SDF2_Rect( pos, hsize + rect_border );
					d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));
					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 0.8, 0.0), f );
		}

		// draw rectangle
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_b ));
			float2	hsize	= Rect_HalfSize( rect_b );

			float	d  = SDF2_Rect( pos, hsize + rect_border );

			if ( ! intersects )
				d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));

					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.8, 0.0, 0.0), f );
		}
	}

#endif
//-----------------------------------------------------------------------------
#if defined(CLIP_RECT) or defined(JOIN_RECT)
	#include "SDF.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float		rect_border	= 0.013;
		const float2	mpos		= un_PerPass.mouse.z > 0.f ?
										ToSNorm( un_PerPass.mouse.xy ) * 1.5 :
										SinCos( un_PerPass.time * 0.5 ) * 0.9;
		const Rect		rect_a		= Rect_FromCenterSize( float2(0.0), float2(1.1) );
		const Rect		rect_b		= Rect_FromCenterSize( mpos, float2(0.5) );
		const bool		intersects	= Rect_Intersects( rect_a, rect_b );
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.2);

		// draw rectangle
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_a ));
			float2	hsize	= Rect_HalfSize( rect_a );

			float	d  = SDF2_Rect( pos, hsize + rect_border );
					d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));
					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 0.8, 0.0), f );
		}

		// draw rectangle
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_b ));
			float2	hsize	= Rect_HalfSize( rect_b );

			float	d  = SDF2_Rect( pos, hsize + rect_border );
					d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));

					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.8, 0.0, 0.0), f );
		}

		// draw intersection
	  #ifdef CLIP_RECT
		Rect	rect_c = Rect_Intersection( rect_a, rect_b );
	  #endif
	  #ifdef JOIN_RECT
		Rect	rect_c = Rect_Join( rect_a, rect_b );
	  #endif

		if ( ! Rect_IsInvalid( rect_c ))
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_c ));
			float2	hsize	= Rect_HalfSize( rect_c );

			float	d  = SDF2_Rect( pos, hsize + rect_border );
					d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));

					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 0.0, 1.0), f );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CLIP_LINE
	#include "SDF.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float		line_width	= 0.02;
		const float		rect_border	= 0.013;
		const Rect		rect_a		= Rect_FromCenterSize( float2(0.0), float2(1.1) );
		const float2	dir			= SinCos( un_PerPass.time * 0.4 );
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		Line2d	line = Line_Create( -dir * 2.0, dir * 2.0 );

		out_Color = float4(0.2);

		// draw rectangle
		{
			float2	pos		= SDF_Move( uv, Rect_Center( rect_a ));
			float2	hsize	= Rect_HalfSize( rect_a );

			float	d  = SDF2_Rect( pos, hsize + rect_border );
					d  = SDF_OpSub( d, SDF2_Rect( pos, hsize - rect_border ));
					d /= MaxOf( hsize );
			float	f  = SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 0.6, 0.0), f );
		}

		// draw ray
		{
			float	d	= SDF2_Line( uv, line.begin, line.end ) - line_width;
			float	f	= SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 0.0, 0.8), f );
		}

		// draw clipped line
		if ( Rect_Clip( rect_a, INOUT line ))
		{
			float	d	= SDF2_Line( uv, line.begin, line.end ) - line_width;
			float	f	= SmoothStep( -d*100.0, 0.8, 1.0 );

			out_Color.rgb = Lerp( out_Color.rgb, float3(0.0, 1.0, 0.0), f );
		}
	}

#endif
//-----------------------------------------------------------------------------

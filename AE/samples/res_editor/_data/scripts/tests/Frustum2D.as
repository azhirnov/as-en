// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	test 2D frustum culling
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CULL_POINT
#	define CULL_LINE
#	define CULL_AABB
#	define CULL_CIRCLE
#	define CULL_TRIANGLE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode	= DynamicUInt();
		RC<DynamicUInt>		anim	= DynamicUInt();
		RC<DynamicFloat>	obj_den	= DynamicFloat();
		RC<DynamicFloat2>	dyn_p0	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p1	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p2	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p3	= DynamicFloat2();

		Slider( mode,	"Mode",		0,	3,	0 );
		Slider( anim,	"Animation", 0,	1,	1 );

		Slider( obj_den, "ObjDensity",	0.01,	2.0,	1.0 );
		Slider( dyn_p0,	"P0",		float2(0.0),	float2(1.0),	float2(0.5) );	// left bottom
		Slider( dyn_p1,	"P1",		float2(0.0),	float2(1.0),	float2(0.5) );	// right bottom
		Slider( dyn_p2,	"P2",		float2(0.0),	float2(1.0),	float2(0.5) );	// left top
		Slider( dyn_p3,	"P3",		float2(0.0),	float2(1.0),	float2(0.5) );	// right top


		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "CULL_POINT" );
			pass.Output(	"out_Color",		rt );
			pass.Constant(	"iPointDensity",	obj_den );
			pass.Constant(	"iAnimation",		anim );
			pass.Constant(	"iP0",				dyn_p0 );
			pass.Constant(	"iP1",				dyn_p1 );
			pass.Constant(	"iP2",				dyn_p2 );
			pass.Constant(	"iP3",				dyn_p3 );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CULL_LINE" );
			pass.Output(	"out_Color",		rt );
			pass.Constant(	"iAnimation",		anim );
			pass.Constant(	"iLineDensity",		obj_den );
			pass.Constant(	"iP0",				dyn_p0 );
			pass.Constant(	"iP1",				dyn_p1 );
			pass.Constant(	"iP2",				dyn_p2 );
			pass.Constant(	"iP3",				dyn_p3 );
			pass.EnableIfEqual( mode, 1 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CULL_AABB" );
			pass.Output(	"out_Color",		rt );
			pass.Constant(	"iAnimation",		anim );
			pass.Constant(	"iRectDensity",		obj_den );
			pass.Constant(	"iP0",				dyn_p0 );
			pass.Constant(	"iP1",				dyn_p1 );
			pass.Constant(	"iP2",				dyn_p2 );
			pass.Constant(	"iP3",				dyn_p3 );
			pass.EnableIfEqual( mode, 2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CULL_CIRCLE" );
			pass.Output(	"out_Color",		rt );
			pass.Constant(	"iAnimation",		anim );
			pass.Constant(	"iCircleDensity",	obj_den );
			pass.Constant(	"iP0",				dyn_p0 );
			pass.Constant(	"iP1",				dyn_p1 );
			pass.Constant(	"iP2",				dyn_p2 );
			pass.Constant(	"iP3",				dyn_p3 );
			pass.EnableIfEqual( mode, 3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Hash.glsl"
	#include "Geometry2D.glsl"
	#include "InvocationID.glsl"

	void  DrawLine (float2 uv, float2 p0, float2 p1, float3 color, float width)
	{
		float	d	= SDF2_Line( uv, p0, p1 ) - width;
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		if ( out_Color.a < 1.0 )
		{
			out_Color.rgb = Lerp( out_Color.rgb, color, f );
			out_Color.a += f;
		}
	}

	void  DrawPoint (float2 uv, float2 center, float radius, float3 color)
	{
		float	d	= SDF2_Circle( SDF_Move( uv, center ), radius );
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		if ( out_Color.a < 1.0 )
		{
			//f *= out_Color.a;
			out_Color.rgb = Lerp( out_Color.rgb, color, f );
		}
	}


	Frustum2d	frustum;

	void  DrawFrustum (float2 uv)
	{
		float2	p0		= float2(-1.0,  1.0) * iP0;		// left bottom
		float2	p1		= float2( 1.0,  1.0) * iP1;		// right bottom
		float2	p2		= float2(-1.0, -1.0) * iP2;		// left top
		float2	p3		= float2( 1.0, -1.0) * iP3;		// right top

		if ( iAnimation == 1 )
		{
			float	t = un_PerPass.time * 0.1;

			p0 = float2(-1.0,  1.0) * Abs(float2(Sin( t ), Cos( t )));
			p1 = float2( 1.0,  1.0) * Abs(float2(Cos( t ), Sin( t )));	t += 0.835;
			p2 = float2(-1.0, -1.0) * Abs(float2(Sin( t ), Cos( t )));	t += 0.543;
			p3 = float2( 1.0, -1.0) * Abs(float2(Cos( t ), Sin( t )));
		}

		const float		w0		= 0.016;
		const float		w1		= 0.012;
		const float		n_len	= 0.2;

		frustum = Frustum2d_FromCornerPoints( p0, p1, p2, p3 );

		// planes
		DrawLine( uv, p0, p1, float3(1.0, 0.0, 0.0), w0 );	// bottom
		DrawLine( uv, p2, p3, float3(0.8, 0.6, 0.0), w0 );	// top
		DrawLine( uv, p0, p2, float3(0.0, 1.0, 0.0), w0 );	// left
		DrawLine( uv, p1, p3, float3(0.2, 0.8, 0.8), w0 );	// right

		// normals
		const float2	c01		= (p0 + p1) * 0.5;
		const float2	c23		= (p2 + p3) * 0.5;
		const float2	c02		= (p0 + p2) * 0.5;
		const float2	c13		= (p1 + p3) * 0.5;

		DrawLine( uv, c01, c01 + frustum.planes[3].xy * n_len, float3(1.0, 0.0, 0.0), w1 );
		DrawLine( uv, c23, c23 + frustum.planes[2].xy * n_len, float3(0.8, 0.6, 0.0), w1 );
		DrawLine( uv, c02, c02 + frustum.planes[0].xy * n_len, float3(0.0, 1.0, 0.0), w1 );
		DrawLine( uv, c13, c13 + frustum.planes[1].xy * n_len, float3(0.2, 0.8, 0.8), w1 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_POINT

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		DrawFrustum( uv );

		// draw points
		{
			float2	center	= RoundToBase( uv, 0.05 * iPointDensity );
			float	radius	= 0.015;
			bool	is_visible = Frustum2d_IsVisible( frustum, center );

			DrawPoint( uv, center, radius, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_LINE

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		DrawFrustum( uv );

		// draw lines
		{
			float	den		= 0.2 * iLineDensity;
			float2	center	= RoundToBase( uv, den );
			float2	dir		= ToSNorm( DHash22( center * 22.0 ));
			float	size	= DHash12( center * 11.0 ) * den + 0.02;
			float2	p0		= center + dir * size;
			float2	p1		= center - dir * size;
			bool	is_visible = Frustum2d_IsVisible( frustum, Line_Create( p0, p1 ));
			float	width	= 0.014;

			DrawLine( uv, p0, p1, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)), width );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_AABB

	void  DrawRect (float2 uv, float2 center, float2 hsize, float3 color)
	{
		float	d	= SDF2_Rect( SDF_Move( uv, center ), hsize );
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		if ( out_Color.a < 1.0 )
		{
			//f *= out_Color.a;
			out_Color.rgb = Lerp( out_Color.rgb, color, f );
		}
	}


	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		DrawFrustum( uv );

		// draw rectangles
		{
			float	size	= 0.2 * iRectDensity;
			float2	center	= RoundToBase( uv, size );
			float2	hsize	= DHash22( center ) * size * 0.5 + 0.015;
			bool	is_visible = Frustum2d_IsVisible( frustum, Rect_FromCenterHalfSize( center, hsize ));

			DrawRect( uv, center, hsize, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_CIRCLE

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		DrawFrustum( uv );

		// draw circles
		{
			float	size	= 0.2 * iCircleDensity;
			float2	center	= RoundToBase( uv, size );
			float	radius	= DHash12( center ) * size * 0.5 + 0.015;
			bool	is_visible = Frustum2d_IsVisible( frustum, Circle_Create( center, radius ));

			DrawPoint( uv, center, radius, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
		}
	}

#endif
//-----------------------------------------------------------------------------

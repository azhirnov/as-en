// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	test 2D frustum functions
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CENTER
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		anim	= DynamicUInt();
		RC<DynamicFloat2>	dyn_p0	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p1	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p2	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p3	= DynamicFloat2();

		Slider( anim,	"Animation", 0,	1,	1 );

		Slider( dyn_p0,	"P0",		float2(0.0),	float2(1.0),	float2(0.5) );	// left bottom
		Slider( dyn_p1,	"P1",		float2(0.0),	float2(1.0),	float2(0.5) );	// right bottom
		Slider( dyn_p2,	"P2",		float2(0.0),	float2(1.0),	float2(0.5) );	// left top
		Slider( dyn_p3,	"P3",		float2(0.0),	float2(1.0),	float2(0.5) );	// right top


		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"iAnimation",	anim );
			pass.Constant(	"iP0",			dyn_p0 );
			pass.Constant(	"iP1",			dyn_p1 );
			pass.Constant(	"iP2",			dyn_p2 );
			pass.Constant(	"iP3",			dyn_p3 );
			pass.Slider(	"iMode",		0,	3 );
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

		out_Color.rgb = Lerp( out_Color.rgb, color, f );
	}


	void  DrawCircle (float2 uv, float2 center, float radius, float3 color)
	{
		uv = SDF_Move( uv, center );

		float	w	= 0.01;
		float	d0	= SDF2_Circle( uv, radius + w );
		float	d1	= SDF2_Circle( uv, radius - w );
		float	d	= SDF_OpSub( d0, d1 );
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		out_Color.rgb = Lerp( out_Color.rgb, color, f );
	}


	void  DrawRect (float2 uv, float2 center, float2 hsize, float3 color)
	{
		uv = SDF_Move( uv, center );

		float	w	= 0.01;
		float	d0	= SDF2_Rect( uv, hsize + w );
		float	d1	= SDF2_Rect( uv, hsize - w );
		float	d	= SDF_OpSub( d0, d1 );
		float	f	= SmoothStep( -d*100.0, 0.5, 1.0 );

		out_Color.rgb = Lerp( out_Color.rgb, color, f );
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
		DrawLine( uv, p0, p1, float3(1.0, 0.0, 0.0), w0 );	// red		- bottom
		DrawLine( uv, p2, p3, float3(0.8, 0.6, 0.0), w0 );	// orange	- top
		DrawLine( uv, p0, p2, float3(0.0, 1.0, 0.0), w0 );	// green	- left
		DrawLine( uv, p1, p3, float3(0.2, 0.8, 0.8), w0 );	// blue		- right

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


	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		DrawFrustum( uv );

		switch ( iMode )
		{
			case 0 :
			{
				Rect	rect = Frustum2d_ToRect( frustum );

				DrawRect( uv, Rect_Center( rect ), Rect_HalfSize( rect ), float3(1.0, 1.0, 0.0) );
				break;
			}
			case 1 :
			{
				Circle	c = Frustum2d_ToCircle( frustum );

				DrawCircle( uv, c.center, c.radius, float3(1.0, 1.0, 0.0) );
				DrawPoint( uv, c.center, 0.01, float3(1.0, 1.0, 0.0) );
				break;
			}
			case 2 :
			{
				float2	points[4];
				Frustum2d_ToCornerPoints( frustum, OUT points );

				float2	cx0 = Average( points[0], points[1] );
				float2	cx1 = Average( points[2], points[3] );
				float2	cy0 = Average( points[0], points[2] );
				float2	cy1 = Average( points[1], points[3] );
				float	w	= 0.01;

				DrawLine( uv, cx0, cx1, float3(0.0, 0.0, 1.0), w );
				DrawLine( uv, cy0, cy1, float3(0.0, 0.6, 0.0), w );
				break;
			}
			case 3 :
			{
				float2	points[4];
				Frustum2d_ToCornerPoints( frustum, OUT points );

				float	w	= 0.01;

				DrawLine( uv, points[0], points[3], float3(0.0, 0.0, 1.0), w );
				DrawLine( uv, points[1], points[2], float3(0.0, 0.6, 0.0), w );
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	test 2D frustum culling
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define CULL_SPHERE
#	define CULL_CONE
#	define CULL_CONE_V2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt>		mode	= DynamicUInt();
		RC<DynamicUInt>		anim	= DynamicUInt();
		RC<DynamicFloat2>	dyn_p0	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p1	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p2	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_p3	= DynamicFloat2();
		RC<DynamicFloat2>	dyn_pos	= DynamicFloat2();

		RC<DynamicFloat>	cone_a		= DynamicFloat();
		RC<DynamicFloat>	cone_h		= DynamicFloat();
		RC<DynamicFloat>	cone_dir	= DynamicFloat();

		Slider( mode,		"Mode",			0,	1,	0 );
		Slider( anim,		"Animation",	0,	1,	1 );

		Slider( dyn_p0,		"P0",			float2(0.0),	float2(1.0),	float2(0.5) );	// left bottom
		Slider( dyn_p1,		"P1",			float2(0.0),	float2(1.0),	float2(0.5) );	// right bottom
		Slider( dyn_p2,		"P2",			float2(0.0),	float2(1.0),	float2(0.5) );	// left top
		Slider( dyn_p3,		"P3",			float2(0.0),	float2(1.0),	float2(0.5) );	// right top

		Slider( dyn_pos,	"Pos",			float2(-1.5),	float2(1.5),	float2(0.77, -0.92) );
		Slider( cone_a,		"Angle",		0.01,			1.0,			0.3 );
		Slider( cone_h,		"Height",		0.1,			1.0,			0.4 );
		Slider( cone_dir,	"Dir",			-1.0,			1.0,			0.0 );


		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "CULL_CONE" );
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"iAnimation",	anim );
			pass.Constant(	"iP0",			dyn_p0 );
			pass.Constant(	"iP1",			dyn_p1 );
			pass.Constant(	"iP2",			dyn_p2 );
			pass.Constant(	"iP3",			dyn_p3 );
			pass.Constant(	"iPos",			dyn_pos );
			pass.Constant(	"iAngle",		cone_a );
			pass.Constant(	"iHeight",		cone_h );
			pass.Constant(	"iDir",			cone_dir );
			pass.Slider(	"iVisTest",		0,		2 );
			pass.EnableIfEqual( mode, 0 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "CULL_CONE_V2" );
			pass.Output(	"out_Color",	rt );
			pass.Constant(	"iPos",			dyn_pos );
			pass.Constant(	"iAngle",		cone_a );
			pass.Constant(	"iHeight",		cone_h );
			pass.Constant(	"iDir",			cone_dir );
			pass.Slider(	"iVisTest",		0,		3 );
			pass.Slider(	"iDensity",		0.01,	2.0,	1.0 );
			pass.EnableIfEqual( mode, 1 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Hash.glsl"
	#include "Cone.glsl"
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
			out_Color.rgb = Lerp( out_Color.rgb, color, f );
			out_Color.a += f;
		}
	}

	void  DrawCone (const float2 uv, const Cone cone, const Quat q, const float3 color)
	{
		const float2	sincos_a	= SinCos( cone.halfAngle );
		const float3	view_dir	= float3( 0.0, 0.0, 1.0 );
		const float		min_d		= 0.001;
		float3			pos			= float3( uv, -3.0 );

		for (int i = 0; i < 128; ++i)
		{
			float3	p = SDF_Move( pos, cone.origin );
					p = SDF_Rotate( p, q );
			float	d = SDF_Cone( p, sincos_a, cone.height );
		//	float	d = SDF_SolidAngle( p, sincos_a, cone.height );

			pos += d * view_dir;

			if ( Abs(d) < min_d )
			{
				if ( out_Color.a < 1.0 )
				{
					out_Color.rgb = color;
					out_Color.a += 1.0;
				}
				break;
			}
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_CONE

	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		Quat	q	= QRotationX( float_Pi );
		q = QMul( q, QRotationZ( float_Pi2 * iDir ));
		q = QNormalize( q );

		const Cone	cone = Cone_Create( float3(iPos, 0.0), -QMul(q, float3(0.0, 1.0, 0.0)), iAngle * float_Pi, iHeight );
		bool		is_visible = false;

		// draw frustum
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

			const Line2d	plane_to_line[] = {
				Line_Create( p0, p2 ),	// left
				Line_Create( p1, p3 ),	// right
				Line_Create( p2, p3 ),	// top
				Line_Create( p0, p1 )	// bottom
			};
			const float3	plane_to_color[] = {
				float3(0.0, 1.0, 0.0),	// left
				float3(0.2, 0.8, 0.8),	// right
				float3(0.8, 0.6, 0.0),	// top
				float3(1.0, 0.0, 0.0)	// bottom
			};

			const float		w0h		= 0.018;
			const float		w0		= 0.013;
			const float		w1		= 0.01;

			Frustum2d	fr			= Frustum2d_FromCornerPoints( p0, p1, p2, p3 );
			uint		plane_bits	= 0;

			const float3	base_center	= Cone_BaseCenter( cone );
			const float		radius		= Cone_BaseRadius( cone );
			const Sphere	sp			= Cone_ToBoundingSphere( cone );
			const float		sin_ha		= Sin( cone.halfAngle );
			const float		cos_ha		= Sin( cone.halfAngle );

			switch ( iVisTest )
			{
				case 0 :
					for (int i = 0; i < 4; ++i)
					{
						float4	plane		= float4( fr.planes[i].xy, 0.0, fr.planes[i].z );
						float	dist_apex	= Dot( plane.xyz, cone.origin ) + plane.w;
						float	dist_base	= Dot( plane.xyz, base_center ) + plane.w + radius;

						if ( dist_apex < 0.0 and dist_base < 0.0 )
						{
							if ( dist_apex > dist_base )
							{
								float2	pt = Line_ProjectPoint( plane_to_line[i], cone.origin.xy );
								DrawLine( uv, pt, cone.origin.xy, plane_to_color[i], w1 );
							}
							else
							{
								float2	pt = Line_ProjectPoint( plane_to_line[i], base_center.xy );
								DrawLine( uv, pt, base_center.xy, plane_to_color[i], w1 );
							}
							plane_bits |= 1 << i;
						}
					}
					break;

				case 1 :
					for (int i = 0; i < 4; ++i)
					{
						float4	plane	= float4( fr.planes[i].xy, 0.0, fr.planes[i].z );
						float	d		= Dot( plane.xyz, sp.center ) + plane.w + sp.radius;

						if ( d < 0.0 )
						{
							float2	pt = Line_ProjectPoint( plane_to_line[i], sp.center.xy );
							DrawLine( uv, pt, sp.center.xy, plane_to_color[i], w1 );

							plane_bits |= 1 << i;
						}
					}
					break;

				case 2 :
					for (int i = 0; i < 4; ++i)
					{
						const float4	plane = float4( -fr.planes[i].xy, 0.0, fr.planes[i].z );
						const float3	dir	= cone.dir;
						const float3	v1	= Cross( plane.xyz,	dir );
						const float3	v2	= Cross( v1,		dir );

						const float3	pt = cone.origin +
											 cone.height * cos_ha * dir +
											 cone.height * sin_ha * v2;

						DrawPoint( uv, pt.xy, w0, plane_to_color[i] );

						{
							float2	proj = Line_ProjectPoint( plane_to_line[i], pt.xy );
							DrawLine( uv, proj, pt.xy, plane_to_color[i], w1 );
						}{
							float2	proj = Line_ProjectPoint( plane_to_line[i], cone.origin.xy );
							DrawLine( uv, proj, cone.origin.xy, plane_to_color[i], w1 );
						}

						if ( Dot( float4(pt, 1.0), plane ) < 0.0		and
							 Dot( float4(cone.origin, 1.0), plane ) < 0.0 )
						{
							plane_bits |= 1 << i;
						}
					}
					break;
			}
			is_visible = (plane_bits == 0);

			// planes
			DrawLine( uv, p0, p2, plane_to_color[0], HasBit( plane_bits, 0 ) ? w0h : w0 );	// green	- left
			DrawLine( uv, p1, p3, plane_to_color[1], HasBit( plane_bits, 1 ) ? w0h : w0 );	// blue		- right
			DrawLine( uv, p2, p3, plane_to_color[2], HasBit( plane_bits, 2 ) ? w0h : w0 );	// orange	- top
			DrawLine( uv, p0, p1, plane_to_color[3], HasBit( plane_bits, 3 ) ? w0h : w0 );	// red		- bottom
		}

		DrawCone( uv, cone, q, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CULL_CONE_V2
	#include "Geometry3D.glsl"
	#include "Intersectors.glsl"

	bool  Frustum2d_IsConeVisible_v2 (const Frustum2d fr, const Cone c)
	{
		const float3	base_center	= Cone_BaseCenter( c );
		const float		radius		= Cone_BaseRadius( c );

		[[unroll]] for (int i = 0; i < 4; ++i)
		{
			const float4	plane		= float4( fr.planes[i].xy, 0.0, fr.planes[i].z );
			const float		dist_apex	= Plane_Distance( plane, c.origin );
			const float		dist_base	= Plane_Distance( plane, base_center ) + radius;

			if ( dist_apex < 0.0 and dist_base < 0.0 )
				return false;
		}
		return true;
	}

	bool  Frustum2d_IsConeVisible_v3 (const Frustum2d fr, const Cone c)
	{
		const float	sin_ha	= Sin( c.halfAngle );
		const float	cos_ha	= Cos( c.halfAngle );
		int			invis	= 0;

		[[unroll]] for (int i = 0; i < 4; ++i)
		{
			const float4	plane = float4( fr.planes[i].xy, 0.0, fr.planes[i].z );
			const float3	v1 = Cross( plane.xyz, c.dir );
			const float3	v2 = Cross( v1, c.dir );

			const float3	pt = c.origin +
								 c.height * cos_ha * c.dir +
								 c.height * sin_ha * v2;

			if ( Dot( float4(pt,       1.0), plane ) < 0.0 or
				 Dot( float4(c.origin, 1.0), plane ) < 0.0 )
				++invis;
		}
		return invis == 0;
	}


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


	void  TestFrustumRect (const float2 uv, const Cone cone, int mode)
	{
		float		size	= 0.2 * iDensity;
		float2		center	= RoundToBase( uv, size );
		float2		hsize	= DHash22( center ) * size * 0.5 + 0.015;
		Frustum2d	fr		= Frustum2d_FromCornerPoints( center + float2( -hsize.x,  hsize.y ),
														  center + float2(  hsize.x,  hsize.y ),
														  center + float2( -hsize.x, -hsize.y ),
														  center + float2(  hsize.x, -hsize.y ) );
		bool		is_visible = false;

		if ( mode == 0 )
			is_visible = Frustum2d_IsConeVisible_v2( fr, cone );
		else
			is_visible = Frustum2d_IsConeVisible_v3( fr, cone );

		DrawRect( uv, center, hsize, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
	}


	void  TestSphere (const float2 uv, const Cone cone, const Quat q, int mode)
	{
		float	size	= 0.2 * iDensity;
		float2	center	= RoundToBase( uv, size );
		float	radius	= DHash12( center ) * size * 0.5 + 0.015;
		bool	is_visible = false;

		if ( mode == 0 )
		{
			float	dist = SDF_Cone( SDF_Rotate( float3(center, 0.0) - cone.origin, q ), SinCos( cone.halfAngle ), cone.height );
			is_visible = dist < radius;
		}
		else
		{
			is_visible = Cone_Sphere_Intersects( cone, Sphere_Create( float3(center, 0.0), radius ));
		}

		DrawPoint( uv, center, radius, (is_visible ? float3(1.0, 0.2, 1.0) : float3(0.0, 0.0, 0.8)) );
	}


	void  Main ()
	{
		const float		uv_scale	= 2.0;
		const float2	uv			= GetGlobalCoordSNormCorrected() * uv_scale;

		out_Color = float4(0.25);

		Quat	q	= QRotationX( float_Pi );
		q = QMul( q, QRotationZ( float_Pi2 * iDir ));
		q = QNormalize( q );

		const Cone	cone = Cone_Create( float3(iPos, 0.0), -QMul(q, float3(0.0, 1.0, 0.0)), iAngle * float_Pi, iHeight );

		DrawCone( uv, cone, q, float3(0.0, 0.6, 0.0) );

		switch ( iVisTest )
		{
			case 0 :	TestFrustumRect( uv, cone, 0 );	break;
			case 1 :	TestFrustumRect( uv, cone, 1 );	break;
			case 2 :	TestSphere( uv, cone, q, 0 );	break;
			case 3 :	TestSphere( uv, cone, q, 1 );	break;
		}
	}

#endif
//-----------------------------------------------------------------------------

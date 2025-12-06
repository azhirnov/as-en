// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	for screenshot test
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Buffer>		buf		= Buffer();

		// setup projection
		{
			const float2	z_range = float2(1.0, 100.0);

			float4x4	persp		= float4x4().Perspective( ToRad(60.0), 1.5, z_range );
			float4x4	inf_persp	= float4x4().InfinitePerspective( ToRad(60.0), 1.5, z_range.x );
			float4x4	frust		= float4x4().Frustum( RectF(-1.0, -1.0, 1.0, 1.0), z_range );
			float4x4	inf_frust	= float4x4().InfiniteFrustum( RectF(-1.0, -1.0, 1.0, 1.0), z_range.x );
			float4x4	rev_z		= float4x4().ReverseZTransform();

			buf.Float( "zRange",					z_range );

			buf.Float( "perspective",				persp );
			buf.Float( "infinitePerspective",		inf_persp );
			buf.Float( "frustum",					frust );
			buf.Float( "infiniteFrustum",			inf_frust );

			buf.Float( "revZ_perspective",			rev_z * persp );
			buf.Float( "revZ_infinitePerspective",	rev_z * inf_persp );
			buf.Float( "revZ_frustum",				rev_z * frust );
			buf.Float( "revZ_infiniteFrustum",		rev_z * inf_frust );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_CBuf",		buf );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef __INTELLISENSE__
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "Matrix.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const float		x		= uv.x;
		const float		y_max	= 32.0;
		const int		i		= int(uv.y * y_max);
		const float		z		= Lerp( un_CBuf.zRange.x, un_CBuf.zRange.y, x );
		const float4	vp		= float4( 0.0, 0.0, un_PerPass.resolution.xy );
		const float4	pos		= float4( 0.0, 0.0, z, 1.0 );

		float			y		= float_nan;
		float			diff	= -1.0;
		const float		err1	= 1.0e+7;

		switch ( i )
		{
			case 0 :	y = ProjectToScreenSpace( un_CBuf.perspective, pos, vp ).z;	break;
			case 1 :	y = ProjectToNormClipSpace( un_CBuf.perspective, pos ).z;	break;
			case 2 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.perspective, pos ).z;
				y = FastProjectZ( un_CBuf.perspective, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 3 :	y = ProjectToScreenSpace( un_CBuf.infinitePerspective, pos, vp ).z;	break;
			case 4 :	y = ProjectToNormClipSpace( un_CBuf.infinitePerspective, pos ).z;	break;
			case 5 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.infinitePerspective, pos ).z;
				y = FastProjectZ( un_CBuf.infinitePerspective, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 6 :	y = ProjectToScreenSpace( un_CBuf.frustum, pos, vp ).z;	break;
			case 7 :	y = ProjectToNormClipSpace( un_CBuf.frustum, pos ).z;	break;
			case 8 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.frustum, pos ).z;
				y = FastProjectZ( un_CBuf.frustum, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 9 :	y = ProjectToScreenSpace( un_CBuf.infiniteFrustum, pos, vp ).z;	break;
			case 10 :	y = ProjectToNormClipSpace( un_CBuf.infiniteFrustum, pos ).z;	break;
			case 11 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.infiniteFrustum, pos ).z;
				y = FastProjectZ( un_CBuf.infiniteFrustum, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 12 :	y = ProjectToScreenSpace( un_CBuf.revZ_perspective, pos, vp ).z;	break;
			case 13 :	y = ProjectToNormClipSpace( un_CBuf.revZ_perspective, pos ).z;		break;
			case 14 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.revZ_perspective, pos ).z;
				y = FastProjectZ( un_CBuf.revZ_perspective, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 15 :	y = ProjectToScreenSpace( un_CBuf.revZ_infinitePerspective, pos, vp ).z;	break;
			case 16 :	y = ProjectToNormClipSpace( un_CBuf.revZ_infinitePerspective, pos ).z;		break;
			case 17 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.revZ_infinitePerspective, pos ).z;
				y = FastProjectZ( un_CBuf.revZ_infinitePerspective, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 18 :	y = ProjectToScreenSpace( un_CBuf.revZ_frustum, pos, vp ).z;	break;
			case 19 :	y = ProjectToNormClipSpace( un_CBuf.revZ_frustum, pos ).z;		break;
			case 20 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.revZ_frustum, pos ).z;
				y = FastProjectZ( un_CBuf.revZ_frustum, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 21 :	y = ProjectToScreenSpace( un_CBuf.revZ_infiniteFrustum, pos, vp ).z;	break;
			case 22 :	y = ProjectToNormClipSpace( un_CBuf.revZ_infiniteFrustum, pos ).z;		break;
			case 23 :
			{
				float	ref = ProjectToNormClipSpace( un_CBuf.revZ_infiniteFrustum, pos ).z;
				y = FastProjectZ( un_CBuf.revZ_infiniteFrustum, z );
				diff = Abs( y - ref ) * err1;
				break;
			}

			case 24 :
			{
				float	p	= FastProjectZ( un_CBuf.perspective, z );
				float	ref = FastUnProjectZ( un_CBuf.perspective, p );
				ref = (ref - un_CBuf.zRange.x) / (un_CBuf.zRange.y - un_CBuf.zRange.x);
				y = ToLinearDepth( p, un_CBuf.zRange );
				diff = Abs( y - ref ) * 1.0e+8;
				break;
			}

			case 25 :
			{
				float ref = (z - un_CBuf.zRange.x) / (un_CBuf.zRange.y - un_CBuf.zRange.x);	// linear [0, 1]
				y = FastProjectZ( un_CBuf.perspective, z );									// non-linear [0, 1]
				y = ToLinearDepth( y, un_CBuf.zRange );										// linear [0, 1]
				diff = Abs( y - ref ) * 1.0e+6;
				break;
			}

			case 26 :
			{
				float ref = uv.x;
				y = ToLinearDepth( ref, un_CBuf.zRange );
				y = ToNonlinearDepth( y, un_CBuf.zRange );
				diff = Abs( y - ref ) * 1.0e+7;
				break;
			}

			case 27 :
			{
				float	ref = FastProjectZ( un_CBuf.infinitePerspective, z );
				y = FastProjectZInf( un_CBuf.zRange.x, z );
				diff = Abs( y - ref ) * 1.0e+7;
				break;
			}

			case 28 :
			{
				float	ref = FastProjectZ( un_CBuf.revZ_infinitePerspective, z );
				y = FastProjectRevZInf( un_CBuf.zRange.x, z );
				diff = Abs( y - ref ) * 1.0e+8;
				break;
			}

			case 29 :
			{
				y = FastProjectZ( un_CBuf.perspective, z );
				y = FastUnProjectZ( un_CBuf.perspective, y );
				diff = Abs( y - z ) / z * 1.0e+6;
				break;
			}

			case 30 :
			{
				y = FastProjectZInf( un_CBuf.zRange.x, z );
				y = FastUnProjectZInf( un_CBuf.zRange.x, y );
				diff = Abs( y - z ) / z * 1.0e+30;		// best accuracy
				break;
			}

			case 31 :
			{
				y = FastProjectRevZInf( un_CBuf.zRange.x, z );
				y = FastUnProjectRevZInf( un_CBuf.zRange.x, y );
				diff = Abs( y - z ) / z * 1.0e+30;		// best accuracy
				break;
			}
		}

		if ( i != int(GetGlobalCoordUNorm(int3(1)).y * y_max) )
		{
			out_Color = float4(0.0);
			return;
		}

		if ( diff < 0.0 )
		{
			// use 'y'
			if ( IsNaN( y ) or IsInfinity( y ))
				out_Color = float4(0.2);
			else{
				out_Color = Rainbow( y );
				//out_Color.a = y;	// for debugging
			}
		}else{
			// use 'diff'
			out_Color = float4(diff);
			if ( diff > 1.0 )
				out_Color = float4(diff - 1.0, 0.0, 0.0, 1.0);
		}
	}

#endif
//-----------------------------------------------------------------------------

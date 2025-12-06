// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define ADD_SDF
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<DynamicUInt2>	tex_pot		= DynamicUInt2();
		RC<Image>			tex			= Image( EPixelFormat::R32F, tex_pot.Exp2().Dimension() );
		RC<Buffer>			cbuf		= Buffer();
		const uint			max_obj		= 4;
		const uint			max_shape	= 7;
		const uint			max_op		= 4;

		cbuf.ArrayLayout(
			"CBuf",
			// array element struct
			"	uint		shape;" +
			"	uint		op;" +
			"	uint		color;" +
			"	float2		opParams;" +
			"	float2		center;" +
			"	float4		params;",
			max_obj
		);

	#if 0	// Default
		const array<float>	params = {
		// pass: SDFBuilder|ADD_SDF=0
				1, 											// iShape
				0, 											// iOp
				0.0000f, 0.0000f, 							// iCenter
				-1.0000f, 0.0800f, 0.0000f, 0.0000f, 		// iParams
				0.0000f, 0.0000f, 							// iOpParams
				0.0882f, 1.0000f, 0.0000f, 1.0000f, 		// iColor

		// pass: SDFBuilder|ADD_SDF=1
				0, 											// iShape
				0, 											// iOp
				0.0000f, 0.0000f, 							// iCenter
				0.0000f, 0.0000f, 0.0000f, 0.0000f, 		// iParams
				0.0000f, 0.0000f, 							// iOpParams
				1.0000f, 0.0000f, 0.0000f, 1.0000f, 		// iColor

		// pass: SDFBuilder|ADD_SDF=2
				0, 											// iShape
				0, 											// iOp
				0.0000f, 0.0000f, 							// iCenter
				0.0000f, 0.0000f, 0.0000f, 0.0000f, 		// iParams
				0.0000f, 0.0000f, 							// iOpParams
				1.0000f, 0.0000f, 0.0000f, 1.0000f, 		// iColor

		// pass: SDFBuilder|ADD_SDF=3
				0, 											// iShape
				0, 											// iOp
				0.0000f, 0.0000f, 							// iCenter
				0.0000f, 0.0000f, 0.0000f, 0.0000f, 		// iParams
				0.0000f, 0.0000f, 							// iOpParams
				1.0000f, 0.0000f, 0.0000f, 1.0000f, 		// iColor
		};
	#endif

	#if 1	// Crater
		const array<float>	params = {
		// pass: SDFBuilder|ADD_SDF=0
				1, 											// iShape
				0, 											// iOp
				0.0000f, 0.0000f, 							// iCenter
				-1.0000f, 0.0800f, 0.0000f, 0.0000f, 		// iParams
				0.0000f, 0.0000f, 							// iOpParams
				0.0882f, 1.0000f, 0.0000f, 1.0000f, 		// iColor

		// pass: SDFBuilder|ADD_SDF=1
				2, 										// iShape
				1, 										// iOp
				0.0000f, 0.3750f, 						// iCenter
				0.7320f, 0.0000f, 0.0000f, 0.0000f, 	// iParams
				0.2500f, 0.0180f, 						// iOpParams
				1.0000f, 0.0000f, 0.0000f, 1.0000f, 	// iColor

		// pass: SDFBuilder|ADD_SDF=2
				2, 										// iShape
				3, 										// iOp
				0.0000f, -0.1250f, 						// iCenter
				0.5010f, 0.0000f, 0.0000f, 0.0000f, 	// iParams
				0.0890f, 0.0000f, 						// iOpParams
				0.0000f, 0.0000f, 1.0000f, 1.0000f, 	// iColor

		// pass: SDFBuilder|ADD_SDF=3
				1, 											// iShape
				1, 											// iOp
				0.0000f, 0.2140f, 							// iCenter
				-1.0000f, 0.0000f, 0.0000f, 0.0000f, 		// iParams
				0.0450f, 0.0000f, 							// iOpParams
				1.0000f, 0.0000f, 1.0000f, 1.0000f, 		// iColor
		};
	#endif

		uint	i = 0;

		for (uint j = 0; j < max_obj; ++j)
		{
			RC<ComputePass>		pass = ComputePass( "", "ADD_SDF="+j );
			pass.ArgInOut( "un_Buffer",		cbuf );
			pass.Slider( "iShape",		0,	max_shape,	int(params[i]) );															++i;
			pass.Slider( "iOp",			0,	max_op,		int(params[i]) );															++i;
			pass.Slider( "iCenter",		float2(-1.0),	float2(1.0),	float2(params[i], params[i+1]) );							i += 2;
			pass.Slider( "iParams",		float4(-1.0),	float4(1.0),	float4(params[i], params[i+1], params[i+2], params[i+3]) );	i += 4;
			pass.Slider( "iOpParams",	float2(-1.0),	float2(1.0),	float2(params[i], params[i+1]) );							i += 2;
			pass.ColorSelector( "iColor",	RGBA32f( params[i], params[i+1], params[i+2], params[i+3] ));							i += 4;
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}

		RC<DynamicFloat2>	xrange	= DynamicFloat2();
		RC<DynamicFloat2>	yrange	= DynamicFloat2();
		Slider( xrange,		"XRange",	float2(-1.f),	float2(1.f),	float2(-0.5f, 0.5f) );
		Slider( yrange,		"YRange",	float2(-1.f),	float2(1.f),	float2(-0.1f, 0.1f) );
		Slider( tex_pot,	"TexDim",	uint2(3),		uint2(10),		uint2(6,4) );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "MAX_SDF="+max_obj+";MAX_SHAPE="+max_shape+";MAX_OP="+max_op );
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_Buffer",	cbuf );
			pass.Slider( "iGrid",		0,		2 );
			pass.Slider( "iView",		0,		3 );
			pass.Slider( "iViewMode",	0,		1 );
			pass.Slider( "iDScale",		1.f,	10.f,	4.f );
			pass.Constant( "iX",		xrange );
			pass.Constant( "iY",		yrange );
		}{
			RC<Postprocess>		pass = Postprocess( "", "EXPORT;MAX_SDF="+max_obj+";MAX_SHAPE="+max_shape+";MAX_OP="+max_op );
			pass.Output( "out_Color",	tex );
			pass.ArgIn( "un_Buffer",	cbuf );
			pass.Constant( "iX",		xrange );
			pass.Constant( "iY",		yrange );
		}

		Present( rt );
		DbgView( tex, DbgViewFlags::NoCopy );
		Export( tex, "sdf2d-.aeimg" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	float	g_DHistory [MAX_SDF];
	float	g_MDHistory [MAX_SDF];


	float  SDF (float2 uv, const float md, const uint idx)
	{
		float2	c = SDF_Move( uv, un_Buffer.elements[idx].center );
		float4	p = un_Buffer.elements[idx].params;
		float2	o = un_Buffer.elements[idx].opParams;
		float	d = float_max;

		#if MAX_SHAPE != 7
		#	error 'max_shape' must be 7
		#endif
		switch ( un_Buffer.elements[idx].shape )
		{
			case 0 :	g_DHistory[idx] = float_max;  return md;
			case 1 :	d = c.y * Sign(p.x) + p.y;  break;
			case 2 :	d = SDF2_Circle( c, p.x );  break;
			case 3 :	d = SDF2_Rect( c, p.xy );  break;
			case 4 :	d = SDF2_RoundedRect( c, p.xy, p.zzzz );  break;
			case 5 :	d = SDF2_Pentagon( c, p.x );  break;
			case 6 :	d = SDF2_Hexagon( c, p.x );  break;
			case 7 :	d = SDF2_Octagon( c, p.x );  break;
		}

		d = SDF_OpRoundedShape( d, o.y );
		g_DHistory[idx] = d;

		if ( md >= float_max )
			return d;

		#if MAX_OP != 4
		#	error 'max_op' must be 4
		#endif
		switch ( un_Buffer.elements[idx].op )
		{
			case 0 :	return SDF_OpUnite( d, md );
			case 1 :	return SDF_OpUnite( d, md, o.x );
			case 2 :	return SDF_OpSub( d, md );
			case 3 :	return SDF_OpSub( d, md, o.x );
			case 4 :	return SDF_OpIntersect( d, md );
		}
	}

#	ifdef EXPORT

		void  Main ()
		{
			float	md	= float_max;
			float2	uv	= GetGlobalCoordUNorm().xy;

			uv.x = Lerp( iX.x, iX.y, uv.x );
			uv.y = Lerp( iY.x, iY.y, uv.y );

			for (uint i = 0; i < MAX_SDF; ++i) {
				md = SDF( uv, md, i );
			}

			out_Color.r = md;
		}

#	else
		void  ViewMode (float3 col, float d)
		{
			if ( iViewMode == 0 )
				out_Color.rgb = TriangleWave( d * 10.0 * iDScale ) * col;
			else
				out_Color.rgb = Abs( d * iDScale ) * col;

			float	dd = gl.fwidth( d );
			float	f  = SmoothStep( Abs(d), 0.0, dd*1.5 );
			out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, f );
		}


		void  Main ()
		{
			float2	uv	= GetGlobalCoordSNormCorrected();
			float	md	= float_max;

			for (uint i = 0; i < MAX_SDF; ++i)
			{
				md = SDF( uv, md, i );
				g_MDHistory[i] = md;
			}

			if ( iView == 0 )
				ViewMode( (md < 0.0 ? float3(0.2, 0.5, 1.0) : float3(1.0, 0.0, 0.0)), md );

			if ( iView == 1 )
			{
				uint2	id		= uint2(~0);
				float2	dist	= float2(float_max);

				for (uint i = 0; i < MAX_SDF; ++i)
				{
					float	d = g_DHistory[i];

					if ( d < dist.x )
					{
						id.y = id.x;
						id.x = i;

						dist.y = dist.x;
						dist.x = d;
					}
					else
					if ( d < dist.y )
					{
						id.y	= i;
						dist.y	= d;
					}
				}

				float4	c0	= float4(1.0);
				float4	c1;

				if ( id.x < MAX_SDF )	c0 = unpackUnorm4x8( un_Buffer.elements[ id.x ].color );
				if ( id.y < MAX_SDF )	c1 = unpackUnorm4x8( un_Buffer.elements[ id.y ].color );  else  c1 = c0;

				if ( dist.x > 0.0 )
				{
					c0 = (md > 0.0 ? float4(0.2) : LerpSRGB( c1, c0, 0.5 ));
					dist.x = md;
				}

				ViewMode( c0.rgb, dist.x );
			}

			if ( iView >= 2 )
			{
				float2	max_dd	= float2(0.0);
				uint2	id		= uint2(0);

				for (uint i = 1; i < MAX_SDF; ++i)
				{
					float	d = Abs( g_MDHistory[i-1] - g_MDHistory[i] );

					if ( d > max_dd.x )
					{
						max_dd.y = max_dd.x;
						max_dd.x = d;

						id.y = id.x;
						id.x = i;
					}
					else
					if ( d > max_dd.y )
					{
						max_dd.y = d;
						id.y     = i;
					}
				}

				if ( iView == 3 ) id.x = id.y;

				float4	col = unpackUnorm4x8( un_Buffer.elements[id.x].color );
				if ( md > 0.0 )	col *= 0.25;
				ViewMode( col.rgb, md );
			}

			if ( iGrid >= 1 )
			{
				out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, AA_QuadGrid_dxdy( uv * 4.0, float2(1.0, 2.5) ).x );

				float2	uv0		= float2( iX.x, iY.x );
				float2	uv1		= float2( iX.y, iY.y );
				float2	center	= (uv0 + uv1) * 0.5;
				float2	hsize	= ( uv1 - uv0 ) * 0.5;
				float	d		= SDF2_Rect( SDF_Move( uv, center ), hsize );
						d		= SmoothStep( d, 0.0, 0.01 );

				if ( iGrid == 2 )
					out_Color.rgb = Lerp( out_Color.rgb, 1.0 - out_Color.rgb, d );
			}

			out_Color.a = 1.0;
		}

#	endif
#endif
//-----------------------------------------------------------------------------
#ifdef ADD_SDF

	void  Main ()
	{
		un_Buffer.elements[ADD_SDF].shape	= iShape;
		un_Buffer.elements[ADD_SDF].op		= iOp;
		un_Buffer.elements[ADD_SDF].color	= packUnorm4x8( iColor );
		un_Buffer.elements[ADD_SDF].opParams= iOpParams;
		un_Buffer.elements[ADD_SDF].center	= iCenter;
		un_Buffer.elements[ADD_SDF].params	= iParams;
	}

#endif
//-----------------------------------------------------------------------------

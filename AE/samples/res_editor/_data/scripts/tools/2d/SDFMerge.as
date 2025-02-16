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
		RC<Buffer>		cbuf	= Buffer();

	#if 1	// Crater
		{
			const array<float>	params = {
			// pass: SDFBuilder|ADD_SDF=0
					1, 											// iShape
					0, 											// iOp
					0.0000f, 0.0000f, 							// iCenter
					-1.0000f, 0.0800f, 0.0000f, 0.0000f, 		// iParams
					0.0000f, 0.0000f, 							// iOpParams

			// pass: SDFBuilder|ADD_SDF=1
					2, 										// iShape
					1, 										// iOp
					0.0000f, 0.3750f, 						// iCenter
					0.7320f, 0.0000f, 0.0000f, 0.0000f, 	// iParams
					0.2500f, 0.0180f, 						// iOpParams

			// pass: SDFBuilder|ADD_SDF=2
					2, 										// iShape
					3, 										// iOp
					0.0000f, -0.1250f, 						// iCenter
					0.5010f, 0.0000f, 0.0000f, 0.0000f, 	// iParams
					0.0890f, 0.0000f, 						// iOpParams

			// pass: SDFBuilder|ADD_SDF=3
					1, 											// iShape
					1, 											// iOp
					0.0000f, 0.2140f, 							// iCenter
					-1.0000f, 0.0000f, 0.0000f, 0.0000f, 		// iParams
					0.0450f, 0.0000f 							// iOpParams
			};
			cbuf.FloatArray( "spline1",		params );
			cbuf.FloatArray( "spline2",		params );
		}
	#endif


		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_Buffer",	cbuf );
			pass.Slider( "iGrid",		0,				1 );
			pass.Slider( "iViewMode",	0,				1 );
			pass.Slider( "iOp",			0,				4 );
			pass.Slider( "iPos1",		float2(-1.f),	float2(1.f),	float2(-0.5f, 0.f) );
			pass.Slider( "iPos2",		float2(-1.f),	float2(1.f),	float2(0.5f, 0.f) );
			pass.Slider( "iScale",		float2(0.1f),	float2(2.f),	float2(0.5f) );
			pass.Slider( "iDScale",		1.f,			10.f,			4.f );
			pass.Slider( "iOpParam",	0.f,			1.f,			0.1f );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	#define SPLINE_PARAMS	10
	#define MAX_OBJ			4


	float  MargeSDF (float d1, float d2, const uint opId, float param)
	{
		switch ( opId )
		{
			case 0 :	return SDF_OpUnite( d1, d2 );
			case 1 :	return SDF_OpUnite( d1, d2, param );
			case 2 :	return SDF_OpSub( d1, d2 );
			case 3 :	return SDF_OpSub( d1, d2, param );
			case 4 :	return SDF_OpIntersect( d1, d2 );
		}
	}


	// same as in 'SDFBuilder.as'
	float  SDF (float2 uv, const float md, const uint shapeId, const uint opId, const float2 center, const float4 p, const float2 o)
	{
		float2	c = SDF_Move( uv, center );
		float	d = float_max;

		switch ( shapeId )
		{
			case 0 :	return md;
			case 1 :	d = c.y * Sign(p.x) + p.y;  break;
			case 2 :	d = SDF2_Circle( c, p.x );  break;
			case 3 :	d = SDF2_Rect( c, p.xy );  break;
			case 4 :	d = SDF2_RoundedRect( c, p.xy, p.zzzz );  break;
			case 5 :	d = SDF2_Pentagon( c, p.x );  break;
			case 6 :	d = SDF2_Hexagon( c, p.x );  break;
			case 7 :	d = SDF2_Octagon( c, p.x );  break;
		}

		d = SDF_OpRoundedShape( d, o.y );

		if ( md >= float_max )
			return d;

		return MargeSDF( d, md, opId, o.x );
	}


	float  SDFGroup (float md, float2 uv, float spline[SPLINE_PARAMS*MAX_OBJ])
	{
		for (uint i = 0; i < MAX_OBJ; ++i)
		{
			const uint	j = i * SPLINE_PARAMS;

			md = SDF( uv, md,
					  uint(spline[j]),												// shapeId
					  uint(spline[j+1]),											// opId
					  float2(spline[j+2], spline[j+3]),								// center
					  float4(spline[j+4], spline[j+5], spline[j+6], spline[j+7]),	// params
					  float2(spline[j+8], spline[j+9])								// opParams
					);
		}
		return md;
	}


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

		#if 0
		{
			md = SDFGroup( md, SDF_Move( uv, iPos1 ) / iScale.x, un_Buffer.spline1 ) * iScale.x;
			md = SDFGroup( md, SDF_Move( uv, iPos2 ) / iScale.y, un_Buffer.spline2 ) * iScale.y;
		}
		#else
		{
			float	d1 = SDFGroup( float_max, SDF_Move( uv, iPos1 ) / iScale.x, un_Buffer.spline1 ) * iScale.x;
			float	d2 = SDFGroup( float_max, SDF_Move( uv, iPos2 ) / iScale.y, un_Buffer.spline2 ) * iScale.y;
			md = MargeSDF( d1, d2, iOp, iOpParam );
		}
		#endif

		ViewMode( (md < 0.0 ? float3(0.2, 0.5, 1.0) : float3(1.0, 0.0, 0.0)), md );

		if ( iGrid == 1 )
			out_Color.rgb = Lerp( float3(1.0), out_Color.rgb, AA_QuadGrid_dxdy( uv * 4.0, float2(1.0, 2.5) ).x );

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------

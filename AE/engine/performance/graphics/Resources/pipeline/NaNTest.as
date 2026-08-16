// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
#	include <pipeline_compiler.as>
#	define SH_FRAG
#	include <aestyle.glsl.h>
#	define TYPE		0
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  CreatePipeline (string dataType, string feature = "")
	{
		string	suffix	= dataType;
		string	rtech	= "NaN.RT";
		string	name	= "NaN.";

		RC<GraphicsPipeline>	ppln = GraphicsPipeline( name+suffix+"."+feature );

		if ( feature.length() > 0 )
		{
			ppln.AddFeatureSet( feature );
			rtech += "."+feature;
		}

		{
			RC<Shader>	vs = Shader();
			vs.LoadSelf();
			ppln.SetVertexShader( vs );
		}{
			RC<Shader>	fs = Shader();
			fs.LoadSelf();
			fs.Define( "TYPE=TYPE_"+dataType );
			ppln.SetFragmentShader( fs );
		}

		// specialization
		{
			RC<GraphicsPipelineSpec>	spec = ppln.AddSpecialization( name+suffix );
			spec.AddToRenderTech( rtech, "Graphics" );

			RenderState	rs;
			rs.inputAssembly.topology = EPrimitive::TriangleList;

			spec.SetRenderState( rs );
		}
	}

	void  ASmain ()
	{
		CreatePipeline( "FP32" );
		CreatePipeline( "FP32_MED" );
		CreatePipeline( "FP32_LOW" );
		CreatePipeline( "FP16",		"ShaderFloatInt16" );
		CreatePipeline( "FP64",		"ShaderFloatInt64" );
		CreatePipeline( "FP16",		"ShaderFloat16" );

		if ( not IsMetal() ) {
			CreatePipeline( "FP64",		"ShaderFloat64" );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_VERT
	#include "CodeTemplates.glsl"

	void Main ()
	{
		gl.Position = FullscreenTrianglePos();
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	#define	TYPE_FP32		1
	#define TYPE_FP32_MED	2
	#define TYPE_FP32_LOW	3
	#define TYPE_FP16		4
	#define TYPE_FP64		5

	layout(location=0) out  uint4  out_Color;

	#if TYPE == TYPE_FP32
	#	define type			float32_t
	#	define type3		f32vec3
	#	define OUTPUT(a)	out_Color.x = floatBitsToUint( a );			out_Color.y = 1;

	#elif TYPE == TYPE_FP32_MED
		precision mediump float;
	#	define type			float
	#	define type3		float3
	#	define OUTPUT(a)	out_Color.x = floatBitsToUint( a );			out_Color.y = 2;

	#elif TYPE == TYPE_FP32_LOW
		precision lowp float;
	#	define type			float
	#	define type3		float3
	#	define OUTPUT(a)	out_Color.x = floatBitsToUint( a );			out_Color.y = 3;

	#elif TYPE == TYPE_FP16
	#	define type			half
	#	define type3		half3
	#	define OUTPUT(a)	out_Color.x = float16BitsToUint16( a );		out_Color.y = 4;

	#elif TYPE == TYPE_FP64
	#	define type			double
	#	define type3		double3
	#	define OUTPUT(a)	out_Color.x = uint(doubleBitsToUint64( a ));	out_Color.y = uint(doubleBitsToUint64( a ) >> 32);
	#endif


	void Main ()
	{
		type	x;

		switch ( int(gl.FragCoord.x) )
		{
		#if TYPE == TYPE_FP32 or TYPE == TYPE_FP32_MED or TYPE == TYPE_FP32_LOW
			case 0 :	x = float_qnan;			break;
			case 1 :	x = float_snan;			break;
			case 2 :	x = float_nan;			break;
			case 3 :	x = 0.f / 0.f;			break;
			case 4 :	x = float_inf;			break;
			case 5 :	x = float_inf_neg;		break;
			case 6 :	x = float_max;			break;
			case 7 :	x = -float_max;			break;

		#elif TYPE == TYPE_FP16
			case 0 :	x = half_nan;			break;
			case 1 :	x = half_nan2;			break;
			case 2 :	x = -half_nan;			break;
			case 3 :	x = -half_nan2;			break;
			case 4 :	x = half_inf;			break;
			case 5 :	x = half_inf_neg;		break;
			case 6 :	x = half_max;			break;
			case 7 :	x = -half_max;			break;

		#elif TYPE == TYPE_FP64
			case 0 :	x = double_qnan;		break;
			case 1 :	x = double_snan;		break;
			case 2 :	x = double_nan;			break;
			case 3 :	x = 0.lf / 0.lf;		break;
			case 4 :	x = double_inf;			break;
			case 5 :	x = double_inf_neg;		break;
			case 6 :	x = double_max;			break;
			case 7 :	x = -double_max;		break;
		#endif
		}

		switch ( int(gl.FragCoord.y) )
		{
			case 1 :	x = Min( x, type(0.0) );					break;
			case 2 :	x = Min( type(0.0), x );					break;
			case 3 :	x = Max( x, type(0.0) );					break;
			case 4 :	x = Max( type(0.0), x );					break;
			case 5 :	x = Clamp( x, type(0.0), type(1.0) );		break;
			case 6 :	x = Clamp( x, type(-1.0), type(1.0) );		break;
			case 7 :	x = IsNaN( x ) ? type(1.0) : type(0.0);		break;
			case 8 :	x = IsInfinity( x ) ? type(1.0) : type(0.0);break;
			case 9 :	x = (bool(x) ? type(1.0) : type(0.0));		break;
			case 10 :	x = (x != x ? type(1.0) : type(0.0));		break;
			case 11 :	x = Step( type(0.0), x );					break;
			case 12 :	x = Step( x, type(0.0) );					break;
			case 13 :	x = Step( type(0.0), -x );					break;
			case 14 :	x = Step( -x, type(0.0) );					break;
			case 15 :	x = SignOrZero( x );						break;
			case 16 :	x = SignOrZero( -x );						break;
			case 17 :	x = SmoothStep( x, type(0.0), type(1.0) );	break;
			case 18 :	x = Normalize( type3( x, x, x )).x;			break;
		}

		OUTPUT(x);
	}

#endif
//-----------------------------------------------------------------------------

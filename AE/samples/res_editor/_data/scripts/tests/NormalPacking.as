// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*

*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#	define ENCODE_NORMAL
#	define DECODE_NORMAL
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Image>			norm_fp32	= Image( EPixelFormat::RGBA32F, SurfaceSize() );
		RC<Image>			norm_fp16	= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>			norm_un16	= Image( EPixelFormat::RGBA16_UNorm, SurfaceSize() );
		RC<Image>			norm_un8	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		RC<DynamicUInt>		p_shape		= DynamicUInt();
		RC<DynamicUInt>		p_cmp		= DynamicUInt();
		RC<DynamicUInt>		p_diff		= DynamicUInt();

		Slider( p_shape,	"Shape",	0,	1 );
		Slider( p_cmp,		"Cmp",		0,	7 );
		Slider( p_diff,		"Diff",		0,	8,	2 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "ENCODE_NORMAL" );
			pass.Output( "out_NormalFp32",	norm_fp32 );
			pass.Output( "out_NormalFp16",	norm_fp16 );
			pass.Output( "out_NormalUn16",	norm_un16 );
			pass.Output( "out_NormalUn8",	norm_un8 );
			pass.Constant( "iShape",		p_shape );
		}{
			RC<Postprocess>		pass = Postprocess( "", "DECODE_NORMAL" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn( "un_NormalFp32",	norm_fp32,	Sampler_NearestClamp );
			pass.ArgIn( "un_NormalFp16",	norm_fp16,	Sampler_NearestClamp );
			pass.ArgIn( "un_NormalUn16",	norm_un16,	Sampler_NearestClamp );
			pass.ArgIn( "un_NormalUn8",	norm_un8,	Sampler_NearestClamp );
			pass.Constant( "iShape",		p_shape );
			pass.Constant( "iCmp",			p_cmp );
			pass.Constant( "iDiff",			p_diff );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "GBuffer.glsl"
	#include "GlobalIndex.glsl"

	float3  Sphere (float2 uv, float2 duv)
	{
		uv = ToSNorm( uv ) * (duv.yx / duv.x);
		return float3( uv, Saturate( 1.0 - LengthSq( uv )) );
	}

	float4  CalcNormalAndIndex ()
	{
		const float2	scale	= float2(4.0,2.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		const uint		idx		= uint(uv.x * scale.x) + uint(uv.y * scale.y) * uint(scale.x);
		const float2	uv2		= Fract( uv * scale );
		const float2	duv		= 1.0 / float2(GetGlobalSize().xy);
		const float2	duv2	= scale / float2(GetGlobalSize().xy);

		switch ( iShape )
		{
			case 0 :	return float4( -ComputeNormalInWS_dxdy( Sphere( uv2, duv2 )), float(idx) );
			case 1 :	return float4( ComputeNormalInWS_dxdy( Sphere( uv2, duv2 )), float(idx) );
		}
	}

	float4  EncodeNormal (uint idx, float3 norm)
	{
		switch ( idx )
		{
			case 0 :	return float4( ToUNorm( norm ), 0.f );
			case 1 :	return float4( ToUNorm( CryTeck_EncodeNormal( norm )), 0.f, 0.f );
			case 2 :	return float4( Stalker_EncodeNormal( norm ), 0.f, 0.f );
			case 3 :	return float4( ToUNorm( Octahedron_EncodeNormal( norm )), 0.f, 0.f );
			case 4 :	return float4( SigOctahedron_EncodeNormal( norm ), 0.f );
			case 5 :	return float4( ToUNorm( Stereo_EncodeNormal( norm )), 0.f, 0.f );
			case 6 :	return float4( Spheremap_EncodeNormal( norm ), 0.f, 0.f );
			case 7 :	return float4( ToUNorm( Spherical_EncodeNormal( norm )), 0.f, 0.f );
		}
	}

	float3  DecodeNormal (uint idx, float4 packed)
	{
		switch ( idx )
		{
			case 0 :	return ToSNorm( packed.xyz );
			case 1 :	return CryTeck_DecodeNormal( ToSNorm( packed.xy ));
			case 2 :	return Stalker_DecodeNormal( packed.xy );
			case 3 :	return Octahedron_DecodeNormal( ToSNorm( packed.xy ));
			case 4 :	return SigOctahedron_DecodeNormal( packed.xyz );
			case 5 :	return Stereo_DecodeNormal( ToSNorm( packed.xy ));
			case 6 :	return Spheremap_DecodeNormal( packed.xy );
			case 7 :	return Spherical_DecodeNormal( ToSNorm( packed.xy ));
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef ENCODE_NORMAL

	void  Main ()
	{
		float4	n_idx	= CalcNormalAndIndex();
		float4	packed	= EncodeNormal( uint(n_idx.w), n_idx.xyz );

		out_NormalFp32	= packed;
		out_NormalFp16	= packed;
		out_NormalUn16	= packed;
		out_NormalUn8	= packed;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef DECODE_NORMAL

	void  Main ()
	{
		float4	n_idx	= CalcNormalAndIndex();
		float4	packed1	= gl.texture.Fetch( un_NormalFp32, int2(gl.FragCoord.xy), 0 );
		float4	packed2	= gl.texture.Fetch( un_NormalFp16, int2(gl.FragCoord.xy), 0 );
		float4	packed3	= gl.texture.Fetch( un_NormalUn16, int2(gl.FragCoord.xy), 0 );
		float4	packed4	= gl.texture.Fetch( un_NormalUn8, int2(gl.FragCoord.xy), 0 );

		float3	norm1	= DecodeNormal( uint(n_idx.w), packed1 );
		float3	norm2	= DecodeNormal( uint(n_idx.w), packed2 );
		float3	norm3	= DecodeNormal( uint(n_idx.w), packed3 );
		float3	norm4	= DecodeNormal( uint(n_idx.w), packed4 );
		float	diff	= Pow( 10.f, float(iDiff) );

		out_Color = float4(1.0);

		switch ( iCmp )
		{
			case 0 :	out_Color.rgb = norm1;		break;
			case 1 :	out_Color.rgb = norm2;		break;
			case 2 :	out_Color.rgb = norm3;		break;
			case 3 :	out_Color.rgb = norm4;		break;

			case 4 :	out_Color.rgb = Abs( norm1 - n_idx.xyz ) * diff;	break;
			case 5 :	out_Color.rgb = Abs( norm2 - n_idx.xyz ) * diff;	break;
			case 6 :	out_Color.rgb = Abs( norm3 - n_idx.xyz ) * diff;	break;
			case 7 :	out_Color.rgb = Abs( norm4 - n_idx.xyz ) * diff;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------

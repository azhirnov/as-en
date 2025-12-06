// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Compare accuracy of normal packing for G-Buffer.

	* RG16_UNorm has better accuracy than RG16F.
	* 'Stalker' packing is better for RG8_UNorm.
	* 'Signed Octahedron' has stable accuracy.
*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
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
		RC<DynamicUInt>		p_mode		= DynamicUInt();

		Slider( p_shape,	"Shape",	0,	4 );
		Slider( p_mode,		"Format",	0,	3 );
		Slider( p_cmp,		"Cmp",		0,	2 );
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
			pass.Constant( "iFormat",		p_mode );
			pass.Constant( "iDiff",			p_diff );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Normal.glsl"
	#include "GBuffer.glsl"
	#include "Geometry.glsl"
	#include "InvocationID.glsl"

	float3  SpherePos (float2 uv, float2 duv)
	{
		uv = ToSNorm( uv ) * (duv.yx / duv.x);
		return UVtoSphereNormal( uv ).xyz;
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
			case 0 :	return float4( -ComputeNormalInWS_dxdy( SpherePos( uv2, duv2 )), float(idx) );
			case 1 :	return float4( ComputeNormalInWS_dxdy( SpherePos( uv2, duv2 )), float(idx) );
			case 2 :	return float4( ComputeNormalInWS_dxdy( SpherePos( uv2, duv2 )) * float3(-1.0,  1.0, -1.0), float(idx) );
			case 3 :	return float4( ComputeNormalInWS_dxdy( SpherePos( uv2, duv2 )) * float3(-1.0, -1.0,  1.0), float(idx) );
			case 4 :	return float4( -ComputeNormalInWS_dxdy( SpherePos( uv2, duv2 )).zxy, float(idx) );
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
		float4	n_idx		= CalcNormalAndIndex();
		float4	packed_fp32	= gl.texture.Fetch( un_NormalFp32, int2(gl.FragCoord.xy), 0 );
		float4	packed_fp16	= gl.texture.Fetch( un_NormalFp16, int2(gl.FragCoord.xy), 0 );
		float4	packed_un16	= gl.texture.Fetch( un_NormalUn16, int2(gl.FragCoord.xy), 0 );
		float4	packed_un8	= gl.texture.Fetch( un_NormalUn8, int2(gl.FragCoord.xy), 0 );

		float3	norm_fp32	= DecodeNormal( uint(n_idx.w), packed_fp32 );
		float3	norm_fp16	= DecodeNormal( uint(n_idx.w), packed_fp16 );
		float3	norm_un16	= DecodeNormal( uint(n_idx.w), packed_un16 );
		float3	norm_un8	= DecodeNormal( uint(n_idx.w), packed_un8 );
		float	diff		= Pow( 10.f, float(iDiff) );
		float3	norm;

		switch ( iFormat )
		{
			case 0 :	norm = norm_fp32;	break;
			case 1 :	norm = norm_fp16;	break;
			case 2 :	norm = norm_un16;	break;
			case 3 :	norm = norm_un8;	break;
		}

		out_Color = float4(1.0);

		switch ( iCmp )
		{
			case 0 :	out_Color.rgb = norm;										break;
			case 1 :	out_Color.rgb = Abs( norm - n_idx.xyz ) * diff;				break;
			case 2 :	out_Color.rgb = float3(Length( norm - n_idx.xyz )) * diff;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------

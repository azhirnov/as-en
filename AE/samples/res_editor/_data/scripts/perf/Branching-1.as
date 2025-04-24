// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#	define MODE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<DynamicUInt>		tex_dim		= DynamicUInt();
		RC<DynamicDim>		dim			= tex_dim.Mul( 256 ).Dimension2();
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		const array<string>	mode_str	= {
			"MulUn", "MulNonun", "BranchUn", "BranchNonun", "MatUn", "MatNonun",
			"FlattenUn", "FlattenNonun", "DontFlattenUn", "DontFlattenNonun"
		};

		Slider( mode, 		"Mode", 	0,	mode_str.size()-1, 0 );
		Slider( count,		"Repeat",	1,	32 );
		Slider( tex_dim,	"TexDim",	1,	16 );

		Label( dim.XY(),	"Dimension" );

		// render loop
		for (uint i = 0; i < mode_str.size(); ++i)
		{
		#if 1
			RC<ComputePass>	pass = ComputePass( "", "MODE=M_"+mode_str[i] );
			pass.ArgOut( "un_Image",	rt );
			pass.LocalSize( 8, 16 );
			pass.DispatchThreads( rt.Dimension() );
		#else
			RC<Postprocess>	pass = Postprocess( "", "MODE=M_"+mode_str[i] );
			pass.Output( "out_Color",	rt,	RGBA32f(0.0) );
		#endif
			pass.EnableIfEqual( mode, i );
			pass.Repeat( count );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef MODE
	#define M_MulUn				0
	#define M_MulNonun			1
	#define M_BranchUn			2
	#define M_BranchNonun		3
	#define M_MatUn				4
	#define M_MatNonun			5

	#define M_FlattenUn			6	// \.
	#define M_FlattenNonun		7	//  |- dosn't have effect
	#define M_DontFlattenUn		8	//  |
	#define M_DontFlattenNonun	9	// /

	#include "InvocationID.glsl"
	#include "CubeMap.glsl"
	#include "Ray.glsl"


	float3  RotateVec1 (const float3 c, const ECubeFace face)
	{
		return	float3( c.z, -c.y, -c.x) * float(face == ECubeFace_XPos) +
				float3(-c.z, -c.y,  c.x) * float(face == ECubeFace_XNeg) +
				float3( c.x,  c.z,  c.y) * float(face == ECubeFace_YPos) +
				float3( c.x, -c.z, -c.y) * float(face == ECubeFace_YNeg) +
				float3( c.x, -c.y,  c.z) * float(face == ECubeFace_ZPos) +
				float3(-c.x, -c.y, -c.z) * float(face == ECubeFace_ZNeg);
	}

	float3  RotateVec2 (const float3 c, const ECubeFace face)
	{
		if ( face == ECubeFace_XPos )	return float3( c.z, -c.y, -c.x);
		if ( face == ECubeFace_XNeg )	return float3(-c.z, -c.y,  c.x);
		if ( face == ECubeFace_YPos )	return float3( c.x,  c.z,  c.y);
		if ( face == ECubeFace_YNeg )	return float3( c.x, -c.z, -c.y);
		if ( face == ECubeFace_ZPos )	return float3( c.x, -c.y,  c.z);
		if ( face == ECubeFace_ZNeg )	return float3(-c.x, -c.y, -c.z);
	}

	// from https://www.shadertoy.com/view/Mtj3DV
	float3x3  GetMatrix (const ECubeFace face)
	{
		float3	p = CM_GetNormal( face );
		float3	a = Abs( p );
		float	c = MaxOf( a );
		float3	s = (c == a.x ? float3(1.0, 0.0, 0.0) :
					(c == a.y ? float3(0.0, 1.0, 0.0) :
								float3(0.0, 0.0, 1.0)));
				s *= Sign( Dot( p, s ));
		float3	q = s.yzx;
		return float3x3( Cross( q, s ), q, s );
	}

	float3  RotateVec3 (const float3 dir, const ECubeFace face)
	{
		return dir * GetMatrix( face );
	}

	float3  RotateVec4 (const float3 c, const ECubeFace face)
	{
		[[flatten]] switch ( face )
		{
			case ECubeFace_XPos :	return float3( c.z, -c.y, -c.x);
			case ECubeFace_XNeg :	return float3(-c.z, -c.y,  c.x);
			case ECubeFace_YPos :	return float3( c.x,  c.z,  c.y);
			case ECubeFace_YNeg :	return float3( c.x, -c.z, -c.y);
			case ECubeFace_ZPos :	return float3( c.x, -c.y,  c.z);
			case ECubeFace_ZNeg :	return float3(-c.x, -c.y, -c.z);
		}
	}

	float3  RotateVec5 (const float3 c, const ECubeFace face)
	{
		[[dont_flatten]] switch ( face )
		{
			case ECubeFace_XPos :	return float3( c.z, -c.y, -c.x);
			case ECubeFace_XNeg :	return float3(-c.z, -c.y,  c.x);
			case ECubeFace_YPos :	return float3( c.x,  c.z,  c.y);
			case ECubeFace_YNeg :	return float3( c.x, -c.z, -c.y);
			case ECubeFace_ZPos :	return float3( c.x, -c.y,  c.z);
			case ECubeFace_ZNeg :	return float3(-c.x, -c.y, -c.z);
		}
	}


	void  Main ()
	{
		float4			col		= float4(0.0);
		const float2	uv		= GetGlobalCoordUNorm().xy;
		float3			dir		= Ray_PlaneTo360( float3(0.0), 0.1, uv ).dir;
		const float3x3	rot		= float3x3(  0.9017,  0.4321, -0.00746,
											-0.3461,  0.7324,  0.5862,
											 0.2588, -0.5260,  0.8100 );

	  #if 0 //def AE_shader_subgroup_ballot
		// 1.5x performance lost on Adreno 660
		const ECubeFace	un_face	= gl.subgroup.BroadcastFirst( ECubeFace( Abs(dir.z) * 6.0 ));
	  #else
		const ECubeFace	un_face	= ECubeFace( GetGroupIndex() % 6 );
	  #endif

	  #if MODE == M_MatUn
		const float3x3	f_mat	= GetMatrix( un_face );
	  #endif

		for (uint i = 0; i < 128; ++i)
		{
			const ECubeFace	face = ECubeFace( Abs(dir.z) * 6.0 );

			#if MODE == M_MulUn
				dir	= RotateVec1( dir, un_face );

			#elif MODE == M_MulNonun
				dir	= RotateVec1( dir, face );

			#elif MODE == M_BranchUn
				dir	= RotateVec2( dir, un_face );

			#elif MODE == M_BranchNonun
				dir	= RotateVec2( dir, face );

			#elif MODE == M_MatUn
				dir	= dir * f_mat;

			#elif MODE == M_MatNonun
				dir	= RotateVec3( dir, face );

			#elif MODE == M_FlattenUn
				dir	= RotateVec4( dir, un_face );

			#elif MODE == M_FlattenNonun
				dir	= RotateVec4( dir, face );

			#elif MODE == M_DontFlattenUn
				dir	= RotateVec5( dir, un_face );

			#elif MODE == M_DontFlattenNonun
				dir	= RotateVec5( dir, face );

			#else
			#	error !!!
			#endif

			dir = rot * dir;
		}

		col.rgb += dir;

		#ifdef SH_COMPUTE
			if ( AllLess( col.rgb, float3(-1.e+20) ))
				gl.image.Store( un_Image, GetGlobalCoord().xy, col );
		#else
			out_Color = Saturate(col) * 0.001;
		#endif
	}

#endif
//-----------------------------------------------------------------------------

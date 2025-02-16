// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_HEIGHT
#	define GEN_COLOR
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain (RC<Collection> collection)
	{
		RC<Image>		height_map;
		RC<Image>		normal_map;
		RC<Image>		albedo_map;
		RC<Image>		emission_map;

		if ( @collection != null )
		{
			@height_map		= collection.Image( "height" );
			@normal_map		= collection.Image( "normal" );
			@albedo_map		= collection.Image( "albedo" );
			@emission_map	= collection.Image( "emission" );
		}
		else
		{
			const uint2	dim = uint2(1024);
			@height_map		= Image( EPixelFormat::RGBA16F, dim, MipmapLevel(~0) );		height_map.Name( "height" );
			@normal_map		= Image( EPixelFormat::RGBA16F, dim, MipmapLevel(~0) );		normal_map.Name( "normal" );
			@albedo_map		= Image( EPixelFormat::RGBA16F, dim, MipmapLevel(~0) );		albedo_map.Name( "albedo" );
			@emission_map	= Image( EPixelFormat::RGBA16F, dim, MipmapLevel(~0) );		emission_map.Name( "emission" );
		}

		string	defines;
		if ( height_map.Is2D() )	defines += "COORD(_uv_)  (_uv_).xy";	else
		if ( height_map.IsCube() )	defines += "COORD(_uv_)  (_uv_)";
		Assert( defines.length() > 0 );

		const uint2		local_size	= uint2( 8, 8 );
		const uint2		face_size	= height_map.Dimension2();
		const uint		layers		= height_map.ArrayLayers();

		// height & normal
		{
			RC<ComputePass>		gen_height	= ComputePass( "", "GEN_HEIGHT;"+defines );
			const uint2			group_count	= (face_size + local_size - 3) / (local_size - 2);	// 1 pixel border

			gen_height.ArgOut( "un_OutHeight", height_map );
			gen_height.ArgOut( "un_OutNormal", normal_map );

			gen_height.LocalSize( local_size );
			gen_height.DispatchGroups( uint3( group_count, layers ));
		}

		// color
		{
			RC<ComputePass>		gen_color = ComputePass( "", "GEN_COLOR;"+defines );

			gen_color.ArgIn(  "un_HeightMap",   height_map );
			gen_color.ArgIn(  "un_NormalMap",   normal_map );
			gen_color.ArgOut( "un_OutAlbedo",   albedo_map );
			gen_color.ArgOut( "un_OutEmission", emission_map );

			gen_color.LocalSize( local_size );
			gen_color.DispatchThreads( uint3( face_size, layers ));
		}

		GenMipmaps( height_map );
		GenMipmaps( normal_map );
		GenMipmaps( albedo_map );
		GenMipmaps( emission_map );

		if ( @collection == null )
		{
			Present( albedo_map );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#define PROJECTION  CM_TangentialSC_Forward

	#include "InvocationID.glsl"
	#include "SDF.glsl"
	#include "CubeMap.glsl"
	#include "Hash.glsl"
	#include "Noise.glsl"
	#include "Normal.glsl"
	#include "Geometry.glsl"
	#include "Color.glsl"

	float2	faceDim;

	int  FaceIdx () {
		return GetGroupCoord().z;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_HEIGHT

	float  FBM (const float3 coord)
	{
		float	total		= 0.0;
		float	amplitude	= 1.0;
		float	freq		= 1.0;

		for (int i = 0; i < 7; ++i)
		{
			total		+= GradientNoise( coord * freq ) * amplitude;
			freq		*= 2.5;
			amplitude	*= 0.5;
		}
		return total;
	}


	float4  GetPosition (const int2 coord)
	{
		float2	ncoord	= UIndexToSNormRound( float2(coord), faceDim );
		float3	pos		= PROJECTION( ncoord, FaceIdx() );
		float	height	= FBM( pos ) * 0.1;

		return float4( pos, height );
	}


	// positions with 1 pixel border to calculate normal
	WGShared float3  s_Positions[ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];


	float3  ReadPosition (int2 local)
	{
		local += 1;
		return s_Positions[ local.x + local.y * gl.WorkGroupSize.x ];
	}

	float3  ReadPosition (int2 local, int2 offset) {
		return ReadPosition( local + offset );
	}


	void  Main ()
	{
		faceDim = float2(gl.image.GetSize( un_OutHeight ).xy);

		const int2		local		= GetLocalCoord().xy - 1;	// \__ 1 px border
		const int2		lsize		= GetLocalSize().xy - 2;	// /
		const int2		group		= GetGroupCoord().xy;
		const int3		coord		= int3( local + lsize * group, FaceIdx() );
		const float4	pos_h		= GetPosition( coord.xy );
		const float3	pos			= pos_h.xyz * (1.0 + pos_h.w);
		const bool		is_active	= IsInsideRect( local, int2(0), lsize );

		s_Positions[ GetLocalIndex() ] = pos;

		gl.memoryBarrier.Shared();
		gl.WorkgroupBarrier();
		gl.memoryBarrier.Shared();

		// calculate smooth normal
		if ( is_active )
		{
			float3	normal;
			SmoothNormal3x3i( OUT normal, ReadPosition, local );

			gl.image.Store( un_OutHeight, COORD(coord), float4(pos_h.w) );
			gl.image.Store( un_OutNormal, COORD(coord), float4(normal, 0.0) );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_COLOR

	FBM_NOISE_Hash( PerlinNoise )
	TURBULENCE_FBM_Hash( PerlinNoiseFBM )

	WGShared float3  s_Positions[ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];
	WGShared float3  s_Normals  [ gl.WorkGroupSize.x * gl.WorkGroupSize.y ];


	void Main ()
	{
		faceDim = float2(gl.image.GetSize( un_HeightMap ).xy);

		const int3	coord	= GetGlobalCoord();

		// read height map
		float3	sphere_pos;
		{
			float	height	= gl.image.Load( un_HeightMap, COORD(coord) ).r;
			float3	norm	= gl.image.Load( un_NormalMap, COORD(coord) ).rgb;
			float2	ncoord	= UIndexToSNormRound( float2(coord.xy), faceDim );
			sphere_pos		= PROJECTION( ncoord, FaceIdx() );

			s_Positions[ GetLocalIndex() ] = sphere_pos * (1.0 + height);
			s_Normals[ GetLocalIndex() ]   = norm;
		}

		gl.memoryBarrier.Shared();
		gl.WorkgroupBarrier();
		gl.memoryBarrier.Shared();


		float3	albedo		= float3(1.0);
		float	emission	= 0.0;
		float	temperature	= 0.0;

		sphere_pos	*= 8.0;
		sphere_pos	+= Turbulence_PerlinNoiseFBM( sphere_pos, CreateFBMParams( 2.0, 0.6, 7 )) * 2.0;

		NoiseParams	np	= CreateNoiseParams();
		np.seedScale	= float3(3.9672);
		np.custom.x		= 1.0;

		float	biom		= DHash13( VoronoiR( sphere_pos, np ).icenter );
		int		mtr_id		= int(biom * 255.0f) & 0xF;

		albedo = HSVtoRGB( float3( biom, 1.0, 1.0 ));

		gl.image.Store( un_OutAlbedo,   COORD(coord), float4(albedo, 0.0) );
		gl.image.Store( un_OutEmission, COORD(coord), float4(emission, temperature, 0.0, 0.0) );
	}

#endif
//-----------------------------------------------------------------------------

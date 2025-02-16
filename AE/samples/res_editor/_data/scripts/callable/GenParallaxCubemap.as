// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain (RC<Collection> collection)
	{
		RC<Image>	height_norm;
		RC<Image>	color;

		if ( @collection != null )
		{
			@height_norm = collection.Image( "height_norm" );
			@color		 = collection.Image( "color" );
		}
		else
		{
			const uint2	dim = uint2(1024);
			@height_norm = Image( EPixelFormat::RGBA16F, dim, MipmapLevel(~0) );		height_norm.Name( "Height & normal" );
			@color		 = Image( EPixelFormat::RGBA8_UNorm, dim, MipmapLevel(~0) );	color.Name( "Color" );
		}

		string	defines;
		if ( color.Is2D() )		defines += "COORD(_uv_)  _uv_.xy";	else
		if ( color.IsCube() )	defines += "COORD(_uv_)  _uv_";
		Assert( defines.length() > 0 );

		RC<ComputePass> pass	= ComputePass( "", defines );
		pass.ArgOut( "un_ColorMap",			color );
		pass.ArgOut( "un_HeightNormalMap",	height_norm );
		pass.LocalSize( 8, 8 );
		pass.DispatchThreads( color.Dimension2_Layers() );

		GenMipmaps( color );
		GenMipmaps( height_norm );

		if ( @collection == null )
		{
			Present( color );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "Color.glsl"
	#include "CubeMap.glsl"
	#include "Noise.glsl"
	#include "Normal.glsl"

	const float	iHeightScale = 0.1;

	int  FaceIdx () {
		return int(gl.WorkGroupID.z);
	}

	float4  GetPosition (int2 coord, int2 offset)
	{
		float2	ncoord		= float2(coord + offset) / float2(GetGlobalSize().xy);
		float3	sphere_pos	= CM_TangentialSC_Forward( ToSNorm(ncoord), FaceIdx() );

		NoiseParams	np	= CreateNoiseParams();
		np.seedScale	= float3(3.9672);
		np.custom.x		= 1.0;

		float	dist	= Voronoi( sphere_pos * 8.0, np );
				dist	= Saturate( 0.5 - dist );
		return float4( sphere_pos * (1.0 + dist * iHeightScale), dist );
	}

	void  Main ()
	{
		const int3	coord	= GetGlobalCoord();
		float		height	= GetPosition( coord.xy, int2(0) ).w;
		float3		norm;

		SmoothNormal3x3i( norm, GetPosition, coord.xy );

		float4	color = Rainbow(height) * (height*1.1 + 0.4);

		gl.image.Store( un_HeightNormalMap, COORD(coord), float4( height, norm ));
		gl.image.Store( un_ColorMap,		COORD(coord), color );
	}

#endif
//-----------------------------------------------------------------------------

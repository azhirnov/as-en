// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain (RC<Collection> collection)
	{
		array<RC<Image>>	images;
		RC<Image>			img_arr;
		const uint2			img_dim		= uint2(1024);

		if ( @collection != null )
		{
			collection.ImageArray( "array", OUT images );
			@img_arr = collection.Image( "layers" );
		}
		else
		{
			const uint	img_count = 32;

			for (uint i = 0; i < img_count; ++i){
				images.push_back( Image( EPixelFormat::RGBA8_UNorm, img_dim, MipmapLevel(~0) ));
			}
			@img_arr = Image( EPixelFormat::RGBA8_UNorm, img_dim, ImageLayer(img_count), MipmapLevel(~0) );
		}

		const uint	max_storage_img = GetFeatureSet().getPerStage_maxStorageImages();

		for (uint first = 0; first < images.size(); first += max_storage_img)
		{
			array<RC<Image>>	tmp;
			for (uint i = first; i < images.size(); ++i)
			{
				tmp.push_back( images[i] );
				if ( tmp.size() >= max_storage_img )
					break;
			}

			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_Images",		tmp );
			pass.Constant( "IMAGE_ARRAY",	1 );
			pass.Constant( "iBaseLayer",	first );
			pass.LocalSize( 16, 8 );
			pass.DispatchThreads( uint3( img_dim, tmp.size() ));
		}

		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgInOut( "un_ImgArray",	img_arr );
			pass.Constant( "iBaseLayer",	0 );
			pass.LocalSize( 16, 8 );
			pass.DispatchThreads( uint3( img_dim, images.size() ));
		}

		GenMipmaps( img_arr );
		for (uint i = 0; i < images.size(); ++i){
			GenMipmaps( images[i] );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Noise.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		uint	layer	= GetGroupCoord().z;	// uniform
		int2	pos		= GetGlobalCoord().xy;

		float	n		= PerlinNoise( float3( pos * 0.01, layer + iBaseLayer ));
		float4	col		= Rainbow(ToUNorm( n ));

	  #ifdef IMAGE_ARRAY
		gl.image.Store( un_Images[layer], pos, col );
	  #else
		gl.image.Store( un_ImgArray, int3(pos, layer), col );
	  #endif
	}

#endif
//-----------------------------------------------------------------------------

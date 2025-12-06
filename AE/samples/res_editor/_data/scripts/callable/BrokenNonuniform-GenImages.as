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
		const uint2			img_dim		= uint2(64);

		if ( @collection != null )
		{
			collection.ImageArray( "array", OUT images );
		}
		else
		{
			const uint	img_count = 32;

			for (uint i = 0; i < img_count; ++i){
				images.push_back( Image( EPixelFormat::RGBA8_UNorm, img_dim, MipmapLevel(~0) ));
			}
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
			pass.Constant( "iBaseLayer",	first );
			pass.Constant( "iNumLayers",	images.size() );
			pass.LocalSize( 16, 8 );
			pass.DispatchThreads( uint3( img_dim, tmp.size() ));
		}

		for (uint i = 0; i < images.size(); ++i){
			GenMipmaps( images[i] );
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		uint	layer	= GetGroupCoord().z;	// uniform
		int2	pos		= GetGlobalCoord().xy;
		float4	col		= Rainbow( float(layer + iBaseLayer) / float(iNumLayers) );

		gl.image.Store( un_Images[layer], pos, col );
	}

#endif
//-----------------------------------------------------------------------------

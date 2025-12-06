// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/AllModels/AllModelLoaders.h"
#include "res_loaders/Assimp/AssimpLoader.h"
#include "res_loaders/CSVMesh/CSVModelLoader.h"

namespace AE::ResLoader
{

/*
=================================================
	LoadModel
=================================================
*/
	bool  AllModelLoaders::LoadModel (OUT IntermScene	&scene,
									  RStream			&stream,
									  const Config		&cfg,
									  EModelFormat		format) __NE___
	{
		if ( format == EModelFormat::CSV )
		{
			CSVModelLoader	loader;
			return loader.LoadModel( OUT scene, stream, cfg, format );
		}

		#ifdef AE_ENABLE_ASSIMP
		{
			AssimpLoader	loader;
			if ( loader.LoadModel( OUT scene, stream, cfg, format ))
				return true;
		}
		#endif

		return false;
	}

/*
=================================================
	LoadModel
=================================================
*/
	bool  AllModelLoaders::LoadModel (OUT IntermScene	&scene,
									  const Path		&scenePath,
									  const Config		&cfg) __NE___
	{
		String	ext = ToString( scenePath.extension() );
		for (char& c : ext) { c = ToLowerCase( c ); }

		if ( ext == ".csv_mdl" )
		{
			CSVModelLoader	loader;
			return loader.LoadModel( OUT scene, scenePath, cfg );
		}

		#ifdef AE_ENABLE_ASSIMP
		{
			AssimpLoader	loader;
			if ( loader.LoadModel( OUT scene, scenePath, cfg ))
				return true;
		}
		#endif

		return false;
	}

} // AE::ResLoader

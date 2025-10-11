// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  yes
*/

#pragma once

#include "res_loaders/Public/ModelLoader.h"

namespace AE::ResLoader
{

	//
	// CSV Model Loader
	//

	class CSVModelLoader final : public IModelLoader
	{
	// methods
	public:
		bool  LoadModel (OUT IntermScene	&scene,
						 RStream			&stream,
						 const Config		&cfg,
						 EModelFormat		format)		__NE_OV;

		bool  LoadModel (OUT IntermScene	&scene,
						 const Path			&scenePath,
						 const Config		&cfg)		__NE_OV;
	};


} // AE::ResLoader

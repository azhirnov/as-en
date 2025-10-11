// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: yes
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
	class IntermMesh;


	//
	// Mesh Loader interface
	//

	class IMeshLoader
	{
	// types
	public:
		struct Config final
		{
			void*	spec	= null;
		};


	// methods
	public:
		virtual ~IMeshLoader ()											__NE___	{}

		virtual bool  LoadMesh (OUT IntermMesh		&mesh,
								RStream				&stream,
								const Config		&cfg,
								EModelFormat		format	= Default)	__NE___ = 0;

		virtual bool  LoadMesh (OUT IntermMesh		&mesh,
								const Path			&meshPath,
								const Config		&cfg)				__NE___ = 0;
	};


} // AE::ResLoader

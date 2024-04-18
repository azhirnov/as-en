// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: yes

	DDS		- without locks (only for allocations)
	DevIL	= internal mutex, single threaded
	STB		- without locks (only for allocations)
	AE		- without locks
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
	class IntermImage;


	//
	// Image Saver interface
	//

	class IImageSaver
	{
	// methods
	public:
		virtual ~IImageSaver ()												__NE___	{}

		bool  SaveImage (const Path	&		filename,
						 const IntermImage&	image,
						 EImageFormat		fileFormat	= Default,
						 Bool				flipY		= False{})			__NE___;

		virtual bool  SaveImage (WStream &			stream,
								 const IntermImage&	image,
								 EImageFormat		fileFormat,
								 Bool				flipY		= False{})	__NE___ = 0;
	};


} // AE::ResLoader

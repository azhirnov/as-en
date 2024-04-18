// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: yes
*/

#pragma once

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{
	class IntermSound;


	//
	// Sound Saver interface
	//

	class ISoundSaver
	{
	// methods
	public:
		virtual ~ISoundSaver ()										__NE___	{}

		bool  SaveSound (const Path	&		filename,
						 const IntermSound&	sound,
						 EAudioFormat		fileFormat	= Default)	__NE___;

		virtual bool  SaveSound (WStream &			stream,
								 const IntermSound&	sound,
								 EAudioFormat		fileFormat)		__NE___ = 0;
	};


} // AE::ResLoader

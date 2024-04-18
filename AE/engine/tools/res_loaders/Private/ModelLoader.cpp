// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Public/Common.h"

namespace AE::ResLoader
{

/*
=================================================
	PathToModelFormat
=================================================
*/
	EModelFormat  PathToModelFormat (const Path &) __NE___
	{
		// TODO
		return Default;
	}

/*
=================================================
	ModelFormatToExt
=================================================
*/
	StringView  ModelFormatToExt (EModelFormat fmt) __NE___
	{
		switch_enum( fmt )
		{
			case EModelFormat::GLTF :		return ".gltf";
			case EModelFormat::Unknown :	break;
		}
		switch_end
		RETURN_ERR( "unknown model format" );
	}


} // AE::ResLoader

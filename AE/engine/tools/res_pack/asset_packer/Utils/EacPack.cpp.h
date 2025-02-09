// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "MtCompression.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4244)
#endif

#include "etcpack.cpp.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif

/*
=================================================
	EacEncode
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  EacEncode (ImageMemView srcView, ImageMemView dstView, const uint threadCount, const float inQuality)
	{
		CHECK_ERR( srcView.Parts().size() == 1 );
		CHECK_ERR( dstView.Parts().size() == 1 );
		CHECK_ERR( EPixelFormat_IsEAC( dstView.Format() ));

		CHECK_ERR_MSG( srcView.Format() == EPixelFormat::RGBA16F,
			"Input image in '"s << ToString(srcView.Format()) <<
			"' format, but BC6 format requires input in 'RGBA16F' format" );


		compressBlockAlpha16();
	}

/*
=================================================
	EacDecode
----
	thread safe:  yes
=================================================
*/
	ND_ static bool  AstcDecode (ImageMemView srcView, ImageMemView dstView, const uint threadCount)
	{
		decompressBlockAlpha16bitC();
	}

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/Video.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/ImageDesc.h"

namespace AE::Graphics
{

/*
=================================================
	IsDefined
=================================================
*/
	bool  VideoProfile::IsDefined () C_NE___
	{
		return	(mode				!= Default) and
				(codec				!= Default) and
				(chromaSubsampling	!= Default) and
				(lumaBitDepth		!= 0)		and
				(chromaBitDepth		!= 0)		and
				not IsNullUnion( spec );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Validate
=================================================
*/
	void  VideoBufferDesc::Validate () __NE___
	{
		// validate as 'BufferDesc'
		{
			BufferDesc	desc;
			desc.size		= size;
			desc.usage		= usage;
			desc.options	= options;
			desc.memType	= memType;
			desc.queues		= queues;

			desc.Validate();

			size		= desc.size;
			usage		= desc.usage;
			options		= desc.options;
			memType		= desc.memType;
			queues		= desc.queues;
		}

		ASSERT( NoBits( videoUsage, EVideoBufferUsage::DecodeDst | EVideoBufferUsage::EncodeSrc ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Validate
=================================================
*/
	void  VideoImageDesc::Validate () __NE___
	{
		// validate as 'ImageDesc'
		{
			ImageDesc	desc;
			desc.dimension		= ImageDim_t{uint3{ dimension, 1u }};
			desc.arrayLayers	= arrayLayers;
			desc.options		= options;
			desc.usage			= usage;
			desc.memType		= memType;
			desc.queues			= queues;

			desc.Validate();

			dimension	= VideoImageDim_t{ desc.dimension };
			arrayLayers	= desc.arrayLayers;
			options		= desc.options;
			usage		= desc.usage;
			memType		= desc.memType;
			queues		= desc.queues;
		}

		ASSERT( NoBits( videoUsage, EVideoImageUsage::DecodeSrc | EVideoImageUsage::EncodeDst ));
	}


} // AE::Graphics

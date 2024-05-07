// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestsGraphics.pch.h"

namespace
{
	static void  BufferView_Test (ResourceManager &resMngr, const BufferDesc &bufDesc)
	{
		if ( not AnyBits( bufDesc.usage, EBufferUsage::UniformTexel | EBufferUsage::StorageTexel ))
			return;

		auto	buffer = resMngr.CreateBuffer( bufDesc );
		TEST( buffer );

		BufferViewDesc	desc;
		desc.format	= EPixelFormat::RGBA8_UNorm;
		desc.Validate( bufDesc );

		TEST( resMngr.IsSupported( buffer, desc ));

		auto	view = resMngr.CreateBufferView( desc, buffer );
		TEST( view );

		resMngr.ReleaseResource( view );
		resMngr.ReleaseResource( buffer );

		resMngr.ForceReleaseResources();
	}


	static void  Buffer_Test (ResourceManager &resMngr)
	{
		ulong	supported	= 0;
		ulong	unsupported	= 0;

		for (uint fmt = 0; fmt < uint(EPixelFormat::_Count); ++fmt)
		{
			for (EBufferUsage usage : AllCombinations<EBufferUsage::All>{})
			{
				for (EBufferOpt options : AllCombinations<EBufferOpt::All>{})
				{
					BufferDesc	desc;
					desc.size		= 1024_b;
					desc.usage		= usage;
					desc.options	= options;
					desc.memType	= EMemoryType::DeviceLocal;

					if ( resMngr.IsSupported( desc ))
					{
						++supported;
						BufferView_Test( resMngr, desc );
					}
					else
						++unsupported;
				}
			}
		}

		TEST( supported > 0 );
		Unused( unsupported );
	}
}

extern void Test_Buffer (ResourceManager &resMngr)
{
	Buffer_Test( resMngr );

	TEST_PASSED();
}

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Format:
		header
		image data:
			mipmap0
				slice0 / layer0
				slice1 / layer1
				...
			mipmap1
			...
*/

#pragma once

#include "base/Math/PackedInt.h"
#include "base/Memory/IAllocator.h"
#include "base/CompileTime/StringToID.h"
#include "serializing/ISerializable.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageMemView.h"

namespace AE::AssetPacker
{
	using namespace AE::Base;
	using namespace AE::Graphics;


	//
	// Image Packer
	//

	class ImagePacker final 
	{
	// types
	public:
		static constexpr ushort		Version			= 1;
		static constexpr ubyte		MipmapInfoFlag	= 1 << 0;
		static constexpr uint		Magic			= uint("gr.Image"_StringToID);


		struct Header
		{
			ushort			version;
			packed_ushort3	dimension;
			ushort			arrayLayers	= 0;
			ushort			mipmaps		= 0;
			EImage			viewType	= Default;
			EPixelFormat	format		= Default;
			ushort			flags		= 0;		// 0, MipmapInfoFlag
			uint			rowSize		= 0;
		};
		STATIC_ASSERT( sizeof(Header) == 20 );
		STATIC_ASSERT( alignof(Header) == 4 );
		

		struct MipmapInfo
		{
			packed_ulong	mipmap0_Offset;
			packed_ulong	mipmap1_Offset;
		};
		STATIC_ASSERT( sizeof(MipmapInfo) == 16 );
		STATIC_ASSERT( alignof(MipmapInfo) == 4 );


		struct ImageData
		{
			IAllocator*		allocator	= null;		// required for 'ReadImage()'
			ImageMemView	memView;				// output

			~ImageData ();
		};


	// variables
	public:
		uint			magic		= Magic;
		Header			header;
		//MipmapInfo	mipInfo;	// TODO


	// methods
	public:

		#ifdef AE_BUILD_ASSET_PACKER
			bool  SaveHeader (WStream &stream) const;
			bool  SaveImage (WStream &stream, const ImageMemView &src);
		#endif

			bool  ReadImage (RStream &stream, INOUT ImageData &) const;

			bool  ReadHeader (RStream &stream);
		ND_ bool  IsValid () const;

		ND_ Bytes  SliceSize () const;
		ND_ Bytes  DataSize () const;
	};


} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTrivialySerializable< AE::AssetPacker::ImagePacker::Header > { static constexpr bool  value = true; };
}
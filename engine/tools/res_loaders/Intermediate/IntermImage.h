// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/Memory/SharedMem.h"

//#include "threading/Memory/TsSharedMem.h"	// TODO

#include "graphics/Public/ImageMemView.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/MipmapLevel.h"
#include "graphics/Public/ImageLayer.h"

namespace AE::ResLoader
{
	using namespace AE::Base;
	using namespace AE::Graphics;


	//
	// Intermediate Image
	//

	class IntermImage final : public EnableRC<IntermImage>
	{
	// types
	public:
		struct Level
		{
		public:
			uint3			dimension;
			EPixelFormat	format		= Default;
			ImageLayer		layer		= 0_layer;
			MipmapLevel		mipmap		= 0_mipmap;
			Bytes			rowPitch;
			Bytes			slicePitch;
		private:
			void*			_pixels		= null;
			RC<SharedMem>	_storage;	// can be null

		public:
			ND_ bool	operator == (const Level &) const;
			
			ND_ bool	SetPixelData (RC<SharedMem> storage);
			ND_ bool	SetPixelData (RC<SharedMem> storage, void* pixels);
			ND_ bool	SetPixelData (RC<SharedMem> storage, void* pixels, Bytes size);

			ND_ bool	SetPixelDataRef (void* pixels);
			ND_ bool	SetPixelDataRef (void* pixels, Bytes size);

			ND_ void*	PixelData ()		const	{ return _pixels; }
			ND_ Bytes	DataSize ()			const	{ return slicePitch * dimension.z; }
			ND_ bool	Empty ()			const	{ return _pixels == null; }
			ND_ bool	IsPixelDataRef ()	const	{ return _pixels != null and _storage; }

			ND_ ArrayView<ubyte>  Pixels () const;
		};

		using ArrayLayers_t		= Array< Level >;			// size == 1 for non-array images
		using Mipmaps_t			= Array< ArrayLayers_t >;


	// variables
	private:
		Path			_srcPath;

		Mipmaps_t		_data;						// mipmaps[] { layers[] { level } }
		EImage			_imageType		= Default;

		bool			_immutable		= false;


	// methods
	public:
		IntermImage () {}
		IntermImage (IntermImage &&);
		explicit IntermImage (Path path) : _srcPath{RVRef(path)} {}
		IntermImage (Mipmaps_t data, EImage type, StringView path = Default) : _srcPath{path}, _data{RVRef(data)}, _imageType{type} {}

		IntermImage&  operator = (IntermImage &&);

			void  MakeImmutable ()							{ _immutable = true; }
			void  ReleaseData ()							{ Reconstruct( INOUT _data ); }
		
		ND_ bool  SetData (Mipmaps_t data, EImage type);
		ND_ bool  SetData (const ImageMemView &memView, RC<SharedMem> storage);

		ND_ bool  Copy (const ImageMemView &memView, SharedMem::Allocator_t allocator);
		ND_ bool  Copy (const ImageMemView &memView);

		ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps, SharedMem::Allocator_t allocator);
		ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, SharedMem::Allocator_t allocator);

		// with default allocator
		ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps);
		ND_ bool  Allocate (EImage type, EPixelFormat fmt, const uint3 &dim);

		ND_ bool  Reserve (EImage type, EPixelFormat fmt, const uint3 &dim, ImageLayer layers, MipmapLevel mipmaps);

		ND_ ImageMemView		ToView (MipmapLevel mipmap, ImageLayer layer);
		ND_ ImageMemView		ToView (MipmapLevel mipmap)			{ return ToView( mipmap, Default ); }
		ND_ ImageMemView		ToView (ImageLayer layer)			{ return ToView( Default, layer ); }
		ND_ ImageMemView		ToView ()							{ return ToView( Default, Default ); }

		ND_ bool				IsValid ()			const;
		ND_ bool				IsImmutable ()		const	{ return _immutable; }
		ND_ bool				IsMutable ()		const	{ return not _immutable; }

		ND_ Path const&			GetPath ()			const	{ return _srcPath; }
		ND_ Mipmaps_t const&	GetData ()			const	{ return _data; }
		ND_ Mipmaps_t *			GetMutableData ()			{ return IsMutable() ? &_data : null; }
		ND_ EImage				GetType ()			const	{ return _imageType; }
		ND_ EImageDim			GetImageDim ()		const;

		ND_ EPixelFormat		PixelFormat ()		const	{ return _data.size() and _data[0].size() ? _data[0][0].format : Default; }
		ND_ uint				MipLevels ()		const	{ return uint(_data.size()); }
		ND_ uint				ArrayLayers ()		const	{ return _data.size() ? uint(_data[0].size()) : 0u; }
		ND_ uint3				Dimension ()		const	{ return _data.size() and _data[0].size() ? _data[0][0].dimension : Default; }
		ND_ Bytes				RowPitch ()			const	{ return _data.size() and _data[0].size() ? _data[0][0].rowPitch : Default; }
	};


} // AE::ResLoader

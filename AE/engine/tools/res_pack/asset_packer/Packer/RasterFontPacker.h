// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/asset_packer/Packer/ImagePacker.h"

namespace AE::AssetPacker
{

	//
	// Raster Font Packer
	//

	class RasterFontPacker
	{
	// types
	public:

		//         .==========.  <---------- top
		//         ||        ||
		//         ||        ||
		//  *----- ||--------||  <--*------- baseline (y = 0)
		//  ^      ||        ||     ^
		//  |      '=========='  <--|------- bottom
		//  |      ^          ^     |
		//  |      |          |     |
		//  |   left         right  |
		//  |<--------------------->|
		//           advance
		//

		struct Glyph
		{
		// variables
			Rectangle<ushort>	texcoord;			// pixels
			RectF				offset;				// pixels	// TODO: use ushort
			float				advance		= 0.0f;	// glyph with including empty space

		// methods
			Glyph ()				__NE___	{}

			ND_ bool  HasImage ()	C_NE___	{ return true; }	// TODO
		};


		struct SDFConfig
		{
			float		scale	= 0.f;	// \__ convert unorm value in texture to distance in pixels
			float		bias	= 0.f;	// /
		};


		union GlyphKey
		{
		// variables
		private:
			struct Packed {
				uint		symbol : 21;
				uint		height : 11;
			}			_packed;
			uint		_value;

		// methods
		public:
			GlyphKey ()											__NE___	: _value{0} {}
			GlyphKey (CharUtf32 symbol, uint heightPx)			__NE___	: _packed{symbol, heightPx} { ASSERT_Eq( _packed.symbol, symbol );  ASSERT_Eq( _packed.height, heightPx ) }

			ND_ CharUtf32	Symbol ()							C_NE___	{ return CharUtf32(_packed.symbol); }
			ND_ uint		HeightInPix ()						C_NE___	{ return uint(_packed.height); }

			ND_ bool		operator == (const GlyphKey &rhs)	C_NE___	{ return _value == rhs._value; }
			ND_ HashVal		CalcHash ()							C_NE___	{ return HashOf(_value); }
		};


		using GlyphMap_t	= FlatHashMap< GlyphKey, Glyph, DefaultHasher_CalcHash<GlyphKey> >;
		using SizeArr_t		= FixedArray< ubyte, 16 >;		// sizes in pixels which is existed in raster font

		static constexpr ushort		Version		= 3;
		static constexpr uint		Magic		= "gr.RFnt"_Hash;
		static constexpr auto		SerID		= Serializing::SerializedID::Optimized_t{"RasterFont"};

		enum class EFileFlags : ushort
		{
			Unknown			= 0,
			HasImage		= 1 << 0,	// image data in current file
			SeparateData	= 1 << 1,	// load image data from another file
			HasResName		= 1 << 2,	// get image from cache
			_BITOPS_
		};

		struct FileHeader
		{
			uint			magic		= Magic;
			ushort			version		= Version;
			EFileFlags		flags		= Default;
		};
		StaticAssert( sizeof(FileHeader) == 8 );


	// variables
	public:
		FileHeader						_header;
		ImagePacker::Header				_imageHeader;		// HasImage or SeparateData
		CachedResourceName::Optimized_t	_imageResName;		// HasResName
		VFS::FileName::Optimized_t		_imageFileName;		// SeparateData

		SDFConfig						sdfConfig;

		GlyphMap_t						glyphMap;
		SizeArr_t						fontHeight;


	// methods
	public:
		ND_ auto*	ImageHeader ()			C_NE___	{ return AllBits( _header.flags, EFileFlags::HasImage ) ? &_imageHeader : null; }
		ND_ auto	ImageResourceName ()	C_NE___	{ return _imageResName; }
		ND_ auto	ImageFileName ()		C_NE___	{ return _imageFileName; }
	};

} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::FileHeader >	: CT_True {};
	template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::SDFConfig >	: CT_True {};
	template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::GlyphKey >	: CT_True {};
	template <> struct TTriviallySerializable< AE::AssetPacker::RasterFontPacker::Glyph >		: CT_True {};
}

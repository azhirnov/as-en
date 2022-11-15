// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "ImagePacker.h"

namespace AE::AssetPacker
{

	//
	// Raster Font Packer
	//

	class RasterFontPacker final : public Serializing::ISerializable
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
			Rectangle<ushort>	texcoord;			// unorm
			RectF				offset;				// pixels
			float				advance		= 0.0f;	// glyph with including empty space
			
			Glyph () {}

			ND_ bool  HasImage () const	{ return true; }	// TODO
		};

		struct SDFConfig
		{
			float			scale		= 0.f;	// value in texture (snorm) to distance in pixels
			float			pixRange2D;			// screenPxRange = heightInPx * pixRange2D
			packed_float2	pixRange3D;			// TODO
		};
		
		union GlyphKey
		{
		// variables
		private:
			ulong	_value;

			struct Packed {
				ulong	symbol;
				ulong	height;
			}		_packed;

		// methods
		public:
			GlyphKey () : _value{0} {}
			GlyphKey (CharUtf32 symbol, uint height) : _packed{symbol, height} {}

			ND_ CharUtf32	Symbol ()						const	{ return CharUtf32(_packed.symbol); }
			ND_ uint		HeightInPix ()					const	{ return uint(_packed.height); }

			ND_ bool		operator == (const GlyphKey &rhs) const	{ return _value == rhs._value; }
			ND_ HashVal		CalcHash ()						  const	{ return HashOf(_value); }
		};

		using GlyphMap_t	= FlatHashMap< GlyphKey, Glyph, DefaultHasher_CalcHash<GlyphKey> >;
		using SizeArr_t		= FixedArray< ubyte, 16 >;		// size in pixels which is supported

		using ImageData		= ImagePacker::ImageData;

		static constexpr ushort		Version		= 1;
		static constexpr uint		Magic		= uint("gr.RFnt"_StringToID);


	// variables
	public:
		uint					magic		= Magic;
		ushort					version		= Version;
		ImagePacker::Header		header;

		SDFConfig				sdfConfig;

		GlyphMap_t				glyphMap;
		SizeArr_t				fontHeight;


	// methods
	public:
		
		#ifdef AE_BUILD_ASSET_PACKER
			bool  SaveImage (WStream &stream, const ImageMemView &src);
		#endif

		ND_ bool  IsValid () const;

			bool  ReadImage (RStream &stream, INOUT ImageData &) const;

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};


} // AE::AssetPacker

namespace AE::Base
{
	template <> struct TTrivialySerializable< AE::AssetPacker::RasterFontPacker::SDFConfig >	{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::AssetPacker::RasterFontPacker::GlyphKey >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::AssetPacker::RasterFontPacker::Glyph >		{ static constexpr bool  value = true; };
}

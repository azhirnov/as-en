// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"
#include "graphics/Public/ResourceEnums.h"
#include "res_loaders/Intermediate/IntermImage.h"
#include "Packer/RasterFontPacker.h"

namespace AE::AssetPacker
{
	using AE::Graphics::EPixelFormat;


	//
	// Script Raster Font
	//

	class ScriptRasterFont final : public EnableScriptRC
	{
	// types
	public:
		enum class ERasterFontMode : uint
		{
			Raster,
			SDF,		// single channel
			MC_SDF,		// multi channel
			_Count
		};

	private:
		using CharsetArr_t	= Array<Pair< CharUtf32, CharUtf32 >>;
		using Glyph			= AssetPacker::RasterFontPacker::Glyph;
		using GlyphKey		= AssetPacker::RasterFontPacker::GlyphKey;
		using Allocator_t	= LinearAllocator<>;
		using IntermImage_t	= Unique< ResLoader::IntermImage >;

		struct GlyphData : Glyph
		{
			CharUtf32		symbol		= 0;
			void*			data		= null;
			uint			dataSize	= 0;
			uint2			dimension;				// pix
			EPixelFormat	format		= Default;
		};

		static constexpr uint	_MaxFontHeight	= MaxValue<ubyte>();
		static constexpr uint	_TabSize		= 4;	// in spaces


	// variables
	private:
		IntermImage_t		_imgData;

		CharsetArr_t		_charset;
		Path				_fontFile;

		ERasterFontMode		_fontMode			= ERasterFontMode::Raster;
		uint				_bitmapHeight		= 1;
		int					_paddingPix			= 1;
		double				_sdfPixRange		= 1.0;	// TODO: remove
		uint				_sdfBorderSize		= 0;

		EPixelFormat		_dstFormat		= EPixelFormat::R8_UNorm;
		EPixelFormat		_intermFormat	= EPixelFormat::R8_UNorm;


	// methods
	public:
		ScriptRasterFont ();
		~ScriptRasterFont ();

		void  Load (const String &fontFile)						__Th___;
		void  AddCharset (uint unicodeFirst, uint unicodeLast)	__Th___;
		void  AddCharset_Ascii ()								__Th___;
		void  AddCharset_Rus ()									__Th___;
		void  ClearCharset ()									__Th___		{ _charset.clear(); }

		void  SetGlyphSize (uint value)							__Th___;
		void  SetGlyphPadding (uint pix)						__Th___;
		void  SetFormat (EPixelFormat fmt)						__Th___;
		void  SetRasterMode (ERasterFontMode value)				__Th___;
		void  SetSDFGlyphBorder (uint pix)						__Th___;
		void  SetSDFPixelRange (float range)					__Th___;

		void  Store (const String &nameInArchive)				__Th___;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;

	private:
		ND_ bool  _Pack (const String &nameInArchive, RC<WStream> stream);

	  #ifdef AE_ENABLE_FREETYPE
		// FreeType
		ND_ bool  _FTLoadGlyphs (Allocator_t &, OUT uint &outHeight, OUT Array<GlyphData> &) const;
		ND_ bool  _FTLoadGlyph (void* face, CharUtf32 unicodeChar, Allocator_t &, OUT GlyphData &) const;
	  #endif

	  #ifdef AE_ENABLE_MSDFGEN
		// SDF
		ND_ bool  _SDFLoadGlyphs (Allocator_t &, OUT float &sdfScale, OUT float &sdfBias, OUT uint &outHeight, OUT Array<GlyphData> &) const;
		ND_ bool  _SDFLoadGlyph (void* face, CharUtf32 unicodeChar, double projScale, Allocator_t &, OUT GlyphData &) const;
	  #endif
	};

	using ScriptRasterFontPtr = ScriptRC< ScriptRasterFont >;


} // AE::AssetPacker

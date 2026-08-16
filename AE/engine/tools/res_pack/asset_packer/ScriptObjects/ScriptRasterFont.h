// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_pack/asset_packer/ScriptObjects/ScriptSharedImage.h"
#include "res_pack/asset_packer/Packer/RasterFontPacker.h"

namespace AE::AssetPacker
{

	//
	// Script Raster Font
	//

	class ScriptRasterFont final : public EnableScriptRC
	{
		friend class ScriptSharedImage;

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
		using SDFConfig		= AssetPacker::RasterFontPacker::SDFConfig;
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

		enum class EState : uint
		{
			Recording,
			Immutable,
			Arranged,
			StoreData,
			Stored,
		};


	// variables
	private:
		Array<GlyphData>		_glyphs;
		Allocator_t				_allocator;

		CharsetArr_t			_charset;
		Path					_fontFile;

		ERasterFontMode			_fontMode			= ERasterFontMode::Raster;
		uint					_bitmapHeight		= 1;
		int						_paddingPix			= 1;
		uint					_sdfBorderSize		= 0;

		EPixelFormat			_dstFormat			= EPixelFormat::R8_UNorm;
		EPixelFormat			_intermFormat		= EPixelFormat::R8_UNorm;
		SDFConfig				_sdfConfig;
		uint					_fontHeight			= 0;

		ImagePacker::Header		_imageHeader;
		String					_nameInMeta;
		String					_sharedImageMeta;
		String					_imageFileName;
		mutable EState			_state				= EState::Recording;


	// methods
	public:
		ScriptRasterFont ();
		~ScriptRasterFont ();

		void  Load (const String &fontFile)										__Th___;
		void  AddCharset (uint unicodeFirst, uint unicodeLast)					__Th___;
		void  AddCharset_Ascii ()												__Th___;
		void  AddCharset_Rus ()													__Th___;

		void  SetGlyphSize (uint value)											__Th___;
		void  SetGlyphPadding (uint pix)										__Th___;
		void  SetFormat (EPixelFormat fmt)										__Th___;
		void  SetRasterMode (ERasterFontMode value)								__Th___;
		void  SetSDFGlyphBorder (uint pix)										__Th___;

		void  Store (const String &nameInArchive)								__Th___;
		void  StoreData (const String &nameInArchive)							__Th___;

		void  PutMeta (const ScriptResourceMetaPtr &, const String &name)		__Th___;
		void  PutData (const ScriptSharedImagePtr &image)						__Th___;

		// used by 'ScriptResourceMeta'
		ND_ bool  _StoreMeta (RC<WStream>, const String &metaArchive = Default)	C_NE___;

		static void  Bind (const ScriptEnginePtr &se)							__Th___;

	private:
		ND_ bool  _Rasterize ()													__NE___;
		ND_ bool  _ToTexture (OUT ScriptTexture &, const String &name)			__NE___;

		ND_ bool  _CopyPixels (INOUT ResLoader::IntermImage &, ArrayView<ScriptSharedImage::Result>) __NE___;

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


} // AE::AssetPacker

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/GraphicsCreateInfo.h"
#include "graphics_hl/Resources/FormattedText.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{
	class ITransferContext;


	//
	// Raster Font
	//

	class RasterFont final : public EnableRC<RasterFont>
	{
	// types
	public:
		using Glyph			= AssetPacker::RasterFontPacker::Glyph;
		using SDFConfig		= AssetPacker::RasterFontPacker::SDFConfig;
		using GlyphKey		= AssetPacker::RasterFontPacker::GlyphKey;
		using GlyphMap_t	= AssetPacker::RasterFontPacker::GlyphMap_t;
		using SizeArr_t		= AssetPacker::RasterFontPacker::SizeArr_t;

		struct AsyncLoader {
		//	ND_ Promise<RC<RasterFont>>  Load (RC<AsyncRStream> stream);
		};
		
		struct Loader {
			ND_ RC<RasterFont>  Load (RC<RStream> stream, ITransferContext &ctx, GfxMemAllocatorPtr alloc = Default);
		};


	// variables
	private:
		StrongImageAndViewID	_imageAndView;
		SDFConfig				_sdfConfig;
		GlyphMap_t				_glyphMap;
		SizeArr_t				_fontHeight;


	// methods
	public:
		RasterFont () {}
		~RasterFont ();
		
		RasterFont&  operator = (RasterFont &&);

		ND_ uint  ValidateHeight (float heightInPx) const;

		ND_ Glyph const*  GetGlyph (CharUtf32 symbol, uint height) const;
		
		void  CalculateDimensions (const float2 &areaSizeInPix, INOUT PrecalculatedFormattedText &) const;

		ND_ float  ScreenPixRange (float heightInPx) const;	// 2D SDF only

		ND_ bool				IsSDF ()			const	{ return _sdfConfig.scale != 0.f; }
		ND_ ImageID				GetImageID ()		const	{ return _imageAndView.image; }
		ND_ ImageViewID			GetViewID ()		const	{ return _imageAndView.view; }
		ND_ ImageAndViewID		GetImageAndView ()	const	{ return _imageAndView; }
		ND_ SDFConfig const&	GetSDFConfig ()		const	{ return _sdfConfig; }
	};


} // AE::Graphics

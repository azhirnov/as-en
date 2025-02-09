// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/Resources/LoadableImage.h"
#include "graphics_hl/Resources/FormattedText.h"

#include "AssetPackerImpl.h"

namespace AE::Graphics
{

	//
	// Raster Font
	//

	class RasterFont final : public CachedResource
	{
	// types
	public:
		using Glyph			= AssetPacker::RasterFontPacker::Glyph;
		using SDFConfig		= AssetPacker::RasterFontPacker::SDFConfig;
		using GlyphKey		= AssetPacker::RasterFontPacker::GlyphKey;
		using GlyphMap_t	= AssetPacker::RasterFontPacker::GlyphMap_t;
		using SizeArr_t		= AssetPacker::RasterFontPacker::SizeArr_t;

		// should be used in background thread
		struct Loader {
			// single file
			ND_ static RC<RasterFont>  Load (VFS::FileName::Ref, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;
			ND_ static RC<RasterFont>  Load (RC<Threading::AsyncRDataSource> ds, GfxMemAllocatorPtr alloc, ResourceUploadManager &) __NE___;

			// meta data + image data
			ND_ static RC<RasterFont>  Load (Serializing::Deserializer &des, ResourceCache&, CachedResourceName::Ref selfName = Default) __NE___;

			// meta data + image data, ready when completely upload to GPU
			ND_ static Promise<RC<RasterFont>>  LoadAsync (Serializing::Deserializer &des, ResourceCache&, CachedResourceName::Ref selfName = Default) __NE___;

		private:
			class OnUploadCompleteTask;
			ND_ static bool  _Load (Serializing::Deserializer &des, ResourceCache&, CachedResourceName::Ref selfName,
									OUT RC<RasterFont> &, OUT ResourceUploadManager::UploadResult *) __NE___;
		};


	// variables
	private:
		ImageID					_imageId;		// '_viewId' will keep strong reference
		Strong<ImageViewID>		_viewId;

		SDFConfig				_sdfConfig;
		GlyphMap_t				_glyphMap;
		SizeArr_t				_fontHeight;


	// methods
	public:
		RasterFont ()															__NE___	{}
		~RasterFont ()															__NE___;

		ND_ Glyph const*	GetGlyph (CharUtf32 symbol, uint heightPx)			C_NE___;

		ND_ uint  ValidateHeight (float heightPx)								C_NE___;

			void  CalculateDimensions (const float2 &areaSizePx,
									   INOUT PrecalculatedFormattedText &)		C_NE___;

		ND_ ImageDesc			GetImageDesc ()									C_NE___;
		ND_ ImageViewDesc		GetViewDesc ()									C_NE___;

		ND_ bool				IsSDF ()										C_NE___	{ return _sdfConfig.scale != 0.f; }
		ND_ ImageID				ImageId ()										C_NE___	{ return _imageId; }
		ND_ ImageViewID			ViewId ()										C_NE___	{ return _viewId; }
		ND_ SDFConfig const&	GetSDFConfig ()									C_NE___	{ return _sdfConfig; }

	private:
		void  _ConvertPixelsToUNorm (float2 invImageDim)						__NE___;
	};


} // AE::Graphics

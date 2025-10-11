// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Resources/LoadableImage.h"
#include "graphics/Resources/RasterFont.h"

namespace AE::Graphics
{
	using namespace AE::Serializing;
	using namespace AE::AssetPacker;

namespace {
#	include "res_pack/asset_packer/Packer/ImagePacker.cpp.h"
#	include "res_pack/asset_packer/Packer/RasterFontPacker.cpp.h"
}

/*
=================================================
	destructor
=================================================
*/
	RasterFont::~RasterFont () __NE___
	{
		if ( _viewId )
			GraphicsScheduler().GetResourceManager().DelayedReleaseResources( _viewId );
	}

/*
=================================================
	GetImageDesc / GetViewDesc
=================================================
*/
	ImageDesc  RasterFont::GetImageDesc () C_NE___
	{
		ASSERT( _imageId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _imageId );
	}

	ImageViewDesc  RasterFont::GetViewDesc () C_NE___
	{
		ASSERT( _viewId );
		return GraphicsScheduler().GetResourceManager().GetDescription( _viewId );
	}

/*
=================================================
	ValidateHeight
=================================================
*/
	uint  RasterFont::ValidateHeight (float heightPx) C_NE___
	{
		if_unlikely( _fontHeight.empty() )
			return 0;

		// SDF must have x2 lower resolution
		heightPx *= IsSDF() ? 0.5f : 1.0f;

		if_unlikely( heightPx <= float(_fontHeight.front()) )
			return _fontHeight.front();

		for (usize i = 1; i < _fontHeight.size(); ++i)
		{
			float	h1 = float(_fontHeight[i-1]);
			float	h2 = float(_fontHeight[i]);

			if_unlikely( heightPx <= h2 )
			{
				return	heightPx < Lerp( h1, h2, 0.5f ) ?
							_fontHeight[i-1] : _fontHeight[i];
			}
		}

		return _fontHeight.back();
	}

/*
=================================================
	GetGlyph
=================================================
*/
	RasterFont::Glyph const*  RasterFont::GetGlyph (CharUtf32 symbol, uint heightPx) C_NE___
	{
		GlyphKey	key { symbol, heightPx };
		auto		it	= _glyphMap.find( key );

		return it != _glyphMap.end() ? &it->second : null;
	}

/*
=================================================
	CalculateDimensions
=================================================
*/
	void  RasterFont::CalculateDimensions (const float2 &areaSizePx, INOUT PrecalculatedFormattedText &result) C_NE___
	{
		using Chunk = FormattedText::Chunk;

		float2		line_px;
		uint		max_height	= 0;

		result._size = float2{0.0f};
		result._lineHeight.clear();

		const auto	ToNextLine = [&] (Chunk const* chunk)
		{
			float	height	= result.Spacing() * max_height;
			result._lineHeight.push_back( height );

			result._size.x	= Max( result._size.x, line_px.x );
			line_px.y		+= height;
			line_px.x		= 0.0f;
			max_height		= chunk ? chunk->height : 0;
		};

		for (auto* chunk = result.Text().GetFirst(); chunk; chunk = chunk->next)
		{
			const uint		font_h_px		= ValidateHeight( chunk->Height() );
			const float		font_scale_px	= chunk->Height() / font_h_px;

			max_height = Max( max_height, chunk->height );

			for (size_t pos = 0; pos < chunk->length;)
			{
				const CharUtf32		c = Utf8Decode( chunk->string, chunk->length, INOUT pos );

				if_unlikely( c == '\n' )
				{
					ToNextLine( chunk );
					continue;
				}

				auto*	glyph = GetGlyph( c, font_h_px );
				if_unlikely( glyph == null )
					continue;

				const float	width_px = glyph->advance * font_scale_px;

				if_unlikely( result.IsWordWrap() and (line_px.x + width_px > areaSizePx.x) )
					ToNextLine( chunk );
				else
					line_px.x += width_px;
			}
		}

		//if ( line_px.x > 0.0f )
		ToNextLine( null );

		result._size.y = Max( result._size.y, line_px.y );
	}

/*
=================================================
	_ConvertPixelsToUNorm
=================================================
*/
	void  RasterFont::_ConvertPixelsToUNorm (const float2 invImageDim) __NE___
	{
		for (auto& glyph : _glyphMap)
		{
			glyph.second.texcoord = FloatToUNormShort( RectF{glyph.second.texcoord} * invImageDim );
		}
	}

/*
=================================================
	Loader::_OnUploadComplete
=================================================
*/
	AsyncCoro  RasterFont::Loader::_OnUploadComplete (RC<RasterFont> font, ResourceUploadManager::UploadResult upload) __NE___
	{
		bool	ok = upload ? upload->IsCompleted() : true;
		font->_SetLoadingStatus( ok ? ELoadingStatus::Complete : ELoadingStatus::Failed );

		font   = null;
		upload = null;
		co_return;
	}

/*
=================================================
	Loader::_Load
=================================================
*/
	bool  RasterFont::Loader::_Load (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName,
									 OUT RC<RasterFont> &font, OUT ResourceUploadManager::UploadResult* outUploadTask) __NE___
	{
		RasterFontPacker	unpacker;
		CHECK_ERR( RasterFontPacker_Deserialize( OUT unpacker, des ));

		CHECK_ERR( unpacker.ImageHeader() == null );
		CHECK_ERR( unpacker.ImageResourceName().IsDefined() );

		font = MakeRC<RasterFont>();
		font->_glyphMap		= RVRef(unpacker.glyphMap);
		font->_fontHeight	= unpacker.fontHeight;
		font->_sdfConfig	= unpacker.sdfConfig;

		auto	img_res = resCache.GetResource<LoadableImage>( CachedResourceName{unpacker.ImageResourceName()} );
		CHECK_ERR_MSG( img_res, "RasterFont image must be in resource cache!" );

		auto	upload = img_res->OnUploadComplete();
		if ( outUploadTask != null )
			*outUploadTask = upload;

		auto	status = img_res->LoadingStatus();
		CHECK_ERR( status >= ELoadingStatus::Created );

		font->_viewId = img_res->CloneImageView();
		CHECK_ERR( font->_viewId );

		font->_imageId = img_res->ImageId();
		font->_SetLoadingStatus( status );

		font->_ConvertPixelsToUNorm( 1.0f / float2{img_res->GetImageDesc().Dimension2()} );

		if ( selfName.IsDefined() )
		{
			if ( auto cached = resCache.InsertResource( selfName, font ))
			{
				ASSERT( cached->ImageId() == font->ImageId() );
				font = RVRef(cached);
				return true;
			}
		}

		// update status after uploading
		if ( upload )
		{
			Scheduler().Run(
				ETaskQueue::Background,
				_OnUploadComplete( font, upload ),
				Tuple{ResourceUploadManager::WeakUploadResult{upload}}
			);
		}
		else
		{
			status = img_res->LoadingStatus();
			CHECK_ERR( status == ELoadingStatus::Complete );
			font->_SetLoadingStatus( status );
		}
		return true;
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<RasterFont>  RasterFont::Loader::Load (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName) __NE___
	{
		RC<RasterFont>	font;
		Unused( _Load( des, resCache, selfName, OUT font, null ));
		return font;
	}

/*
=================================================
	Loader::LoadAsync
=================================================
*/
	Promise<RC<RasterFont>>  RasterFont::Loader::LoadAsync (Serializing::Deserializer &des, ResourceCache &resCache, CachedResourceName::Ref selfName) __NE___
	{
		RC<RasterFont>						font;
		ResourceUploadManager::UploadResult	upload;

		if ( _Load( des, resCache, selfName, OUT font, OUT &upload ))
		{
			return Scheduler().Run(
						ETaskQueue::Background,
						DeferResult< RC<RasterFont> >( RVRef(font) ),
						Tuple{RVRef(upload)} );
		}
		return Default;
	}

/*
=================================================
	Loader::Load
=================================================
*/
	RC<RasterFont>  RasterFont::Loader::Load (RC<Threading::AsyncRDataSource> file, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		CHECK_ERR( file );

		RasterFontPacker	unpacker;
		Bytes				file_offset;
		{
			auto	stream = MakeRC<BufferedRStream>( MakeRC<Threading::SyncRStreamOnAsyncDS>( file ));
			{
				Serializing::Deserializer	des {stream};
				CHECK_ERR( RasterFontPacker_Deserialize( OUT unpacker, des ));
				CHECK_ERR( AllBits( unpacker._header.flags, RasterFontPacker::EFileFlags::HasImage ));
			}
			file_offset = stream->Position();
		}

		auto&	img_header	= *unpacker.ImageHeader();
		auto	font		= MakeRC<RasterFont>();
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		GAutorelease<ImageID>	image_id = res_mngr.CreateImage( img_header.ToDesc().SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), Default, RVRef(alloc) );
		CHECK_ERR( image_id );

		font->_viewId = res_mngr.CreateImageView( ImageViewDesc{img_header.viewType}, image_id );
		CHECK_ERR( font->_viewId );

		auto	upload = uploadMngr.CreateTask();
		CHECK_ERR( upload );

		font->_SetLoadingStatus( ELoadingStatus::Created );

		CHECK_ERR( uploadMngr.EnqueueImage( upload, image_id, RVRef(file), file_offset,
											EUploadFlags::UsedWhileUploading, Default, EResourceState::FragmentShader | EResourceState::ShaderSample ));

		font->_SetLoadingStatus( ELoadingStatus::Uploading );

		font->_glyphMap		= RVRef(unpacker.glyphMap);
		font->_fontHeight	= unpacker.fontHeight;
		font->_sdfConfig	= unpacker.sdfConfig;

		font->_ConvertPixelsToUNorm( 1.0f / float2{img_header.dimension} );

		Scheduler().Run(
			ETaskQueue::Background,
			_OnUploadComplete( font, upload ),
			Tuple{ResourceUploadManager::WeakUploadResult{upload}}
		);
		return font;
	}

	RC<RasterFont>  RasterFont::Loader::Load (VFS::FileName::Ref name, GfxMemAllocatorPtr alloc, ResourceUploadManager &uploadMngr) __NE___
	{
		RC<Threading::AsyncRDataSource>		file;
		CHECK_ERR( GetVFS().Open( OUT file, name ));
		return Loader::Load( RVRef(file), RVRef(alloc), uploadMngr );
	}

} // AE::Graphics

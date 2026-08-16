// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "graphics/UI/Drawable.h"
#include "graphics/UI/System.h"

#include "graphics/Resources/ResourceUploadManager.h"
#include "graphics/Resources/LoadableImage.h"
#include "graphics/Resources/StaticImageAtlas.h"
#include "graphics/Resources/RasterFont.h"

namespace AE::UI
{
	using namespace AE::Graphics;

namespace {
/*
=================================================
	UpdatePipeline
=================================================
*/
	ND_ static bool  UpdatePipeline (OUT GraphicsPipelineID &pipeline, PipelineName::Ref pplnName, const StyleCollection &styleCol) __NE___
	{
		pipeline = styleCol.GetRTech().GetGraphicsPipeline( pplnName );
		CHECK_ERR( pipeline );
		return true;
	}

/*
=================================================
	UpdateImageUV
=================================================
*/
	ND_ static bool  UpdateImageUV (OUT StyleCollection::IStyle::UV_t &uv, CachedResourceName::Ref resName, ImageInAtlasName::Ref imgName, const ResourceCache &resCache) __NE___
	{
		auto	atlas = resCache.GetResource<StaticImageAtlas>( resName );
		CHECK_ERR( atlas );
		return atlas->Get( imgName, OUT uv );
	}

/*
=================================================
	UpdateFont
=================================================
*/
	ND_ static bool  UpdateFont (OUT RC<RasterFont> &font, CachedResourceName::Ref resName, const ResourceCache &resCache) __NE___
	{
		font = resCache.GetResource<RasterFont>( resName );
		CHECK_ERR( font );
		return true;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize
=================================================
*/
	bool  StyleCollection::ColorStyle::Deserialize (const StyleCollection &styleCol, const ResourceCache &, Serializing::Deserializer &des) __NE___
	{
		PipelineName	ppln_name;
		return	des( OUT ppln_name, OUT colors )				and
				UpdatePipeline( OUT pipeline, ppln_name, styleCol );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize
=================================================
*/
	bool  StyleCollection::ImageStyle::Deserialize (const StyleCollection &styleCol, const ResourceCache &resCache, Serializing::Deserializer &des) __NE___
	{
		PipelineName		ppln_name;
		CachedResourceName	atlas_name;
		ImageInAtlasName	img_name;

		return	des( OUT ppln_name, OUT atlas_name, OUT img_name, OUT scale_color ) and
				UpdatePipeline( OUT pipeline, ppln_name, styleCol )					and
				UpdateImageUV( OUT uv, atlas_name, img_name, resCache );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize
=================================================
*
	bool  StyleCollection::ImageAnimationStyle::Deserialize (const StyleCollection &styleCol, const ResourceCache &, Serializing::Deserializer &des) __NE___
	{
		PipelineName	ppln_name;
		return	des( OUT ppln_name ) and des( OUT uv_scale_color ) and
				UpdatePipeline( OUT pipeline, ppln_name, styleCol );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Deserialize
=================================================
*/
	bool  StyleCollection::FontStyle::Deserialize (const StyleCollection &styleCol, const ResourceCache &resCache, Serializing::Deserializer &des) __NE___
	{
		CachedResourceName	font_name;
		PipelineName		ppln_name;

		return	des( OUT ppln_name, OUT font_name, OUT colors )		and
				UpdatePipeline( OUT pipeline, ppln_name, styleCol )	and
				UpdateFont( OUT font, font_name, resCache );
	}

	StyleCollection::FontStyle::FontStyle () __NE___ {}
	StyleCollection::FontStyle::~FontStyle () __NE___ {}
//-----------------------------------------------------------------------------



/*
=================================================
	UnsafeSetter
=================================================
*/
	void  StyleCollection::UnsafeSetter::SetAnimSpeed (StyleCollection &self, float value)
	{
		self._settings.colorAnimSpeed = value;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	AnimationSettings
=================================================
*/
	StyleCollection::AnimationSettings::AnimationSettings () __NE___ :
		colorAnimSpeed{ 2.f }
	{}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	StyleCollection::StyleCollection () __NE___
	{}

	StyleCollection::~StyleCollection () __NE___
	{}

/*
=================================================
	GetStyle
=================================================
*/
	Ptr<const StyleCollection::IStyle>  StyleCollection::GetStyle (StyleName::Ref id) C_NE___
	{
		auto	it = _styleMap.find( id );
		if_likely( it != _styleMap.end() )
			return it->second.get();

		return null;
	}

/*
=================================================
	GetFontStyle
=================================================
*/
	Ptr<const StyleCollection::FontStyle>  StyleCollection::GetFontStyle (StyleName::Ref id) C_NE___
	{
		auto	style = GetStyle( id );

		if ( style and style->GetType() == TypeIdOf<FontStyle>() )
			return style;

		return null;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  StyleCollection::Initialize (const CreateInfo &ci) __NE___
	{
		CHECK_ERR( ci.rtech );
		CHECK_ERR( ci.stream );
		CHECK_ERR( AllBits( ci.stream->GetSourceType(), IDataSource::ESourceType::Buffered ));
		CHECK_ERR( ci.dummyImage );

		Serializing::Deserializer	des	{ci.stream};

		CHECK_ERR( _DeserializeResources( des, ci.resCache, ci.uploadMngr ));
		CHECK_ERR( _InitGraphics( ci.rtech, ci.dummyImage, ci.ubSize ));
		CHECK_ERR( _DeserializeStyles( des, ci.resCache ));  // throw

		return true;
	}

/*
=================================================
	InitializeAsync
=================================================
*/
	AsyncTask  StyleCollection::InitializeAsync (CreateInfoAsync &ci) __NE___
	{
		using namespace AE::Threading;

		CHECK_ERR( ci.rtech );
		CHECK_ERR( ci.stream );
		CHECK_ERR( AllBits( ci.stream->GetSourceType(), IDataSource::ESourceType::Buffered ));
		CHECK_ERR( ci.dummyImage );

		AsyncTask	load_res = Scheduler().Run(
			ETaskQueue::Background,
			[] (StyleCollection &self, auto stream, auto& resCache, auto& uploadMngr) -> AsyncCoro
			{
				Serializing::Deserializer	des	{RVRef(stream)};
				CHECK_CE( self._DeserializeResources( des, resCache, uploadMngr ));
				co_return;
			}
			( *this, ci.stream, ci.resCache, ci.uploadMngr ),
			Tuple{},
			"UI.StyleCollection.LoadResource" );

		AsyncTask	init_gfx =
			[] (StyleCollection &self, auto loadRTech, auto loadRes, const Bytes ubSize, auto dummyImg)
				-> InlineCoro<ETaskQueue::Background>
			{
				auto	res		= Coro_WaitResultOrCancel( loadRTech, loadRes );
				auto&	rtech	= res.template get<0>();

				CHECK_CE( self._InitGraphics( rtech, dummyImg, ubSize ));
				co_return;
			}
			( *this, ci.rtech, load_res, ci.ubSize, ci.dummyImage );

		AsyncTask	load_styles = Scheduler().Run(
			ETaskQueue::Background,
			[] (StyleCollection &self, auto stream, auto& resCache) -> AsyncCoro
			{
				Serializing::Deserializer	des	{stream};
				CHECK_CE( self._DeserializeStyles( des, resCache ));  // throw
				co_return;
			}
			( *this, ci.stream, ci.resCache ),
			Tuple{ load_res },
			"UI.StyleCollection.LoadStyles" );

		return Scheduler().WaitAsync( ETaskQueue::Background, Tuple{ init_gfx, load_styles });
	}

/*
=================================================
	_InitGraphics
=================================================
*/
	bool  StyleCollection::_InitGraphics (RenderTechPipelinesPtr rtech, ImageViewID dummyImage, const Bytes ubSize) __NE___
	{
		DRC_EXLOCK( _drCheckGraphics );
		DRC_SHAREDLOCK( _drCheckResources );

		CHECK_ERR( _imageRGBA or _imageAlpha );

		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		ub_align	= DeviceLimits.res.minUniformBufferOffsetAlign;
		const Bytes		buf_size	= AlignUp( ubSize, ub_align ) * MaxScreens * rts.GetMaxFrames();
		const auto		info		= rtech->GetPass( RenderTechPassName{"Main"} );

		_dynamicUBufSize = AlignUp( ubSize, ub_align );

		_dynamicUBuf = res_mngr.CreateBuffer( BufferDesc{ buf_size, EBufferUsage::TransferDst | EBufferUsage::Uniform }.SetMemory( EMemoryType::DeviceLocal ),
											  "UI dynamic ubuffer" );
		CHECK_ERR( _dynamicUBuf );
		CHECK_ERR( info.dsLayoutId.IsValid() );

		_sharedDescSet = res_mngr.CreateDescriptorSet( info.dsLayoutId, null, "UI global DS" );
		CHECK_ERR( _sharedDescSet );
		CHECK_ERR( info.dsIndex == IDrawable::GlobalMaterial::dsIndex );
		{
			DescriptorUpdater	updater;
			CHECK_ERR( updater.Set( _sharedDescSet, EDescUpdateMode::Partialy ));

			CHECK_ERR( updater.BindBuffer( UniformName{"globalUB"},	_dynamicUBuf, 0_b, ubSize ));
			//CHECK_ERR( updater.BindBuffer( UniformName{"mtrUB"},	_dynamicUBuf, 0_b, ubSize ));

			CHECK_ERR( updater.BindImage( UniformName{"un_ImageRGBA"},	_imageRGBA  ? _imageRGBA->ViewId()  : dummyImage ));
			CHECK_ERR( updater.BindImage( UniformName{"un_ImageAlpha"},	_imageAlpha ? _imageAlpha->ViewId() : dummyImage ));

			CHECK_ERR( updater.Flush() );
		}

		_rtech = RVRef(rtech);
		return true;
	}

/*
=================================================
	_DeserializeStyles
=================================================
*/
	bool  StyleCollection::_DeserializeStyles (Serializing::Deserializer	&des,
											   ResourceCache				&resCache) __Th___
	{
		DRC_EXLOCK( _drCheckStyles );
		DRC_SHAREDLOCK( _drCheckGraphics );
		DRC_SHAREDLOCK( _drCheckResources );

		CHECK_ERR( _rtech );

		PipelineName	dbg_ppln_name;
		uint			style_count		= 0;

		CHECK_ERR( des( OUT dbg_ppln_name, OUT style_count ));

		if ( dbg_ppln_name.IsDefined() )
		{
			_dbgPpln = _rtech->GetGraphicsPipeline( dbg_ppln_name );
			ASSERT( _dbgPpln );
		}

		_styleMap.reserve( style_count );	// throw

		bool	result = true;
		for (uint i = 0; result and (i < style_count); ++i)
		{
			StyleName::Optimized_t	name;
			EType					type;

			result = des( OUT name, OUT type );

			switch_enum( type )
			{
				case EType::ColorStyle :
				{
					auto	style = MakeUnique<ColorStyle>();		// TODO: custom allocator
					result &= style->Deserialize( *this, resCache, des ) and
							  _styleMap.emplace( name, RVRef(style) ).second;	// throw
					break;
				}
				case EType::ImageStyle :
				{
					auto	style = MakeUnique<ImageStyle>();
					result &= style->Deserialize( *this, resCache, des ) and
							  _styleMap.emplace( name, RVRef(style) ).second;	// throw
					break;
				}
				case EType::ImageAnimationStyle :
				{
				//	auto	style = MakeUnique<ImageAnimationStyle>();
				//	result &= style->Deserialize( *this, resCache, des ) and
				//			  _styleMap.emplace( name, RVRef(style) ).second;	// throw
					result = false;
					break;
				}
				case EType::FontStyle :
				{
					auto	style = MakeUnique<FontStyle>();
					result &= style->Deserialize( *this, resCache, des ) and
							  _styleMap.emplace( name, RVRef(style) ).second;	// throw
					break;
				}

				case EType::_Count :
				default :
					result = false;		break;
			}
			switch_end
			ASSERT( result );
		}

		return result;
	}

/*
=================================================
	_DeserializeResources
=================================================
*/
	bool  StyleCollection::_DeserializeResources (Serializing::Deserializer	&des,
												  ResourceCache				&resCache,
												  ResourceUploadManager		&uploadMngr) __Th___
	{
		using namespace AE::AssetPacker;
		DRC_EXLOCK( _drCheckResources );

		for (;;)
		{
			Serializing::SerializedID	id;
			CHECK_ERR( des( OUT id ));

			switch ( uint{id} )
			{
				case uint{ImagePacker::SerID} :
				{
					CachedResourceName	name;
					CHECK_ERR( des( OUT name ) and name.IsDefined() );

					auto	img = LoadableImage::Loader::Load( des, null, resCache, uploadMngr, name );
					CHECK_ERR( img );

					switch ( img->GetImageDesc().format )
					{
						case EPixelFormat::R8_UNorm :
							CHECK_ERR( not _imageAlpha );
							_imageAlpha = RVRef(img);
							break;

						case EPixelFormat::RGBA8_UNorm :
							CHECK_ERR( not _imageRGBA );
							_imageRGBA = RVRef(img);
							break;

						default :
							RETURN_ERR( "unsupported pixel format" );
					}
					break;
				}

				case uint{ImageAtlasPacker::SerID} :
				{
					CachedResourceName	name;
					CHECK_ERR( des( OUT name ) and name.IsDefined() );

					auto	atlas = StaticImageAtlas::Loader::Load( des, resCache, name );
					CHECK_ERR( atlas );

					CHECK_ERR_MSG( AnyEqual( atlas->GetImageDesc().format, EPixelFormat::R8_UNorm, EPixelFormat::RGBA8_UNorm ),
						"unsupported pixel format" );
					break;
				}

				case uint{RasterFontPacker::SerID} :
				{
					CachedResourceName	name;
					CHECK_ERR( des( OUT name ) and name.IsDefined() );

					auto	font = RasterFont::Loader::Load( des, resCache, name );
					CHECK_ERR( font );

					CHECK_ERR_MSG( AnyEqual( font->GetImageDesc().format, EPixelFormat::R8_UNorm, EPixelFormat::RGBA8_UNorm ),
						"unsupported pixel format" );
					break;
				}

				case uint{SerID} :
					return true;  // continue in '_DeserializeStyles()'

				default:
					RETURN_ERR( "unsupported serialized object type" );
			}
		}
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  StyleCollection::Deinitialize () __NE___
	{
		DRC_EXLOCK( _drCheckGraphics );
		DRC_EXLOCK( _drCheckResources );
		DRC_EXLOCK( _drCheckStyles );

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		_rtech		= null;
		_imageRGBA	= null;
		_imageAlpha	= null;

		res_mngr.DelayedReleaseResources( _dynamicUBuf, _sharedDescSet );

		_styleMap.clear();
	}

} // AE::UI

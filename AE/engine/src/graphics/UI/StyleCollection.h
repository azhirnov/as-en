// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  yes (only const methods)
*/

#pragma once

#include "pch/VFS.h"
#include "graphics/UI/LayoutEnums.h"

namespace AE::UI
{

	//
	// Style Collection
	//

	class StyleCollection final
	{
		friend class SystemImpl;

	// types
	public:
		enum class EType : ubyte
		{
			ColorStyle,
			ImageStyle,
			FontStyle,
			ImageAnimationStyle,
			_Count
		};


		class IStyle
		{
		// types
		public:
			using UV_t = Rectangle<ushort>;

			struct UVScaleColor
			{
				UV_t		uv;
				float		scale	= 1.f;
				RGBA8u		color;

				UVScaleColor ()											__NE___ {}
				UVScaleColor (RGBA8u col)								__NE___ : color{col} {}
				UVScaleColor (RGBA8u col, float scale)					__NE___ : scale{scale}, color{col} {}
				UVScaleColor (UV_t uv, Pair<float,RGBA8u> scale_color)	__NE___ : uv{uv}, scale{scale_color.first}, color{scale_color.second} {}

				ND_ RectF  UV ()										C_NE___	{ return UNormShortToFloat( uv ); }
			};

		// variables
		public:
			GraphicsPipelineID		pipeline;

		// methods
		public:
				IStyle ()												__NE___ {}
				virtual ~IStyle ()										__NE___ {}
			ND_ virtual UVScaleColor  Get (EStyleIndex idx)				C_NE___ = 0;
			ND_ virtual bool  Deserialize (const StyleCollection &,
										   const Graphics::ResourceCache &,
										   Serializing::Deserializer &)	__NE___ = 0;
		};


		class ColorStyle final : public IStyle
		{
		// variables
		public:
			StaticArray< RGBA8u, uint(EStyleIndex::_Count) >	colors;

		// methods
		public:
			ColorStyle () __NE___ {}
			UVScaleColor  Get (EStyleIndex idx)		C_NE_OV	{ return UVScaleColor{ colors[ uint(idx) ] }; }
			bool  Deserialize (const StyleCollection &, const Graphics::ResourceCache &, Serializing::Deserializer &) __NE_OV;
		};


		class ImageStyle final : public IStyle
		{
		// variables
		public:
			UV_t															uv;
			StaticArray< Pair<float, RGBA8u>, uint(EStyleIndex::_Count) >	scale_color;

		// methods
		public:
			ImageStyle () __NE___ {}
			UVScaleColor  Get (EStyleIndex idx)		C_NE_OV	{ return UVScaleColor{ uv, scale_color[ uint(idx) ]}; }
			bool  Deserialize (const StyleCollection &, const Graphics::ResourceCache &, Serializing::Deserializer &) __NE_OV;
		};


		class ImageAnimationStyle final : public IStyle
		{
		// variables
		public:
			StaticArray< UVScaleColor, uint(EStyleIndex::_Count) >		uv_scale_color;

		// methods
		public:
			ImageAnimationStyle () __NE___ {}
			UVScaleColor  Get (EStyleIndex idx)		C_NE_OV	{ return uv_scale_color[ uint(idx) ]; }
			bool  Deserialize (const StyleCollection &, const Graphics::ResourceCache &, Serializing::Deserializer &) __NE_OV;
		};


		/*class FontStyle final : public IStyle
		{
		// variables
		public:
			RC<RasterFont>		font;

		// methods
		public:
			FontStyle () __NE___ {}
			UVScaleColor  Get (EStyleIndex idx)		C_NE_OV;
			bool  Deserialize (const StyleCollection &, const Graphics::ResourceCache &, Serializing::Deserializer &) __NE_OV;
		};*/


		struct AnimationSettings
		{
			float			colorAnimSpeed;		// 1 / seconds

			AnimationSettings () __NE___;
		};

		static constexpr uint	MaxScreens	= 32;	// TODO: minimize
		static constexpr auto	SerID		= Serializing::SerializedID::Optimized_t{"UIStyle"};

		struct CreateInfoAsync
		{
			Promise<RenderTechPipelinesPtr>				rtech;
			RC<RStream>									stream;
			Ref<Graphics::ResourceCache>				resCache;
			Ref<Graphics::ResourceUploadManager>		uploadMngr;
			Bytes32u									ubSize;
			Graphics::ImageViewID						dummyImage;
		};

		struct CreateInfo
		{
			RenderTechPipelinesPtr						rtech;
			RC<RStream>									stream;
			Ref<Graphics::ResourceCache>				resCache;
			Ref<Graphics::ResourceUploadManager>		uploadMngr;
			Bytes32u									ubSize;
			Graphics::ImageViewID						dummyImage;
		};

		struct UnsafeSetter
		{
			static void  SetAnimSpeed (StyleCollection &, float value);		// in 1/s
		};

	private:
		using StyleMap_t	= FlatHashMap< StyleName::Optimized_t, Unique<IStyle> >;
		using FontCache_t	= FixedMap< VFS::FileName::Optimized_t, RC<RasterFont>, 8 >;


	// variables
	private:
		RenderTechPipelinesPtr		_rtech;

		StyleMap_t					_styleMap;
		AnimationSettings			_settings;

		Strong<DescriptorSetID>		_sharedDescSet;
		Strong<BufferID>			_dynamicUBuf;
		Bytes16u					_dynamicUBufSize;

		GraphicsPipelineID			_dbgPpln;

		RC<LoadableImage>			_imageRGBA;
		RC<LoadableImage>			_imageAlpha;

		DRC_ONLY(
			Threading::RWDataRaceCheck	_drCheckGraphics;
			Threading::RWDataRaceCheck	_drCheckResources;
			Threading::RWDataRaceCheck	_drCheckStyles;
		)


	// methods
	public:
		StyleCollection ()													__NE___;
		~StyleCollection ()													__NE___;

		ND_ AsyncTask	InitializeAsync (CreateInfoAsync &ci)				__NE___;
		ND_ bool		Initialize (const CreateInfo &ci)					__NE___;
			void		Deinitialize ()										__NE___;

		ND_ auto		GetStyle (StyleName::Ref id)						C_NE___ -> Ptr<const IStyle>;

	//	ND_ Ptr<const FontStyle>	GetFontStyle (StyleName::Ref id)		C_NE___;
		ND_ GraphicsPipelineID		GetDebugDrawPipeline ()					C_NE___	{ return _dbgPpln; }

		ND_ auto const&				GetSettings ()							C_NE___	{ return _settings; }
		ND_ auto const&				GetRTech ()								C_NE___	{ return *_rtech; }


	private:
		ND_ bool  _DeserializeStyles (Serializing::Deserializer &,
									  Graphics::ResourceCache &)			__Th___;

		ND_ bool  _DeserializeResources (Serializing::Deserializer &,
										 Graphics::ResourceCache &,
										 Graphics::ResourceUploadManager &)	__Th___;

		ND_ bool  _InitGraphics (RenderTechPipelinesPtr	rtech,
								 Graphics::ImageViewID	dummyImage,
								 Bytes					ubSize)				__NE___;
	};


} // AE::UI

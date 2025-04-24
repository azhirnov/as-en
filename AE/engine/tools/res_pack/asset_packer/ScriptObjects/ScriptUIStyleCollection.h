// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

#include "graphics/UI/StyleCollection.h"

namespace AE::AssetPacker
{

	//
	// Script UI Style Collection
	//

	class ScriptUIStyleCollection final : public EnableScriptRC
	{
	// types
	public:
		using StyleCollection	= UI::StyleCollection;
		using ImageAtlasInfo	= ObjectStorage::ImageAtlasInfo;
		using EType				= StyleCollection::EType;


		class BaseStyle : public EnableScriptRC
		{
		protected:
			ScriptUIStyleCollection &		_collection;

		public:
			explicit BaseStyle (ScriptUIStyleCollection &c) : _collection{c} {}

			ND_ virtual bool  Serialize (Serializing::Serializer &)			C_NE___ = 0;
		};


		//
		// Color Style
		//
		class ScriptColorStyle final : public BaseStyle
		{
		private:
			Graphics::PipelineName			_pplnName;
			StyleCollection::ColorStyle		_internal;

		public:
			explicit ScriptColorStyle (ScriptUIStyleCollection &c) : BaseStyle{c} {}

			void  SetPipeline  (const String &name)							__Th___;
			void  SetDisabled  (const RGBA8u &col)							__Th___;
			void  SetEnabled   (const RGBA8u &col)							__Th___;
			void  SetMouseOver (const RGBA8u &col)							__Th___;
			void  SetTouchDown (const RGBA8u &col)							__Th___;
			void  SetSelected  (const RGBA8u &col)							__Th___;

			bool  Serialize (Serializing::Serializer &)						C_NE___;

			static void  Bind (const ScriptEnginePtr &se)					__Th___;
		};
		using ScriptColorStylePtr = ScriptRC< ScriptColorStyle >;


		//
		// Image Style
		//
		class ScriptImageStyle final : public BaseStyle
		{
		private:
			Graphics::PipelineName			_pplnName;
			Graphics::CachedResourceName	_atlasName;
			Graphics::ImageInAtlasName		_imageName;
			StyleCollection::ImageStyle		_internal;

		public:
			explicit ScriptImageStyle (ScriptUIStyleCollection &c) : BaseStyle{c} {}

			void  SetImage      (const String &metaRes, const String &name)	__Th___;
			void  SetPipeline   (const String &name)						__Th___;

			void  SetDisabled1  (const RGBA8u &col)							__Th___;
			void  SetEnabled1   (const RGBA8u &col)							__Th___;
			void  SetMouseOver1 (const RGBA8u &col)							__Th___;
			void  SetTouchDown1 (const RGBA8u &col)							__Th___;
			void  SetSelected1  (const RGBA8u &col)							__Th___;

			void  SetDisabled2  (const RGBA8u &col, float scale)			__Th___;
			void  SetEnabled2   (const RGBA8u &col, float scale)			__Th___;
			void  SetMouseOver2 (const RGBA8u &col, float scale)			__Th___;
			void  SetTouchDown2 (const RGBA8u &col, float scale)			__Th___;
			void  SetSelected2  (const RGBA8u &col, float scale)			__Th___;

			bool  Serialize (Serializing::Serializer &)						C_NE___;

			static void  Bind (const ScriptEnginePtr &se)					__Th___;
		};
		using ScriptImageStylePtr = ScriptRC< ScriptImageStyle >;


		//
		// Image Animation Style
		//
		class ScriptImageAnimationStyle final : public BaseStyle
		{
		private:
			Graphics::PipelineName					_pplnName;
			Graphics::CachedResourceName			_atlasName;
			StyleCollection::ImageAnimationStyle	_internal;

		public:
			explicit ScriptImageAnimationStyle (ScriptUIStyleCollection &c) : BaseStyle{c} {}

			void  SetPipeline  (const String &name)										__Th___;
			void  SetAtlas     (const String &metaRes)									__Th___;

			void  SetDisabled1  (const RGBA8u &col, const String &imgName)				__Th___;
			void  SetEnabled1   (const RGBA8u &col, const String &imgName)				__Th___;
			void  SetMouseOver1 (const RGBA8u &col, const String &imgName)				__Th___;
			void  SetTouchDown1 (const RGBA8u &col, const String &imgName)				__Th___;
			void  SetSelected1  (const RGBA8u &col, const String &imgName)				__Th___;

			void  SetDisabled2  (const RGBA8u &col, const String &imgName, float scale)	__Th___;
			void  SetEnabled2   (const RGBA8u &col, const String &imgName, float scale)	__Th___;
			void  SetMouseOver2 (const RGBA8u &col, const String &imgName, float scale)	__Th___;
			void  SetTouchDown2 (const RGBA8u &col, const String &imgName, float scale)	__Th___;
			void  SetSelected2  (const RGBA8u &col, const String &imgName, float scale)	__Th___;

			bool  Serialize (Serializing::Serializer &)									C_NE___;

			static void  Bind (const ScriptEnginePtr &se)								__Th___;
		};
		using ScriptImageAnimationStylePtr = ScriptRC< ScriptImageAnimationStyle >;


		//
		// Font Style
		//
	/*	class ScriptFontStyle final : public BaseStyle
		{
		private:
			StyleCollection::FontStyle		_internal;
			String							_fontName;

		public:
			explicit ScriptFontStyle (ScriptUIStyleCollection &c) : BaseStyle{c} {}

			void  SetDisabled  (const RGBA8u &col)							__Th___;
			void  SetEnabled   (const RGBA8u &col)							__Th___;
			void  SetMouseOver (const RGBA8u &col)							__Th___;
			void  SetTouchDown (const RGBA8u &col)							__Th___;
			void  SetSelected  (const RGBA8u &col)							__Th___;
			void  SetFont      (const String &fontName)						__Th___;
			void  SetPipeline  (const String &name)							__Th___;

			bool  Serialize (Serializing::Serializer &)						C_NE___;

			static void  Bind (const ScriptEnginePtr &se)					__Th___;
		};
		using ScriptFontStylePtr = ScriptRC< ScriptFontStyle >;
		*/

	private:
		using StyleMap_t	= HashMap< String, ScriptRC<BaseStyle> >;


	// variables
	private:
		StyleMap_t				_styleMap;

		String					_metaDataFileName;

		Graphics::PipelineName	_dbgPplnName;


	// methods
	public:
		ScriptUIStyleCollection ();
		~ScriptUIStyleCollection ();

		void  SetResources (const String &metaArchive)						__Th___;
		void  SetDebugPipeline (const String &pplnName)						__Th___;

		ScriptColorStyle*	AddColorStyle (const String &name)				__Th___;
		ScriptImageStyle*	AddImageStyle (const String &name)				__Th___;
	//	ScriptFontStyle*	AddFontStyle (const String &name)				__Th___;

		void  Store (const String &nameInArchive)							__Th___;

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

	private:
		void  _Pack (RC<WStream> stream)									__Th___;
		void  _CheckPipeline (const String &name)							__Th___;
		void  _CheckFont (const String &name)								__Th___;
	};

	using ScriptUIStyleCollectionPtr = ScriptRC< ScriptUIStyleCollection >;


} // AE::AssetPacker

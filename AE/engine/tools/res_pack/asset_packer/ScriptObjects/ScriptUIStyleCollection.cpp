// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_pack/asset_packer/ScriptObjects/ScriptUIStyleCollection.h"

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection,								"UIStyleCollection" );
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptColorStyle,				"UIColorStyle" );
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptImageStyle,				"UIImageStyle" );
//AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptImageAnimationStyle,	"UIImageAnimationStyle" );
//AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptFontStyle,			"UIFontStyle" );


namespace AE::AssetPacker
{
	using Graphics::CachedResourceName;
	using Graphics::ImageInAtlasName;
	using Graphics::PipelineName;
	using UI::EStyleIndex;
	using UI::StyleName;


/*
=================================================
	Set*
=================================================
*/
	void  ScriptUIStyleCollection::ScriptColorStyle::SetDisabled (const RGBA8u &col)	__Th___	{ _internal.colors[uint(EStyleIndex::Disabled)]	 = col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetEnabled (const RGBA8u &col)		__Th___	{ _internal.colors[uint(EStyleIndex::Enabled)]	 = col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetMouseOver (const RGBA8u &col)	__Th___	{ _internal.colors[uint(EStyleIndex::MouseOver)] = col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetTouchDown (const RGBA8u &col)	__Th___	{ _internal.colors[uint(EStyleIndex::TouchDown)] = col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetSelected (const RGBA8u &col)	__Th___	{ _internal.colors[uint(EStyleIndex::Selected)]	 = col; }

	void  ScriptUIStyleCollection::ScriptColorStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_pplnName = PipelineName{name};
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  ScriptUIStyleCollection::ScriptColorStyle::Serialize (Serializing::Serializer &ser) C_NE___
	{
		const EType  type = EType::ColorStyle;
		return ser( type, _pplnName, _internal.colors );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIStyleCollection::ScriptColorStyle::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptColorStyle>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		AS_METHOD( binder, ScriptColorStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled"} );
		AS_METHOD( binder, ScriptColorStyle::SetEnabled,	"Enabled",		{"colorWhenEnabled"} );
		AS_METHOD( binder, ScriptColorStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver"} );
		AS_METHOD( binder, ScriptColorStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown"} );
		AS_METHOD( binder, ScriptColorStyle::SetSelected,	"Selected",		{"colorWhenSelected"} );
		AS_METHOD( binder, ScriptColorStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Set*
=================================================
*/
	void  ScriptUIStyleCollection::ScriptImageStyle::SetDisabled1 (const RGBA8u &col)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Disabled)]  = MakePair( 1.f, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetEnabled1 (const RGBA8u &col)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Enabled)]   = MakePair( 1.f, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetMouseOver1 (const RGBA8u &col)	__Th___	{ _internal.scale_color[uint(EStyleIndex::MouseOver)] = MakePair( 1.f, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetTouchDown1 (const RGBA8u &col)	__Th___	{ _internal.scale_color[uint(EStyleIndex::TouchDown)] = MakePair( 1.f, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetSelected1 (const RGBA8u &col)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Selected)]  = MakePair( 1.f, col ); }

	void  ScriptUIStyleCollection::ScriptImageStyle::SetDisabled2 (const RGBA8u &col, float scale)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Disabled)]  = MakePair( scale, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetEnabled2 (const RGBA8u &col, float scale)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Enabled)]   = MakePair( scale, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetMouseOver2 (const RGBA8u &col, float scale)	__Th___	{ _internal.scale_color[uint(EStyleIndex::MouseOver)] = MakePair( scale, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetTouchDown2 (const RGBA8u &col, float scale)	__Th___	{ _internal.scale_color[uint(EStyleIndex::TouchDown)] = MakePair( scale, col ); }
	void  ScriptUIStyleCollection::ScriptImageStyle::SetSelected2 (const RGBA8u &col, float scale)	__Th___	{ _internal.scale_color[uint(EStyleIndex::Selected)]  = MakePair( scale, col ); }

	void  ScriptUIStyleCollection::ScriptImageStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_pplnName = PipelineName{name};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetImage (const String &metaRes, const String &name) __Th___
	{
		auto&	storage	= *ObjectStorage::Instance();
		auto	atlas	= storage.GetAtlas( _collection._metaDataFileName, metaRes );  // throw

		atlas->Contains( name );

		storage.AddName< CachedResourceName >( metaRes );	// throw
		storage.AddName< ImageInAtlasName >( name );		// throw

		_atlasName = CachedResourceName{metaRes};
		_imageName = ImageInAtlasName{name};
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  ScriptUIStyleCollection::ScriptImageStyle::Serialize (Serializing::Serializer &ser) C_NE___
	{
		const EType  type = EType::ImageStyle;
		return ser( type, _pplnName, _atlasName, _imageName, _internal.scale_color );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIStyleCollection::ScriptImageStyle::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptImageStyle>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		AS_METHOD( binder, ScriptImageStyle::SetDisabled1,	"Disabled",		{"colorWhenDisabled"} );
		AS_METHOD( binder, ScriptImageStyle::SetEnabled1,	"Enabled",		{"colorWhenEnabled"} );
		AS_METHOD( binder, ScriptImageStyle::SetMouseOver1,	"MouseOver",	{"colorWhenMouseOver"} );
		AS_METHOD( binder, ScriptImageStyle::SetTouchDown1,	"TouchDown",	{"colorWhenTouchDown"} );
		AS_METHOD( binder, ScriptImageStyle::SetSelected1,	"Selected",		{"colorWhenSelected"} );

		AS_METHOD( binder, ScriptImageStyle::SetDisabled2,	"Disabled",		{"colorWhenDisabled", "scale"} );
		AS_METHOD( binder, ScriptImageStyle::SetEnabled2,	"Enabled",		{"colorWhenEnabled", "scale"} );
		AS_METHOD( binder, ScriptImageStyle::SetMouseOver2,	"MouseOver",	{"colorWhenMouseOver", "scale"} );
		AS_METHOD( binder, ScriptImageStyle::SetTouchDown2,	"TouchDown",	{"colorWhenTouchDown", "scale"} );
		AS_METHOD( binder, ScriptImageStyle::SetSelected2,	"Selected",		{"colorWhenSelected", "scale"} );

		AS_METHOD( binder, ScriptImageStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
		AS_METHOD( binder, ScriptImageStyle::SetImage,		"Image",		{"atlasMetaResName", "imageInAtlas"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Set*
=================================================
*
	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetAtlas (const String &metaRes) __Th___
	{
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetDisabled (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.disabled	= col;
		_internal.image.disabled	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetEnabled (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.enabled		= col;
		_internal.image.enabled		= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetMouseOver (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.mouseOver	= col;
		_internal.image.mouseOver	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetTouchDown (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.touchDown	= col;
		_internal.image.touchDown	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetSelected (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.selected	= col;
		_internal.image.selected	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_internal._pplnName = PipelineName{name};
	}

/*
=================================================
	Serialize
=================================================
*
	bool  ScriptUIStyleCollection::ScriptImageAnimationStyle::Serialize (Serializing::Serializer &ser) C_NE___
	{
	}

/*
=================================================
	Bind
=================================================
*
	void  ScriptUIStyleCollection::ScriptImageAnimationStyle::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptImageAnimationStyle>	binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled", "imageNameInAtlas"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetEnabled,	"Enabled",		{"colorWhenEnabled", "imageNameInAtlas"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver", "imageNameInAtlas"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown", "imageNameInAtlas"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetSelected,	"Selected",		{"colorWhenSelected", "imageNameInAtlas"} );
		AS_METHOD( binder, ScriptImageAnimationStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Set*
=================================================
*
	void  ScriptUIStyleCollection::ScriptFontStyle::SetDisabled (const RGBA8u &col)		__Th___	{ _internal.color.disabled	= col; }
	void  ScriptUIStyleCollection::ScriptFontStyle::SetEnabled (const RGBA8u &col)		__Th___	{ _internal.color.enabled	= col; }
	void  ScriptUIStyleCollection::ScriptFontStyle::SetMouseOver (const RGBA8u &col)	__Th___	{ _internal.color.mouseOver	= col; }
	void  ScriptUIStyleCollection::ScriptFontStyle::SetTouchDown (const RGBA8u &col)	__Th___	{ _internal.color.touchDown	= col; }
	void  ScriptUIStyleCollection::ScriptFontStyle::SetSelected (const RGBA8u &col)		__Th___	{ _internal.color.selected	= col; }

	void  ScriptUIStyleCollection::ScriptFontStyle::SetFont (const String &fontName)	__Th___
	{
		ObjectStorage::Instance()->RequireFont( fontName );

		_fontName = fontName;
	}

	void  ScriptUIStyleCollection::ScriptFontStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_internal._pplnName = PipelineName{name};
	}

/*
=================================================
	Serialize
=================================================
*
	bool  ScriptUIStyleCollection::ScriptFontStyle::Serialize (Serializing::Serializer &ser) C_NE___
	{
		// TODO: validate

		CHECK_THROW( Serialize_FontStyle( _internal, ser ));
	}

/*
=================================================
	Bind
=================================================
*
	void  ScriptUIStyleCollection::ScriptFontStyle::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptFontStyle>		binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		AS_METHOD( binder, ScriptFontStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled"} );
		AS_METHOD( binder, ScriptFontStyle::SetEnabled,		"Enabled",		{"colorWhenEnabled"} );
		AS_METHOD( binder, ScriptFontStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver"} );
		AS_METHOD( binder, ScriptFontStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown"} );
		AS_METHOD( binder, ScriptFontStyle::SetSelected,	"Selected",		{"colorWhenSelected"} );
		AS_METHOD( binder, ScriptFontStyle::SetFont,		"Font",			{"fontName"} );
		AS_METHOD( binder, ScriptFontStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptUIStyleCollection::ScriptUIStyleCollection ()
	{}

	ScriptUIStyleCollection::~ScriptUIStyleCollection ()
	{
		CHECK_MSG( _styleMap.empty(),
			"call 'UIStyleCollection::Store()' to store ui style to archive" );
	}

/*
=================================================
	AddColorStyle
=================================================
*/
	ScriptUIStyleCollection::ScriptColorStyle*  ScriptUIStyleCollection::AddColorStyle (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<StyleName>( name );

		ScriptColorStylePtr		result{ new ScriptColorStyle{ *this }};

		CHECK_THROW_MSG( _styleMap.emplace( name, result ).second,
			"Style '"s << name << "' is already exists" );

		return result.Detach();
	}

/*
=================================================
	AddImageStyle
=================================================
*/
	ScriptUIStyleCollection::ScriptImageStyle*  ScriptUIStyleCollection::AddImageStyle (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<StyleName>( name );

		ScriptImageStylePtr		result{ new ScriptImageStyle{ *this }};

		CHECK_THROW_MSG( _styleMap.emplace( name, result ).second,
			"Style '"s << name << "' is already exists" );

		return result.Detach();
	}

/*
=================================================
	AddFontStyle
=================================================
*
	ScriptUIStyleCollection::ScriptFontStyle*  ScriptUIStyleCollection::AddFontStyle (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<StyleName>( name );

		ScriptFontStylePtr		result{ new ScriptFontStyle{ *this }};

		CHECK_THROW_MSG( _styleMap.emplace( name, result ).second,
			"Style '"s << name << "' is already exists" );

		return result.Detach();
	}

/*
=================================================
	Store
=================================================
*/
	void  ScriptUIStyleCollection::Store (const String &nameInArchive) __Th___
	{
		{
			auto	wmem = MakeRC<ArrayWStream>();
			try{
				_Pack( wmem );  // throw
			}catch(...) {
				CHECK_THROW_MSG( false,
					"failed to serialize UI style collection '"s << nameInArchive << "'" );
			}

			MemRefRStream	rmem {wmem->GetData()};
			ObjectStorage::Instance()->AddToArchive( nameInArchive, rmem, EArchivePackerFileType::InMemory );  // throw
		}

		_styleMap.clear();
	}

/*
=================================================
	_Pack
=================================================
*/
	void  ScriptUIStyleCollection::_Pack (RC<WStream> stream) __Th___
	{
		CHECK_THROW( not _metaDataFileName.empty() );

		// copy resource meta data
		{
			auto	res_meta_data = ObjectStorage::Instance()->ExtractFromArchive( _metaDataFileName );  // throw

			CHECK_THROW( DataSourceUtils::BufferedCopy( *stream, *res_meta_data ) == res_meta_data->Size() );
		}

		Serializing::Serializer		ser {stream};

		CHECK_THROW( ser( UI::StyleCollection::SerID ));

		// serialize debug pipeline
		{
			CHECK_THROW( ser( _dbgPplnName ));
		}

		// serialize style map
		CHECK_THROW( ser( uint(_styleMap.size()) ));

		for (auto& [name, base] : _styleMap)
		{
			CHECK_THROW_MSG( ser( StyleName::Optimized_t{name} ) and base->Serialize( ser ),
				"Failed to serialize UI Style '"s << name << "'" );
		}
	}

/*
=================================================
	_CheckPipeline
=================================================
*/
	void  ScriptUIStyleCollection::_CheckPipeline (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<PipelineName>( name );  // throw
	}

/*
=================================================
	_CheckFont
=================================================
*/
	void  ScriptUIStyleCollection::_CheckFont (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _metaDataFileName.empty(), "Not defined meta data file with UI resources" );

		ObjectStorage::Instance()->RequireFont( _metaDataFileName, name );  // throw
	}

/*
=================================================
	SetResources
=================================================
*/
	void  ScriptUIStyleCollection::SetResources (const String &metaArchive) __Th___
	{
		CHECK_THROW_MSG( _metaDataFileName.empty(), "already defined" );

		_metaDataFileName = metaArchive;
	}

/*
=================================================
	SetDebugPipeline
=================================================
*/
	void  ScriptUIStyleCollection::SetDebugPipeline (const String &pplnName) __Th___
	{
		_CheckPipeline( pplnName );  // throw

		_dbgPplnName = PipelineName{pplnName};
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIStyleCollection::Bind (const ScriptEnginePtr &se) __Th___
	{
		ScriptColorStyle::Bind( se );
		ScriptImageStyle::Bind( se );
		//ScriptImageAnimationStyle::Bind( se );
		//ScriptFontStyle::Bind( se );

		Scripting::ClassBinder<ScriptUIStyleCollection>		binder{ se };
		binder.CreateRef();
		AS_METHOD( binder, ScriptUIStyleCollection::SetResources,			"Resources",				{"metaDataFileNameInArchive"} );
		AS_METHOD( binder, ScriptUIStyleCollection::SetDebugPipeline,		"DebugPipeline",			{"pplnName"} );
		AS_METHOD( binder, ScriptUIStyleCollection::AddColorStyle,			"AddColorStyle",			{"name"} );
		AS_METHOD( binder, ScriptUIStyleCollection::AddImageStyle,			"AddImageStyle",			{"name"} );
	//	AS_METHOD( binder, ScriptUIStyleCollection::AddImageAnimationStyle,	"AddImageAnimationStyle",	{"name"} );
	//	AS_METHOD( binder, ScriptUIStyleCollection::AddFontStyle,			"AddFontStyle",				{"name"} );
		AS_METHOD( binder, ScriptUIStyleCollection::Store,					"Store",					{"nameInArchive"} );
	}


} // AE::AssetPacker

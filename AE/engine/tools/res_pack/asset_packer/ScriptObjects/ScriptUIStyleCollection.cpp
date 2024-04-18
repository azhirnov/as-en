// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptUIStyleCollection.h"
#include "graphics_hl/UI/StyleCollection.cpp.h"

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection,					"UIStyleCollection" );
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptColorStyle,	"UIColorStyle" );
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptImageStyle,	"UIImageStyle" );
AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptUIStyleCollection::ScriptFontStyle,	"UIFontStyle" );


namespace AE::AssetPacker
{
	using Graphics::ImageInAtlasName;
	using Graphics::PipelineName;
	using UI::StyleName;

/*
=================================================
	Set*
=================================================
*/
	void  ScriptUIStyleCollection::ScriptColorStyle::SetDisabled (const RGBA8u &col)	__Th___	{ _internal.color.disabled	= col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetEnabled (const RGBA8u &col)		__Th___	{ _internal.color.enabled	= col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetMouseOver (const RGBA8u &col)	__Th___	{ _internal.color.mouseOver	= col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetTouchDown (const RGBA8u &col)	__Th___	{ _internal.color.touchDown	= col; }
	void  ScriptUIStyleCollection::ScriptColorStyle::SetSelected (const RGBA8u &col)	__Th___	{ _internal.color.selected	= col; }

	void  ScriptUIStyleCollection::ScriptColorStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_internal._pplnName = PipelineName{name};
	}

/*
=================================================
	Serialize
=================================================
*/
	void  ScriptUIStyleCollection::ScriptColorStyle::Serialize (Serializing::Serializer &ser) __Th___
	{
		// TODO: validate

		CHECK_THROW( Serialize_ColorStyle( _internal, ser ));
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
		binder.AddMethod( &ScriptColorStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled"} );
		binder.AddMethod( &ScriptColorStyle::SetEnabled,	"Enabled",		{"colorWhenEnabled"} );
		binder.AddMethod( &ScriptColorStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver"} );
		binder.AddMethod( &ScriptColorStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown"} );
		binder.AddMethod( &ScriptColorStyle::SetSelected,	"Selected",		{"colorWhenSelected"} );
		binder.AddMethod( &ScriptColorStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Set*
=================================================
*/
	void  ScriptUIStyleCollection::ScriptImageStyle::SetDisabled (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.disabled	= col;
		_internal.image.disabled	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetEnabled (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.enabled		= col;
		_internal.image.enabled		= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetMouseOver (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.mouseOver	= col;
		_internal.image.mouseOver	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetTouchDown (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.touchDown	= col;
		_internal.image.touchDown	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetSelected (const RGBA8u &col, const String &imgName) __Th___
	{
		_collection._CheckImageInAtlas( imgName );

		_internal.color.selected	= col;
		_internal.image.selected	= ImageInAtlasName::Optimized_t{imgName};
	}

	void  ScriptUIStyleCollection::ScriptImageStyle::SetPipeline (const String &name)	__Th___
	{
		_collection._CheckPipeline( name );
		_internal._pplnName = PipelineName{name};
	}

/*
=================================================
	Serialize
=================================================
*/
	void  ScriptUIStyleCollection::ScriptImageStyle::Serialize (Serializing::Serializer &ser) __Th___
	{
		// TODO: validate

		CHECK_THROW( Serialize_ImageStyle( _internal, ser ));
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
		binder.AddMethod( &ScriptImageStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled", "imageNameInAtlas"} );
		binder.AddMethod( &ScriptImageStyle::SetEnabled,	"Enabled",		{"colorWhenEnabled", "imageNameInAtlas"} );
		binder.AddMethod( &ScriptImageStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver", "imageNameInAtlas"} );
		binder.AddMethod( &ScriptImageStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown", "imageNameInAtlas"} );
		binder.AddMethod( &ScriptImageStyle::SetSelected,	"Selected",		{"colorWhenSelected", "imageNameInAtlas"} );
		binder.AddMethod( &ScriptImageStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Set*
=================================================
*/
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
*/
	void  ScriptUIStyleCollection::ScriptFontStyle::Serialize (Serializing::Serializer &ser) __Th___
	{
		// TODO: validate

		CHECK_THROW( Serialize_FontStyle( _internal, ser ));
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptUIStyleCollection::ScriptFontStyle::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptFontStyle>		binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
		binder.AddMethod( &ScriptFontStyle::SetDisabled,	"Disabled",		{"colorWhenDisabled"} );
		binder.AddMethod( &ScriptFontStyle::SetEnabled,		"Enabled",		{"colorWhenEnabled"} );
		binder.AddMethod( &ScriptFontStyle::SetMouseOver,	"MouseOver",	{"colorWhenMouseOver"} );
		binder.AddMethod( &ScriptFontStyle::SetTouchDown,	"TouchDown",	{"colorWhenTouchDown"} );
		binder.AddMethod( &ScriptFontStyle::SetSelected,	"Selected",		{"colorWhenSelected"} );
		binder.AddMethod( &ScriptFontStyle::SetFont,		"Font",			{"fontName"} );
		binder.AddMethod( &ScriptFontStyle::SetPipeline,	"Pipeline",		{"pplnName"} );
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
*/
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
			auto	wmem = MakeRC<MemWStream>();
			try{
				_Pack( wmem );  // throw
			}catch(...) {
				CHECK_THROW_MSG( false,
					"failed to serialize UI style collection '"s << nameInArchive << "'" );
			}

			auto	rmem = wmem->ToRStream();
			ObjectStorage::Instance()->AddToArchive( nameInArchive, *rmem, EArchivePackerFileType::InMemory );  // throw
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
		Serializing::Serializer		ser {stream};

		// serialize atlas name
		{
			VFS::FileName	name;

			if ( _imageAtlas )
				name = VFS::FileName{_imageAtlas->Name()};

			CHECK_THROW( ser( name ));
		}

		// serialize debug pipeline
		{
			CHECK_THROW( ser( _dbgPplnName ));
		}

		// serialize style map
		CHECK_THROW( ser( uint(_styleMap.size()) ));

		for (auto& [name, base] : _styleMap)
		{
			CHECK_THROW( ser( StyleName::Optimized_t{name} ));

			if ( auto* color_style = DynCast<ScriptColorStyle>( base.Get() ))
			{
				CHECK_THROW( ser( EType::ColorStyle ));
				color_style->Serialize( ser );  // throw
			}
			else
			if ( auto* image_style = DynCast<ScriptImageStyle>( base.Get() ))
			{
				CHECK_THROW( ser( EType::ImageStyle ));
				image_style->Serialize( ser );  // throw
			}
			else
			if ( auto* font_style = DynCast<ScriptFontStyle>( base.Get() ))
			{
				CHECK_THROW( ser( EType::FontStyle ));
				font_style->Serialize( ser );  // throw
			}
			else
				CHECK_THROW_MSG( false, "unsupported style: '"s << name << "'" );
		}
	}

/*
=================================================
	_CheckImageInAtlas
=================================================
*/
	void  ScriptUIStyleCollection::_CheckImageInAtlas (const String &imgName) __Th___
	{
		CHECK_THROW_MSG( _imageAtlas );
		_imageAtlas->Contains( imgName );
		ObjectStorage::Instance()->AddName<ImageInAtlasName>( imgName );
	}

/*
=================================================
	_CheckPipeline
=================================================
*/
	void  ScriptUIStyleCollection::_CheckPipeline (const String &name) __Th___
	{
		ObjectStorage::Instance()->AddName<PipelineName>( name );
	}

/*
=================================================
	SetAtlas
=================================================
*/
	void  ScriptUIStyleCollection::SetAtlas (const String &atlasName) __Th___
	{
		_imageAtlas = ObjectStorage::Instance()->GetAtlas( atlasName );  // throw
	}

/*
=================================================
	SetDebugPipeline
=================================================
*/
	void  ScriptUIStyleCollection::SetDebugPipeline (const String &pplnName) __Th___
	{
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
		ScriptFontStyle::Bind( se );

		Scripting::ClassBinder<ScriptUIStyleCollection>		binder{ se };
		binder.CreateRef();
		binder.AddMethod( &ScriptUIStyleCollection::SetAtlas,			"Atlas",			{"atlasName"} );
		binder.AddMethod( &ScriptUIStyleCollection::SetDebugPipeline,	"DebugPipeline",	{"pplnName"} );
		binder.AddMethod( &ScriptUIStyleCollection::AddColorStyle,		"AddColorStyle",	{"name"} );
		binder.AddMethod( &ScriptUIStyleCollection::AddImageStyle,		"AddImageStyle",	{"name"} );
		binder.AddMethod( &ScriptUIStyleCollection::AddFontStyle,		"AddFontStyle",		{"name"} );
		binder.AddMethod( &ScriptUIStyleCollection::Store,				"Store",			{"nameInArchive"} );
	}


} // AE::AssetPacker

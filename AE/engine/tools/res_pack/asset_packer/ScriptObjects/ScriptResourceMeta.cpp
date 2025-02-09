// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptResourceMeta.h"
#include "ScriptObjects/ScriptTexture.h"
#include "ScriptObjects/ScriptImageAtlas.h"
#include "ScriptObjects/ScriptRasterFont.h"

#include "Packer/ImageAtlasPacker.h"

namespace AE::AssetPacker
{

/*
=================================================
	constructor / destructor
=================================================
*/
	ScriptResourceMeta::ScriptResourceMeta ()
	{}

	ScriptResourceMeta::~ScriptResourceMeta ()
	{
		CHECK( _uniqueNames.empty() );
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptResourceMeta::Add (ScriptImageAtlas &atlas, const String &name) __Th___
	{
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second,
			"Resource meta data with name '"s << name << "' already exists" );

		CHECK_THROW_MSG( _atlases.emplace( ScriptImageAtlasPtr{&atlas}, name ).second,
			"Atlas '"s << name << "' already exists" );

		ObjectStorage::Instance()->AddName< Graphics::CachedResourceName >( name );  // throw
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptResourceMeta::Add (ScriptRasterFont &font, const String &name) __Th___
	{
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second,
			"Resource meta data with name '"s << name << "' already exists" );

		CHECK_THROW_MSG( _fonts.emplace( ScriptRasterFontPtr{&font}, name ).second,
			"Font '"s << name << "' already exists" );

		ObjectStorage::Instance()->AddName< Graphics::CachedResourceName >( name );  // throw
	}

/*
=================================================
	Add
=================================================
*/
	void  ScriptResourceMeta::Add (ScriptTexture &image, const String &name) __Th___
	{
		CHECK_THROW_MSG( _uniqueNames.insert( name ).second,
			"Resource meta data with name '"s << name << "' already exists" );

		CHECK_THROW_MSG( _images.emplace( ScriptTexturePtr{&image}, name ).second,
			"Image '"s << name << "' already exists" );

		ObjectStorage::Instance()->AddName< Graphics::CachedResourceName >( name );  // throw
	}

/*
=================================================
	Store
=================================================
*/
	void  ScriptResourceMeta::Store (const String &nameInArchive) __Th___
	{
		StaticAssert( IsTriviallySerializable< Serializing::SerializedID::Optimized_t >);
		CHECK_THROW( not nameInArchive.empty() );

		auto	stream = MakeRC<ArrayWStream>();

		for (auto& [res, name] : _images) {
			CHECK_THROW( stream->Write( ImagePacker::SerID ) and
						 stream->Write( CachedResourceName::Optimized_t{name} ) and
						 res->_StoreMeta( stream, nameInArchive ));
		}

		for (auto& [res, name] : _atlases) {
			CHECK_THROW( stream->Write( ImageAtlasPacker::SerID ) and
						 stream->Write( CachedResourceName::Optimized_t{name} ) and
						 res->_StoreMeta( stream, nameInArchive ));
		}

		for (auto& [res, name] : _fonts) {
			CHECK_THROW( stream->Write( RasterFontPacker::SerID ) and
						 stream->Write( CachedResourceName::Optimized_t{name} ) and
						 res->_StoreMeta( stream, nameInArchive ));
		}

		ObjectStorage::Instance()->AddTemp( nameInArchive, stream->ReleaseData(), EArchivePackerFileType::InMemory ); // throw

		_images.clear();
		_atlases.clear();
		_fonts.clear();
		_uniqueNames.clear();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptResourceMeta::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptResourceMeta>	binder{ se };
		binder.CreateRef();

		binder.AddMethod( &ScriptResourceMeta::Store,	"Store",	{"nameInArchive"} );
	}


} // AE::AssetPacker

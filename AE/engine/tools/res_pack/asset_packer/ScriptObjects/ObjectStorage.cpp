// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ObjectStorage.h"
#include "ScriptObjects/ScriptTexture.h"
#include "ScriptObjects/ScriptSharedImage.h"
#include "ScriptObjects/ScriptImageAtlas.h"
#include "ScriptObjects/ScriptRasterFont.h"
#include "ScriptObjects/ScriptMesh.h"
#include "ScriptObjects/ScriptModel.h"
#include "ScriptObjects/ScriptMaterial.h"
#include "ScriptObjects/ScriptResourceMeta.h"
#include "ScriptObjects/ScriptUIStyleCollection.h"
#include "ScriptObjects/ScriptUIWidget.h"

#include "scripting/Impl/EnumBinder.h"

namespace AE::AssetPacker
{


/*
=================================================
	ImageAtlasInfo
=================================================
*/
	void  ObjectStorage::ImageAtlasInfo::SetFileName (const String &name) __Th___
	{
		CHECK_THROW( _fileName.empty() );
		CHECK_THROW( _metaRes.empty() );

		_fileName = name;
	}

	void  ObjectStorage::ImageAtlasInfo::SetMetaResource (const String &name) __Th___
	{
		CHECK_THROW( _fileName.empty() );
		CHECK_THROW( _metaRes.empty() );

		_metaRes = name;
	}

	void  ObjectStorage::ImageAtlasInfo::Add (const String &name) __Th___
	{
		_set.insert( name );
	}

	void  ObjectStorage::ImageAtlasInfo::Contains (const String &imgName) C_Th___
	{
		CHECK_THROW_MSG( HashTable_Contains( _set, imgName ),
			"ImageAtlas (file '"s << _fileName << "' / meta '" << _metaRes << "') does not contains image '" << imgName << "'" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ObjectStorage::ObjectStorage () {}
	ObjectStorage::~ObjectStorage () {}

/*
=================================================
	ShaderStorage_Instance
=================================================
*/
	ObjectStorage*&  ShaderStorage_Instance ()
	{
		static thread_local ObjectStorage*	inst = null;
		return inst;
	}

	Ptr<ObjectStorage>  ObjectStorage::Instance ()
	{
		return ShaderStorage_Instance();
	}

	void  ObjectStorage::SetInstance (ObjectStorage* inst)
	{
		ShaderStorage_Instance() = inst;
	}

/*
=================================================
	AddToArchive
=================================================
*/
	void  ObjectStorage::AddToArchive (const String &name, RStream &stream) __Th___
	{
		return AddToArchive( name, stream, VFS::ArchivePacker::EFileType::Brotli );
	}

	void  ObjectStorage::AddToArchive (const String &name, RStream &stream, EArchivePackerFileType fileType) __Th___
	{
		AddName<FileName>( name ); // throw

		CHECK_THROW_MSG( HashTable_NotContains( _tempFiles, name ),
			"File '"s << name << "' already exists in archive" );

		CHECK_THROW_MSG( _archive.Add( FileName::WithString_t{name}, stream, fileType ),
			"Failed to add file '"s << name << "' to archive" );
	}

/*
=================================================
	AddTemp
----
	Only temporary files can be extracted from archive.
	In 'SaveArchive()' all temporary files will be stored to archive.
=================================================
*/
	void  ObjectStorage::AddTemp (const String &name, Array<ubyte> data) __Th___
	{
		AddTemp( name, RVRef(data), EArchivePackerFileType::Raw );
	}

	void  ObjectStorage::AddTemp (const String &name, Array<ubyte> data, EArchivePackerFileType fileType) __Th___
	{
		CHECK_THROW_MSG( not data.empty() );

		AddName<FileName>( name ); // throw

		CHECK_THROW_MSG( not _archive.Exists( FileName{name} ),
			"File '"s << name << "' already exists in archive" );

		CHECK_THROW_MSG( _tempFiles.emplace( name, MakePair( RVRef(data), fileType )).second,
			"File '"s << name << "' already exists in archive" );
	}

/*
=================================================
	ExtractFromArchive
=================================================
*/
	RC<RStream>  ObjectStorage::ExtractFromArchive (const String &name) __Th___
	{
		CHECK_THROW_MSG( not name.empty() );

		auto	it = _tempFiles.find( name );
		CHECK_THROW_MSG( it != _tempFiles.end(),
			"File '"s << name << "' can not be extracted from archive" );

		auto	result = MakeRC<ArrayRStream>( RVRef(it->second.first) );

		_tempFiles.erase( it );
		return result;
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  ObjectStorage::Initialize (const Path &tempFile)
	{
		return _archive.Create( tempFile );
	}

/*
=================================================
	SaveArchive
=================================================
*/
	bool  ObjectStorage::SaveArchive (const Path &filename)
	{
		AE_LOGI( "Store archive: '"s << ToString(filename) << "'" );

		// add temp files
		{
			for (auto& [name, data_and_type] : _tempFiles)
			{
				MemRefRStream	stream {ArrayView<ubyte>{data_and_type.first}};

				CHECK_THROW_MSG( _archive.Add( FileName::WithString_t{name}, stream, data_and_type.second ),
					"Failed to add file '"s << name << "' to archive" );
			}
			_tempFiles.clear();
		}

		bool	result = _archive.Store( filename );

		_atlasMap.clear();
		_fontMap.clear();

		return result;
	}

/*
=================================================
	AddAtlas
=================================================
*/
	void  ObjectStorage::AddAtlas (RC<ImageAtlasInfo> info) __Th___
	{
		CHECK_THROW_MSG( info );
		CHECK_THROW_MSG( info->HasFileName() );

		CHECK_THROW_MSG( _atlasMap.emplace( String{info->FileName()}, info ).second,
			"ImageAtlas '"s << info->FileName() << "' is already exists" );
	}

	void  ObjectStorage::AddAtlas (const String &metaArchive, RC<ImageAtlasInfo> info) __Th___
	{
		CHECK_THROW_MSG( info );
		CHECK_THROW_MSG( info->HasMetaResName() );
		CHECK_THROW_MSG( not metaArchive.empty() );

		CHECK_THROW_MSG( _atlasMap.emplace( String{metaArchive} << '%' << info->MetaResName(), info ).second,
			"ImageAtlas meta '"s << info->MetaResName() << "' with meta file '" << metaArchive << "' is already exists" );
	}

/*
=================================================
	GetAtlas
=================================================
*/
	RC<ObjectStorage::ImageAtlasInfo>  ObjectStorage::GetAtlas (const String &nameInArchive) __Th___
	{
		CHECK_THROW_MSG( not nameInArchive.empty() );

		auto	it = _atlasMap.find( nameInArchive );

		CHECK_THROW_MSG( it != _atlasMap.end(),
			"ImageAtlas '"s << nameInArchive << "' is not exists" );

		return it->second;
	}

	RC<ObjectStorage::ImageAtlasInfo>  ObjectStorage::GetAtlas (const String &metaArchive, const String &nameInMeta) __Th___
	{
		CHECK_THROW_MSG( not metaArchive.empty() );
		CHECK_THROW_MSG( not nameInMeta.empty() );

		auto	it = _atlasMap.find( String{metaArchive} << '%' << nameInMeta );

		CHECK_THROW_MSG( it != _atlasMap.end(),
			"ImageAtlas meta '"s << nameInMeta << "' with meta file '" << metaArchive << "' is not exists" );

		return it->second;
	}

/*
=================================================
	AddFont
=================================================
*/
	void  ObjectStorage::AddFont (const String &nameInArchive) __Th___
	{
		CHECK_THROW_MSG( not nameInArchive.empty() );

		CHECK_THROW_MSG( _fontMap.insert( nameInArchive ).second,
			"Font '"s << nameInArchive << "' is already exists" );
	}

	void  ObjectStorage::AddFont (const String &metaArchive, const String &nameInMeta) __Th___
	{
		CHECK_THROW_MSG( not metaArchive.empty() );
		CHECK_THROW_MSG( not nameInMeta.empty() );

		CHECK_THROW_MSG( _fontMap.insert( String{metaArchive} << '%' << nameInMeta ).second,
			"Font meta '"s << nameInMeta << "' with meta file '" << metaArchive << "' is already exists" );
	}

/*
=================================================
	RequireFont
=================================================
*/
	void  ObjectStorage::RequireFont (const String &nameInArchive) __Th___
	{
		CHECK_THROW_MSG( not nameInArchive.empty() );

		CHECK_THROW_MSG( HashTable_Contains( _fontMap, nameInArchive ),
			"Font '"s << nameInArchive << "' is not exists" );
	}

	void  ObjectStorage::RequireFont (const String &metaArchive, const String &nameInMeta) __Th___
	{
		CHECK_THROW_MSG( not metaArchive.empty() );
		CHECK_THROW_MSG( not nameInMeta.empty() );

		CHECK_THROW_MSG( HashTable_Contains( _fontMap, String{metaArchive} << '%' << nameInMeta ),
			"Font meta '"s << nameInMeta << "' with meta file '" << metaArchive << "' is not exists" );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ObjectStorage::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace AE::Scripting;

		CHECK_THROW_MSG( se->Create( True{"gen cpp header"} ));

		CoreBindings::BindStdTypes( se );
		CoreBindings::BindScalarMath( se );
		CoreBindings::BindVectorMath( se );
		CoreBindings::BindRect( se );
		CoreBindings::BindString( se );
		CoreBindings::BindColor( se );
		CoreBindings::BindToString( se, true, true, true, true );
		CoreBindings::BindLog( se );
		GraphicsBindings::BindEnums( se );
		GraphicsBindings::BindTypes( se );

		{
			EnumBinder<ECubeFace>	binder{ se };
			binder.Create();
			switch_enum( ECubeFace::XPos )
			{
				#define BIND( _name_ )	case ECubeFace::_name_ : binder.AddValue( #_name_, ECubeFace::_name_ );
				BIND( XPos )
				BIND( XNeg )
				BIND( YPos )
				BIND( YNeg )
				BIND( ZPos )
				BIND( ZNeg )
				#undef BIND
				default : break;
			}
			switch_end
		}

		ScriptResourceMeta::Bind( se );
		ScriptSharedImage::Bind( se );
		ScriptTexture::Bind( se );
		ScriptImageAtlas::Bind( se );
		ScriptRasterFont::Bind( se );
		ScriptMesh::Bind( se );
		ScriptModel::Bind( se );
		ScriptMaterial::Bind( se );
		ScriptUIStyleCollection::Bind( se );
		ScriptUIWidget::Bind( se );
	}

} // AE::AssetPacker

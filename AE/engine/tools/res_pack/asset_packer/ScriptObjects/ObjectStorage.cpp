// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ObjectStorage.h"
#include "ScriptObjects/ScriptTexture.h"
#include "ScriptObjects/ScriptImageAtlas.h"
#include "ScriptObjects/ScriptRasterFont.h"
#include "ScriptObjects/ScriptMesh.h"
#include "ScriptObjects/ScriptModel.h"
#include "ScriptObjects/ScriptMaterial.h"
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
    void  ObjectStorage::ImageAtlasInfo::SetName (const String &name) __Th___
    {
        CHECK_THROW( _name.empty() );
        _name = name;
    }

    void  ObjectStorage::ImageAtlasInfo::Add (const String &name) __Th___
    {
        _set.insert( name );
    }

    void  ObjectStorage::ImageAtlasInfo::Contains (const String &imgName) C_Th___
    {
        CHECK_THROW_MSG( _set.contains( imgName ),
            "ImageAtlas '"s << _name << "' does not contains image '" << imgName << "'" );
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
        static thread_local ObjectStorage*  inst = null;
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

        CHECK_THROW_MSG( _archive.Add( FileName::WithString_t{name}, stream, fileType ),
            "Failed to add file '"s << name << "' to archive" );
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

        bool    result = _archive.Store( filename );

        _atlasMap.clear();
        _fontMap.clear();

        return result;
    }

/*
=================================================
    AddAtlas
=================================================
*/
    void  ObjectStorage::AddAtlas (const String &nameInArchive, RC<ImageAtlasInfo> info) __Th___
    {
        CHECK_THROW_MSG( info );

        CHECK_THROW_MSG( _atlasMap.emplace( nameInArchive, info ).second,
            "ImageAtlas '"s << nameInArchive << "' is already exists" );
    }

/*
=================================================
    GetAtlas
=================================================
*/
    RC<ObjectStorage::ImageAtlasInfo>  ObjectStorage::GetAtlas (const String &nameInArchive) __Th___
    {
        auto    it = _atlasMap.find( nameInArchive );

        CHECK_THROW_MSG( it != _atlasMap.end(),
            "ImageAtlas '"s << nameInArchive << "' is not exists" );

        return it->second;
    }

/*
=================================================
    AddFont
=================================================
*/
    void  ObjectStorage::AddFont (const String &nameInArchive) __Th___
    {
        CHECK_THROW_MSG( _fontMap.insert( nameInArchive ).second,
            "Font '"s << nameInArchive << "' is already exists" );
    }

/*
=================================================
    RequireFont
=================================================
*/
    void  ObjectStorage::RequireFont (const String &nameInArchive) __Th___
    {
        CHECK_THROW_MSG( _fontMap.contains( nameInArchive ),
            "Font '"s << nameInArchive << "' is not exists" );
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
            EnumBinder<ECubeFace>   binder{ se };
            binder.Create();
            switch_enum( ECubeFace::XPos )
            {
                #define BIND( _name_ )  case ECubeFace::_name_ : binder.AddValue( #_name_, ECubeFace::_name_ );
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

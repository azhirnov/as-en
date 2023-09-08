// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/NamedID_HashCollisionCheck.h"
#include "base/Utils/FileSystem.h"

#include "vfs/Common.h"
#include "vfs/Archive/ArchivePacker.h"

#include "scripting/Bindings/CoreBindings.h"
#include "graphics/Scripting/GraphicsBindings.h"

#include "../pipeline_compiler/Packer/HashToName.h"

#include "Common.h"

namespace AE::AssetPacker
{
    using namespace AE::Base;

    using AE::VFS::FileName;


    //
    // Object Storage
    //

    class ObjectStorage final
    {
    // types
    public:
        class ImageAtlasInfo final : public EnableRC<ImageAtlasInfo>
        {
        private:
            HashSet< String >   _set;
            String              _name;

        public:
            void  SetName (const String &name)      __Th___;
            void  Add (const String &name)          __Th___;
            void  Contains (const String &name)     C_Th___;

            ND_ StringView  Name ()                 C_NE___ { return _name; }
        };

    private:
        using HashToNameMap_t   = HashMap< HashToName::NameHash, String, HashToName::NameHashHash >;
        using AtlasMap_t        = HashMap< String, RC<ImageAtlasInfo> >;
        using FontMap_t         = HashSet< String >;


    // variables
    private:
        VFS::ArchivePacker          _archive;

        AtlasMap_t                  _atlasMap;
        FontMap_t                   _fontMap;

        Path                        _currentPath;   // for script

        HashToNameMap_t             _hashToName;
        NamedID_HashCollisionCheck  _hashCollisionCheck;


    // methods
    public:
        ObjectStorage ();
        ~ObjectStorage ();

        void  SetScriptFolder (const Path &path)                                                            { _currentPath = path; }

        Path const&  GetScriptFolder ()                                                             const   { return _currentPath; }

            void                AddAtlas (const String &nameInArchive, RC<ImageAtlasInfo> info)     __Th___;
        ND_ RC<ImageAtlasInfo>  GetAtlas (const String &nameInArchive)                              __Th___;

            void                AddFont (const String &nameInArchive)                               __Th___;
            void                RequireFont (const String &nameInArchive)                           __Th___;

        void  AddToArchive (const String &name, RStream &stream)                                    __Th___;
        void  AddToArchive (const String &name, RStream &stream, EArchivePackerFileType fileType)   __Th___;

        ND_ bool  Initialize (const Path &tempFile);
        ND_ bool  SaveArchive (const Path &filename);

        template <typename NameType>
        void  AddName (const String &name)                                                          __Th___;

        static void  Bind (const ScriptEnginePtr &se)                                               __Th___;

        ND_ static Ptr<ObjectStorage>  Instance ();
            static void  SetInstance (ObjectStorage* inst);

        ND_ static String  ValidateFileName (const String &name);
    };


/*
=================================================
    AddName
=================================================
*/
    template <typename NameType>
    void  ObjectStorage::AddName (const String &name) __Th___
    {
        CHECK_THROW_MSG( not name.empty() );
        CHECK_THROW_MSG( name.length() <= NameType::MaxStringLength() );

        typename NameType::WithString_t     name_hash {name};

        _hashCollisionCheck.Add( name_hash );

        _hashToName.emplace( HashToName::NameHash{ uint(name_hash.GetHash32()), NameType::GetUID() }, name );
    }


} // AE::AssetPacker

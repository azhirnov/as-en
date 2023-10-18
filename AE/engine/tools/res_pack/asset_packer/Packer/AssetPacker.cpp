// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "scripting/Impl/ScriptFn.h"
#include "scripting/Impl/ScriptEngine.inl.h"
#include "ScriptObjects/ObjectStorage.h"
#include "AssetPacker.h"

namespace AE::AssetPacker
{
namespace
{
    using namespace AE::Scripting;

/*
=================================================
    PackAssetsImpl
=================================================
*/
    ND_ static bool  PackAssetsImpl (const AssetInfo *info)
    {
        CHECK_ERR( info != null );
        CHECK_ERR( (info->inFileCount > 0) and (info->inFiles != null) );
        CHECK_ERR( (info->inIncludeFolderCount > 0) == (info->inIncludeFolders != null) );
        CHECK_ERR( info->tempFile != null );
        CHECK_ERR( info->outputArchive != null );

        ScriptEnginePtr     script_engine = MakeRC<ScriptEngine>();
        ObjectStorage       obj_storage;
        ObjectStorage::SetInstance( &obj_storage );

        CHECK_ERR( obj_storage.Initialize( Path{info->tempFile} ));
        CATCH_ERR( ObjectStorage::Bind( script_engine ));

        Array<Path>     script_include_dirs;
        for (usize i = 0; i < info->inIncludeFolderCount; ++i)
        {
            Path    path {info->inIncludeFolders[i]};
            if ( FileSystem::IsDirectory( path ))
                script_include_dirs.push_back( RVRef(path) );
            else
                AE_LOGI( "Skip invalid include directory: '"s << ToString(path) << "'" );
        }

        for (usize i = 0; i < info->inFileCount; ++i)
        {
            Path                        path        {info->inFiles[i]};
            const String                ansi_path   = ToString(path);
            ScriptEngine::ModuleSource  src;

            {
                FileRStream     file {path};

                if ( not file.IsOpen() )
                {
                    AE_LOGI( "Failed to open script file: '"s << ansi_path << "'" );
                    continue;
                }

                src.name = ToString( path.filename().replace_extension("") );

                if ( not file.Read( file.RemainingSize(), OUT src.script ))
                {
                    AE_LOGI( "Failed to read script file: '"s << ansi_path << "'" );
                    continue;
                }
            }

            src.dbgLocation     = SourceLoc{ ansi_path, 0 };
            src.usePreprocessor = true;

            ScriptModulePtr     module = script_engine->CreateModule( {src}, {"SCRIPT"}, script_include_dirs );
            if ( not module )
            {
                AE_LOGI( "Failed to parse script file: '"s << ansi_path << "'" );
                continue;
            }

            auto    fn = script_engine->CreateScript< void() >( "ASmain", module );
            if ( not fn )
            {
                AE_LOGI( "Failed to create script context for file: '"s << ansi_path << "'" );
                continue;
            }

            obj_storage.SetScriptFolder( path.parent_path() );

            if ( not fn->Run() )
                return false;
        }

        const Path  arch_fname  = FileSystem::ToAbsolute( info->outputArchive );
        CHECK_ERR( obj_storage.SaveArchive( arch_fname ));

        if ( info->outputScriptFile != null )
        {
            CHECK_ERR( script_engine->SaveCppHeader( info->outputScriptFile ));
        }

        ObjectStorage::SetInstance( null );
        return true;
    }

} // namespace


/*
=================================================
    PackAssets
=================================================
*/
    extern "C" bool AE_AP_API  PackAssets (const AssetInfo *info)
    {
        AE::Base::StaticLogger::LoggerScope log{};

        const auto  path = FileSystem::CurrentPath();
        const bool  res  = PackAssetsImpl( info );

        ObjectStorage::SetInstance( null );

        // restore current path
        FileSystem::SetCurrentPath( path );

        return res;
    }

} // AE::AssetPacker

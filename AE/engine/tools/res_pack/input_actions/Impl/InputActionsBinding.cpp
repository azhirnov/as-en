// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Impl/ObjectStorage.h"

namespace AE::InputActions
{
namespace
{
    using namespace AE::Scripting;

/*
=================================================
    ConvertInputActionsImpl
=================================================
*/
    static bool  ConvertInputActionsImpl (const InputActionsInfo *info)
    {
        CHECK_ERR( info != null );
        CHECK_ERR( (info->inFileCount > 0) == (info->inFiles != null) );
        CHECK_ERR( info->outputPackName != null );

        ScriptEnginePtr     script_engine   = MakeRC<ScriptEngine>();
        const Path          pack_fname      = FileSystem::ToAbsolute( info->outputPackName );
        ObjectStorage       obj_storage;

        CATCH_ERR( obj_storage.Bind( script_engine ));
        ObjectStorage::SetInstance( &obj_storage );

        // run script files
        {
            FlatHashSet< Path, PathHasher >     unique_files;

            for (usize i = 0; i < info->inFileCount; ++i)
            {
                CHECK_ERR( info->inFiles[i] != null );

                Path    path { FileSystem::ToAbsolute( info->inFiles[i] )};

                if ( unique_files.insert( path ).second )
                {
                    CHECK_ERR( obj_storage.AddBindings( script_engine, path ));
                }
            }
        }

        CHECK_ERR( obj_storage.Save( pack_fname ));

        if ( info->outputScriptFile != null )
            CHECK_ERR( script_engine->SaveCppHeader( info->outputScriptFile ));

        if ( info->outputCppFile != null )
            CHECK_ERR( obj_storage.SaveCppTypes( info->outputCppFile ));

        ObjectStorage::SetInstance( null );
        return true;
    }

} // namespace


/*
=================================================
    ConvertInputActions
=================================================
*/
    extern "C" bool AE_IA_API ConvertInputActions (const InputActionsInfo *info)
    {
        AE::Base::StaticLogger::LoggerScope log{};

        const auto  path = FileSystem::CurrentPath();
        const bool  res  = ConvertInputActionsImpl( info );

        // restore current path
        FileSystem::SetCurrentPath( path );

        return res;
    }

} // AE::InputActions

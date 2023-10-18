// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "PipelineCompiler.h"

#include "base/DataSource/MemStream.h"
#include "base/DataSource/FileStream.h"
#include "base/CompileTime/FunctionInfo.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"

#include "serializing/Serializer.h"

#include "scripting/Impl/EnumBinder.h"
#include "scripting/Impl/ScriptFn.h"
#include "scripting/Bindings/CoreBindings.h"

#include "graphics/Scripting/GraphicsBindings.h"

#include "ScriptObjects/ObjectStorage.h"

#include "PipelineCompilerImpl.h"

namespace AE::PipelineCompiler
{
namespace 
{
    using namespace AE::Scripting;

/*
=================================================
    BuildPipelineList
=================================================
*/
    ND_ static bool  BuildPipelineList (const PipelinesInfo *info,
                                        OUT Array<Path> &outPipelines, OUT Array<Path> &outPipelineIncludeDirs,
                                        OUT Array<Path> &outShaderIncludeDirs, OUT Array<Path> &outShaderDirs)
    {
        struct PathInfo
        {
            Path                path;
            uint                priority    = 0;
            EPathParamsFlags    flags       = Default;
        };

        HashSet< Path, PathHasher >     unique_ppln;
        Deque<PathInfo>                 ppln_folders;
        Array<PathInfo>                 pipelines;

        for (usize i = 0; i < info->pipelineFolderCount; ++i)
        {
            auto&   item    = info->pipelineFolders[i];
            Path    path    { item.path };

            if ( not FileSystem::IsDirectory( path ))
            {
                AE_LOGI( "Can't find folder: '"s << ToString(path) << "'" );
                continue;
            }

            ppln_folders.push_back( PathInfo{ RVRef(path), uint(item.priority), EPathParamsFlags(item.flags) });
        }

        std::sort( ppln_folders.begin(), ppln_folders.end(), [](auto& lhs, auto& rhs) { return lhs.priority < rhs.priority; });

        for (; not ppln_folders.empty();)
        {
            const Path  path    = RVRef(ppln_folders.front().path);
            const uint  prio    = ppln_folders.front().priority;
            const auto  flags   = ppln_folders.front().flags;
            ppln_folders.pop_front();

            for (auto& file : FileSystem::Enum( path ))
            {
                if ( file.IsDirectory() and AllBits( flags, EPathParamsFlags::Recursive ))
                {
                    ppln_folders.push_back( PathInfo{ file.Get(), prio, flags });
                    continue;
                }

                Path    tmp = FileSystem::ToAbsolute( file.Get() );
                if ( not unique_ppln.insert( tmp ).second )
                    continue;

                if ( tmp.extension() != ".as" )
                    continue;

                pipelines.push_back( PathInfo{ RVRef(tmp), prio, EPathParamsFlags::Unknown });
            }
        }

        for (usize i = 0; i < info->inPipelineCount; ++i)
        {
            Path    path{ info->inPipelines[i].path };

            if ( not FileSystem::IsFile( path ))
            {
                AE_LOGI( "Can't find pipeline: '"s << ToString(path) << "'" );
                continue;
            }

            path = FileSystem::ToAbsolute( path );
            if ( not unique_ppln.insert( path ).second )
                continue;

            pipelines.push_back( PathInfo{ RVRef(path), uint(info->inPipelines[i].priority), EPathParamsFlags::Unknown });
        }

        std::sort( pipelines.begin(), pipelines.end(), [](auto& lhs, auto& rhs) { return lhs.priority < rhs.priority; });

        outPipelines.reserve( pipelines.size() );
        for (auto& item : pipelines) {
            outPipelines.push_back( RVRef(item.path) );
        }

        for (usize i = 0; i < info->shaderIncludeDirCount; ++i)
        {
            Path    path{ info->shaderIncludeDirs[i] };

            if ( not FileSystem::IsDirectory( path ))
            {
                AE_LOGI( "Can't find shader include folder: '"s << ToString(path) << "'" );
                continue;
            }

            outShaderIncludeDirs.push_back( FileSystem::ToAbsolute( path ));
        }

        for (usize i = 0; i < info->shaderFolderCount; ++i)
        {
            Path    path{ info->shaderFolders[i] };

            if ( not FileSystem::IsDirectory( path ))
            {
                AE_LOGI( "Can't find shader folder: '"s << ToString(path) << "'" );
                continue;
            }

            outShaderDirs.push_back( FileSystem::ToAbsolute( path ));
        }

        for (usize i = 0; i < info->pipelineIncludeDirCount; ++i)
        {
            Path    path{ info->pipelineIncludeDirs[i] };

            if ( not FileSystem::IsDirectory( path ))
            {
                AE_LOGI( "Can't find pipeline include folder: '"s << ToString(path) << "'" );
                continue;
            }

            outPipelineIncludeDirs.push_back( FileSystem::ToAbsolute( path ));
        }

        return true;
    }

/*
=================================================
    LoadPipelines
=================================================
*/
    ND_ static bool  LoadPipelines (ObjectStorage &storage, const ScriptEnginePtr &scriptEngine, const Array<Path> &pipelines, ArrayView<Path> includeDirs)
    {
        for (auto& path : pipelines)
        {
            if ( not storage.CompilePipeline( scriptEngine, path, includeDirs ))
                continue;
        }

        CHECK_ERR( storage.BuildRenderTechniques() );
        return true;
    }

/*
=================================================
    CompilePipelinesImpl
=================================================
*/
    ND_ bool  CompilePipelinesImpl (const PipelinesInfo *info)
    {
        CHECK_ERR( info != null );
        CHECK_ERR( (info->shaderIncludeDirCount > 0) == (info->shaderIncludeDirs != null) );
        CHECK_ERR( (info->pipelineIncludeDirCount > 0) == (info->pipelineIncludeDirs != null) );
        CHECK_ERR( (info->inPipelineCount > 0) == (info->inPipelines != null) );
        CHECK_ERR( (info->pipelineFolderCount > 0) == (info->pipelineFolders != null) );
        CHECK_ERR( (info->shaderFolderCount > 0) == (info->shaderFolders != null) );
        CHECK_ERR( info->outputPackName != null );

        ScriptEnginePtr     script_engine = MakeRC<ScriptEngine>();
        PipelineStorage     ppln_storage;
        ObjectStorage       obj_storage;
        Array< Path >       pipelines;
        Array< Path >       ppln_include_dirs;
        {
            Array< Path >   shader_dirs;
            Array< Path >   shader_include_dirs;
            CHECK_ERR( BuildPipelineList( info, OUT pipelines, OUT ppln_include_dirs, OUT shader_include_dirs, OUT shader_dirs ));

            CHECK_ERR( not pipelines.empty() );
            //CHECK_ERR( not shader_dirs.empty() );

            obj_storage.pplnStorage     = &ppln_storage;
            obj_storage.shaderFolders   = RVRef(shader_dirs);

          #ifdef AE_METAL_TOOLS
            obj_storage.metalCompiler   = MakeUnique<MetalCompiler>( shader_include_dirs );
          #endif

            obj_storage.spirvCompiler   = MakeUnique<SpirvCompiler>( shader_include_dirs );
            obj_storage.spirvCompiler->SetDefaultResourceLimits();

            ObjectStorage::SetInstance( &obj_storage );
        }

        const Path  pack_fname          = FileSystem::ToAbsolute( info->outputPackName );
        const Path  cpp_structs_fname   = info->outputCppStructsFile != null ? FileSystem::ToAbsolute( info->outputCppStructsFile ) : Default;
        const Path  cpp_names_fname     = info->outputCppNamesFile   != null ? FileSystem::ToAbsolute( info->outputCppNamesFile   ) : Default;

        CATCH_ERR( ObjectStorage::Bind( script_engine ));

        CHECK_ERR( LoadPipelines( obj_storage, script_engine, pipelines, ppln_include_dirs ));

        CHECK_ERR( not obj_storage.hashCollisionCheck.HasCollisions() );

        CHECK_ERR( obj_storage.SavePack( pack_fname, info->addNameMapping ));

        if ( not cpp_structs_fname.empty() )
            CHECK_ERR( obj_storage.SaveCppStructs( cpp_structs_fname ));

        if ( not cpp_names_fname.empty() )
            CHECK_ERR( obj_storage.SaveCppNames( cpp_names_fname, info->cppReflectionFlags ));

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
    CompilePipelines
=================================================
*/
    extern "C" bool AE_PC_API  CompilePipelines (const PipelinesInfo *info)
    {
        AE::Base::StaticLogger::LoggerScope log{};

        const auto  path = FileSystem::CurrentPath();
        const bool  res  = CompilePipelinesImpl( info );

        // restore current path
        FileSystem::SetCurrentPath( path );

        return res;
    }

} // AE::PipelineCompiler

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/Algorithms/StringUtils.h"

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"
#include "scripting/Impl/ScriptFn.h"
//#include "scripting/Impl/ScriptEngine.inl.h"

#include "vfs/Archive/ArchivePacker.h"

#include "pipeline_compiler/PipelineCompilerImpl.h"
#include "input_actions/InputActionsBinding.h"
#include "asset_packer/AssetPacker.h"

using namespace AE;
using namespace AE::Base;
using namespace AE::Scripting;


namespace
{
    using EReflectionFlags = PipelineCompiler::EReflectionFlags;

/*
=================================================
    ConvertArray
=================================================
*/
    using EPathParamsFlags = AE::PipelineCompiler::EPathParamsFlags;

    struct PathParams2
    {
        BasicString<CharType>   path;
        usize                   priority    : 16;
        usize                   flags       : 8;
    };
    ND_ static Array<AE::PipelineCompiler::PathParams>  ConvertArray (const Array<PathParams2> &src)
    {
        Array<AE::PipelineCompiler::PathParams>     dst;
        dst.resize( src.size() );

        for (usize i = 0; i < src.size(); ++i)
        {
            dst[i].path     = src[i].path.c_str();
            dst[i].priority = src[i].priority;
            dst[i].flags    = src[i].flags;
        }
        return dst;
    }

/*
=================================================
    ConvertString
=================================================
*/
    ND_ static BasicString<CharType>  ConvertString (const String &src)
    {
        BasicString<CharType>   dst;
        dst.assign( src.begin(), src.end() );
        return dst;
    }

    ND_ static BasicString<CharType>  ConvertString (const WString &src)
    {
        BasicString<CharType>   dst;
        dst.assign( src.begin(), src.end() );
        return dst;
    }

    ND_ static BasicString<CharType>  ConvertString (const Path &src)
    {
        return ConvertString( src.native() );
    }

/*
=================================================
    ConvertArray
=================================================
*/
    ND_ static Array<const CharType *>  ConvertArray (const Array< BasicString<CharType> > &src)
    {
        Array<const CharType *>     dst;
        dst.resize( src.size() );

        for (usize i = 0; i < src.size(); ++i) {
            dst[i] = src[i].c_str();
        }
        return dst;
    }

/*
=================================================
    ScriptPipelineCompiler
=================================================
*/
    class ScriptPipelineCompiler final : public AngelScriptHelper::SimpleRefCounter
    {
    private:
        Array< PathParams2 >                _pipelineFolders;
        Array< PathParams2 >                _pipelines;

        Array< BasicString<CharType> >      _shaderFolders;
        Array< BasicString<CharType> >      _includeDirs;
        EReflectionFlags                    _reflFlags      = Default;

        String                              _outputCppStructsFile;
        String                              _outputCppNamesFile;


    public:
        void  AddPipelineFolder (const String &path, uint priority, EPathParamsFlags flags) __Th___
        {
            PathParams2&    params = _pipelineFolders.emplace_back();
            params.path     = ConvertString( path );
            params.priority = priority;
            params.flags    = usize(flags);
        }

        void  AddPipelineFolder2 (const String &path, EPathParamsFlags flags) __Th___
        {
            return AddPipelineFolder( path, uint(_pipelineFolders.size() + _pipelines.size()), flags );
        }

        void  AddPipelineFolder3 (const String &path) __Th___
        {
            return AddPipelineFolder2( path, Default );
        }

        void  AddPipeline (const String &path, uint priority, EPathParamsFlags flags) __Th___
        {
            PathParams2&    params = _pipelines.emplace_back();
            params.path     = ConvertString( path );
            params.priority = priority;
            params.flags    = usize(flags);
        }

        void  AddPipeline2 (const String &path, EPathParamsFlags flags) __Th___
        {
            return AddPipeline( path, uint(_pipelineFolders.size() + _pipelines.size()), flags );
        }

        void  AddPipeline3 (const String &path) __Th___
        {
            return AddPipeline2( path, Default );
        }

        void  AddShaderFolder (const String &path) __Th___
        {
            _shaderFolders.push_back( ConvertString( path ));
        }

        void  AddIncludeDir (const String &path) __Th___
        {
            _includeDirs.push_back( ConvertString( path ));
        }

        void  SetOutputCPPFile1 (const String &structs, const String &names, uint flags) __Th___
        {
            SetOutputCPPFile2( structs, names, EReflectionFlags(flags) );
        }

        void  SetOutputCPPFile2 (const String &structs, const String &names, EReflectionFlags flags) __Th___
        {
            CHECK_THROW_MSG( _outputCppStructsFile.empty() );
            CHECK_THROW_MSG( _outputCppNamesFile.empty() );

            _outputCppStructsFile   = structs;
            _outputCppNamesFile     = names;
            _reflFlags              = flags;
        }

        void  Compile1 (const String &outputPackName) __Th___
        {
            return _Compile( outputPackName, false );
        }

        void  Compile4 (const String &outputPackName) __Th___
        {
            return _Compile( outputPackName, true );
        }

    private:
        void  _Compile (const String &outputPackName, const bool addNameMapping) __Th___
        {
            using namespace AE::PipelineCompiler;

            const auto  output_pack_name        = ConvertString( outputPackName );
            const auto  output_cpp_types_file   = ConvertString( _outputCppStructsFile );
            const auto  output_cpp_names_file   = ConvertString( _outputCppNamesFile );

            const auto  pipeline_folders        = ConvertArray( _pipelineFolders );
            const auto  pipelines               = ConvertArray( _pipelines );
            const auto  shader_folders          = ConvertArray( _shaderFolders );
            const auto  include_dirs            = ConvertArray( _includeDirs );

            PipelinesInfo   info = {};

            // input pipelines
            info.pipelineFolders        = pipeline_folders.data();
            info.pipelineFolderCount    = pipeline_folders.size();
            info.inPipelines            = pipelines.data();
            info.inPipelineCount        = pipelines.size();

            // input shaders
            info.shaderFolders          = shader_folders.data();
            info.shaderFolderCount      = shader_folders.size();

            // include directories
            info.includeDirs            = include_dirs.data();
            info.includeDirCount        = include_dirs.size();

            // output
            info.outputPackName         = output_pack_name.c_str();
            info.outputCppStructsFile   = output_cpp_types_file.empty() ? null : output_cpp_types_file.c_str();
            info.outputCppNamesFile     = output_cpp_names_file.empty() ? null : output_cpp_names_file.c_str();
            info.cppReflectionFlags     = _reflFlags;
            info.addNameMapping         = addNameMapping;

            CHECK_THROW_MSG( CompilePipelines( &info ));

            // reset
            _pipelineFolders.clear();
            _pipelines.clear();
            _shaderFolders.clear();
            _includeDirs.clear();
        }
    };

/*
=================================================
    ScriptInputActions
=================================================
*/
    class ScriptInputActions final : public AngelScriptHelper::SimpleRefCounter
    {
    private:
        Array< BasicString<CharType> >  _files;
        String                          _outputCppFile;

    public:
        void  Add (const String &filename) __Th___
        {
            _files.push_back( ConvertString( filename ));
        }

        void  SetOutputCPPFile (const String &value) __Th___
        {
            CHECK_THROW_MSG( _outputCppFile.empty() );
            _outputCppFile = value;
        }

        void  Convert (const String &outputName) __Th___
        {
            using namespace AE::InputActions;

            CHECK_THROW_MSG( not _files.empty() );

            const auto  output      = ConvertString( outputName );
            const auto  temp        = ConvertArray( _files );
            const auto  output_cpp  = ConvertString( _outputCppFile );

            InputActionsInfo    info = {};
            info.inFiles        = temp.data();
            info.inFileCount    = temp.size();
            info.outputPackName = output.c_str();
            info.outputCppFile  = output_cpp.empty() ? null : output_cpp.c_str();

            CHECK_THROW_MSG( ConvertInputActions( &info ));

            _files.clear();
        }
    };

/*
=================================================
    ScriptAssetPacker
=================================================
*/
    class ScriptAssetPacker final : public AngelScriptHelper::SimpleRefCounter
    {
    private:
        Array< BasicString<CharType> >  _files;
        BasicString<CharType>           _tempFile;

    public:
        void  Add (const String &filename) __Th___
        {
            const auto  ext = Path{filename}.extension().string();
            CHECK_THROW_MSG( ext == ".as" );

            _files.push_back( ConvertString( filename ));
        }

        void  AddFolder (const String &inFolder) __Th___
        {
            Deque<Path>     stack;
            stack.push_back( Path{inFolder} );

            for (; not stack.empty();)
            {
                Path    folder = RVRef(stack.front());
                stack.pop_front();

                for (auto& path : FileSystem::Enum( folder ))
                {
                    if ( path.IsDirectory() )
                    {
                        stack.push_back( path.Get() );
                    }
                    else
                    {
                        const auto  ext = path.Get().extension().string();
                        if ( ext == ".as" )
                            _files.push_back( ConvertString( path.Get() ));
                    }
                }
            }
        }

        void  SetTempFile (const String &fileName) __Th___
        {
            CHECK_THROW_MSG( not FileSystem::Exists( fileName ) or
                             FileSystem::IsFile( fileName ));

            const Path  path {fileName};

            FileSystem::Remove( path );
            FileSystem::CreateDirectories( path.parent_path() );

            _tempFile = ConvertString( FileSystem::ToAbsolute( path ));
        }

        void  ToArchive (const String &outputName) __Th___
        {
            using namespace AE::AssetPacker;

            const auto  output  = ConvertString( FileSystem::ToAbsolute( outputName ));
            const auto  temp    = ConvertArray( _files );

            AssetInfo   info    = {};
            info.inFiles        = temp.data();
            info.inFileCount    = temp.size();
            info.tempFile       = _tempFile.c_str();
            info.outputArchive  = output.c_str();

            CHECK_THROW_MSG( PackAssets( &info ));

            FileSystem::Remove( _tempFile );

            _files.clear();
            _tempFile.clear();
        }
    };

/*
=================================================
    ScriptArchive
=================================================
*/
    using EFileType = VFS::ArchivePacker::EFileType;

    class ScriptArchive final : public AngelScriptHelper::SimpleRefCounter
    {
    private:
        VFS::ArchivePacker      _archive;
        BasicString<CharType>   _tempFile;
        EFileType               _defaultType    = EFileType::Raw;

    public:
        void  Add1 (const String &name, const String &filename, EFileType type) __Th___
        {
            CHECK_THROW_MSG( name.length() < VFS::FileName::MaxStringLength() );
            CHECK_THROW_MSG( _archive.Add( VFS::FileName::WithString_t{name}, Path{filename}, type ));
        }

        void  Add2 (const String &filename, EFileType type) __Th___
        {
            Add1( filename, filename, type );
        }

        void  Add3 (const String &name, const String &filename) __Th___
        {
            Add1( name, filename, _defaultType );
        }

        void  Add4 (const String &filename) __Th___
        {
            Add1( filename, filename, _defaultType );
        }

        void  AddArchive (const String &filename) __Th___
        {
            CHECK_THROW_MSG( _archive.AddArchive( Path{filename} ));
        }

        void  Store (const String &filename) __Th___
        {
            CHECK_THROW_MSG( _archive.Store( Path{filename} ));

            FileSystem::Remove( _tempFile );
            _tempFile.clear();
        }

        void  SetDefaultFileType (EFileType type)
        {
            _defaultType = type;
        }

        void  SetTempFile (const String &fileName) __Th___
        {
            CHECK_THROW_MSG( not FileSystem::Exists( fileName ) or
                             FileSystem::IsFile( fileName ));

            const Path  path {fileName};

            FileSystem::Remove( path );
            FileSystem::CreateDirectories( path.parent_path() );

            CHECK_THROW_MSG( _archive.Create( FileSystem::ToAbsolute( path )));
            _tempFile = ConvertString( FileSystem::ToAbsolute( path ));
        }
    };

} // namespace


AE_DECL_SCRIPT_OBJ_RC(  ScriptPipelineCompiler, "PipelineCompiler"  );
AE_DECL_SCRIPT_OBJ_RC(  ScriptInputActions,     "InputActions"      );
AE_DECL_SCRIPT_OBJ_RC(  ScriptAssetPacker,      "AssetPacker"       );
AE_DECL_SCRIPT_OBJ_RC(  ScriptArchive,          "Archive"           );
AE_DECL_SCRIPT_TYPE(    EPathParamsFlags,       "EPathParamsFlags"  );
AE_DECL_SCRIPT_TYPE(    EFileType,              "EFileType"         );
AE_DECL_SCRIPT_TYPE(    EReflectionFlags,       "EReflectionFlags"  );


namespace
{
/*
=================================================
    GetSharedFeatureSetPath
    GetSharedShadersPath
    GetCanvasVerticesPath
    GetOutputDir
=================================================
*/
    static String  GetSharedFeatureSetPath ()   { return AE_SHARED_DATA "/feature_set"; }
    static String  GetSharedShadersPath ()      { return AE_SHARED_DATA "/shaders"; }
    static String  GetCanvasVerticesPath ()     { return AE_CANVAS_VERTS; }

    static String   _s_OutputDir;
    static String  GetOutputDir ()              { return _s_OutputDir; }

/*
=================================================
    DeleteFolder
=================================================
*/
    static void  DeleteFolder (const String &folder)
    {
        FileSystem::RemoveAll( Path{folder} );
    }

/*
=================================================
    Bind
=================================================
*/
    static void  Bind (const ScriptEnginePtr &se) __Th___
    {
        CoreBindings::BindStdTypes( se );
        CoreBindings::BindScalarMath( se );
        CoreBindings::BindVectorMath( se );
        CoreBindings::BindArray( se );
        CoreBindings::BindString( se );

        se->AddFunction( &GetSharedFeatureSetPath,  "GetSharedFeatureSetPath"   );
        se->AddFunction( &GetSharedShadersPath,     "GetSharedShadersPath"      );
        se->AddFunction( &GetCanvasVerticesPath,    "GetCanvasVerticesPath"     );
        se->AddFunction( &GetOutputDir,             "GetOutputDir"              );
        se->AddFunction( &DeleteFolder,             "DeleteFolder"              );

        // pipeline compiler path params
        {
            EnumBinder<EPathParamsFlags>    binder{ se };
            binder.Create();
            binder.AddValue( "Unknown",     EPathParamsFlags::Unknown   );
            binder.AddValue( "Recursive",   EPathParamsFlags::Recursive );
            STATIC_ASSERT( uint(EPathParamsFlags::All) == 1 );
        }

        // pipeline compiler path params
        {
            EnumBinder<EFileType>   binder{ se };
            binder.Create();
            binder.AddValue( "Raw",             EFileType::Raw  );
            binder.AddValue( "Brotli",          EFileType::Brotli   );
            binder.AddValue( "InMemory",        EFileType::InMemory );
            binder.AddValue( "BrotliInMemory",  EFileType::BrotliInMemory   );
            STATIC_ASSERT( uint(EFileType::All) == 7 );
        }

        // 
        {
            EnumBinder<EReflectionFlags>    binder{ se };
            binder.Create();
            binder.AddValue( "RenderTechniques",            EReflectionFlags::RenderTechniques  );
            binder.AddValue( "RTechPass_Pipelines",         EReflectionFlags::RTechPass_Pipelines   );
            binder.AddValue( "RTech_ShaderBindingTable",    EReflectionFlags::RTech_ShaderBindingTable  );
            binder.AddValue( "All",                         EReflectionFlags::All   );
            STATIC_ASSERT( uint(EReflectionFlags::All) == 7 );
        }

        // pipeline compiler
        {
            ClassBinder<ScriptPipelineCompiler>     binder{ se };
            binder.CreateRef();
            binder.AddMethod( &ScriptPipelineCompiler::AddPipelineFolder,   "AddPipelineFolder"         );
            binder.AddMethod( &ScriptPipelineCompiler::AddPipelineFolder2,  "AddPipelineFolder"         );
            binder.AddMethod( &ScriptPipelineCompiler::AddPipelineFolder3,  "AddPipelineFolder"         );
            binder.AddMethod( &ScriptPipelineCompiler::AddPipeline,         "AddPipeline"               );
            binder.AddMethod( &ScriptPipelineCompiler::AddPipeline2,        "AddPipeline"               );
            binder.AddMethod( &ScriptPipelineCompiler::AddPipeline3,        "AddPipeline"               );
            binder.AddMethod( &ScriptPipelineCompiler::AddShaderFolder,     "AddShaderFolder"           );
            binder.AddMethod( &ScriptPipelineCompiler::AddIncludeDir,       "IncludeDir"                );

            binder.AddMethod( &ScriptPipelineCompiler::SetOutputCPPFile1,   "SetOutputCPPFile"          );
            binder.AddMethod( &ScriptPipelineCompiler::SetOutputCPPFile2,   "SetOutputCPPFile"          );
            binder.AddMethod( &ScriptPipelineCompiler::Compile1,            "Compile"                   );
            binder.AddMethod( &ScriptPipelineCompiler::Compile4,            "CompileWithNameMapping"    );
        }

        // input actions
        {
            ClassBinder<ScriptInputActions>     binder{ se };
            binder.CreateRef();
            binder.AddMethod( &ScriptInputActions::Add,                 "Add"               );
            binder.AddMethod( &ScriptInputActions::SetOutputCPPFile,    "SetOutputCPPFile"  );
            binder.AddMethod( &ScriptInputActions::Convert,             "Convert"           );
        }

        // asset packer
        {
            ClassBinder<ScriptAssetPacker>      binder{ se };
            binder.CreateRef();
            binder.AddMethod( &ScriptAssetPacker::Add,          "Add"           );
            binder.AddMethod( &ScriptAssetPacker::AddFolder,    "AddFolder"     );
            binder.AddMethod( &ScriptAssetPacker::SetTempFile,  "SetTempFile"   );
            binder.AddMethod( &ScriptAssetPacker::ToArchive,    "ToArchive"     );
        }

        // archive
        {
            ClassBinder<ScriptArchive>      binder{ se };
            binder.CreateRef();
            binder.AddMethod( &ScriptArchive::SetTempFile,          "SetTempFile"       );
            binder.AddMethod( &ScriptArchive::SetDefaultFileType,   "SetDefaultFileType");
            binder.AddMethod( &ScriptArchive::Add1,                 "Add"               );
            binder.AddMethod( &ScriptArchive::Add2,                 "Add"               );
            binder.AddMethod( &ScriptArchive::Add3,                 "Add"               );
            binder.AddMethod( &ScriptArchive::Add4,                 "Add"               );
            binder.AddMethod( &ScriptArchive::AddArchive,           "AddArchive"        );
            binder.AddMethod( &ScriptArchive::Store,                "Store"             );
        }
    }

/*
=================================================
    RunScript
=================================================
*/
    ND_ static bool  RunScript (const Path &respackScript, const Path &outputDir)
    {
        AE_LOGI( "OfflinePacker args: \n"
                 "\""s << ToString(respackScript) << "\" \"" << ToString(outputDir) << "\"\n" );

        CHECK_ERR( FileSystem::IsFile( respackScript ));

        FileSystem::SetCurrentPath( respackScript.parent_path() );

        FileSystem::CreateDirectories( outputDir );
        CHECK_ERR( FileSystem::IsDirectory( outputDir ));

        _s_OutputDir = ToString(outputDir);
        AE_LOGI( "Output folder: '"s << ToString( _s_OutputDir ) << "'" );
        CHECK_ERR( outputDir == Path{_s_OutputDir} );   // if path has unicode
        _s_OutputDir << '/';

        const auto  ansi_path = ToString( respackScript );
        String      script;

        {
            FileRStream     file {respackScript};

            CHECK_ERR_MSG( file.IsOpen(),
                "Failed to open script '"s << ansi_path << "'" );

            CHECK_ERR_MSG( file.Read( file.RemainingSize(), OUT script ),
                "Failed to read script '"s << ansi_path << "'" );
        }

        ScriptEnginePtr se = MakeRC<ScriptEngine>();
        CHECK_ERR( se->Create( True{"gen cpp header"} ));

        CATCH_ERR( Bind( se ));

        auto    mod = se->CreateModule({ScriptEngine::ModuleSource{ "def"s, RVRef(script), SourceLoc{ansi_path}, True{"preprocessor"} }});
        CHECK_ERR( mod );

        auto    scr = se->CreateScript< void () >( "ASmain", mod );
        CHECK_ERR( scr and scr->Run() );

        CHECK_ERR( se->SaveCppHeader( Path{AE_SHARED_DATA} / "scripts/offline_packer.as" ));

        return true;
    }

} // namespace


/*
=================================================
    main
=================================================
*/
int main (int argc, char* argv[])
{
    AE::Base::StaticLogger::LoggerScope log{};  // don't check for memleak because of false possitive in 'SpirvToMsl'

    CHECK_ERR( argc == 2 or argc == 3, -1 );

    Path    output_dir;
    if ( argc == 3 )
        output_dir = Path{argv[2]};
    else
        output_dir = FileSystem::CurrentPath();

    CHECK_ERR( RunScript( FileSystem::ToAbsolute(Path{argv[1]}),
                          FileSystem::ToAbsolute(output_dir) ), -2 );
    return 0;
}

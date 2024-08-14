// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/File.h"
#include "base/Algorithms/StringUtils.h"

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"
#include "scripting/Impl/ScriptFn.h"
#include "scripting/Impl/ScriptEngine.inl.h"

#include "vfs/Archive/ArchivePacker.h"

#include "pipeline_compiler/PipelineCompiler.h"
#include "input_actions/InputActionsBinding.h"
#include "asset_packer/AssetPacker.h"

#ifdef AE_OFFLINE_PACKER_USE_STATIC_LIBS
#	define STATIC_LIBS	1
#else
#	define STATIC_LIBS	0
#endif

using namespace AE;
using namespace AE::Base;
using namespace AE::Scripting;


namespace
{
	using EReflectionFlags = PipelineCompiler::EReflectionFlags;

	static Array<Path>	s_SearchDirs;

/*
=================================================
	ConvertArray
=================================================
*/
	struct PathParams2
	{
		BasicString<CharType>	path;
		usize					priority	: 16;
		usize					flags		: 8;
	};

	template <typename R>
	ND_ static Array<R>  ConvertArray (const Array<PathParams2> &src)
	{
		Array<R>	dst;
		dst.resize( src.size() );

		for (usize i = 0; i < src.size(); ++i)
		{
			dst[i].path		= src[i].path.c_str();
			dst[i].priority	= src[i].priority;
			dst[i].flags	= src[i].flags;
		}
		return dst;
	}

/*
=================================================
	ConvertString
=================================================
*/
	ND_ inline BasicString<CharType>  ConvertString (const String &src)
	{
		BasicString<CharType>	dst;
		dst.assign( src.begin(), src.end() );
		return dst;
	}

	ND_ inline BasicString<CharType>  ConvertString (const WString &src)
	{
		BasicString<CharType>	dst;
		dst.assign( src.begin(), src.end() );
		return dst;
	}

	ND_ static BasicString<CharType>  ConvertString (const Path &src)
	{
		return ConvertString( src.native() );
	}

/*
=================================================
	FindPath
=================================================
*/
	ND_ static Path  FindPath (const String &src, StringView msg) __Th___
	{
		Path	path = FileSystem::ToAbsolute( src );

		if ( not FileSystem::IsFileOrDirectory( path ))
		{
			bool	found = false;

			for (const auto& dir : s_SearchDirs)
			{
				Path	path2 = dir / src;

				if ( FileSystem::IsFileOrDirectory( path2 ))
				{
					path	= RVRef(path2);
					found	= true;
					break;
				}
			}
			CHECK_THROW_MSG( found, String{msg} << " '" << src << "' is not exist" );
		}
		return path;
	}

/*
=================================================
	FindPathAndConvertString
=================================================
*/
	ND_ static BasicString<CharType>  FindPathAndConvertString (const String &src, StringView msg) __Th___
	{
		return ConvertString( FindPath( src, msg ));
	}

/*
=================================================
	ConvertArray
=================================================
*/
	ND_ static Array<const CharType *>  ConvertArray (const Array< BasicString<CharType> > &src)
	{
		Array<const CharType *>		dst;
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
		Array< PathParams2 >				_pipelines;

		Array< BasicString<CharType> >		_shaderFolders;
		Array< BasicString<CharType> >		_shaderIncludeDirs;
		Array< BasicString<CharType> >		_pplnIncludeDirs;
		EReflectionFlags					_reflFlags		= Default;

		Path								_outputCppStructsFile;
		Path								_outputCppNamesFile;

		Library												_lib;
		decltype(&AE::PipelineCompiler::CompilePipelines)	_fnCompilePipelines	= null;


	public:
		void  AddPipelineRecursiveFolder (const String &path) __Th___
		{
			return _AddPipeline( path, uint(_pipelines.size()), PipelineCompiler::EPathParamsFlags::RecursiveFolder );
		}

		void  AddPipelineFolder (const String &path) __Th___
		{
			return _AddPipeline( path, uint(_pipelines.size()), PipelineCompiler::EPathParamsFlags::Folder );
		}

		void  AddPipeline (const String &path) __Th___
		{
			return _AddPipeline( path, uint(_pipelines.size()), PipelineCompiler::EPathParamsFlags::File );
		}

		void  AddShaderFolder (const String &path) __Th___
		{
			_shaderFolders.push_back( FindPathAndConvertString( path, "Shader folder" ));
		}

		void  AddShaderIncludeDir (const String &path) __Th___
		{
			_shaderIncludeDirs.push_back( FindPathAndConvertString( path, "Shader include directory" ));
		}

		void  AddPipelineIncludeDir (const String &path) __Th___
		{
			_pplnIncludeDirs.push_back( FindPathAndConvertString( path, "Pipeline include directory" ));
		}

		void  SetOutputCPPFile1 (const String &structs, const String &names, uint flags) __Th___
		{
			SetOutputCPPFile2( structs, names, EReflectionFlags(flags) );
		}

		void  SetOutputCPPFile2 (const String &structs, const String &names, EReflectionFlags flags) __Th___
		{
			CHECK_THROW_MSG( _outputCppStructsFile.empty() );
			CHECK_THROW_MSG( _outputCppNamesFile.empty() );

			_outputCppStructsFile	= FileSystem::ToAbsolute( structs );
			_outputCppNamesFile		= FileSystem::ToAbsolute( names );
			_reflFlags				= flags;
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
		void  _AddPipeline (const String &path, uint priority, PipelineCompiler::EPathParamsFlags flags) __Th___
		{
			PathParams2&	params = _pipelines.emplace_back();
			params.path		= FindPathAndConvertString( path, "Pipeline file/folder" );
			params.priority	= priority;
			params.flags	= usize(flags);
		}

		void  _Compile (const String &outputPackName, const bool addNameMapping) __Th___
		{
			using namespace AE::PipelineCompiler;

			#if STATIC_LIBS
				_fnCompilePipelines = &CompilePipelines;
			#else
			if ( _fnCompilePipelines == null )
			{
				CHECK_THROW_MSG( _lib.Load( AE_PIPELINE_COMPILER_LIBRARY ));
				CHECK_THROW_MSG( _lib.GetProcAddr( "CompilePipelines", OUT _fnCompilePipelines ));
			}
			#endif

			const auto	output_pack_name		= ConvertString( FileSystem::ToAbsolute( outputPackName ));
			const auto	output_cpp_types_file	= ConvertString( _outputCppStructsFile );
			const auto	output_cpp_names_file	= ConvertString( _outputCppNamesFile );

			const auto	pipelines				= ConvertArray<PipelineCompiler::PathParams>( _pipelines );
			const auto	shader_folders			= ConvertArray( _shaderFolders );
			const auto	shader_include_dirs		= ConvertArray( _shaderIncludeDirs );
			const auto	ppln_include_dirs		= ConvertArray( _pplnIncludeDirs );

			PipelinesInfo	info = {};

			// input pipelines
			info.inPipelines			= pipelines.data();
			info.inPipelineCount		= pipelines.size();

			// input shaders
			info.shaderFolders			= shader_folders.data();
			info.shaderFolderCount		= shader_folders.size();

			// shader include directories
			info.shaderIncludeDirs		= shader_include_dirs.data();
			info.shaderIncludeDirCount	= shader_include_dirs.size();

			// pipeline include directories
			info.pipelineIncludeDirs	= ppln_include_dirs.data();
			info.pipelineIncludeDirCount= ppln_include_dirs.size();

			// output
			info.outputPackName			= output_pack_name.c_str();
			info.outputCppStructsFile	= output_cpp_types_file.empty() ? null : output_cpp_types_file.c_str();
			info.outputCppNamesFile		= output_cpp_names_file.empty() ? null : output_cpp_names_file.c_str();
			info.cppReflectionFlags		= _reflFlags;
			info.addNameMapping			= addNameMapping;

			CHECK_THROW_MSG( _fnCompilePipelines( &info ));

			// reset
			_pipelines.clear();
			_shaderFolders.clear();
			_shaderIncludeDirs.clear();
			_pplnIncludeDirs.clear();
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
		Array< BasicString<CharType> >	_files;
		Array< BasicString<CharType> >	_include;
		Path							_outputCppFile;

		Library												_lib;
		decltype(&AE::InputActions::ConvertInputActions)	_fnConvertInputActions	= null;

	public:
		void  Add (const String &filename) __Th___
		{
			_files.push_back( FindPathAndConvertString( filename, "File" ));
		}

		void  Include (const String &folder) __Th___
		{
			_include.push_back( FindPathAndConvertString( folder, "Include directory" ));
		}

		void  SetOutputCPPFile (const String &value) __Th___
		{
			CHECK_THROW_MSG( _outputCppFile.empty() );
			_outputCppFile = FileSystem::ToAbsolute( value );
		}

		void  Convert (const String &outputName) __Th___
		{
			using namespace AE::InputActions;

			#if STATIC_LIBS
				_fnConvertInputActions = &ConvertInputActions;
			#else
			if ( _fnConvertInputActions == null )
			{
				CHECK_THROW_MSG( _lib.Load( AE_ASSET_PACKER_LIBRARY ));
				CHECK_THROW_MSG( _lib.GetProcAddr( "ConvertInputActions", OUT _fnConvertInputActions ));
			}
			#endif

			CHECK_THROW_MSG( not _files.empty() );

			const auto	output		= ConvertString( FileSystem::ToAbsolute( outputName ));
			const auto	files		= ConvertArray( _files );
			const auto	include		= ConvertArray( _include );
			const auto	output_cpp	= ConvertString( _outputCppFile );

			InputActionsInfo	info	= {};
			info.inFiles				= files.data();
			info.inFileCount			= files.size();
			info.inIncludeFolders		= include.data();
			info.inIncludeFolderCount	= include.size();
			info.outputPackName			= output.c_str();
			info.outputCppFile			= output_cpp.empty() ? null : output_cpp.c_str();

			CHECK_THROW_MSG( _fnConvertInputActions( &info ));

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
		Array< PathParams2 >			_files;
		BasicString<CharType>			_tempFile;
		Array< BasicString<CharType> >	_include;

		Library									_lib;
		decltype(&AE::AssetPacker::PackAssets)	_fnPackAssets	= null;

	public:
		void  Add (const String &filename) __Th___
		{
			const auto	ext = Path{filename}.extension().string();
			CHECK_THROW_MSG( ext == ".as" );

			PathParams2&	params = _files.emplace_back();
			params.path		= FindPathAndConvertString( filename, "Asset file" );
			params.priority	= uint(_files.size()-1);
			params.flags	= usize(AssetPacker::EPathParamsFlags::File);
		}

		void  AddFolder (const String &inFolder) __Th___
		{
			PathParams2&	params = _files.emplace_back();
			params.path		= FindPathAndConvertString( inFolder, "Folder with assets" );
			params.priority	= uint(_files.size()-1);
			params.flags	= usize(AssetPacker::EPathParamsFlags::RecursiveFolder);
		}

		void  Include (const String &folder) __Th___
		{
			_include.push_back( FindPathAndConvertString( folder, "Include directory" ));
		}

		void  SetTempFile (const String &fileName) __Th___
		{
			CHECK_THROW_MSG( not FileSystem::IsDirectory( fileName ) or
							 FileSystem::IsFile( fileName ),
				"Temp file '"s << fileName << "' must not be an existing folder" );

			const Path	path {fileName};

			FileSystem::DeleteFile( path );
			FileSystem::CreateDirectories( path.parent_path() );

			_tempFile = ConvertString( FileSystem::ToAbsolute( path ));
		}

		void  ToArchive (const String &outputName) __Th___
		{
			using namespace AE::AssetPacker;

			#if STATIC_LIBS
				_fnPackAssets = &PackAssets;
			#else
			if ( _fnPackAssets == null )
			{
				CHECK_THROW_MSG( _lib.Load( AE_INPUT_ACTIONS_BINDING_LIBRARY ));
				CHECK_THROW_MSG( _lib.GetProcAddr( "PackAssets", OUT _fnPackAssets ));
			}
			#endif

			CHECK_THROW_MSG( not _files.empty() );

			const auto	output		= ConvertString( FileSystem::ToAbsolute( outputName ));
			const auto	files		= ConvertArray<AssetPacker::PathParams>( _files );
			const auto	include		= ConvertArray( _include );

			AssetInfo	info			= {};
			info.inFiles				= files.data();
			info.inFileCount			= files.size();
			info.inIncludeFolders		= include.data();
			info.inIncludeFolderCount	= include.size();
			info.tempFile				= _tempFile.c_str();
			info.outputArchive			= output.c_str();

			CHECK_THROW_MSG( _fnPackAssets( &info ));

			FileSystem::DeleteFile( _tempFile );

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
		VFS::ArchivePacker		_archive;
		EFileType				_defaultType	= EFileType::Raw;

	public:
		ND_ VFS::ArchivePacker&  GetArchive ()
		{
			CHECK_THROW_MSG( _archive.IsCreated() );
			return _archive;
		}

		void  Add1 (const String &name, const String &filename, EFileType type) __Th___
		{
			CHECK_THROW_MSG( _archive.IsCreated() );
			CHECK_THROW_MSG( name.length() < VFS::FileName::MaxStringLength() );
			CHECK_THROW_MSG( _archive.Add( VFS::FileName::WithString_t{name}, FindPath( filename, "VFS file" ), type ));
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
			CHECK_THROW_MSG( _archive.IsCreated() );
			CHECK_THROW_MSG( _archive.AddArchive( FindPath( filename, "Archive" )));
		}

		void  Store (const String &filename) __Th___
		{
			CHECK_THROW_MSG( _archive.IsCreated() );

			Path	path = _archive.TempFilePath();
			CHECK_THROW_MSG( _archive.Store( Path{filename} ),
				"Failed to store archive to '"s << filename << "'" );

			FileSystem::DeleteFile( path );
		}

		void  SetDefaultFileType (EFileType type)
		{
			_defaultType = type;
		}

		void  SetTempFile (const String &fileName) __Th___
		{
			CHECK_THROW_MSG( not _archive.IsCreated() );
			CHECK_THROW_MSG( not FileSystem::IsDirectory( fileName ) or
							 FileSystem::IsFile( fileName ),
				"Temp file '"s << fileName << "' must not be an existing folder" );

			const Path	path = FileSystem::ToAbsolute( fileName );

			FileSystem::DeleteFile( path );
			FileSystem::CreateDirectories( path.parent_path() );

			CHECK_THROW_MSG( _archive.Create( path ));
		}
	};

} // namespace


AE_DECL_SCRIPT_OBJ_RC(	ScriptPipelineCompiler,	"PipelineCompiler"	);
AE_DECL_SCRIPT_OBJ_RC(	ScriptInputActions,		"InputActions"		);
AE_DECL_SCRIPT_OBJ_RC(	ScriptAssetPacker,		"AssetPacker"		);
AE_DECL_SCRIPT_OBJ_RC(	ScriptArchive,			"Archive"			);
AE_DECL_SCRIPT_TYPE(	EFileType,				"EFileType"			);
AE_DECL_SCRIPT_TYPE(	EReflectionFlags,		"EReflectionFlags"	);


namespace
{
/*
=================================================
	GetSharedFeatureSetPath
	GetSharedShadersPath
	GetCanvasVerticesPath
	GetUIBindingsPath
	GetOutputDir
	IsGLSLCompilerSupported
	IsMetalCompilerSupported
=================================================
*/
	static String	GetSharedFeatureSetPath ()	{ return AE_SHARED_DATA "/feature_set"; }
	static String	GetSharedShadersPath ()		{ return AE_SHARED_DATA "/shaders"; }
	static String	GetCanvasVerticesPath ()	{ return AE_CANVAS_VERTS; }
	static String	GetUIBindingsPath ()		{ return AE_UI_BINDINGS; }

	static String	_s_OutputDir;
	static String	GetOutputDir ()				{ return _s_OutputDir; }

	static bool		IsGLSLCompilerSupported ()
	{
	#if defined(AE_ENABLE_GLSLANG) or defined(AE_PIPELINE_COMPILER_LIBRARY)
		return true;
	#else
		return false;
	#endif
	}

	static bool		IsMetalCompilerSupported ()
	{
	#if defined(AE_METAL_TOOLS) and defined(AE_ENABLE_SPIRV_CROSS)
		return true;
	#else
		return false;
	#endif
	}

/*
=================================================
	DeleteFolder
=================================================
*/
	static void  DeleteFolder (const String &folder)
	{
		FileSystem::DeleteDirectory( Path{folder} );
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
		CoreBindings::BindLog( se );

		se->AddFunction( &GetSharedFeatureSetPath,	"GetSharedFeatureSetPath"	);
		se->AddFunction( &GetSharedShadersPath,		"GetSharedShadersPath"		);
		se->AddFunction( &GetCanvasVerticesPath,	"GetCanvasVerticesPath"		);
		se->AddFunction( &GetUIBindingsPath,		"GetUIBindingsPath"			);
		se->AddFunction( &GetOutputDir,				"GetOutputDir"				);
		se->AddFunction( &DeleteFolder,				"DeleteFolder"				);
		se->AddFunction( &IsGLSLCompilerSupported,	"IsGLSLCompilerSupported"	);
		se->AddFunction( &IsMetalCompilerSupported,	"IsMetalCompilerSupported"	);

		// pipeline compiler path params
		{
			EnumBinder<EFileType>	binder{ se };
			binder.Create();
			switch_enum( EFileType::Unknown )
			{
				case EFileType::Unknown :
				case EFileType::All :
				case EFileType::_Last :
				#define CASE( _name_ )	case EFileType::_name_ :  binder.AddValue( #_name_, EFileType::_name_ );
				CASE( Raw )
				CASE( Brotli )
				CASE( InMemory )
				CASE( BrotliInMemory )
				CASE( ZStd )
				CASE( ZStdInMemory )
				#undef CASE
				default : break;
			}
			switch_end
		}

		//
		{
			EnumBinder<EReflectionFlags>	binder{ se };
			binder.Create();
			switch_enum( EReflectionFlags::Unknown )
			{
				case EReflectionFlags::Unknown :
				case EReflectionFlags::_Last :
				#define CASE( _name_ )	case EReflectionFlags::_name_ :  binder.AddValue( #_name_, EReflectionFlags::_name_ );
				CASE( RenderTechniques )
				CASE( RTechPass_Pipelines )
				CASE( RTech_ShaderBindingTable )
				CASE( All )
				#undef CASE
				default : break;
			}
			switch_end
		}

		// pipeline compiler
		{
			ClassBinder<ScriptPipelineCompiler>		binder{ se };
			binder.CreateRef();
			binder.AddMethod( &ScriptPipelineCompiler::AddPipelineFolder,			"AddPipelineFolder"			);
			binder.AddMethod( &ScriptPipelineCompiler::AddPipelineRecursiveFolder,	"AddPipelineFolderRecursive");
			binder.AddMethod( &ScriptPipelineCompiler::AddPipeline,					"AddPipeline"				);
			binder.AddMethod( &ScriptPipelineCompiler::AddShaderFolder,				"AddShaderFolder"			);
			binder.AddMethod( &ScriptPipelineCompiler::AddShaderIncludeDir,			"ShaderIncludeDir"			);
			binder.AddMethod( &ScriptPipelineCompiler::AddPipelineIncludeDir,		"PipelineIncludeDir"		);

			binder.AddMethod( &ScriptPipelineCompiler::SetOutputCPPFile1,			"SetOutputCPPFile"			);
			binder.AddMethod( &ScriptPipelineCompiler::SetOutputCPPFile2,			"SetOutputCPPFile"			);
			binder.AddMethod( &ScriptPipelineCompiler::Compile1,					"Compile"					);
			binder.AddMethod( &ScriptPipelineCompiler::Compile4,					"CompileWithNameMapping"	);
		}

		// input actions
		{
			ClassBinder<ScriptInputActions>		binder{ se };
			binder.CreateRef();
			binder.AddMethod( &ScriptInputActions::Add,					"Add"				);
			binder.AddMethod( &ScriptInputActions::Include,				"Include"			);
			binder.AddMethod( &ScriptInputActions::SetOutputCPPFile,	"SetOutputCPPFile"	);
			binder.AddMethod( &ScriptInputActions::Convert,				"Convert"			);
		}

		// asset packer
		{
			ClassBinder<ScriptAssetPacker>		binder{ se };
			binder.CreateRef();
			binder.AddMethod( &ScriptAssetPacker::Add,					"Add"				);
			binder.AddMethod( &ScriptAssetPacker::AddFolder,			"AddFolder"			);
			binder.AddMethod( &ScriptAssetPacker::Include,				"Include"			);
			binder.AddMethod( &ScriptAssetPacker::SetTempFile,			"SetTempFile"		);
			binder.AddMethod( &ScriptAssetPacker::ToArchive,			"ToArchive"			);
		}

		// archive
		{
			ClassBinder<ScriptArchive>		binder{ se };
			binder.CreateRef();
			binder.Comment( "Initialize archive, set path to temporary file which will be used to store archive before 'Store()' call." );
			binder.AddMethod( &ScriptArchive::SetTempFile,				"SetTempFile"			);
			binder.AddMethod( &ScriptArchive::SetDefaultFileType,		"SetDefaultFileType"	);
			binder.AddMethod( &ScriptArchive::Add1,						"Add",					{"nameInArchive", "filePath", "archiveFileType"} );
			binder.AddMethod( &ScriptArchive::Add2,						"Add",					{"filePath", "archiveFileType"} );
			binder.AddMethod( &ScriptArchive::Add3,						"Add",					{"nameInArchive", "filePath"} );
			binder.AddMethod( &ScriptArchive::Add4,						"Add",					{"filePath"} );
			binder.AddMethod( &ScriptArchive::AddArchive,				"AddArchive"			);
			binder.AddMethod( &ScriptArchive::Store,					"Store"					);
		}
	}

/*
=================================================
	RunScript
=================================================
*/
	ND_ static bool  RunScript (const Path &respackScript, const Path &outputDir)
	{
		{
			String	str = "OfflinePacker args: \n";
			str << "-i \""s << ToString(respackScript) << "\" -o \"" << ToString(outputDir) << "\"\n";
			for (auto& dir : s_SearchDirs) {
				str << " -d \"" << ToString(dir) << "\"\n";
			}
			AE_LOGI( str );
		}

		CHECK_ERR( FileSystem::IsFile( respackScript ));

		FileSystem::SetCurrentPath( respackScript.parent_path() );

		FileSystem::CreateDirectories( outputDir );
		CHECK_ERR( FileSystem::IsDirectory( outputDir ));

		_s_OutputDir = ToString(outputDir);
		CHECK_ERR( FileSystem::Equal( outputDir, Path{_s_OutputDir} ));	// if path has unicode
		_s_OutputDir << '/';

		const auto	ansi_path = ToString( respackScript );
		String		script;

		{
			FileRStream		file {respackScript};

			CHECK_ERR_MSG( file.IsOpen(),
				"Failed to open script '"s << ansi_path << "'" );

			CHECK_ERR_MSG( file.Read( file.RemainingSize(), OUT script ),
				"Failed to read script '"s << ansi_path << "'" );
		}

		ScriptEnginePtr	se = MakeRC<ScriptEngine>();
		CHECK_ERR( se->Create( True{"gen cpp header"} ));

		NOTHROW_ERR( Bind( se ));

		auto	mod = se->CreateModule({ScriptEngine::ModuleSource{ "def"s, RVRef(script), SourceLoc{ansi_path}, True{"preprocessor"} }});
		CHECK_ERR( mod );

		auto	scr = se->CreateScript< void () >( "ASmain", mod );
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
#ifndef AE_OFFLINE_PACKER_LIB

	int main (int argc, char* argv[])
	{
		AE::Base::StaticLogger::LoggerScope log{};	// don't check for memleak because of false possitive in 'SpirvToMsl'

		Path	input_script;
		Path	output_dir		= FileSystem::CurrentPath();

		s_SearchDirs.clear();

		for (int i = 1; i+1 < argc; i += 2)
		{
			auto	type	= StringView{argv[i+0]};
			if ( type == "-i" )
				input_script = FileSystem::ToAbsolute( Path{ argv[i+1] });
			else
			if ( type == "-o" )
				output_dir = FileSystem::ToAbsolute( Path{ argv[i+1] });
			else
			if ( type == "-d" )
				s_SearchDirs.push_back( FileSystem::ToAbsolute( Path{ argv[i+1] }));
			else
				RETURN_ERR( "unknown command: '"s << type << "' + '" << argv[i+1] << "'", -1 );
		}

		CHECK_ERR( RunScript( input_script, output_dir ), -2 );
		return 0;
	}

#endif

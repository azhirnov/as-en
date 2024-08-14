// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/File.h"
#include "scripting/Impl/ScriptFn.h"
#include "scripting/Impl/ScriptEngine.inl.h"
#include "ScriptObjects/ObjectStorage.h"
#include "AssetPacker.h"

namespace AE::AssetPacker
{
namespace
{
	AE_BIT_OPERATORS( EPathParamsFlags );

	using namespace AE::Scripting;

/*
=================================================
	BuildFileList
=================================================
*/
	ND_ static bool  BuildFileList (const AssetInfo* info, OUT Array<Path> &outFiles)
	{
		struct PathInfo
		{
			Path				path;
			uint				priority	= 0;
			EPathParamsFlags	flags		= Default;
		};

		HashSet< Path, PathHasher >		unique_files;
		Deque<PathInfo>					folders;
		Array<PathInfo>					sorted_files;

		for (usize i = 0; i < info->inFileCount; ++i)
		{
			auto&	item = info->inFiles[i];

			if ( not AnyBits( item.flags, EPathParamsFlags::Folder | EPathParamsFlags::RecursiveFolder ))
				continue;

			Path	path {item.path};

			if ( not FileSystem::IsDirectory( path ))
			{
				AE_LOGW( "Can't find folder: '"s << ToString(path) << "'" );
				continue;
			}

			folders.push_back( PathInfo{ RVRef(path), uint(item.priority), EPathParamsFlags(item.flags) });
		}

		std::sort( folders.begin(), folders.end(), [](auto& lhs, auto& rhs) { return lhs.priority < rhs.priority; });

		for (; not folders.empty();)
		{
			const Path	path	= RVRef(folders.front().path);
			const uint	prio	= folders.front().priority;
			const auto	flags	= folders.front().flags;
			folders.pop_front();

			for (auto& file : FileSystem::Enum( path ))
			{
				if ( file.IsDirectory() and AllBits( flags, EPathParamsFlags::RecursiveFolder ))
				{
					folders.push_back( PathInfo{ file.Get(), prio, flags });
					continue;
				}

				Path	tmp = FileSystem::ToAbsolute( file.Get() );
				if ( not unique_files.insert( tmp ).second )
					continue;

				if ( tmp.extension() != ".as" )
					continue;

				sorted_files.push_back( PathInfo{ RVRef(tmp), prio, EPathParamsFlags::Unknown });
			}
		}

		for (usize i = 0; i < info->inFileCount; ++i)
		{
			if ( AnyBits( info->inFiles[i].flags, EPathParamsFlags::Folder | EPathParamsFlags::RecursiveFolder ))
				continue;

			Path	path{ info->inFiles[i].path };

			if ( not FileSystem::IsFile( path ))
			{
				AE_LOGW( "Can't find file: '"s << ToString(path) << "'" );
				continue;
			}

			path = FileSystem::ToAbsolute( path );
			if ( not unique_files.insert( path ).second )
				continue;

			sorted_files.push_back( PathInfo{ RVRef(path), uint(info->inFiles[i].priority), EPathParamsFlags::Unknown });
		}

		std::sort( sorted_files.begin(), sorted_files.end(), [](auto& lhs, auto& rhs) { return lhs.priority < rhs.priority; });

		outFiles.reserve( sorted_files.size() );
		for (auto& item : sorted_files) {
			outFiles.push_back( RVRef(item.path) );
		}

		return true;
	}

/*
=================================================
	PackAssetsImpl
=================================================
*/
	ND_ static bool  PackAssetsImpl (const AssetInfo* info)
	{
		CHECK_ERR( info != null );
		CHECK_ERR( (info->inFileCount > 0) and (info->inFiles != null) );
		CHECK_ERR( (info->inIncludeFolderCount > 0) == (info->inIncludeFolders != null) );
		CHECK_ERR( info->tempFile != null );
		CHECK_ERR( info->outputArchive != null );

		ScriptEnginePtr		script_engine = MakeRC<ScriptEngine>();
		ObjectStorage		obj_storage;
		ObjectStorage::SetInstance( &obj_storage );

		CHECK_ERR( obj_storage.Initialize( Path{info->tempFile} ));
		NOTHROW_ERR( ObjectStorage::Bind( script_engine ));

		Array<Path>		script_include_dirs;
		for (usize i = 0; i < info->inIncludeFolderCount; ++i)
		{
			Path	path {info->inIncludeFolders[i]};
			if ( FileSystem::IsDirectory( path ))
				script_include_dirs.push_back( RVRef(path) );
			else
				AE_LOGI( "Skip invalid include directory: '"s << ToString(path) << "'" );
		}

		Array<Path>		script_files;
		CHECK_ERR( BuildFileList( info, OUT script_files ));

		for (const Path& path : script_files)
		{
			const String				ansi_path	= ToString(path);
			ScriptEngine::ModuleSource	src;

			{
				FileRStream		file {path};

				if ( not file.IsOpen() )
				{
					AE_LOGI( "Failed to open script file: '"s << ansi_path << "'" );
					continue;
				}

				src.name = ToString( path.stem() );

				if ( not file.Read( file.RemainingSize(), OUT src.script ))
				{
					AE_LOGI( "Failed to read script file: '"s << ansi_path << "'" );
					continue;
				}
			}

			src.dbgLocation		= SourceLoc{ ansi_path, 0 };
			src.usePreprocessor	= true;

			ScriptModulePtr		module = script_engine->CreateModule( {src}, {"SCRIPT"}, script_include_dirs );
			if ( not module )
			{
				AE_LOGI( "Failed to parse script file: '"s << ansi_path << "'" );
				continue;
			}

			auto	fn = script_engine->CreateScript< void() >( "ASmain", module );
			if ( not fn )
			{
				AE_LOGI( "Failed to create script context for file: '"s << ansi_path << "'" );
				continue;
			}

			obj_storage.SetScriptFolder( path.parent_path() );

			if ( not fn->Run() )
				return false;
		}
		CHECK_ERR_MSG( not obj_storage.HasHashCollisions(), "Hash collision detected!" );

		const Path	arch_fname	= FileSystem::ToAbsolute( info->outputArchive );
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
	extern "C" bool AE_AP_API  PackAssets (const AssetInfo* info)
	{
		AE::Base::StaticLogger::LoggerScope log{};

		const auto	path = FileSystem::CurrentPath();
		const bool	res	 = PackAssetsImpl( info );

		ObjectStorage::SetInstance( null );

		// restore current path
		FileSystem::SetCurrentPath( path );

		return res;
	}

} // AE::AssetPacker

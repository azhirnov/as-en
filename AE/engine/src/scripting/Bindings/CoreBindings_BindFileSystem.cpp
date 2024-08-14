// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
/*
=================================================
	FileSystem_DirectoryIter
=================================================
*/
	struct FileSystem_DirectoryIter
	{
		FileSystem::DirectoryIter	_it;

		FileSystem_DirectoryIter ()	{}
		FileSystem_DirectoryIter (const Path &p) : _it{FileSystem::Enum(p)} {}

		void	Inc ()				__Th___	{ CHECK_THROW( IsValid() );  ++_it; }
		bool	IsFile ()			C_Th___	{ CHECK_THROW( IsValid() );  return _it->IsFile(); }
		bool	IsDirectory ()		C_Th___	{ CHECK_THROW( IsValid() );  return _it->IsDirectory(); }
		bool	IsValid ()			C_Th___	{ return _it != Default; }

		String	Path ()				C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get() ); }
		String	Extension ()		C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().extension() ); }
		String	FileName ()			C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().filename() ); }
		String	Stem ()				C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().stem() ); }
		String	ParentPath ()		C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().parent_path() ); }

		static void  Ctor1 (void* mem, const String &path)
		{
			PlacementNew<FileSystem_DirectoryIter>( OUT mem, path );
		}
	};

/*
=================================================
	FileSystem_RecursiveDirectoryIter
=================================================
*/
	struct FileSystem_RecursiveDirectoryIter
	{
		FileSystem::RecursiveDirectoryIter	_it;

		FileSystem_RecursiveDirectoryIter ()	{}
		FileSystem_RecursiveDirectoryIter (const Path &p) : _it{FileSystem::EnumRecursive(p)} {}

		void	Inc ()				__Th___	{ CHECK_THROW( IsValid() );  ++_it; }
		bool	IsFile ()			C_Th___	{ CHECK_THROW( IsValid() );  return _it->IsFile(); }
		bool	IsDirectory ()		C_Th___	{ CHECK_THROW( IsValid() );  return _it->IsDirectory(); }
		bool	IsValid ()			C_Th___	{ return _it != Default; }

		String	Path ()				C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get() ); }
		String	Extension ()		C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().extension() ); }
		String	FileName ()			C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().filename() ); }
		String	Stem ()				C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().stem() ); }
		String	ParentPath ()		C_Th___	{ CHECK_THROW( IsValid() );  return ToString( _it->Get().parent_path() ); }

		static void  Ctor1 (void* mem, const String &path)
		{
			PlacementNew<FileSystem_RecursiveDirectoryIter>( OUT mem, path );
		}
	};

/*
=================================================
	ReadTextFile
=================================================
*/
	static String  ReadTextFile (const String &path) __Th___
	{
		FileRStream	stream {path};
		String		result;

		CHECK_THROW_MSG( stream.IsOpen(),
			"Can't open file '"s << path << "' for reading" );

		CHECK_THROW_MSG( stream.Read( stream.RemainingSize(), OUT result ),
			"Failed to read file '"s << path << "'" );

		return result;
	}

/*
=================================================
	WriteTextFile
=================================================
*/
	static void  WriteTextFile (const String &path, const String &text) __Th___
	{
		FileWStream	stream {path};

		CHECK_THROW_MSG( stream.IsOpen(),
			"Can't open file '"s << path << "' for writing" );

		CHECK_THROW_MSG( stream.Write( text ),
			"Failed to write to a file '"s << path << "'" );
	}

/*
=================================================
	AppendTextFile
=================================================
*/
	static void  AppendTextFile (const String &path, const String &text) __Th___
	{
		FileWStream	stream { path, FileWStream::EMode::OpenAppend };

		CHECK_THROW_MSG( stream.IsOpen(),
			"Can't open file '"s << path << "' for append" );

		CHECK_THROW_MSG( stream.Write( text ),
			"Failed to append to a file '"s << path << "'" );
	}

} // namespace
} // AE::Scripting


AE_DECL_SCRIPT_OBJ( AE::Scripting::FileSystem_DirectoryIter,			"FS_DirectoryIterator"		);
AE_DECL_SCRIPT_OBJ( AE::Scripting::FileSystem_RecursiveDirectoryIter,	"FS_RecursiveDirectoryIter"	);


namespace AE::Scripting
{
namespace
{
/*
=================================================
	FileSystem_DirectoryIter_Bind
=================================================
*/
	template <typename T>
	static void  FileSystem_DirectoryIter_Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<T>	binder {se};
		binder.CreateClassValue();
		binder.AddConstructor( &T::Ctor1,	{"folder"} );
		binder.AddMethod( &T::IsFile,		"IsFile"		);
		binder.AddMethod( &T::IsDirectory,	"IsDirectory"	);
		binder.AddMethod( &T::IsValid,		"IsValid"		);
		binder.AddMethod( &T::Inc,			"Inc"			);
		binder.AddMethod( &T::Path,			"Path"			);
		binder.AddMethod( &T::Extension,	"Extension"		);
		binder.AddMethod( &T::FileName,		"FileName"		);
		binder.AddMethod( &T::Stem,			"Stem"			);
		binder.AddMethod( &T::ParentPath,	"ParentPath"	);
	}

} // namespace

/*
=================================================
	BindFileSystem
=================================================
*/
	void  CoreBindings::BindFileSystem (const ScriptEnginePtr &se, bool allowWrite) __Th___
	{
		FileSystem_DirectoryIter_Bind< FileSystem_DirectoryIter >( se );
		FileSystem_DirectoryIter_Bind< FileSystem_RecursiveDirectoryIter >( se );

		se->AddFunction( &ReadTextFile,		"ReadTextFile",		{"path"} );

		if ( allowWrite )
		{
			se->AddFunction( &WriteTextFile,		"WriteFile",		{"path", "text"} );
			se->AddFunction( &AppendTextFile,		"AppendFile",		{"path", "text"} );
		}
	}

} // AE::Scripting

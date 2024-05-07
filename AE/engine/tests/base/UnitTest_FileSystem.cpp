// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	static void  FileSystem_SearchForward_Test ()
	{
		Path	res;
		TEST( FileSystem::SearchForward( FileSystem::CurrentPath(), Path{"b1.txt"}, 3, OUT res ));

		auto	ref = FileSystem::ToAbsolute( Path{"b/b1.txt"} );
		TEST( res == ref );

		TEST( not FileSystem::SearchForward( Path{"b4.txt"}, 3, OUT res ));
		TEST( not FileSystem::SearchForward( FileSystem::CurrentPath().append("a"), Path{"b1.txt"}, 3, OUT res ));
	}


	static void  FileSystem_SearchBackward_Test ()
	{
		Path	res;
		TEST( FileSystem::SearchBackward( FileSystem::CurrentPath().append("a/aa"), Path{"b/b1.txt"}, 3, OUT res ));

		auto	ref = FileSystem::ToAbsolute( Path{"b/b1.txt"} );
		TEST( res == ref );

		TEST( not FileSystem::SearchBackward( FileSystem::CurrentPath().append("a/aa"), Path{"b1.txt"}, 3, OUT res ));
	}


	static void  FileSystem_Search_Test ()
	{
		Path	res;
		TEST( FileSystem::Search( FileSystem::CurrentPath(),				Path{"b1.txt"}, 3, 3, OUT res ));
		TEST( FileSystem::Search( FileSystem::CurrentPath().append("a"),	Path{"b1.txt"}, 3, 3, OUT res ));
		TEST( FileSystem::Search( FileSystem::CurrentPath().append("a/aa"),	Path{"b1.txt"}, 3, 3, OUT res ));
	}


	static void  FileSystem_FindUnique_Test ()
	{
		const Path	path {"temp"};
		FileSystem::CreateDirectory( path );

		usize	i		= 0;
		usize	count	= 0;

		const auto	BuildName = [&] (OUT Path &p, usize idx)
		{{
			++count;
			p = path / (ToString(idx) << ".a");
		}};

		const auto	Consume = [&] (const Path &p) -> bool
		{{
			TEST( p.stem() == ToString(i) );

			FileSystem::CreateEmptyFile( p );
			return true; // exit
		}};

		for (; i < 1000; ++i)
		{
			FileSystem::FindUnusedFilename( BuildName, Consume );
		}

		FileSystem::DeleteDirectory( path );

		TEST( count < 13'000 );
	}


	static void  FileSystem_MakeUniqueName_Test ()
	{
		Path	fname = "unique.file-name.txt";
		TEST( FileSystem::MakeUniqueName( INOUT fname ));
		TEST( fname == "unique.file-name-0.txt" );
	}


	static void  FileSystem_MergeDirectory_Test ()
	{
		{
			TEST( FileSystem::CopyDirectory( "b", "temp1" ));
			TEST( FileSystem::MergeDirectory( "c", "temp1", FileSystem::ECopyOpt::FileKeep ));
			TEST( FileSystem::DeleteFile( "temp1/b1.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b2.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b3.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/c1.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/c2.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/c3.txt" ));
			TEST( FileSystem::DeleteEmptyDirectory( "temp1" ));
		}
		{
			TEST( FileSystem::CopyDirectory( "b", "temp1" ));
			TEST( FileSystem::MergeDirectory( "b", "temp1", FileSystem::ECopyOpt::FileKeepBoth ));
			TEST( FileSystem::DeleteFile( "temp1/b1.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b2.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b3.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b1-0.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b2-0.txt" ));
			TEST( FileSystem::DeleteFile( "temp1/b3-0.txt" ));
			TEST( FileSystem::DeleteEmptyDirectory( "temp1" ));
		}
	}


	static void  FileSystem_Test1 ()
	{
		Path	p0 = FileSystem::ToAbsolute( Path{} );
		TEST_Eq( p0, FileSystem::CurrentPath() );

		TEST( FileSystem::SetCurrentPath( Path{} ));
		TEST( FileSystem::CreateDirectories( Path{} ));

		Path	p1 = FileSystem::Normalize( Path{} );
		TEST_Eq( p1, Path{} );
	}


	static void  FileSystem_Init ()
	{
		TEST( FileSystem::CreateDirectories( "a/aa" ));
		TEST( FileSystem::CreateDirectory( "b" ));
		TEST( FileSystem::CreateDirectory( "c" ));

		TEST( FileSystem::CreateEmptyFile( "1.txt" ));
		TEST( FileSystem::CreateEmptyFile( "2.txt" ));
		TEST( FileSystem::CreateEmptyFile( "3.txt" ));
		TEST( FileSystem::CreateEmptyFile( "unique.file-name.txt" ));

		TEST( FileSystem::CreateEmptyFile( "a/a1.txt" ));
		TEST( FileSystem::CreateEmptyFile( "a/a2.txt" ));
		TEST( FileSystem::CreateEmptyFile( "a/a3.txt" ));

		TEST( FileSystem::CreateEmptyFile( "a/aa/aa1.txt" ));

		TEST( FileSystem::CreateEmptyFile( "b/b1.txt" ));
		TEST( FileSystem::CreateEmptyFile( "b/b2.txt" ));
		TEST( FileSystem::CreateEmptyFile( "b/b3.txt" ));

		TEST( FileSystem::CreateEmptyFile( "c/c1.txt" ));
		TEST( FileSystem::CreateEmptyFile( "c/c2.txt" ));
		TEST( FileSystem::CreateEmptyFile( "c/c3.txt" ));
	}
}


extern void UnitTest_FileSystem (const Path &curr)
{
	const Path	folder = curr / "fs_test";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	FileSystem_Init();

	FileSystem_SearchForward_Test();
	FileSystem_SearchBackward_Test();
	FileSystem_Search_Test();
	FileSystem_FindUnique_Test();
	FileSystem_MakeUniqueName_Test();

	FileSystem_MergeDirectory_Test();

	FileSystem_Test1();

	FileSystem::SetCurrentPath( curr );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}

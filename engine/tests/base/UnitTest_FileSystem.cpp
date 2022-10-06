// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/FileSystem.h"
#include "base/Platforms/WindowsFile.h"
#include "base/Platforms/ThreadUtils.h"
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

	
#ifdef AE_PLATFORM_WINDOWS
	static void  WinFile_Test1 ()
	{
		WindowsRFile	rfile{ Path{"test_data.txt"}, WindowsRFile::EFlags::SequentialScan };
		TEST( rfile.IsOpen() );

		String	data;
		TEST( rfile.Read( rfile.Size(), OUT data ));

		TEST( data == "o;ianlisdkjbnpoqwi1u4iehwquifnjsduhcbosdifhapodi" );
	}

	static void  WinFile_Test2 ()
	{
		WindowsRFile	rfile{ Path{"test_data.txt"}, WindowsRFile::EFlags::Async };
		TEST( rfile.IsOpen() );

		const auto	AsyncRead = [](void* userData, Bytes readn, Bytes offset, const void* buffer) {{
									Unused( offset );
									*Cast<StringView>(userData) = StringView{ Cast<char>(buffer), usize(readn) };
								 }};

		char		buf1[512] = {};
		char		buf2[512] = {};
		StringView	range1, range2;

		TEST( rfile.ReadAsync( 10_b, buf1, Bytes::SizeOf(buf1), &range1, AsyncRead ));
		TEST( rfile.ReadAsync( 0_b, buf2, 10_b, &range2, AsyncRead ));

		TEST( ThreadUtils::WaitIO( milliseconds{10} ));

		TEST( range1 == "jbnpoqwi1u4iehwquifnjsduhcbosdifhapodi" );
		TEST( range2 == "o;ianlisdk" );
	}
#endif
}


extern void UnitTest_FileSystem ()
{
	CHECK_FATAL( FileSystem::FindAndSetCurrent( "fs_test", 5 ));

	FileSystem_SearchForward_Test();
	FileSystem_SearchBackward_Test();
	FileSystem_Search_Test();

	#ifdef AE_PLATFORM_WINDOWS
	WinFile_Test1();
	WinFile_Test2();
	#endif

	TEST_PASSED();
}

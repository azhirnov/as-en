// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	ND_ static Array<ubyte>  GenRandomArray (Bytes size)
	{
		Array<ubyte>	temp;
		temp.resize( usize(size) );

		Base::Random	rnd;
		for (usize i = 0; i < temp.size(); ++i)
		{
			temp[i] = rnd.Uniform<ubyte>() & 0xF;
		}
		return temp;
	}

	static void  CreateFile (const Path &path, Bytes size)
	{
		FileWStream		file {path};
		TEST( file.IsOpen() );

		auto	arr = GenRandomArray( size );
		TEST( file.Write( arr.data(), ArraySizeOf(arr) ));

		file.Flush();
	}

	static void  InitFS (const Path &watchDir)
	{
		FileSystem::DeleteDirectory( watchDir );
		FileSystem::CreateDirectory( watchDir );

		CreateFile( watchDir / "a0.tmp", 1_KiB );
		CreateFile( watchDir / "a1.tmp", 2_KiB );
		CreateFile( watchDir / "a2.tmp", 3_KiB );
		CreateFile( watchDir / "a3.tmp", 4_KiB );

		FileSystem::CreateDirectory( watchDir / "AA" );
		CreateFile( watchDir / "AA/b0.tmp", 1_KiB );
		CreateFile( watchDir / "AA/b1.tmp", 2_KiB );
		CreateFile( watchDir / "AA/b3.tmp", 3_KiB );
		
		FileSystem::CreateDirectory( watchDir / "BB" );
		CreateFile( watchDir / "BB/c0.tmp", 1_KiB );
		CreateFile( watchDir / "BB/c1.tmp", 2_KiB );
		CreateFile( watchDir / "BB/c2.tmp", 3_KiB );
	}
	
	static void  AppendFile (const Path &path, Bytes size)
	{
		FileWStream		file { path, FileWStream::EMode::OpenAppend };
		TEST( file.IsOpen() );

		auto	arr = GenRandomArray( size );
		TEST( file.Write( arr.data(), ArraySizeOf(arr) ));

		file.Flush();
	}

	inline void  LogEvents (const FileWatch::EventArray_t &events)
	{
		String	str {"Events:\n"};
		for (auto& ev : events)
		{
			str << "  path: '" << ToString( ev.path ) << "', action: " << ToString( ev.action ) << '\n';
		}
		AE_LOGI( str );
	}

	static void  Compare (INOUT FileWatch::EventArray_t &events, const FileWatch::EventArray_t &ref, Bool sortEvents = False{})
	{
		if ( sortEvents )
			std::sort( events.begin(), events.end(), [](auto& lhs, auto& rhs) { return lhs.path < rhs.path; });

		LogEvents( events );

		TEST_Eq( events.size(), ref.size() );

		auto	lhs_it	= events.begin();
		auto	rhs_it	= ref.begin();

		for (usize i = 0; i < events.size(); ++i, ++lhs_it, ++rhs_it)
		{
			TEST_Eq( lhs_it->path, rhs_it->path );
			TEST_Eq( lhs_it->action, rhs_it->action );
		}
	}


#ifdef AE_PLATFORM_WINDOWS
	static void FileWatch_Test1 (const Path &watchDir)
	{
		InitFS( watchDir );

		EFileSystemWatchBits filter;
		filter.insert( EFileSystemWatch::FileCreated );
		filter.insert( EFileSystemWatch::FileDeleted );
		filter.insert( EFileSystemWatch::FileModified );
		filter.insert( EFileSystemWatch::DirCreated );
		filter.insert( EFileSystemWatch::DirDeleted );
		filter.insert( EFileSystemWatch::DirModified );

		FileWatch	file_watch;
		TEST( file_watch.Start( watchDir, filter ));

		FileWatch::EventArray_t	events;
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::DeleteFile( watchDir / "BB/c2.tmp" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB/c2.tmp",	EFileSystemAction::Removed}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		CreateFile( watchDir / "BB/c3.tmp", 4_KiB );

		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB",			EFileSystemAction::Modified}} );

		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB/c3.tmp",	EFileSystemAction::Added},
								{"BB/c3.tmp",	EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::Rename( watchDir / "a0.tmp", watchDir / "AA/a0.tmp" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"AA",			EFileSystemAction::Modified}} );

		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"a0.tmp",		EFileSystemAction::Removed},
								{"AA/a0.tmp",	EFileSystemAction::Added},
								{"AA",			EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::DeleteDirectory( watchDir / "BB" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB",			EFileSystemAction::Modified}} );
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB/c0.tmp",	EFileSystemAction::Removed},
								{"BB/c1.tmp",	EFileSystemAction::Removed},
								{"BB/c3.tmp",	EFileSystemAction::Removed},
								{"BB",			EFileSystemAction::Modified},
								{"BB",			EFileSystemAction::Removed}} );

		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------
		
		TEST( FileSystem::CreateDirectory( watchDir / "CCC" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC",			EFileSystemAction::Added}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		AppendFile( watchDir / "AA/a0.tmp", 1_KiB );
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"AA/a0.tmp",	EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		file_watch.Stop();
	}
#endif

#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
	static void FileWatch_Test1 (const Path &watchDir)
	{
		InitFS( watchDir );

		EFileSystemWatchBits filter;
		filter.insert( EFileSystemWatch::FileCreated );
		filter.insert( EFileSystemWatch::FileDeleted );
		filter.insert( EFileSystemWatch::FileModified );
		filter.insert( EFileSystemWatch::DirCreated );
		filter.insert( EFileSystemWatch::DirDeleted );
		filter.insert( EFileSystemWatch::DirModified );

		FileWatch	file_watch;
		TEST( file_watch.Start( watchDir, filter ));

		FileWatch::EventArray_t	events;
		TEST( not file_watch.GetEvents( OUT events ));
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::DeleteFile( watchDir / "BB/c2.tmp" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB/c2.tmp",		EFileSystemAction::Removed}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		CreateFile( watchDir / "BB/c3.tmp", 4_KiB );

		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB/c3.tmp",		EFileSystemAction::Added},
								{"BB/c3.tmp",		EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::Rename( watchDir / "a0.tmp", watchDir / "AA/a0.tmp" ));

		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"a0.tmp",			EFileSystemAction::Renamed_OldName},
								{"AA/a0.tmp",		EFileSystemAction::Renamed_NewName}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		TEST( FileSystem::DeleteDirectory( watchDir / "BB" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"BB",				EFileSystemAction::Removed},
								{"BB/c0.tmp",		EFileSystemAction::Removed},
								{"BB/c1.tmp",		EFileSystemAction::Removed},
								{"BB/c3.tmp",		EFileSystemAction::Removed}}, True{"sort"} );

		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------
		
		TEST( FileSystem::CreateDirectory( watchDir / "CCC" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC",				EFileSystemAction::Added}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		AppendFile( watchDir / "AA/a0.tmp", 1_KiB );
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"AA/a0.tmp",		EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------
		
		CreateFile( watchDir / "CCC/d0.tmp", 4_KiB );
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC/d0.tmp",		EFileSystemAction::Added},
								{"CCC/d0.tmp",		EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------
		
		TEST( FileSystem::CreateDirectory( watchDir / "CCC" / "DD" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC/DD",			EFileSystemAction::Added}} );

		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		CreateFile( watchDir / "CCC/DD/d1.tmp", 4_KiB );
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC/DD/d1.tmp",	EFileSystemAction::Added},
								{"CCC/DD/d1.tmp",	EFileSystemAction::Modified}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------
		
		TEST( FileSystem::DeleteDirectory( watchDir / "CCC" ));
		
		TEST( file_watch.GetEvents( OUT events ));
		Compare( INOUT events, {{"CCC/d0.tmp",		EFileSystemAction::Removed},
								{"CCC/DD/d1.tmp",	EFileSystemAction::Removed},
								{"CCC/DD",			EFileSystemAction::Removed},
								{"CCC",				EFileSystemAction::Removed}} );
		
		TEST( not file_watch.GetEvents( OUT events ));
		LogEvents( events );
		TEST_Eq( events.size(), 0 );
		//---------------------------------------

		file_watch.Stop();
	}
#endif

#ifdef AE_PLATFORM_APPLE
	static void  FileWatch_Test1 (const Path &folder)
	{
		// TODO
	}
#endif
}

extern void UnitTest_FileWatch (const Path &curr)
{
	Path	watch_dir = curr / "FSWatch";

	FileWatch_Test1( watch_dir );
	
	FileSystem::DeleteDirectory( watch_dir );

	TEST_PASSED();
}


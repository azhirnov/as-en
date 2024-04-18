// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../../tests/threading/UnitTest_Common.h"
#include "../base/Perf_Common.h"

#include "base/DataSource/UnixFile.h"
#include "base/DataSource/WindowsFile.h"
#include "threading/DataSource/UnixAsyncDataSource.h"
#include "threading/DataSource/WinAsyncDataSource.h"

using namespace AE::Threading;

namespace
{
	using EStatus	= IAsyncTask::EStatus;

  #ifdef AE_PLATFORM_WINDOWS
	const auto	rflags	= WinFileRDataSource::EFlags::NoBuffering | WinFileRDataSource::EFlags::RandomAccess;
	const auto	wflags	= WinFileWDataSource::EFlags::NoBuffering | WinFileWDataSource::EFlags::NoCaching;
  #else
	const auto	rflags	= UnixFileRDataSource::EFlags::Direct;
	const auto	wflags	= UnixFileWDataSource::EFlags::Direct;
  #endif

  #ifdef AE_PLATFORM_ANDROID
	const auto		c_CoreId		= ECpuCoreId(6);
  #else
	const auto		c_CoreId		= ECpuCoreId(0);
  #endif

	const ulong		c_FileSize		= 128ull << 20;	// Mb
	const uint		c_BufferSize	= 4u << 10;		// Kb
	StaticAssert( IsMultipleOf( c_FileSize, c_BufferSize ));


	template <typename RFile, typename WFile>
	static void  SyncSeqReadDS (IntervalProfiler &profiler)
	{
		CHECK( ThreadUtils::SetAffinity( uint(c_CoreId) ));

		profiler.BeginTest( "Sync Sequential Read" );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			Array<ulong>	buf;	buf.resize( c_BufferSize / sizeof(ulong) );

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0; i < buf.size(); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf.data(), ArraySizeOf(buf) ) == c_BufferSize );
			}

			TEST( wfile.Capacity() == c_FileSize );
		}
		AE_LOGI( "begin sync read test" );
		{
			RC<RDataSource>	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST( rfile->Size() == c_FileSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				Unused( rfile->ReadBlock( Bytes{pos}, buf.data() + Bytes{pos}, Bytes{c_BufferSize} ));
			}
			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (buf[j] == pos+i);
				}
				CHECK( valid );
			}
		}
		profiler.EndTest();
	}


	template <typename RFile, typename WFile>
	static void  AsyncSeqReadDS (IntervalProfiler &profiler)
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1), c_CoreId};
		TEST( scheduler->GetFileIOService() );

		profiler.BeginTest( "Async Sequential Read" );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			Array<ulong>	buf;	buf.resize( c_BufferSize / sizeof(ulong) );

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0; i < buf.size(); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf.data(), ArraySizeOf(buf) ) == c_BufferSize );
			}

			TEST( wfile.Capacity() == c_FileSize );
		}
		AE_LOGI( "begin async read test" );
		{
			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST( rfile->Size() == c_FileSize );

			Array<AsyncDSRequest>	req_arr;
			req_arr.reserve( c_FileSize / c_BufferSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (;;)
				{
					AsyncDSRequest	req = rfile->ReadBlock( Bytes{pos}, buf.data() + Bytes{pos}, Bytes{c_BufferSize}, null );
					if_unlikely( req->IsCancelled() )
					{
						Unused( scheduler->GetFileIOService()->ProcessEvents() );
						continue;
					}
					req_arr.push_back( RVRef(req) );

					if ( (req_arr.size() & 0xF) == 0 )
						Unused( scheduler->GetFileIOService()->ProcessEvents() );
					break;
				}
			}

			for (;;)
			{
				Unused( scheduler->GetFileIOService()->ProcessEvents() );

				usize	complete = 0;
				for (auto& req : req_arr)
				{
					complete += usize{req->IsFinished()};
				}

				if ( complete == req_arr.size() )
					break;
			}

			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (buf[j] == pos+i);
				}
				CHECK( valid );
			}

			req_arr.clear();
			TEST( rfile.use_count() == 1 );
		}
		profiler.EndTest();
	}


	template <typename RFile, typename WFile>
	static void  SyncRndReadDS (IntervalProfiler &profiler)
	{
		CHECK( ThreadUtils::SetAffinity( uint(c_CoreId) ));

		profiler.BeginTest( "Sync Random Read" );

		Array<ulong>	pos_arr;
		pos_arr.reserve( c_FileSize / c_BufferSize );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			Array<ulong>	buf;	buf.resize( c_BufferSize / sizeof(ulong) );

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0; i < buf.size(); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf.data(), ArraySizeOf(buf) ) == c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST( wfile.Capacity() == c_FileSize );
		}
		AE_LOGI( "begin sync read test" );
		{
			ShuffleArray( INOUT pos_arr );

			RC<RDataSource>	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST( rfile->Size() == c_FileSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();
			for (ulong pos : pos_arr)
			{
				Unused( rfile->ReadBlock( Bytes{pos}, buf.data() + Bytes{pos}, Bytes{c_BufferSize} ));
			}
			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (buf[j] == pos+i);
				}
				CHECK( valid );
			}
		}
		profiler.EndTest();
	}


	template <typename RFile, typename WFile>
	static void  AsyncRndReadDS (IntervalProfiler &profiler)
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1), c_CoreId};
		TEST( scheduler->GetFileIOService() );

		profiler.BeginTest( "Async Random Read" );

		Array<ulong>	pos_arr;
		pos_arr.reserve( c_FileSize / c_BufferSize );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			Array<ulong>	buf;	buf.resize( c_BufferSize / sizeof(ulong) );

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0; i < buf.size(); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf.data(), ArraySizeOf(buf) ) == c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST( wfile.Capacity() == c_FileSize );
		}
		AE_LOGI( "begin async read test" );
		{
			ShuffleArray( INOUT pos_arr );

			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST( rfile->Size() == c_FileSize );

			Array<AsyncDSRequest>	req_arr;
			req_arr.reserve( c_FileSize / c_BufferSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();

			for (ulong pos : pos_arr)
			{
				for (;;)
				{
					AsyncDSRequest	req = rfile->ReadBlock( Bytes{pos}, buf.data() + Bytes{pos}, Bytes{c_BufferSize}, null );
					if_unlikely( req->IsCancelled() )
					{
						Unused( scheduler->GetFileIOService()->ProcessEvents() );
						continue;
					}
					req_arr.push_back( RVRef(req) );

					if ( (req_arr.size() & 0xF) == 0 )
						Unused( scheduler->GetFileIOService()->ProcessEvents() );
					break;
				}
			}

			for (;;)
			{
				Unused( scheduler->GetFileIOService()->ProcessEvents() );

				usize	complete = 0;
				for (auto& req : req_arr)
				{
					complete += usize{req->IsFinished()};
				}

				if ( complete == req_arr.size() )
					break;
			}

			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (buf[j] == pos+i);
				}
				CHECK( valid );
			}

			req_arr.clear();
			TEST( rfile.use_count() == 1 );
		}
		profiler.EndTest();
	}
}

extern void  PerfTest_AsyncFile ()
{
  # ifdef AE_PLATFORM_ANDROID
	const Path	curr	{"/storage/emulated/0/Android/data/AE.Test/cache"};
	const Path	folder	= curr / "ds_test";
  # else
	const Path	curr	= FileSystem::CurrentPath();
	const Path	folder	{AE_CURRENT_DIR "/ds_test"};
  # endif

	FileSystem::RemoveAll( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	IntervalProfiler	profiler{ "AsyncFile test" };

	#ifdef AE_PLATFORM_WINDOWS
		SyncSeqReadDS< WinFileRDataSource, WinFileWDataSource >( profiler );
		AsyncSeqReadDS< WinAsyncRDataSource, WinFileWDataSource >( profiler );

		SyncRndReadDS< WinFileRDataSource, WinFileWDataSource >( profiler );
		AsyncRndReadDS< WinAsyncRDataSource, WinFileWDataSource >( profiler );
	#else
		SyncSeqReadDS< UnixFileRDataSource, UnixFileWDataSource >( profiler );
		AsyncSeqReadDS< UnixAsyncRDataSource, UnixFileWDataSource >( profiler );

		SyncRndReadDS< UnixFileRDataSource, UnixFileWDataSource >( profiler );
		AsyncRndReadDS< UnixAsyncRDataSource, UnixFileWDataSource >( profiler );
	#endif

	FileSystem::SetCurrentPath( curr );
	FileSystem::RemoveAll( folder );

	TEST_PASSED();
}

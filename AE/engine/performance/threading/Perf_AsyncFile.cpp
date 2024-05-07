// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

#include "base/DataSource/UnixFile.h"
#include "base/DataSource/WindowsFile.h"
#include "threading/DataSource/FileAsyncDataSource.h"

namespace
{
	using EStatus	= IAsyncTask::EStatus;

  #ifdef AE_PLATFORM_ANDROID
	const auto	rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::RandomAccess;
	const auto	wflags	= FileWDataSource::EMode::Unknown;	// WriteSeq/WriteBlock returns 0, WriteBlock crashes

  #else
	const auto	rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::RandomAccess;
	const auto	wflags	= FileWDataSource::EMode::Direct;
  #endif

	auto			c_CoreId		= ECpuCoreId(0);

	const ulong		c_FileSize		= 128ull << 20;	// Mb
	const uint		c_BufferSize	= 4u << 10;		// Kb

	const uint		c_WaitIOFreq	= 0xF;	// 1 or 'c_WaitIOFreq' requests will trigger IO event handling

	StaticAssert( IsMultipleOf( c_FileSize, c_BufferSize ));


	template <typename RFile, typename WFile>
	static void  SyncSeqReadDS (IntervalProfiler &profiler)
	{
		Unused( ThreadUtils::SetAffinity( uint(c_CoreId) ));

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

				TEST_Eq( wfile.WriteSeq( buf.data(), ArraySizeOf(buf) ), c_BufferSize );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
		}
		AE_LOGI( "begin sync read test" );
		{
			auto	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				TEST_Eq( Bytes{pos}, rfile->Position() );
				TEST_Eq( rfile->ReadSeq( buf.data() + Bytes{pos}, Bytes{c_BufferSize} ), Bytes{c_BufferSize});
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
				TEST( valid );
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

				TEST_Eq( wfile.WriteSeq( buf.data(), ArraySizeOf(buf) ), c_BufferSize );
			}

			TEST( wfile.Position() == c_FileSize );
		}
		AE_LOGI( "begin async read test" );
		{
			auto	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

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

					if_unlikely( (req_arr.size() & c_WaitIOFreq) == 0 )
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
				TEST( valid );
			}

			req_arr.clear();
			TEST_Eq( rfile.use_count(), 1 );
		}
		profiler.EndTest();
	}


	template <typename RFile, typename WFile>
	static void  SyncRndReadDS (IntervalProfiler &profiler)
	{
		Unused( ThreadUtils::SetAffinity( uint(c_CoreId) ));

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

				TEST_Eq( wfile.WriteSeq( buf.data(), ArraySizeOf(buf) ), c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
		}
		AE_LOGI( "begin sync read test" );
		{
			ShuffleArray( INOUT pos_arr );

			auto	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			Array<ulong>	buf;	buf.resize( c_FileSize / sizeof(ulong) );

			profiler.BeginIteration();
			for (ulong pos : pos_arr)
			{
				TEST_Eq( rfile->ReadBlock( Bytes{pos}, buf.data() + Bytes{pos}, Bytes{c_BufferSize} ), Bytes{c_BufferSize});
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
				TEST( valid );
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

				TEST_Eq( wfile.WriteSeq( buf.data(), ArraySizeOf(buf) ), c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
		}
		AE_LOGI( "begin async read test" );
		{
			ShuffleArray( INOUT pos_arr );

			auto	rfile = MakeRC<RFile>( fname, rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

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

					if_unlikely( (req_arr.size() & c_WaitIOFreq) == 0 )
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
				TEST( valid );
			}

			req_arr.clear();
			TEST_Eq( rfile.use_count(), 1 );
		}
		profiler.EndTest();
	}
}

extern void  PerfTest_AsyncFile (const Path &testFolder)
{
	const Path	folder = testFolder / "ds_test";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	{
		auto&	arch = CpuArchInfo::Get();
		if ( auto* core = arch.GetCore( ECoreType::Performance ))
			c_CoreId = ECpuCoreId(core->FirstLogicalCore());
	}

	IntervalProfiler	profiler{ "AsyncFile test" };

	SyncSeqReadDS< FileRStream,				FileWStream >( profiler );
	AsyncSeqReadDS< FileAsyncRDataSource,	FileWStream >( profiler );

	SyncRndReadDS< FileRDataSource,			FileWStream >( profiler );
	AsyncRndReadDS< FileAsyncRDataSource,	FileWStream >( profiler );

	FileSystem::SetCurrentPath( testFolder );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}

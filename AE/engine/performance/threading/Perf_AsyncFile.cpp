// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

#include "base/DataSource/UnixFile.h"
#include "base/DataSource/WindowsFile.h"
#include "threading/DataSource/FileAsyncDataSource.h"

namespace
{
  #ifdef AE_PLATFORM_ANDROID
	const auto	seq_rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::SequentialScan;
	const auto	rnd_rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::RandomAccess;
	const auto	wflags		= FileWDataSource::EMode::Unknown;	// WriteSeq/WriteBlock returns 0, WriteBlock crashes

  #else
	const auto	seq_rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::SequentialScan;
	const auto	rnd_rflags	= FileRDataSource::EMode::Direct | FileRDataSource::EMode::RandomAccess;
	const auto	wflags		= FileWDataSource::EMode::Direct;
  #endif

	auto			c_CoreId		= ECpuCoreId(0);

	ulong			c_FileSize		= 0;
	uint			c_BufferSize	= 0;

	const uint		c_WaitIOFreq	= 0xF;	// 1 or 'c_WaitIOFreq' requests will trigger IO event handling


	static void  ClearFileCache ()
	{
		#ifdef AE_PLATFORM_UNIX_BASED
			PlatformUtils::ClearFileCache();
		#endif
	}


	static String  IOBandwidth (nanoseconds dt)
	{
		return ToStringSfx( double(c_FileSize) * 1.0e+9 / double(dt.count()) ) << "B/s";
	}

	template <typename FileType>
	static bool  AllocBuffer (OUT DynUntypedStorage &buf, Bytes bufSize, const FileType &file)
	{
		const Bytes		align = Max( Bytes{file.DirectAccessAlign().ptrAlign}, AlignOf<ulong> );

		return buf.Alloc( bufSize, align, null );
	}


	template <typename RFile, typename WFile>
	static void  SyncSeqReadDS (IntervalProfiler &profiler)
	{
		Unused( ThreadUtils::SetAffinity( uint(c_CoreId) ));

		profiler.BeginTest( "Sync Sequential Read", IOBandwidth );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_BufferSize}, wfile ));

			auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0, cnt = c_BufferSize/sizeof(ulong); i < cnt; ++i) {
					ptr[i] = pos + i;
				}

				TEST_Eq( wfile.WriteSeq( buf.Data(), buf.Size() ), c_BufferSize );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
			ClearFileCache();
		}
		AE_LOGI( "begin sync read test" );
		{
			auto	rfile = MakeRC<RFile>( fname, seq_rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_FileSize}, *rfile ));

			profiler.BeginIteration();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				TEST_Eq( Bytes{pos}, rfile->Position() );
				TEST_Eq( rfile->ReadSeq( OUT buf.Data() + Bytes{pos}, Bytes{c_BufferSize} ), Bytes{c_BufferSize});
			}
			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			const auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (ptr[j] == pos+i);
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

		profiler.BeginTest( "Async Sequential Read", IOBandwidth );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_BufferSize}, wfile ));

			auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0, cnt = c_BufferSize/sizeof(ulong); i < cnt; ++i) {
					ptr[i] = pos + i;
				}

				TEST_Eq( wfile.WriteSeq( buf.Data(), buf.Size() ), c_BufferSize );
			}

			TEST( wfile.Position() == c_FileSize );
			ClearFileCache();
		}
		AE_LOGI( "begin async read test" );
		{
			auto	rfile = MakeRC<RFile>( fname, seq_rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			Array<AsyncDSRequest>	req_arr;
			req_arr.reserve( c_FileSize / c_BufferSize );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_FileSize}, *rfile ));

			profiler.BeginIteration();

			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (;;)
				{
					AsyncDSRequest	req = rfile->ReadBlock( Bytes{pos}, buf.Data() + Bytes{pos}, Bytes{c_BufferSize}, null );
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
			const auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (ptr[j] == pos+i);
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

		profiler.BeginTest( "Sync Random Read", IOBandwidth );

		Array<ulong>	pos_arr;
		pos_arr.reserve( c_FileSize / c_BufferSize );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_BufferSize}, wfile ));

			auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0, cnt = c_BufferSize/sizeof(ulong); i < cnt; ++i) {
					ptr[i] = pos + i;
				}

				TEST_Eq( wfile.WriteSeq( buf.Data(), buf.Size() ), c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
			ClearFileCache();
		}
		AE_LOGI( "begin sync read test" );
		{
			ShuffleArray( INOUT pos_arr );

			auto	rfile = MakeRC<RFile>( fname, rnd_rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_FileSize}, *rfile ));

			profiler.BeginIteration();
			for (ulong pos : pos_arr)
			{
				TEST_Eq( rfile->ReadBlock( Bytes{pos}, buf.Data() + Bytes{pos}, Bytes{c_BufferSize} ), Bytes{c_BufferSize});
			}
			profiler.EndIteration();

			// validate data
			AE_LOGI( "validate data" );
			const auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (ptr[j] == pos+i);
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

		profiler.BeginTest( "Async Random Read", IOBandwidth );

		Array<ulong>	pos_arr;
		pos_arr.reserve( c_FileSize / c_BufferSize );

		const Path		fname {"perf1_data.bin"};
		{
			WFile	wfile { fname, wflags };
			TEST( wfile.IsOpen() );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_BufferSize}, wfile ));

			auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				for (uint i = 0, cnt = c_BufferSize/sizeof(ulong); i < cnt; ++i) {
					ptr[i] = pos + i;
				}

				TEST_Eq( wfile.WriteSeq( buf.Data(), buf.Size() ), c_BufferSize );
				pos_arr.push_back( pos );
			}

			TEST_Eq( wfile.Position(), c_FileSize );
			ClearFileCache();
		}
		AE_LOGI( "begin async read test" );
		{
			ShuffleArray( INOUT pos_arr );

			auto	rfile = MakeRC<RFile>( fname, rnd_rflags );
			TEST( rfile->IsOpen() );
			TEST_Eq( rfile->Size(), c_FileSize );

			Array<AsyncDSRequest>	req_arr;
			req_arr.reserve( c_FileSize / c_BufferSize );

			DynUntypedStorage	buf;
			TEST( AllocBuffer( OUT buf, Bytes{c_FileSize}, *rfile ));

			profiler.BeginIteration();

			for (ulong pos : pos_arr)
			{
				for (;;)
				{
					AsyncDSRequest	req = rfile->ReadBlock( Bytes{pos}, buf.Data() + Bytes{pos}, Bytes{c_BufferSize}, null );
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
			const auto*	ptr = buf.Ptr<ulong>();
			for (ulong pos = 0; pos < c_FileSize; pos += c_BufferSize)
			{
				bool	valid = true;
				for (ulong i = 0, j = pos/sizeof(ulong), cnt = c_BufferSize / sizeof(ulong); i < cnt; ++i, ++j)
				{
					valid = (ptr[j] == pos+i);
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

	AE_LOGI( "CPU core: "s << ToString( uint(c_CoreId) ));

	const Pair<ulong, uint>  fileSize_blockSize [] = {
	  #ifdef AE_PLATFORM_ANDROID
		{ 256ull << 20, 32u << 10 },	// 256 MiB, 32 KiB
		{ 256ull << 20, 16u << 10 },	// 256 MiB, 16 KiB
		{ 256ull << 20,  8u << 10 },	// 256 MiB, 8 KiB
		{ 256ull << 20,  4u << 10 }		// 256 MiB, 4 KiB
	  #else
		{ 1ull << 30,   1u << 20 },		// 1 GiB, 1 MiB
		{ 1ull << 30, 256u << 10 },		// 1 GiB, 256 KiB
		{ 1ull << 30,  64u << 10 },		// 1 GiB, 64 KiB
		{ 1ull << 30,  16u << 10 },		// 1 GiB, 16 KiB
		{ 1ull << 30,   8u << 10 },		// 1 GiB, 8 KiB
		{ 256ull << 20, 4u << 10 }		// 256 MiB, 4 KiB
	  #endif
	};

	for (auto [fs, bs] : fileSize_blockSize)
	{
		c_FileSize		= fs;
		c_BufferSize	= bs;

		AE_LOGI( "File size / block size: "s << ToString( Bytes{c_FileSize} ) << " / " << ToString( Bytes{c_BufferSize} ));

		CHECK( IsMultipleOf( c_FileSize, c_BufferSize ));

		IntervalProfiler	profiler{ "AsyncFile test "s << ToString( Bytes{c_FileSize} ) << " / " << ToString( Bytes{c_BufferSize} )};

		SyncSeqReadDS< FileRStream,				FileWStream >( profiler );
		AsyncSeqReadDS< FileAsyncRDataSource,	FileWStream >( profiler );

		SyncRndReadDS< FileRDataSource,			FileWStream >( profiler );
		AsyncRndReadDS< FileAsyncRDataSource,	FileWStream >( profiler );
	}

	FileSystem::SetCurrentPath( testFolder );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}

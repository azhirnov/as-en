// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "threading/DataSource/WinAsyncDataSource.h"
#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

#ifndef AE_DISABLE_THREADS
namespace
{
	using EStatus		= IAsyncTask::EStatus;
	using ESourceType	= IDataSource::ESourceType;


	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		const ulong	file_size	= 128ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		STATIC_ASSERT( file_size % buf_size == 0 );

		const Path		fname {"ds11_data.txt"};
		{
			WFile	wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));
			
			ulong	buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;
			
			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ) == buf_size );
				pos += buf_size;
			}

			TEST( wfile.Capacity() == file_size );
		}
		{
			RC<RFile>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST( rfile->Size() == file_size );
			
			ulong	pos = 0;
			while ( pos < file_size + buf_size )
			{
				auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
				TEST( req );
				TEST( req.use_count() == 2 );

				auto	task = AsyncTask{req->AsPromise().Then( [pos] (const AsyncRDataSource::Result_t &res)
								{
									TEST( res.data != null );
									TEST( res.dataSize == (pos < file_size ? buf_size : 0) );
									TEST( res.offset == pos );
									TEST( res.request.use_count() == 1 );		// because executed sequentially and synchroniously

									ulong	ref_buf [buf_size / sizeof(ulong)];

									for (uint i = 0; i < CountOf(ref_buf); ++i) {
										ref_buf[i] = pos + i;
									}
									TEST( MemEqual( res.data, ref_buf, res.dataSize ));
								})};
				TEST( task );

				for (;;)
				{
					if ( scheduler->GetFileIOService()->ProcessEvents() )
						break;
				}

				TEST( req->IsCompleted() );
				req = null;

				TEST( scheduler->Wait( {task}, EThreadArray{ EThread::Worker } ));
				TEST( task->Status() == EStatus::Completed );

				pos += buf_size;
			}
		}
	}
		

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test2 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::WorkerConfig::CreateNonSleep(
				EThreadArray{ EThread::Worker, EThread::FileIO }
			)));

		const ulong	file_size	= 32ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		STATIC_ASSERT( file_size % buf_size == 0 );

		const Path		fname {"ds12_data.txt"};
		{
			WFile	wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));
			
			ulong	buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;
			
			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ) == buf_size );
				pos += buf_size;
			}

			TEST( wfile.Capacity() == file_size );
		}
		{
			RC<RFile>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST( rfile->Size() == file_size );

			class ReadFileTask final : public IAsyncTask
			{
			private:
				RC<RFile>	rfile;
				ulong		pos		= 0;

			public:
				ReadFileTask (RC<RFile> rfile) : IAsyncTask{ETaskQueue::Worker}, rfile{rfile} {}

				void  Run () override
				{
					if ( pos < file_size + buf_size )
					{
						auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
						TEST( req );

						auto	task = AsyncTask{req->AsPromise().Then( [cur_pos = pos] (const AsyncRDataSource::Result_t &res)
										{
											TEST( res.data != null );
											TEST( res.dataSize == (cur_pos < file_size ? buf_size : 0) );
											TEST( res.offset == cur_pos );
											TEST( res.request.use_count() > 0 );	// is alive

											ulong	ref_buf [buf_size / sizeof(ulong)];

											for (uint i = 0; i < CountOf(ref_buf); ++i) {
												ref_buf[i] = cur_pos + i;
											}
											TEST( MemEqual( res.data, ref_buf, res.dataSize ));
										})};
						TEST( task );

						req = null;

						pos += buf_size;

						return Continue( Tuple{ task });
					}
					// complete
				}

				StringView	DbgName ()	C_NE_OV	{ return "ReadFileTask"; }
			};
			
			auto	task = scheduler->Run<ReadFileTask>( Tuple{rfile} );
			TEST( scheduler->Wait( {task} ));
			TEST( task->Status() == EStatus::Completed );
		}
	}
	

#ifdef AE_HAS_COROUTINE
	template <typename RFile, typename WFile>
	static CoroTask  AsyncReadDS_Test3_Coro ()
	{
		const ulong	file_size	= 32ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		STATIC_ASSERT( file_size % buf_size == 0 );

		const Path		fname {"ds13_data.txt"};
		{
			WFile	wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));
			
			ulong	buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;
			
			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ) == buf_size );
				pos += buf_size;
			}

			TEST( wfile.Capacity() == file_size );
		}
		{
			RC<RFile>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST( rfile->Size() == file_size );
			
			ulong	pos = 0;
			while ( pos < file_size + buf_size )
			{
				auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
				TEST( req );

				auto	res = co_await req->AsPromise();
				req = null;

				TEST( res.data != null );
				TEST( res.dataSize == (pos < file_size ? buf_size : 0) );
				TEST( res.offset == pos );
				TEST( res.request.use_count() > 0 );	// is alive

				ulong	ref_buf [buf_size / sizeof(ulong)];

				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}
				TEST( MemEqual( res.data, ref_buf, res.dataSize ));

				pos += buf_size;
			}
		}
	}

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test3 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::WorkerConfig::CreateNonSleep(
				EThreadArray{ EThread::Worker, EThread::FileIO }
			)));

		auto	task = scheduler->Run( AsyncReadDS_Test3_Coro< RFile, WFile >() );
		TEST( scheduler->Wait({ AsyncTask{task} }));
		TEST( AsyncTask{task}->Status() == EStatus::Completed );
	}
#endif


	template <typename RFile, typename WFile>
	static void  AsyncWriteDS_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		const ulong	file_size	= 128ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		STATIC_ASSERT( file_size % buf_size == 0 );

		const Path		fname {"ds21_data.txt"};
		{
			RC<WFile>	wfile = MakeRC<WFile>( fname );
			TEST( wfile->IsOpen() );
			TEST( AllBits( wfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::Async ));
			
			ulong	pos = 0;

			while ( pos < file_size )
			{
				auto	src_buf = wfile->Alloc( Bytes{buf_size} );
				TEST( src_buf );
				
				ulong*	buf		= Cast<ulong>(src_buf->Data());
				usize	count	= usize(src_buf->Size() / SizeOf<ulong>);

				for (uint i = 0; i < count; ++i) {
					buf[i] = pos + i;
				}

				auto	req = wfile->WriteBlock( Bytes{pos}, Bytes{buf_size}, src_buf );
				TEST( req );
				TEST( req.use_count() == 2 );
				
				auto	task = AsyncTask{req->AsPromise().Then( [pos] (const AsyncWDataSource::Result_t &res)
																{
																	TEST( pos == res.offset );
																	TEST( buf_size == res.dataSize );
																	TEST( res.data == null );
																	TEST( res.request == null );
																})};
				TEST( task );
				
				for (;;)
				{
					if ( scheduler->GetFileIOService()->ProcessEvents() )
						break;
				}
				
				TEST( req->IsCompleted() );
				req = null;

				TEST( scheduler->Wait( {task}, EThreadArray{ EThread::Worker } ));
				TEST( task->Status() == EStatus::Completed );

				pos += buf_size;
			}
		}
		{
			RFile	rfile {fname};
			TEST( rfile.IsOpen() );
			TEST( AllBits( rfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess ));
			TEST( rfile.Size() == file_size );
			
			ulong	dst_buf [buf_size / sizeof(ulong)];
			ulong	ref_buf [buf_size / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < file_size )
			{
				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}

				TEST( rfile.ReadBlock( Bytes{pos}, OUT dst_buf, Sizeof(dst_buf) ) == buf_size );
				TEST( MemEqual( dst_buf, ref_buf ));

				pos += buf_size;
			}
		}
	}
}


extern void UnitTest_AsyncDataSource ()
{
	const Path	curr	= FileSystem::CurrentPath();
	const Path	folder	{AE_CURRENT_DIR "/ds_test"};
	
	FileSystem::RemoveAll( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	#ifdef AE_PLATFORM_WINDOWS
		AsyncReadDS_Test1< WinAsyncRDataSource, FileWDataSource >();
		AsyncReadDS_Test2< WinAsyncRDataSource, FileWDataSource >();
	#  ifdef AE_HAS_COROUTINE
			AsyncReadDS_Test3< WinAsyncRDataSource, FileWDataSource >();
	#  endif
		AsyncWriteDS_Test1< FileRDataSource, WinAsyncWDataSource >();
	#endif
		
	FileSystem::SetCurrentPath( curr );
	TEST_PASSED();
}

#else

extern void UnitTest_AsyncDataSource ()
{}

#endif // AE_DISABLE_THREADS

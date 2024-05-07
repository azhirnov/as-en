// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#include "threading/DataSource/FileAsyncDataSource.h"

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
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"ds11_data.bin"};
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
			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST_Eq( rfile->Size(), file_size );

			ulong	pos = 0;
			while ( pos < file_size + buf_size )
			{
				auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
				TEST( req );	// always non-null
				TEST_GE( req.use_count(), 1 );

				auto	task = AsyncTask{req->AsPromise( ETaskQueue::PerFrame )
								.Then(	[pos] (const AsyncRDataSource::Result_t &res)
										{
											TEST( res.data != null );
											TEST_Eq( res.dataSize, (pos < file_size ? buf_size : 0) );
											TEST_Eq( res.pos, pos );
											TEST_Gt( res.rc.use_count(), 0 );		// because executed sequentially and synchronously

											ulong	ref_buf [buf_size / sizeof(ulong)];

											for (uint i = 0; i < CountOf(ref_buf); ++i) {
												ref_buf[i] = pos + i;
											}
											TEST( MemEqual( res.data, ref_buf, res.dataSize ));
										})};
				TEST( task );

				for (;;)
				{
					if ( scheduler->GetFileIOService()->ProcessEvents() or req->IsFinished() )
						break;
				}

				TEST( req->IsCompleted() );
				req = null;

				TEST( scheduler->Wait( {task}, EThreadArray{ EThread::PerFrame }, c_MaxTimeout ));
				TEST( task->Status() == EStatus::Completed );

				pos += buf_size;
			}
			TEST( rfile.use_count() == 1 );
		}
	}


	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test2 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame, EThread::FileIO }, "worker"
			}));

		const ulong	file_size	= 32ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"ds12_data.bin"};
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

			TEST_Eq( wfile.Capacity(), file_size );
		}
		{
			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST_Eq( rfile->Size(), file_size );

			class ReadFileTask final : public IAsyncTask
			{
			private:
				RC<AsyncRDataSource>	rfile;
				ulong					pos		= 0;

			public:
				ReadFileTask (RC<AsyncRDataSource> rfile) __NE___ : IAsyncTask{ETaskQueue::PerFrame}, rfile{RVRef(rfile)} {}

				void  Run () __Th_OV
				{
					if ( pos < file_size + buf_size )
					{
						auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
						TEST( req );	// always non-null

						auto	task = AsyncTask{req->AsPromise( ETaskQueue::PerFrame )
										.Then(	[cur_pos = pos] (const AsyncRDataSource::Result_t &res)
												{
													TEST( res.data != null );
													TEST_Eq( res.dataSize, (cur_pos < file_size ? buf_size : 0) );
													TEST_Eq( res.pos, cur_pos );
													TEST_Gt( res.rc.use_count(), 0 );	// is alive

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
			TEST( scheduler->Wait( {task}, c_MaxTimeout ));
			TEST( task->Status() == EStatus::Completed );

			task = null;
			TEST_Eq( rfile.use_count(), 1 );
		}
	}


#ifdef AE_HAS_COROUTINE
	template <typename RFile, typename WFile>
	static CoroTask  AsyncReadDS_Test3_Coro ()
	{
		const ulong	file_size	= 32ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"ds13_data.bin"};
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
			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST_Eq( rfile->Size(), file_size );

			ulong	pos = 0;
			while ( pos < file_size + buf_size )
			{
				auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{buf_size} );
				TEST( req );	// always non-null

				auto	res = co_await req->AsPromise( ETaskQueue::PerFrame );

				req = null;		// request is not used anymore, but memory in 'res' must be alive

				TEST( res.data != null );
				TEST_Eq( res.dataSize, (pos < file_size ? buf_size : 0) );
				TEST_Eq( res.pos, pos );
				TEST_Gt( res.rc.use_count(), 0 );	// is alive

				ulong	ref_buf [buf_size / sizeof(ulong)];

				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}
				TEST( MemEqual( res.data, ref_buf, res.dataSize ));

				pos += buf_size;
			}
			TEST_Eq( rfile.use_count(), 1 );
		}
	}

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test3 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{
				EThreadArray{ EThread::PerFrame, EThread::FileIO }, "worker"
			}));

		auto	task = scheduler->Run( AsyncReadDS_Test3_Coro< RFile, WFile >() );
		TEST( scheduler->Wait( {AsyncTask{task}}, c_MaxTimeout ));
		TEST( AsyncTask{task}->Status() == EStatus::Completed );
	}

#endif // AE_HAS_COROUTINE


	template <typename RFile, typename WFile>
	static void  AsyncWriteDS_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		const ulong	file_size	= 128ull << 20;	// Mb
		const uint	buf_size	= 4u << 10;		// Kb
		StaticAssert( IsMultipleOf( file_size, buf_size ));

		const Path		fname {"ds21_data.bin"};
		{
			RC<AsyncWDataSource>	wfile = MakeRC<WFile>( fname );
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
				TEST( req );	// always non-null
				TEST_GE( req.use_count(), 1 );

				auto	task = AsyncTask{req->AsPromise( ETaskQueue::PerFrame )
								.Then(	[pos] (const AsyncWDataSource::Result_t &res)
										{
											TEST_Eq( pos, res.pos );
											TEST_Eq( buf_size, res.dataSize );
											TEST( res.data == null );
											TEST( res.rc == null );
										})};
				TEST( task );

				for (;;)
				{
					if ( scheduler->GetFileIOService()->ProcessEvents() or req->IsFinished() )
						break;
				}

				TEST( req->IsCompleted() );
				req = null;

				TEST( scheduler->Wait( {task}, EThreadArray{ EThread::PerFrame }, c_MaxTimeout ));
				TEST( task->Status() == EStatus::Completed );

				pos += buf_size;
			}
			TEST_Eq( wfile.use_count(), 1 );
		}
		{
			RFile	rfile {fname};
			TEST( rfile.IsOpen() );
			TEST( AllBits( rfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess ));
			TEST_Eq( rfile.Size(), file_size );

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


extern void UnitTest_AsyncDataSource (const Path &curr)
{
	const Path	folder = curr / "ds_test";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	AsyncReadDS_Test1< FileAsyncRDataSource, StdFileWDataSource >();
	AsyncReadDS_Test2< FileAsyncRDataSource, StdFileWDataSource >();
  # ifdef AE_HAS_COROUTINE
	AsyncReadDS_Test3< FileAsyncRDataSource, StdFileWDataSource >();
  # endif
	AsyncWriteDS_Test1< StdFileRDataSource, FileAsyncWDataSource >();

	// TODO: async stream

	FileSystem::SetCurrentPath( curr );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}

#else

extern void UnitTest_AsyncDataSource ()
{}

#endif // AE_DISABLE_THREADS

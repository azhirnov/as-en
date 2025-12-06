// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

#include "threading/DataSource/FileAsyncDataSource.h"

namespace
{
	using ESourceType	= IDataSource::ESourceType;

	static const EThreadArray	c_ThreadArr	{ EThread::PerFrame, EThread::Background, EThread::FileIO };

	static constexpr ulong	c_RFileSize		= 32ull << 20;	// MiB
	static constexpr uint	c_RBufSize		= 4u << 10;		// KiB
	StaticAssert( IsMultipleOf( c_RFileSize, c_RBufSize ));


	template <typename RFile, typename WFile>
	static AsyncCoro  AsyncReadDS_Impl (const Path fname, AsyncCoro (*fn)(RC<AsyncRDataSource> rfile))
	{
		{
			WFile	wfile {fname};
			TEST( wfile.IsOpen() );
			TEST( AllBits( wfile.GetSourceType(), ESourceType::RandomAccess | ESourceType::WriteAccess ));

			ulong	buf [c_RBufSize / sizeof(ulong)];
			ulong	pos = 0;

			while ( pos < c_RFileSize )
			{
				for (uint i = 0; i < CountOf(buf); ++i) {
					buf[i] = pos + i;
				}

				TEST( wfile.WriteBlock( Bytes{pos}, buf, Sizeof(buf) ) == c_RBufSize );
				pos += c_RBufSize;
			}

			TEST( wfile.Capacity() == c_RFileSize );
		}
		{
			RC<AsyncRDataSource>	rfile = MakeRC<RFile>( fname );
			TEST( rfile->IsOpen() );
			TEST( AllBits( rfile->GetSourceType(), ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async ));
			TEST_Eq( rfile->Size(), c_RFileSize );

			auto	task = Scheduler().Run( ETaskQueue::Background, fn( rfile ));
			co_await task;
			task = null;

			TEST_Eq( rfile.use_count(), 1 );
		}
		co_return;
	}
//-----------------------------------------------------------------------------



	static auto  CanceledRequest_Test1_Coro () -> InlineCoro<>
	{
		auto&	static_req = _Coro_::IAsyncDataSourceRequest::CanceledRequest::s_canceled;
		TEST_Eq( static_req.use_count(), 1 );

		auto	request = static_req.GetRC();
		TEST( request );
		TEST( request->Status() == EAsyncDSRequestStatus::Canceled );
		TEST_Eq( request.use_count(), 2 );

		auto	res = co_await WeakAsyncDSRequest{ RVRef(request) };
		TEST( not res );
		TEST( res.status == EAsyncDSRequestStatus::Canceled );

		//co_return;	// optional
	}

	static void  CanceledRequest_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		AsyncTask	task = CanceledRequest_Test1_Coro();

		TEST( scheduler->Wait( {task}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Completed );
	}
//-----------------------------------------------------------------------------



	static AsyncCoro  AsyncReadDS_Test1_Coro (RC<AsyncRDataSource> rfile)
	{
		ulong	pos = 0;
		while ( pos < c_RFileSize + c_RBufSize )
		{
			auto	req = rfile->ReadBlock( Bytes{pos}, Bytes{c_RBufSize} );
			TEST( req );	// always non-null

			{
				auto	res = co_await req;

				req = null;		// request is not used anymore, but memory in 'res' must be alive

				TEST( res );
				TEST( res.data != null );
				TEST( res.status == EAsyncDSRequestStatus::Completed );
				TEST_Eq( res.dataSize, (pos < c_RFileSize ? c_RBufSize : 0) );
				TEST_Eq( res.pos, pos );
				TEST_Gt( res.rc.use_count(), 0 );	// is alive

				ulong	ref_buf [c_RBufSize / sizeof(ulong)];

				for (uint i = 0; i < CountOf(ref_buf); ++i) {
					ref_buf[i] = pos + i;
				}
				TEST( MemEqual( res.data, ref_buf, res.dataSize ));
			}

			pos += c_RBufSize;
		}
		co_return;
	}

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		auto	fn		= &AsyncReadDS_Test1_Coro;
		auto	task	= scheduler->Run( ETaskQueue::Background,
										  AsyncReadDS_Impl< RFile, WFile >( Path{"ds11_data.bin"}, fn ));

		TEST( scheduler->Wait( {AsyncTask{task}}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Completed );
	}
//-----------------------------------------------------------------------------



	template <typename RFile>
	static auto  AsyncReadDS_Test2_Coro () -> InlineCoro<>
	{
		RC<AsyncRDataSource>	rfile	= MakeRC<RFile>( Path{ "ds99_data.bin" });
		TEST( not rfile->IsOpen() );

		StaticLogger::Deinitialize( false );

		auto	req = rfile->ReadBlock( Bytes{c_RBufSize}, Bytes{c_RBufSize} );

		StaticLogger::InitDefault();

		TEST( req );	// always non-null

		TEST( req->Status() == EAsyncDSRequestStatus::Canceled );

		auto	res = co_await req;

		req = null;		// request is not used anymore

		TEST( not res );
		TEST( res.status == EAsyncDSRequestStatus::Canceled );
		TEST( res.data == null );
		TEST( res.rc == null );

		TEST_Eq( rfile.use_count(), 1 );

		co_return;
	}

	template <typename RFile>
	static void  AsyncReadDS_Test2 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		AsyncTask	task = AsyncReadDS_Test2_Coro< RFile >();

		TEST( scheduler->Wait( {task}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Completed );
	}
//-----------------------------------------------------------------------------



	static AsyncCoro  AsyncReadDS_Test3_Coro (RC<AsyncRDataSource> rfile)
	{
		const auto	TestResult = [] (auto& res, ulong pos)
		{{
			TEST( res.data != null );
			TEST( res.status == EAsyncDSRequestStatus::Completed );
			TEST_Eq( res.dataSize, (pos < c_RFileSize ? c_RBufSize : 0) );
			TEST_Eq( res.pos, pos );
			TEST_Gt( res.rc.use_count(), 0 );	// is alive

			ulong	ref_buf [c_RBufSize / sizeof(ulong)];

			for (uint i = 0; i < CountOf(ref_buf); ++i) {
				ref_buf[i] = pos + i;
			}
			TEST( MemEqual( res.data, ref_buf, res.dataSize ));
		}};

		ulong	pos = 0;
		while ( pos < c_RFileSize + c_RBufSize )
		{
			auto	pos1	= pos;
			auto	req1	= rfile->ReadBlock( Bytes{pos1}, Bytes{c_RBufSize} );
			TEST( req1 );	// always non-null

			pos += c_RBufSize;

			auto	pos2	= pos;
			auto	req2	= rfile->ReadBlock( Bytes{pos2}, Bytes{c_RBufSize} );
			TEST( req2 );	// always non-null

			pos += c_RBufSize;

			auto	res12 = Coro_WaitResult( req1, req2 );

			TEST( res12 );
			TEST( not req1 );
			TEST( not req2 );

			auto& [res1, res2] = res12;

			TestResult( res1, pos1 );
			TestResult( res2, pos2 );
		}
		co_return;
	}

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test3 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		auto	fn		= &AsyncReadDS_Test3_Coro;
		auto	task	= scheduler->Run( ETaskQueue::Background,
										  AsyncReadDS_Impl< RFile, WFile >( Path{"ds13_data.bin"}, fn ));

		TEST( scheduler->Wait( {AsyncTask{task}}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Completed );
	}
//-----------------------------------------------------------------------------



	static AsyncCoro  AsyncReadDS_Test4_Coro (RC<AsyncRDataSource> rfile)
	{
		auto	canceled_req = _Coro_::IAsyncDataSourceRequest::CanceledRequest::s_canceled.GetRC();

		ulong	pos = 0;
		while ( pos < c_RFileSize + c_RBufSize )
		{
			auto	pos1	= pos;
			auto	req1	= rfile->ReadBlock( Bytes{pos1}, Bytes{c_RBufSize} );
			TEST( req1 );	// always non-null

			pos += c_RBufSize;

			auto	res12 = Coro_WaitResultOrCancel( req1, canceled_req );

			TEST( false );
			TEST( not res12 );

			AsyncDSRequest::Value_t::Result	res1 = res12.get<0>();
			AsyncDSRequest::Value_t::Result	res2 = res12.get<1>();

			TEST( res1 );
			TEST( res2 );
		}
		co_return;
	}

	template <typename RFile, typename WFile>
	static void  AsyncReadDS_Test4 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		auto	fn		= &AsyncReadDS_Test4_Coro;
		auto	task	= scheduler->Run( ETaskQueue::Background,
										  AsyncReadDS_Impl< RFile, WFile >( Path{"ds14_data.bin"}, fn ));

		TEST( scheduler->Wait( {AsyncTask{task}}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Canceled );
	}
//-----------------------------------------------------------------------------



	template <typename RFile, typename WFile>
	static AsyncCoro  AsyncWriteDS_Test1_Coro ()
	{
		static constexpr ulong	file_size	= 128ull << 20;	// MiB
		static constexpr uint	buf_size	= 4u << 10;		// KiB
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

				{
					auto	res = co_await req;

					req = null;		// request is not used anymore

					TEST( res );
					TEST( res.status == EAsyncDSRequestStatus::Completed );
					TEST_Eq( pos, res.pos );
					TEST_Eq( buf_size, res.dataSize );
					TEST( res.data == null );
					TEST( res.rc == null );
				}

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
		co_return;
	}

	template <typename RFile, typename WFile>
	static void  AsyncWriteDS_Test1 ()
	{
		LocalTaskScheduler	scheduler	{IOThreadCount(1)};
		TEST( scheduler->GetFileIOService() );

		scheduler->AddThread( ThreadMngr::CreateThread( ThreadMngr::ThreadConfig{ c_ThreadArr, "worker" }));

		auto	task = scheduler->Run( ETaskQueue::Background, AsyncWriteDS_Test1_Coro< RFile, WFile >() );
		TEST( scheduler->Wait( {AsyncTask{task}}, c_MaxTimeout ));
		TEST( task->Status() == ETaskStatus::Completed );
	}
//-----------------------------------------------------------------------------
}


extern void UnitTest_AsyncDataSource (const Path &curr)
{
	const Path	folder = curr / "ds_test";

	FileSystem::DeleteDirectory( folder );
	FileSystem::CreateDirectories( folder );
	TEST( FileSystem::SetCurrentPath( folder ));

	CanceledRequest_Test1();

	AsyncReadDS_Test1< FileAsyncRDataSource, StdFileWDataSource >();
	AsyncReadDS_Test2< FileAsyncRDataSource >();
	AsyncReadDS_Test3< FileAsyncRDataSource, StdFileWDataSource >();
	AsyncReadDS_Test4< FileAsyncRDataSource, StdFileWDataSource >();	// TODO: problem on Mac/Linux

	AsyncWriteDS_Test1< StdFileRDataSource, FileAsyncWDataSource >();

	// TODO: async stream

	FileSystem::SetCurrentPath( curr );
	FileSystem::DeleteDirectory( folder );

	TEST_PASSED();
}

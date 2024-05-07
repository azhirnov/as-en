// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Linux AIO notes:
	- Asynchronous appending write may silently be synchronous.
	- May block in many cases, using 'Direct' flag will minimize syncs.

	io_uring notes:
	- Not supported on Android.
*/

#pragma once

#include "threading/DataSource/AsyncDataSource.h"
#include "threading/Containers/LfIndexedPool.h"

#ifdef AE_PLATFORM_UNIX_BASED
# if not defined(AE_ASYNCIO_USE_LINUX_AIO)	and \
	 not defined(AE_ASYNCIO_USE_IO_URING)	and \
	 not defined(AE_ASYNCIO_USE_POSIX_AIO)	and \
	 not defined(AE_ASYNCIO_USE_BSD_AIO)
#	error Unix AsyncFile requires one of: Linux AIO, Posix AIO, io_uring
# endif

namespace AE::Threading
{
	class UnixAsyncRDataSource;
	class UnixAsyncWDataSource;


	//
	// Unix IO Service
	//
	class UnixIOService final : public IOService
	{
	// types
	public:
		using Index_t		= uint;
		using File_t		= int;		// fd

		class AsyncRDataSourceApi;
		class AsyncWDataSourceApi;

	private:
	  #ifdef AE_ASYNCIO_USE_LINUX_AIO
		using LinuxAIO_iocb				= UntypedStorage< sizeof(ulong)*8, alignof(ulong) >;	// iocb
		using LinuxAIO_aio_context_t	= usize;												// aio_context_t

		struct LinuxAIO_CtxPerThread
		{
			SpinLockRelaxed			guard;			// protects internal content of 'ctx'
			LinuxAIO_aio_context_t	ctx		= 0;
		};
		using LinuxAIO_CtxArray_t		= StaticArray< LinuxAIO_CtxPerThread, 8 >;
	  #endif

	  #ifdef AE_ASYNCIO_USE_IO_URING
		using IOURing_t			= UntypedStorage< sizeof(ulong)*(13+11+3), alignof(ulong) >;	// io_uring

		struct IOURing_PerThread
		{
			SpinLockRelaxed		guard;		// protects internal content of 'ring'
			IOURing_t			ring;
			bool				created	= false;
		};
		using IOURing_Array_t	= StaticArray< IOURing_PerThread, 8 >;
	  #endif

	  #if defined(AE_ASYNCIO_USE_POSIX_AIO) and defined(AE_PLATFORM_APPLE)
		using PosixAIO_aiocb		= UntypedStorage< sizeof(ulong)*10, alignof(ulong) >;	// aiocb
	  #endif
	  #if defined(AE_ASYNCIO_USE_POSIX_AIO) and defined(AE_PLATFORM_LINUX)
		using PosixAIO_aiocb		= UntypedStorage< sizeof(ulong)*21, alignof(ulong) >;	// aiocb
	  #endif
	  #ifdef AE_ASYNCIO_USE_BSD_AIO
		using PosixAIO_aiocb		= UntypedStorage< sizeof(ulong)*10, alignof(ulong) >;	// aiocb
	  #endif


		//
		// Request Base
		//
		class _RequestBase : public Threading::_hidden_::IAsyncDataSourceRequest
		{
		// variables
		protected:
		  #ifdef AE_ASYNCIO_USE_LINUX_AIO
			uint				_queueIndex	= UMax;
			Bytes				_offset;
			LinuxAIO_iocb		_aioCb;
		  #endif

		  #if defined(AE_ASYNCIO_USE_POSIX_AIO) or \
			  defined(AE_ASYNCIO_USE_BSD_AIO)
			Bytes				_offset;
			PosixAIO_aiocb		_aioCb;
		  #endif

			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<>				_memRC;			// keep memory alive


		// methods
		protected:
			_RequestBase ()								__NE___;

				void  _Init (RC<> mem)					__NE___;
				void  _Cleanup ()						__NE___;
			ND_ bool  _Cancel ()						__NE___;

		private:
			friend class UnixIOService;
				void  _Complete (Bytes size, bool)		__NE___;
		};


		//
		// Read Request
		//
		class ReadRequestApi;
		class ReadRequest final : public _RequestBase
		{
		// variables
		private:
		  #ifdef AE_ASYNCIO_USE_IO_URING
			Bytes						_offset;
			void*						_data		= null;
		  #endif

			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<UnixAsyncRDataSource>	_dataSource;	// keep alive until request is in progress


		// methods
		public:
			// IAsyncDataSourceRequest //
			Result		GetResult ()					C_NE_OV;
			bool		Cancel ()						__NE_OV;
			Promise_t	AsPromise (ETaskQueue)			__NE_OV;

		private:
			friend class AsyncRDataSourceApi;
			ND_ bool  _Create (RC<UnixAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> dst) __NE___;

			ND_ ResultWithRC  _GetResult ()				__NE___;

				void  _ReleaseObject ()					__NE_OV;
		};


		//
		// Write Request
		//
		class WriteRequestApi;
		class WriteRequest final : public _RequestBase
		{
		// variables
		private:
		  #ifdef AE_ASYNCIO_USE_IO_URING
			Bytes						_offset;
		  #endif

			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<UnixAsyncWDataSource>	_dataSource;	// keep alive until request is in progress


		// methods
		public:
			// IAsyncDataSourceRequest //
			Result		GetResult ()					C_NE_OV;
			bool		Cancel ()						__NE_OV;
			Promise_t	AsPromise (ETaskQueue)			__NE_OV;

		private:
			friend class AsyncWDataSourceApi;
			ND_ bool  _Create (RC<UnixAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> src) __NE___;

			ND_ ResultWithRC  _GetResult ()				__NE___;

				void  _ReleaseObject ()					__NE_OV;
		};


	private:
		template <typename T, usize ChunkSize, usize MaxChunks>
		using PoolTmpl				= LfIndexedPool< T, Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

		static constexpr uint		_ReqChunkSize = 4u << 10;
		using ReadRequestPool_t		= PoolTmpl< ReadRequest,  _ReqChunkSize, 32 >;
		using WriteRequestPool_t	= PoolTmpl< WriteRequest, _ReqChunkSize, 32 >;


	// variables
	private:
		ReadRequestPool_t		_readResultPool;
		WriteRequestPool_t		_writeResultPool;

	  #ifdef AE_ASYNCIO_USE_LINUX_AIO
		AE_DISABLE_OFFSETOF_WARNINGS(
		static constexpr uint	_LinuxAIO_CBOffset		= offsetof( _RequestBase, _aioCb );
		)
		static constexpr uint	_LinuxAIO_MinRequests	= 16;
		LinuxAIO_CtxArray_t		_aioContextArr			= {};
	  #endif

	  #ifdef AE_ASYNCIO_USE_IO_URING
		static constexpr uint	_IOuring_MinRequests	= 16;
		IOURing_Array_t			_iouringArray			= {};
	  #endif

	  #ifdef AE_ASYNCIO_USE_BSD_AIO
		int						_kQueue					= -1;
	  #endif

	  #ifdef AE_ASYNCIO_USE_POSIX_AIO
		Mutex						_queueGuard;
		usize						_queueLastIdx		= 0;
		RingBuffer<_RequestBase*>	_queue;
	  #endif


	// methods
	public:
		~UnixIOService ()											__NE_OV;

		ND_ bool		IsInitialized ()							C_NE___;


		// IOService //
		usize			ProcessEvents ()							__NE_OV;
		EIOServiceType  GetIOServiceType ()							C_NE_OV	{ return EIOServiceType::File; }


	  #ifdef AE_ASYNCIO_USE_BSD_AIO
		ND_ int			_GetKQueue ()								__NE___	{ ASSERT( IsInitialized() );  return _kQueue; }
	  #endif
	  #ifdef AE_ASYNCIO_USE_POSIX_AIO
			void		_AsyncWaitToComplete (_RequestBase* req)	__NE___	{ EXLOCK( _queueGuard );  _queue.push_back( req ); }
	  #endif

	private:
		friend class TaskScheduler;
		explicit UnixIOService (uint maxAccessThreads)				__NE___;

	  #ifdef AE_ASYNCIO_USE_LINUX_AIO
		ND_ LinuxAIO_CtxPerThread&  _GetLinuxAIOContext ()			__NE___;
		ND_ LinuxAIO_CtxPerThread*  _GetLinuxAIOContext (uint idx)	__NE___	{ return null; }
	  #endif
	  #ifdef AE_ASYNCIO_USE_IO_URING
		ND_ IOURing_PerThread&		_GetIOURing ()					__NE___;
		ND_ IOURing_PerThread*		_GetIOURing (uint idx)			__NE___	{ return null; }
	  #endif
	};
//-----------------------------------------------------------------------------



	//
	// Unix Read-only Async File
	//
	class UnixAsyncRDataSource final : public AsyncRDataSource
	{
	// types
	public:
		using EMode	= UnixFileRStream::EMode;
	private:
		using Handle_t	= UnixIOService::File_t;	// fd

		static constexpr EMode	c_DefaultMode = EMode::RandomAccess;


	// variables
	private:
		Handle_t		_file		= -1;
		const Bytes		_fileSize;
		const EMode		_mode;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixAsyncRDataSource (Handle_t file, EMode mode DEBUG_ONLY(, Path filename))		__NE___;

	public:
		explicit UnixAsyncRDataSource (const char* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncRDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncRDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncRDataSource (const Path &path, EMode mode = c_DefaultMode)		__NE___;

		~UnixAsyncRDataSource ()															__NE_OV;

		ND_ Handle_t	Handle ()															__NE___	{ return _file; }


		// AsyncRDataSource //
		bool			IsOpen ()															C_NE_OV	{ return _file >= 0; }
		ESourceType		GetSourceType ()													C_NE_OV;

		Bytes			Size ()																C_NE_OV	{ return _fileSize; }

		AsyncDSRequest	ReadBlock (Bytes pos, Bytes size)									__NE_OV;
		AsyncDSRequest	ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem)			__NE_OV;

		bool			CancelAllRequests ()												__NE_OV;

		using AsyncRDataSource::ReadBlock;
	};
//-----------------------------------------------------------------------------



	//
	// Unix Write-only Async File
	//
	class UnixAsyncWDataSource final : public AsyncWDataSource
	{
	// types
	public:
		using EMode	= UnixFileWStream::EMode;
	private:
		using Handle_t	= UnixIOService::File_t;	// fd

		static constexpr EMode	c_DefaultMode = EMode::Unknown;


	// variables
	private:
		Handle_t		_file		= -1;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixAsyncWDataSource (Handle_t file, EMode mode DEBUG_ONLY(, Path filename))		__NE___;

	public:
		explicit UnixAsyncWDataSource (const char* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncWDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncWDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixAsyncWDataSource (const Path &path, EMode mode = c_DefaultMode)		__NE___;

		~UnixAsyncWDataSource ()															__NE_OV;

		ND_ Handle_t	Handle ()															__NE___	{ return _file; }


		// AsyncWDataSource //
		bool			IsOpen ()															C_NE_OV	{ return _file >= 0; }
		ESourceType		GetSourceType ()													C_NE_OV;

		AsyncDSRequest	WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem)	__NE_OV;
		bool			CancelAllRequests ()												__NE_OV;
		RC<SharedMem>	Alloc (SizeAndAlign)												__NE_OV;

		using AsyncWDataSource::WriteBlock;
	};


} // AE::Threading

#endif // AE_PLATFORM_UNIX_BASED

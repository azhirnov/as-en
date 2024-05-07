// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Note:
	- For maximum performance use 'NoBuffering' flag, but it requires to align size to FS block size (512b .. 4Kb).
	- OS file cache may not work for asyncIO, but unaligned access is allowed.
	- Cancellation may not work, actually it has effect only for network filesystem.
*/

#pragma once

#include "threading/DataSource/AsyncDataSource.h"
#include "threading/Containers/LfIndexedPool.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Threading
{
	class WinAsyncRDataSource;
	class WinAsyncWDataSource;


	//
	// Windows IO Service
	//
	class WindowsIOService final : public IOService
	{
	// types
	public:
		using IOPort_t		= UntypedStorage< sizeof(void*), alignof(void*) >;		// HANDLE
		using File_t		= UntypedStorage< sizeof(void*), alignof(void*) >;		// HANDLE
		using Overlapped_t	= UntypedStorage< sizeof(ulong)*4, alignof(ulong) >;	// OVERLAPPED
		using Index_t		= uint;

		class AsyncRDataSourceApi;
		class AsyncWDataSourceApi;

	private:

		//
		// Request Base
		//
		class _RequestBase : public Threading::_hidden_::IAsyncDataSourceRequest
		{
		// variables
		protected:
			Overlapped_t		_overlapped;

			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<>				_memRC;			// keep memory alive


		// methods
		protected:
			_RequestBase ()								__NE___;

				void  _Init (Bytes pos, RC<> mem)		__NE___;
				void  _Cleanup ()						__NE___;
			ND_ bool  _Cancel (const File_t &file)		__NE___;

		private:
			friend class WindowsIOService;
				void  _Complete (Bytes size, long err)	__NE___;
		};


		//
		// Read Request
		//
		class ReadRequestApi;
		class ReadRequest final : public _RequestBase
		{
		// variables
		private:
			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			void*						_data		= null;
			RC<WinAsyncRDataSource>		_dataSource;	// keep alive until request is in progress


		// methods
		public:
			// IAsyncDataSourceRequest //
			Result		GetResult ()					C_NE_OV;
			bool		Cancel ()						__NE_OV;
			Promise_t	AsPromise (ETaskQueue)			__NE_OV;

		private:
			friend class AsyncRDataSourceApi;
			ND_ bool  _Create (RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___;

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
			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<WinAsyncWDataSource>		_dataSource;	// keep alive until request is in progress


		// methods
		public:
			// IAsyncDataSourceRequest //
			Result		GetResult ()					C_NE_OV;
			bool		Cancel ()						__NE_OV;
			Promise_t	AsPromise (ETaskQueue)			__NE_OV;

		private:
			friend class AsyncWDataSourceApi;
			ND_ bool  _Create (RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___;

			ND_ ResultWithRC  _GetResult ()				__NE___;

				void  _ReleaseObject ()					__NE_OV;
		};


	private:
		static constexpr uint		c_OverlappedOffset = offsetof( _RequestBase, _overlapped );

		template <typename T, usize ChunkSize, usize MaxChunks>
		using PoolTmpl				= LfIndexedPool< T, Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

		static constexpr uint		_ReqChunkSize = 4u << 10;
		using ReadRequestPool_t		= PoolTmpl< ReadRequest,  _ReqChunkSize, 32 >;
		using WriteRequestPool_t	= PoolTmpl< WriteRequest, _ReqChunkSize, 32 >;


	// variables
	private:
		IOPort_t			_ioCompletionPort;

		ReadRequestPool_t	_readResultPool;
		WriteRequestPool_t	_writeResultPool;


	// methods
	public:
		~WindowsIOService ()								__NE___;

		ND_ bool			IsInitialized ()				C_NE___;

		ND_ IOPort_t const&	GetIOCompletionPort ()			C_NE___	{ return _ioCompletionPort; }


		// IOService //
		usize				ProcessEvents ()				__NE_OV;
		EIOServiceType		GetIOServiceType ()				C_NE_OV	{ return EIOServiceType::File; }


	private:
		friend class TaskScheduler;
		explicit WindowsIOService (uint maxAccessThreads)	__NE___;
	};
//-----------------------------------------------------------------------------



	//
	// Windows Read-only Async File
	//
	class WinAsyncRDataSource final : public AsyncRDataSource
	{
	// types
	public:
		using EMode	= WinFileRStream::EMode;
	private:
		using File_t	= WindowsIOService::File_t;

		static constexpr EMode	c_DefaultMode = EMode::RandomAccess;


	// variables
	private:
		File_t			_file;
		const Bytes		_fileSize;
		const EMode		_mode;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		WinAsyncRDataSource (const File_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinAsyncRDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinAsyncRDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncRDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinAsyncRDataSource (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncRDataSource (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncRDataSource (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinAsyncRDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinAsyncRDataSource ()																__NE_OV;

		ND_ File_t const&	Handle ()														__NE___	{ return _file; }


		// AsyncRDataSource //
		bool			IsOpen ()															C_NE_OV;
		ESourceType		GetSourceType ()													C_NE_OV;

		Bytes			Size ()																C_NE_OV	{ return _fileSize; }

		AsyncDSRequest	ReadBlock (Bytes pos, Bytes size)									__NE_OV;
		AsyncDSRequest	ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem)			__NE_OV;

		bool			CancelAllRequests ()												__NE_OV;

		using AsyncRDataSource::ReadBlock;
	};
//-----------------------------------------------------------------------------



	//
	// Windows Write-only Async File
	//
	class WinAsyncWDataSource final : public AsyncWDataSource
	{
	// types
	public:
		using EMode	= WinFileWStream::EMode;
	private:
		using File_t	= WindowsIOService::File_t;

		static constexpr EMode	c_DefaultMode = EMode::SharedRead;


	// variables
	private:
		File_t			_file;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		WinAsyncWDataSource (const File_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinAsyncWDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinAsyncWDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncWDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinAsyncWDataSource (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncWDataSource (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinAsyncWDataSource (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinAsyncWDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinAsyncWDataSource ()																__NE_OV;

		ND_ File_t const&	Handle ()														__NE___	{ return _file; }


		// AsyncWDataSource //
		bool			IsOpen ()															C_NE_OV;
		ESourceType		GetSourceType ()													C_NE_OV;

		AsyncDSRequest	WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem)	__NE_OV;
		bool			CancelAllRequests ()												__NE_OV;
		RC<SharedMem>	Alloc (SizeAndAlign)												__NE_OV;

		using AsyncWDataSource::WriteBlock;
	};


} // AE::Threading

#endif // AE_PLATFORM_WINDOWS

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/FixedTupleArray.h"
#include "base/DataSource/WindowsFile.h"
#include "threading/DataSource/AsyncDataSource.h"
#include "threading/Containers/LfIndexedPool3.h"

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
		using IOPort_t		= UntypedStorage< sizeof(void*), alignof(void*) >;
		using File_t		= UntypedStorage< sizeof(void*), alignof(void*) >;
		using Overlapped_t	= UntypedStorage< sizeof(ulong)*4, alignof(ulong) >;
		using Index_t		= uint;

		class AsyncRDataSourceApi;
		class AsyncWDataSourceApi;

	private:
		static constexpr uint	OverlappedOffset = sizeof(void*)*4;

		using AsyncRequestPtr = RC<_hidden_::IAsyncDataSourceRequest>;


		//
		// Request Base
		//
		class _RequestBase : public _hidden_::IAsyncDataSourceRequest
		{
		// types
		protected:
			using Dependencies_t	= FixedTupleArray< 4, AsyncTask, ubyte >;	// { task, bitIndex }

		// variables
		protected:
			Atomic<void*>		_file;

			Overlapped_t		_overlapped;
			
			SpinLock			_depsGuard;
			Dependencies_t		_deps;

			// read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
			RC<SharedMem>		_data;
			RC<IDataSource>		_dataSource;

			const Index_t		_indexInPool;	// can be used in 'ReadRequestApi' or 'WriteRequestApi'


		// methods
		public:

			// IAsyncDataSourceRequest //
			bool		Cancel ()				__NE_OF;
			Result		GetResult ()			C_NE_OF;

		protected:
			explicit _RequestBase (Index_t idx)	__NE___;

			void  _Init (Bytes offset, RC<SharedMem> data, RC<IDataSource> ds, const File_t &file) __NE___;
			void  _Cleanup ()					__NE___;
			
			ND_ ResultWithRC  _GetResult1 ()	__NE___;
			ND_ ResultWithRC  _GetResult2 ()	__NE___;

		private:
			friend class WindowsIOService;
			void  _Complete (Bytes size)		__NE___;

			ND_ bool  _AddOnCompleteDependency (AsyncTask task, INOUT uint &index) __NE___;
		};
		

		//
		// Read Request
		//
		class ReadRequestApi;
		class ReadRequest final : public _RequestBase
		{
		// methods
		public:
			explicit ReadRequest (Index_t idx)	__NE___ : _RequestBase{idx} {}
			
			// IAsyncDataSourceRequest //
			Promise_t	AsPromise ()			__NE_OF;

		private:
			friend class AsyncRDataSourceApi;
			ND_ bool  _Create (RC<WinAsyncRDataSource> file, Bytes offset, Bytes size, RC<SharedMem> dst) __NE___;
			
				void  _ReleaseObject () __NE_OV;
		};


		//
		// Write Request
		//
		class WriteRequestApi;
		class WriteRequest final : public _RequestBase
		{
		// methods
		public:
			explicit WriteRequest (Index_t idx)	__NE___	: _RequestBase{idx} {}
			
			// IAsyncDataSourceRequest //
			Promise_t	AsPromise ()			__NE_OF;

		private:
			friend class AsyncWDataSourceApi;
			ND_ bool  _Create (RC<WinAsyncWDataSource> file, Bytes offset, Bytes size, RC<SharedMem> src) __NE___;
			
				void  _ReleaseObject () __NE_OV;
		};


	private:
		template <typename T, usize ChunkSize, usize MaxChunks>
		using PoolTmpl			= LfIndexedPool2< T, Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

		using ReadRequestPool_t	= PoolTmpl< ReadRequest,  1u<<10, 8 >;
		using WriteRequestPool_t	= PoolTmpl< WriteRequest, 1u<<10, 8 >;


	// variables
	private:
		IOPort_t			_ioCompletionPort;

		ReadRequestPool_t	_readResultPool;
		WriteRequestPool_t	_writeResultPool;


	// methods
	public:
		~WindowsIOService ()						__NE___;

		ND_ bool			IsInitialized ()		C_NE___;

		ND_ IOPort_t const&	GetIOCompletionPort ()	C_NE___	{ return _ioCompletionPort; }


		// IOService //
		usize			ProcessEvents ()			__NE_OV;
		EIOServiceType  GetIOServiceType ()			C_NE_OV	{ return EIOServiceType::File; }
		

	private:
		friend class TaskScheduler;
		explicit WindowsIOService (uint maxThreads)	__NE___;

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE_OV;
	};

		
	class WindowsIOService::ReadRequestApi
	{
		friend class ReadRequest;
		static void  Recycle (Index_t indexInPool) __NE___;
	};

	class WindowsIOService::AsyncRDataSourceApi
	{
		friend class WinAsyncRDataSource;
		ND_ static AsyncDSRequest  CreateResult (RC<WinAsyncRDataSource> file, Bytes offset, Bytes size, RC<SharedMem> block) __NE___;
	};

	class WindowsIOService::WriteRequestApi
	{
		friend class WriteRequest;
		static void  Recycle (Index_t indexInPool) __NE___;
	};

	class WindowsIOService::AsyncWDataSourceApi
	{
		friend class WinAsyncWDataSource;
		ND_ static AsyncDSRequest  CreateResult (RC<WinAsyncWDataSource> file, Bytes offset, Bytes size, RC<SharedMem> block) __NE___;
	};
//-----------------------------------------------------------------------------



	//
	// Windows Read-only Async File
	//
	class WinAsyncRDataSource final : public AsyncRDataSource
	{
	// types
	public:
		using EFlags	= WinRFileStream::EFlags;
	private:
		using File_t	= WindowsIOService::File_t;

		static constexpr EFlags	DefaultFlags	= EFlags::RandomAccess;


	// variables
	private:
		File_t			_file;
		const Bytes		_fileSize;
		const EFlags	_flags;
		
		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		WinAsyncRDataSource (const File_t &file, EFlags flags DEBUG_ONLY(, Path filename))	__NE___;

	public:
		WinAsyncRDataSource (const char* filename, EFlags flags = DefaultFlags)				__NE___;
		WinAsyncRDataSource (NtStringView filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncRDataSource (const String &filename, EFlags flags = DefaultFlags)			__NE___;
		
		WinAsyncRDataSource (NtWStringView filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncRDataSource (const wchar_t* filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncRDataSource (const WString &filename, EFlags flags = DefaultFlags)			__NE___;

		WinAsyncRDataSource (const Path &path, EFlags flags = DefaultFlags)					__NE___;

		~WinAsyncRDataSource ()									__NE_OV;

		ND_ File_t const&	Handle ()							__NE___	{ return _file; }


		// AsyncRDataSource //
		bool			IsOpen ()								C_NE_OV;
		ESourceType		GetSourceType ()						C_NE_OV;

		Bytes			Size ()									C_NE_OV	{ return _fileSize; }

		AsyncDSRequest	ReadBlock (Bytes, Bytes)				__NE_OV;
		AsyncDSRequest	ReadBlock (Bytes, Bytes, RC<SharedMem>)	__NE_OV;

		void			CancelAllRequests ()					__NE_OV;
	};
//-----------------------------------------------------------------------------


	
	//
	// Windows Write-only Async File
	//
	class WinAsyncWDataSource final : public AsyncWDataSource
	{
	// types
	public:
		using EFlags	= WinWFileStream::EFlags;
	private:
		using File_t	= WindowsIOService::File_t;
		
		static constexpr EFlags	DefaultFlags	= EFlags::Unknown;


	// variables
	private:
		File_t			_file;
		const EFlags	_flags;
		
		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		WinAsyncWDataSource (const File_t &file, EFlags flags DEBUG_ONLY(, Path filename))	__NE___;

	public:
		WinAsyncWDataSource (const char* filename, EFlags flags = DefaultFlags)				__NE___;
		WinAsyncWDataSource (NtStringView filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncWDataSource (const String &filename, EFlags flags = DefaultFlags)			__NE___;
		
		WinAsyncWDataSource (NtWStringView filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncWDataSource (const wchar_t* filename, EFlags flags = DefaultFlags)			__NE___;
		WinAsyncWDataSource (const WString &filename, EFlags flags = DefaultFlags)			__NE___;

		WinAsyncWDataSource (const Path &path, EFlags flags = DefaultFlags)					__NE___;
		
		~WinAsyncWDataSource ()									__NE_OV;

		ND_ File_t const&	Handle ()							__NE___	{ return _file; }


		// AsyncWDataSource //
		bool			IsOpen ()								C_NE_OV;
		ESourceType		GetSourceType ()						C_NE_OV;

		//Bytes			Size ()									C_NE_OV;

		AsyncDSRequest	WriteBlock (Bytes, Bytes, RC<SharedMem>)__NE_OV;
		void			CancelAllRequests ()					__NE_OV;
		RC<SharedMem>	Alloc (Bytes size)						__NE_OV;
	};


} // AE::Threading

#endif // AE_PLATFORM_WINDOWS

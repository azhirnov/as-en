// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_NETWORKING
# include "vfs/Network/NetworkStorageBase.h"

namespace AE::VFS
{

	//
	// Network Storage Client
	//
	class NetworkStorageClient final : public NetworkStorageBase
	{
	// types
	private:
		enum class EStatus : ubyte
		{
			Initial,
			Pending,
			Open,
			Failed,
			Closed,
		};


		//
		// Async Write Block Task
		//
		class AsyncWriteBlockTask final : public IAsyncTask
		{
		// variables
		private:
			const NDSRequestID	_id;
			const void *		_data;
			const Bytes			_dataSize;
			Bytes				_sent;
			uint				_partIdx	= 0;
			Bytes				_pos;
			RC<>				_memRC;				// keep memory alive

		// methods
		public:
			AsyncWriteBlockTask (NDSRequestID id, const void* data, Bytes dataSize, Bytes pos, RC<> mem) __NE___ :
				IAsyncTask{ ETaskQueue::Background },
				_id{id}, _data{data}, _dataSize{dataSize},
				_pos{pos}, _memRC{RVRef(mem)}
			{}

			void		Run ()			__Th_OV;
			void		OnCancel ()		__NE_OV;
			StringView	DbgName ()		C_NE_OV	{ return "AsyncWriteBlockTask"; }
		};



		//
		// Protected Pointer for Request
		//
		template <typename T>
		struct Shared : MovableOnly
		{
			T*		_req	= null;

			Shared ()								__NE___	{}
			explicit Shared (T* req)				__NE___	: _req{req} {}
			Shared (Shared &&other)					__NE___ : _req{other._req} { other._req = null; }
			~Shared ()								__NE___	{ if_likely( _req ) _req->Guard().unlock_shared(); }

			ND_ T*  operator -> ()					__NE___	{ NonNull( _req );  return _req; }
			ND_ explicit operator bool ()			C_NE___	{ return _req != null; }
		};

		template <typename T>
		struct Exclusive : MovableOnly
		{
			T*		_req	= null;

			Exclusive ()							__NE___	{}
			Exclusive (Exclusive &&other)			__NE___ : _req{other._req} { other._req = null; }
			explicit Exclusive (T* req)				__NE___	: _req{req} {}
			explicit Exclusive (Shared<T> &&other)	__NE___ : _req{other._req} { other._req = null;  if_likely( _req ) _req->Guard().shared_to_exclusive(); }
			~Exclusive ()							__NE___	{ if_likely( _req ) _req->Guard().unlock(); }

			ND_ T*  operator -> ()					__NE___	{ NonNull( _req );  return _req; }
			ND_ explicit operator bool ()			C_NE___	{ return _req != null; }
		};



		//
		// Request Base
		//
		class _RequestBase : public Threading::_hidden_::IAsyncDataSourceRequest
		{
		// variables
		protected:
			mutable RWSpinLock	_guard;
			ReqGen_t			_generation	{0};
			Bytes				_pos;


		// methods
		protected:
			void  _Cleanup ()								__NE___;

		public:
			ND_ ReqGen_t		Generation ()				C_NE___	{ ASSERT( not _guard.is_unlocked() );  return _generation; }
			ND_ RWSpinLock&		Guard ()					__NE___	{ return _guard; }
		};


		//
		// Read Request
		//
		class NetReadRequest final : public _RequestBase
		{
		// variables
		private:
			Bytes		_dataSize;			// requested, actual may be less
			void*		_data		= null;
			uint		_partCount	= 0;
			RC<>		_memRC;				// keep memory alive


		// methods
		public:
			ND_ bool	Init (Bytes pos, Bytes size, void* data, RC<> mem)		__NE___;
				void	Complete (Bytes size, HashVal64 hash)					__NE___;
				void	Update (ushort partIdx, const void* data, Bytes size)	__NE___;

			// IAsyncDataSourceRequest //
			Result		GetResult ()						C_NE_OV;
			bool		Cancel ()							__NE_OV;
			Promise_t	AsPromise (ETaskQueue)				__NE_OV;

		private:
				void	_ReleaseObject ()					__NE_OV;

			ND_ ResultWithRC  _GetResult ()					__NE___;
		};


		//
		// Write Request
		//
		class NetWriteRequest final : public _RequestBase
		{
		// variables
		private:
			AsyncTask	_task;


		// methods
		public:
			ND_ bool	Init (AsyncTask task, Bytes pos)	__NE___;
				void	Complete (Bytes written)			__NE___;
				void	Failed ()							__NE___	{ Complete( 0_b ); }

			// IAsyncDataSourceRequest //
			Result		GetResult ()						C_NE_OV;
			bool		Cancel ()							__NE_OV;
			Promise_t	AsPromise (ETaskQueue)				__NE_OV;

		private:
				void	_ReleaseObject ()					__NE_OV;

			ND_ ResultWithRC  _GetResult ()					__NE___;
		};


		//
		// Network Async Read-only Data Source
		//
		class NetRDataSource final : public AsyncRDataSource
		{
		// variables
		public:
			Atomic<DSGen_t>			_generation		{0};
			Atomic<EStatus>			_open			{EStatus::Initial};
			AtomicByte<Bytes>		_fileSize;
			Atomic<Index_t>			_index			{~Index_t{0}};


		// methods
		public:
			ND_ DSGen_t			Generation ()													C_NE___	{ return _generation.load(); }
			ND_ NetDataSourceID	ID ()															C_NE___	{ return NetDataSourceID{ _index.load(), Generation() }; }

			// AsyncRDataSource //
			bool			IsOpen ()															C_NE_OV	{ return AnyEqual( _open.load(), EStatus::Open, EStatus::Pending ); }
			ESourceType		GetSourceType ()													C_NE_OV;
			Bytes			Size ()																C_NE_OV	{ return _fileSize.load(); }
			ReadRequestPtr  ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem)			__NE_OV;
			ReadRequestPtr  ReadBlock (Bytes pos, Bytes size)									__NE_OV;
			bool			CancelAllRequests ()												__NE_OV;

			// EnableRC //
			void			_ReleaseObject ()													__NE_OV;

			ND_ bool		_ReadBlockImpl (OUT ReadRequestPtr &, Bytes pos, void* data,
											Bytes dataSize, RC<> mem)							__NE___;
		};


		//
		// Network Async Write-only Data Source
		//
		class NetWDataSource final : public AsyncWDataSource
		{
		// variables
		public:
			Atomic<DSGen_t>			_generation		{0};
			Atomic<EStatus>			_open			{EStatus::Initial};
			Atomic<Index_t>			_index			{~Index_t{0}};


		// methods
		public:
			ND_ DSGen_t			Generation ()													C_NE___	{ return _generation.load(); }
			ND_ NetDataSourceID	ID ()															C_NE___	{ return NetDataSourceID{ _index.load(), Generation() }; }

			// AsyncWDataSource //
			bool			IsOpen ()															C_NE_OV	{ return AnyEqual( _open.load(), EStatus::Open, EStatus::Pending ); }
			ESourceType		GetSourceType ()													C_NE_OV;
			RC<SharedMem>	Alloc (SizeAndAlign)												__NE_OV;
			WriteRequestPtr	WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem)	__NE_OV;
			bool			CancelAllRequests ()												__NE_OV;

			// EnableRC //
			void			_ReleaseObject ()													__NE_OV;

			ND_ bool		_WriteBlockImpl (OUT WriteRequestPtr &, Bytes pos, const void* data,
											 Bytes dataSize, RC<> mem)							__NE___;
		};


	private:
		template <typename T, usize Count>
		using PoolTmpl				= LfStaticIndexedPool< T, Index_t, Count, GlobalLinearAllocatorRef >;

		using ReadRequestPool_t		= PoolTmpl< NetReadRequest,  _ReqCount >;
		using WriteRequestPool_t	= PoolTmpl< NetWriteRequest, _ReqCount >;
		using RDataSourcePool_t		= PoolTmpl< NetRDataSource,  _FileCount >;
		using WDataSourcePool_t		= PoolTmpl< NetWDataSource,  _FileCount >;


		class MsgProducer final :
			public AsyncCSMessageProducer< LfLinearAllocator< usize{NetConfig::ChannelStorageSize * 4}, usize{8_b}, 2 >>
		{
		public:
			EnumSet<EChannel>  GetChannels ()	C_NE_OV	{ return {EChannel::Reliable}; }
		};


		class MsgConsumer final : public ICSMessageConsumer
		{
		private:
			NetworkStorageClient&	_client;

		public:
			MsgConsumer (NetworkStorageClient &)				__NE___;

			// ICSMessageConsumer //
			CSMessageGroupID  GetGroupID ()						C_NE_OV	{ return CSMessageGroup::NetVFS; }
			void  Consume (ChunkList<const CSMessagePtr>)		__NE_OV;
		};


	// variables
	private:
		StaticRC<MsgProducer>	_msgProducer;
		StaticRC<MsgConsumer>	_msgConsumer;

		ReadRequestPool_t		_readResultPool;
		WriteRequestPool_t		_writeResultPool;

		RDataSourcePool_t		_readDSPool;
		WDataSourcePool_t		_writeDSPool;


	// methods
	public:
		NetworkStorageClient ()											__NE___;
		~NetworkStorageClient ()										__NE___;

		ND_ bool  Init (StringView prefix)								__NE___;

		ND_ auto  OpenForRead (FileName::Ref name)						__NE___	-> RC<AsyncRDataSource>;
		ND_ auto  OpenForWrite (FileName::Ref name)						__NE___	-> RC<AsyncWDataSource>;

		ND_ ICSMessageProducer&  GetMessageProducer ()					__NE___ { return *_msgProducer; }
		ND_ ICSMessageConsumer&  GetMessageConsumer ()					__NE___ { return *_msgConsumer; }


	private:
		ND_ auto  _GetReadReq (NDSRequestID id)							__NE___ -> Shared<NetReadRequest>;
		ND_ auto  _GetWriteReq (NDSRequestID id)						__NE___ -> Shared<NetWriteRequest>;

		ND_ auto  _GetReadDS (NetDataSourceID id)						__NE___ -> NetRDataSource*;
		ND_ auto  _GetWriteDS (NetDataSourceID id)						__NE___ -> NetWDataSource*;

		template <typename T>
		ND_ auto  _CreateMsg (Bytes extraSize = 0_b)					__NE___	{ return _msgProducer->CreateMsg<T>( extraSize ); }

		template <typename T>
		ND_ auto  _CreateMsgOpt (Bytes extraSize = 0_b)					__NE___	{ return _msgProducer->CreateMsgOpt<T>( extraSize ); }

		template <typename T>
		ND_ bool  _AddMessage (T &msg)									__NE___	{ return _msgProducer->AddMessage( msg ); }


	private:
		void  _OpenForReadResult (CSMsg_VFS_OpenForReadResult const&)	__NE___;
		void  _OpenForWriteResult (CSMsg_VFS_OpenForWriteResult const&)	__NE___;

		void  _CloseReadFile (CSMsg_VFS_CloseReadFile const&)			__NE___;
		void  _CloseWriteFile (CSMsg_VFS_CloseWriteFile const&)			__NE___;

		void  _ReadResult (CSMsg_VFS_ReadResult const&)					__NE___;
		void  _ReadComplete (CSMsg_VFS_ReadComplete const&)				__NE___;

		void  _WriteComplete (CSMsg_VFS_WriteComplete const&)			__NE___;
	};


} // AE::VFS

#endif // AE_ENABLE_NETWORKING

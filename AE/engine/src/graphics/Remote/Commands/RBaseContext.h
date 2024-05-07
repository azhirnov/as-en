// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/ContextValidation.h"
# include "graphics/Remote/Commands/RBarrierManager.h"
# include "graphics/Remote/Commands/RAccumDeferredBarriers.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{

	//
	// Remote software Command Buffer
	//

	class RSoftwareCmdBuf
	{
	// types
	private:
		using HostToDevCopy_t	= RemoteGraphics::Msg::CmdBuf_Bake::HostToDevCopy_t;
		using DevToHostCopy_t	= RemoteGraphics::Msg::CmdBuf_Bake::DevToHostCopy_t;

		struct HostToDevCopy
		{
			const void*		ptr		= null;
			Bytes			begin	{UMax};
			Bytes			end;
		};
		using HostToDevCopy2_t	= FlatHashMap< RmDevicePtr, HostToDevCopy >;

		static constexpr uint	c_MaxBlocks	= 8;

		struct Block
		{
			void*	data	= null;
			Bytes	size;			// used memory size
		};
		using Blocks_t	= FixedArray< Block, c_MaxBlocks >;


	// variables
	private:
		uint								_cmdCount	= 0;

		RC<ArrayWStream>					_memStream;
		Unique<Serializing::Serializer>		_ser;

		Bytes								_blockCapacity;
		Blocks_t							_blocks;		// linear allocator
		HostToDevCopy_t						_hostToDev;
		HostToDevCopy2_t					_hostToDev2;
		DevToHostCopy_t						_devToHost;


	// methods
	public:
		RSoftwareCmdBuf ()																__NE___;

		ND_ RmCommandBufferID		Bake (uint exeIdx, RmCommandBatchID)				__NE___;
		ND_ RmDrawCommandBufferID	Bake (uint drawIdx, RmDrawCommandBatchID)			__NE___;
		ND_ bool					IsValid ()											C_NE___;

			void   AddCommand (const RemoteGraphics::Msg::CmdBuf_Bake::BaseCmd &)		__Th___;
		ND_	void*  Allocate (RmDevicePtr ptr, Bytes size)								__Th___;
			void   Allocate (OUT void* &, OUT RmDeviceOffset &, Bytes size)				__Th___;
			void   CopyHostToDev (RmDevicePtr dst, void* src, Bytes offset, Bytes size)	__Th___;
		ND_	void*  ReadbackAlloc (FrameUID frameId, RmDevicePtr ptr, Bytes size)		__Th___;
			void   PipelineBarrier (const RDependencyInfo &)							__Th___;
			void   WriteTimestamp (const RQueryManager::Query &, uint, EPipelineScope)	__Th___;

	private:
		void  _Upload (OUT Array<RemoteGraphics::Msg::UploadData> &,
					   OUT Array<RemoteGraphics::Msg::BaseMsg*> &)						__NE___;
		void  _Allocate (OUT void* &, OUT RmDeviceOffset &, Bytes size)					__Th___;

		void  _Cleanup (auto &sbm)														__NE___;
	};

	using RSoftwareCmdBufPtr = Unique< RSoftwareCmdBuf >;



	//
	// Remote Indirect Context base class
	//

	class _RBaseContext
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = true;

		using CmdBuf_t = RSoftwareCmdBufPtr;


	// variables
	protected:
		CmdBuf_t	_cmdbuf;


	// methods
	public:
		virtual ~_RBaseContext ()															__NE___	{ DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }

		void  PipelineBarrier (const RDependencyInfo &info)									__Th___	{ _cmdbuf->PipelineBarrier( info ); }
		void  CopyHostToDev (RmDevicePtr dst, void* src, Bytes offset, Bytes size)			__Th___	{ return _cmdbuf->CopyHostToDev( dst, src, offset, size ); }

	protected:
		explicit _RBaseContext (DebugLabel dbg)												__Th___ : _RBaseContext{ dbg, Default } {}
		explicit _RBaseContext (CmdBuf_t cmdbuf)											__Th___	: _cmdbuf{RVRef(cmdbuf)} { CHECK_THROW( _IsValid() ); }
		_RBaseContext (DebugLabel dbg, CmdBuf_t cmdbuf)										__Th___;

		ND_	void* _Allocate (RmDevicePtr ptr, Bytes size)									__Th___	{ return _cmdbuf->Allocate( ptr, size ); }

			void  _DebugMarker (DebugLabel dbg)												__Th___;
			void  _PushDebugGroup (DebugLabel dbg)											__Th___;
			void  _PopDebugGroup ()															__Th___;

		ND_ bool  _IsValid ()																C_NE___	{ return _cmdbuf and _cmdbuf->IsValid(); }

		ND_ RmCommandBufferID		_EndCommandBuffer (uint, RmCommandBatchID)				__Th___;
		ND_ RmDrawCommandBufferID	_EndCommandBuffer (uint, RmDrawCommandBatchID)			__Th___;
		ND_ CmdBuf_t				_ReleaseCommandBuffer ()								__Th___;

		ND_ static CmdBuf_t  _ReuseOrCreateCommandBuffer (CmdBuf_t cmdbuf, DebugLabel dbg)	__Th___;
	};



	//
	// Remote Indirect Context base class with barrier manager
	//

	class RBaseContext : public _RBaseContext
	{
	// types
	protected:
		using ECtxType = IGraphicsProfiler::EContextType;


	// variables
	protected:
		RBarrierManager		_mngr;


	// methods
	public:
		RBaseContext (const RenderTask &, CmdBuf_t, DebugLabel, ECtxType)					__Th___;
		~RBaseContext ()																	__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
			void	_CommitBarriers ()														__Th___;
		ND_ bool	_NoPendingBarriers ()													C_NE___	{ return _mngr.NoPendingBarriers(); }

		ND_	void*	_ReadbackAlloc (RmDevicePtr ptr, Bytes size)							__Th___	{ return _cmdbuf->ReadbackAlloc( _mngr.GetFrameId(), ptr, size ); }
			void	_WriteTimestamp (const RQueryManager::Query &, uint, EPipelineScope)	__Th___;

		ND_ RmCommandBufferID	_EndCommandBuffer (ECtxType)								__Th___;
		ND_ CmdBuf_t			_ReleaseCommandBuffer (ECtxType)							__Th___;

	private:
		RBaseContext (const RenderTask &, CmdBuf_t, DebugLabel, ECtxType, int)				__Th___;
	};



/*
=================================================
	AddCommand
=================================================
*/
	inline void  RSoftwareCmdBuf::AddCommand (const RemoteGraphics::Msg::CmdBuf_Bake::BaseCmd &cmd) __Th___
	{
		++_cmdCount;
		CHECK_THROW( (*_ser)( &cmd ));
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS

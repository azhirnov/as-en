// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Resources/Buffer.h"
#include "Resources/BufferView.h"
#include "Core/RenderGraph.h"
#include "Passes/Renderer.h"

namespace AE::ResEditor
{
namespace
{
	static constexpr auto	c_DevAddrUsage =	EBufferUsage::ShaderAddress	| EBufferUsage::ShaderBindingTable |
												EBufferUsage::ASBuild_ReadOnly	| EBufferUsage::ASBuild_Scratch;
}

/*
=================================================
	LoadOp2::IsDefined
=================================================
*/
	bool  Buffer::LoadOp2::IsDefined () C_NE___
	{
		return file or (not data.empty()) or clear;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	Buffer::Buffer (Renderer& renderer, StringView dbgName) __NE___ :
		IResource{ renderer },
		_flags{ Default },
		_dbgName{ dbgName }
	{}

	Buffer::Buffer (IDs_t				ids,
					const BufferDesc &	desc,
					Bytes				staticSize,
					Bytes				elemSize,
					LoadOp				loadOp,
					ShaderStructName	typeName,
					Renderer &			renderer,
					RC<DynamicUInt>		inDynCount,
					RC<DynamicUInt>		outDynCount,
					StringView			dbgName,
					EBufferFlags		flags,
					Array<RC<Buffer>>	refBuffers,
					Array<RC<BufferView>> views) __NE___ :
		IResource{ renderer },
		_typeName{ typeName },
		_staticSize{ staticSize },
		_elemSize{ elemSize },
		_requiredBufDesc{ desc },
		_inDynCount{ RVRef(inDynCount) },
		_outDynCount{ RVRef(outDynCount) },
		_loadOp{ RVRef(loadOp) },
		_flags{ flags },
		_dbgName{ dbgName },
		_refBuffers{ RVRef(refBuffers) },
		_views{ RVRef(views) }
	{
		for (usize i = 0; i < ids.size(); ++i) {
			Unused( _ids[i].Attach( RVRef(ids[i]) ));
		}
	}

/*
=================================================
	_Init
=================================================
*/
	void  Buffer::_Init () __Th___
	{
		if ( _inDynCount )
			CHECK_THROW( _elemSize > 0_b );

		if ( HasHistory() )
			CHECK_THROW( not _inDynCount );

		if ( not _loadOp.IsDefined() )
		{
			CHECK_THROW( _ids[0].IsValid() );
			_uploadStatus.store( EUploadStatus::Completed );

			auto&	res_mngr = RenderGraph().GetStateTracker();
			for (auto& id : _ids) {
				res_mngr.AddResourceIfNotTracked( id.Get() );
			}
		}
		else
		{
			_uploadStatus.store( EUploadStatus::InProgress );

			if ( _loadOp.file )
			{
				_loadOp.request = _loadOp.file->ReadRemaining( 0_b );	// TODO: read by blocks?
				CHECK_THROW( _loadOp.request );

				if ( _outDynCount )
					_outDynCount->Set( 0 );
			}

			_DtTrQueue().EnqueueForUpload( GetRC() );
		}

		if ( AnyBits( _requiredBufDesc.usage, c_DevAddrUsage ))
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();
			for (usize i = 0; i < _ids.size(); ++i) {
				_address[i] = BitCast<ulong>(res_mngr.GetResourcesOrThrow( _ids[i].Get() ).GetDeviceAddress());
			}
		}
	}

/*
=================================================
	Create
=================================================
*/
	RC<Buffer>  Buffer::Create (Renderer&	renderer,
								StringView	dbgName) __Th___
	{
		return RC<Buffer>{ new Buffer{ renderer, dbgName }};
	}

	RC<Buffer>  Buffer::Create (IDs_t				ids,
								const BufferDesc &	desc,
								Bytes				staticSize,
								Bytes				elemSize,
								LoadOp				loadOp,
								ShaderStructName	typeName,
								Renderer &			renderer,
								RC<DynamicUInt>		inDynCount,
								RC<DynamicUInt>		outDynCount,
								StringView			dbgName,
								EBufferFlags		flags,
								Array<RC<Buffer>>	refBuffers,
								Array<RC<BufferView>> refViews)	__Th___
	{
		RC<Buffer>	res{ new Buffer{ RVRef(ids), desc, staticSize, elemSize, RVRef(loadOp), typeName, renderer,
									 RVRef(inDynCount), RVRef(outDynCount), dbgName, flags, RVRef(refBuffers), RVRef(refViews) }};
		res->_Init();
		return res;
	}

/*
=================================================
	destructor
=================================================
*/
	Buffer::~Buffer () __NE___
	{
		Cancel();

		{
			auto&	res_mngr = RenderGraph().GetStateTracker();
			for (auto& a_id : _ids)
			{
				auto	id = a_id.Release();
				res_mngr.ReleaseResource( id );
			}
		}
	}

/*
=================================================
	destructor
=================================================
*/
	void  Buffer::Cancel () __NE___
	{
		IResource::Cancel();

		if ( _loadOp.file )		_loadOp.file->CancelAllRequests();
		if ( _loadOp.request )	_loadOp.request->Cancel();
		_loadOp = Default;

		if ( _storeOp.file )	CHECK( not _storeOp.file->CancelAllRequests() );	// all write requests must complete
		_storeOp = Default;
	}

/*
=================================================
	RequireResize
=================================================
*/
	bool  Buffer::RequireResize () C_Th___
	{
		if ( not _inDynCount )
			return false;

		uint	count = uint(ArraySize());

		if_likely( not _inDynCount->IsChanged( INOUT count ) or count == 0 )
			return false;

		return true;
	}

/*
=================================================
	Resize
=================================================
*/
	bool  Buffer::Resize (TransferCtx_t &ctx) __Th___
	{
		if ( not _inDynCount )
			return true;

		CHECK_ERR( not HasHistory() );

		uint	count = uint(ArraySize());

		if_likely( not _inDynCount->IsChanged( INOUT count ) or count == 0 )
			return true;

		BufferDesc	desc = _requiredBufDesc;
		desc.size = _staticSize + Max( count, 1u ) * _elemSize;

		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	rs_track	= RenderGraph().GetStateTracker();

		auto	buf = res_mngr.CreateBuffer( desc, _dbgName, _Renderer().ChooseAllocator( True{"dynamic"}, desc.size ));
		CHECK_ERR( buf );

		rs_track.AddResource( buf.Get(),
							  EResourceState::_InvalidState,	// current is not used
							  EResourceState::General,			// default
							  ctx.GetCommandBatchRC() );

		for (auto& id : _ids)
		{
			auto	old_buf	= id.Attach( res_mngr.AcquireResource( buf.Get() ));
			res_mngr.ReleaseResource( old_buf );	// release dummy resource
		}

		if ( AnyBits( desc.usage, c_DevAddrUsage ))
		{
			for (usize i = 0; i < _ids.size(); ++i) {
				_address[i] = BitCast<ulong>(res_mngr.GetResourcesOrThrow( _ids[i].Get() ).GetDeviceAddress());
			}
		}

		for (auto& view : _views) {
			view->_OnBufferResized( buf.Get(), desc.size );
		}

		if ( _loadOp.clear )
			ctx.FillBuffer( buf, 0_b, desc.size, 0 );

		if ( _outDynCount )
			_outDynCount->Set( count );

		res_mngr.ReleaseResource( buf );
		return true;
	}

/*
=================================================
	_SetUploadStatus
=================================================
*/
	void  Buffer::_SetUploadStatus (EUploadStatus newStatus) __NE___
	{
		_loadOp = Default;

		IResource::_SetUploadStatus( newStatus );

		if ( newStatus == EUploadStatus::Completed and _outDynCount )
			_outDynCount->Set( CheckCast{ ArraySize() });
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  Buffer::Upload (TransferCtx_t &ctx)
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		const auto	CopyHistory = [this, &ctx] (Bytes size)
		{{
			if ( this->HasHistory() )
			{
				for (usize i = 1; i < _ids.size(); ++i)
				{
					BufferCopy	copy;
					copy.srcOffset	= 0_b;
					copy.dstOffset	= 0_b;
					copy.size		= size;

					ctx.CopyBuffer( _ids[0].Get(), _ids[i].Get(), {copy} );
				}
			}
		}};

		ASSERT( _loadOp.IsDefined() );

		if ( not _ids[0].IsValid() )
			return _uploadStatus.load();  // retry later

		if ( _loadOp.clear )
		{
			const Bytes	size = GetBufferDesc().size;

			if ( HasHistory() )
			{
				for (auto& id : _ids) {
					ctx.FillBuffer( id.Get(), 0_b, size, 0 );
				}
			}
			else
				ctx.FillBuffer( _ids[0].Get(), 0_b, size, 0 );

			_SetUploadStatus( EUploadStatus::Completed );
			return _uploadStatus.load();
		}

		if ( not _loadOp.data.empty() )
		{
			CHECK_ERR( ctx.UploadBuffer( _ids[0].Get(), 0_b, _loadOp.data, EStagingHeapType::Dynamic ), EUploadStatus::InProgress );
			CopyHistory( ArraySizeOf(_loadOp.data) );

			_SetUploadStatus( EUploadStatus::Completed );
			return _uploadStatus.load();
		}

		if_unlikely( not _loadOp.request or _loadOp.request->IsCancelled() )
		{
			_SetUploadStatus( EUploadStatus::Canceled );
			return _uploadStatus.load();
		}

		if ( not _loadOp.request->IsCompleted() )
			return EUploadStatus::InProgress;


		// extract data
		ArrayView<ubyte>	loaded_data = _loadOp.request->GetResult().AsArray<ubyte>();

		if_unlikely( not _loadOp.stream.IsInitialized() )
		{
			const Bytes	size = GetBufferDesc().size;

			CHECK( loaded_data.size() == size );
			_loadOp.stream = BufferStream{ _ids[0].Get(), UploadBufferDesc{ 0_b, size }.DynamicHeap() };

			ctx.ResourceState( _ids[0].Get(), EResourceState::Invalidate );
		}
		ASSERT( _loadOp.stream.BufferId() == _ids[0] );

		// upload
		ArrayView<ubyte>	pending = loaded_data.section( usize(_loadOp.stream.pos), UMax );

		BufferMemView	dst_mem;
		ctx.UploadBuffer( INOUT _loadOp.stream, OUT dst_mem );

		if ( not dst_mem.Empty() )
		{
			Unused( dst_mem.CopyFrom( pending ));

			CopyHistory( _loadOp.stream.End() );
		}
		else
		{
			if ( not _loadOp.stream.IsCompleted() )
				return EUploadStatus::NoMemory;
		}

		// streaming complete
		if ( _loadOp.stream.IsCompleted() )
		{
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}

/*
=================================================
	CreateAndStore
=================================================
*/
	RC<Buffer>  Buffer::CreateAndStore (const Buffer	&src,
										const StoreOp	&storeOp,
										Bytes			offset,
										Bytes			size,
										StringView		dbgName) __Th___
	{
		CHECK_ERR( storeOp.file );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		RC<Buffer>	result		{new Buffer{ src._Renderer(), dbgName }};

		for (auto& dst : result->_ids)
		{
			Unused( dst.Attach( res_mngr.AcquireResource( src._ids[0].Get() )));
		}

		size = Min( size, src.GetBufferDesc().size );

		result->_uploadStatus.store( EUploadStatus::InProgress );

		result->_storeOp = StoreOp2{storeOp};
		result->_storeOp.stream = BufferStream{ result->GetBufferId(0),	// TODO: use frame id ?
												ReadbackBufferDesc{}.Offset( offset ).DataSize( size ).AnyHeap() };

		result->_DtTrQueue().EnqueueForReadback( result );
		return result;
	}

/*
=================================================
	Readback
=================================================
*/
	IResource::EUploadStatus  Buffer::Readback (TransferCtx_t &ctx)
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		ASSERT( _storeOp.IsDefined() );

		const Bytes	offset = _storeOp.stream.pos;

		ctx.ReadbackBuffer( INOUT _storeOp.stream )
			.Then(	GetRC<Buffer>(), _storeOp.file, offset,
					[] (Promise<BufferMemView> readOp, RC<Buffer> self, RC<AsyncWDataSource> file, Bytes offset)
						-> InlineCoro<ETaskQueue::Background>
					{
						BufferMemView	mem_view	= co_await readOp;
						auto			mem			= file->Alloc( mem_view.DataSize() );
						CHECK_CE( mem );

						CHECK( mem_view.CopyTo( OUT mem->Data(), mem->Size() ) == mem->Size() );

						Unused( file->WriteBlock( offset, mem->Size(), mem ));
					});

		if ( _storeOp.stream.IsCompleted() )
		{
			_storeOp = Default;
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}

/*
=================================================
	GetBufferDesc
=================================================
*/
	BufferDesc  Buffer::GetBufferDesc () const
	{
		return GraphicsScheduler().GetResourceManager().GetDescription( _ids[0].Get() );
	}


} // AE::ResEditor

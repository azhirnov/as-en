// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Passes/Renderer.h"

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
	Buffer::Buffer (Renderer& renderer, StringView dbgName) :
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
					Array<RC<Buffer>>	refBuffers) __Th___ :
		IResource{ renderer },
		_typeName{ typeName },
		_staticSize{ staticSize },
		_elemSize{ elemSize },
		_bufDesc{ desc },
		_inDynCount{ RVRef(inDynCount) },
		_outDynCount{ RVRef(outDynCount) },
		_loadOp{ RVRef(loadOp) },
		_flags{ flags },
		_dbgName{ dbgName },
		_refBuffers{ RVRef(refBuffers) }
	{
		if ( _inDynCount )
			CHECK_THROW( _elemSize > 0_b );

		if ( HasHistory() )
			CHECK_THROW( not _inDynCount );

		for (usize i = 0; i < ids.size(); ++i) {
			Unused( _ids[i].Attach( RVRef(ids[i]) ));
		}

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
				_loadOp.request = _loadOp.file->ReadRemaining( 0_b );	// TODO: optimize?
				CHECK_THROW( _loadOp.request );

				if ( _outDynCount )
					_outDynCount->Set( 0 );
			}

			_DtTrQueue().EnqueueForUpload( GetRC() );
		}

		if ( AnyBits( desc.usage, c_DevAddrUsage ))
		{
			auto&	res_mngr = GraphicsScheduler().GetResourceManager();
			for (usize i = 0; i < _ids.size(); ++i) {
				_address[i] = BitCast<ulong>(res_mngr.GetResourcesOrThrow( _ids[i].Get() ).GetDeviceAddress());
			}
		}
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
			return false;

		BufferDesc	desc = _bufDesc.Read();
		desc.size = _staticSize + Max( count, 1u ) * _elemSize;

		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	rs_track	= RenderGraph().GetStateTracker();

		auto	buf = res_mngr.CreateBuffer( desc, _dbgName, _Renderer().ChooseAllocator( True{"dynamic"}, desc.size ));
		CHECK_ERR( buf );

		_bufDesc.Write( desc );

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

		if ( _loadOp.clear )
			ctx.FillBuffer( buf, 0_b, desc.size, 0 );

		if ( _outDynCount )
			_outDynCount->Set( count );

		res_mngr.ReleaseResource( buf );
		return true;
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
			if ( HasHistory() )
			{
				for (auto& id : _ids) {
					ctx.FillBuffer( id.Get(), 0_b, _bufDesc->size, 0 );
				}
			}
			else
				ctx.FillBuffer( _ids[0].Get(), 0_b, _bufDesc->size, 0 );

			_SetUploadStatus( EUploadStatus::Completed );
			return _uploadStatus.load();
		}

		if ( not _loadOp.data.empty() )
		{
			CHECK_ERR( ctx.UploadBuffer( _ids[0].Get(), 0_b, _loadOp.data, EStagingHeapType::Dynamic ), EUploadStatus::InProgress );
			CopyHistory( ArraySizeOf(_loadOp.data) );

			_loadOp.data = {};
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

		auto	loaded_data = _loadOp.request->GetResult().AsArray<ubyte>();

		if_unlikely( not _loadOp.stream.IsInitialized() )
		{
			CHECK( loaded_data.size() == _bufDesc->size );
			_loadOp.stream = BufferStream{ _ids[0].Get(), UploadBufferDesc{ 0_b, _bufDesc->size }.DynamicHeap() };

			ctx.ResourceState( _ids[0].Get(), EResourceState::Invalidate );
		}
		ASSERT( _loadOp.stream.BufferId() == _ids[0] );

		auto	pending = loaded_data.section( usize(_loadOp.stream.pos), UMax );

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

		if ( _loadOp.stream.IsCompleted() )
		{
			_loadOp = Default;
			_SetUploadStatus( EUploadStatus::Completed );

			if ( _outDynCount )
				_outDynCount->Set( CheckCast<uint>( ArraySize() ));
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
										StringView		dbgName) __Th___
	{
		CHECK_ERR( storeOp.file );

		auto&		res_mngr	= GraphicsScheduler().GetResourceManager();
		RC<Buffer>	result		{new Buffer{ src._Renderer(), dbgName }};

		for (auto& dst : result->_ids)
		{
			Unused( dst.Attach( res_mngr.AcquireResource( src._ids[0].Get() )));
		}

		result->_bufDesc.Write( res_mngr.GetDescription( result->_ids[0] ));

		result->_uploadStatus.store( EUploadStatus::InProgress );

		result->_storeOp = StoreOp2{storeOp};
		result->_storeOp.stream = BufferStream{ result->_ids[0], ReadbackBufferDesc{}.DataSize( src.GetBufferDesc().size ).AnyHeap() };

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
			.Then(	[self = GetRC<Buffer>(), file = _storeOp.file, offset] (const BufferMemView &memView) __Th___
					{
						auto	mem = file->Alloc( memView.DataSize() );
						CHECK_THROW( mem );

						CHECK( memView.CopyTo( OUT mem->Data(), mem->Size() ) == mem->Size() );

						Unused( file->WriteBlock( offset, mem->Size(), mem ));
					},
					"Buffer::Readback",
					ETaskQueue::Background
				);

		if ( _storeOp.stream.IsCompleted() )
		{
			_storeOp = Default;
			_SetUploadStatus( EUploadStatus::Completed );
		}

		return _uploadStatus.load();
	}


} // AE::ResEditor

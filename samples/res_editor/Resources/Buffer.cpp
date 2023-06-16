// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Passes/FrameGraph.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{
	
/*
=================================================
	LoadOp2::IsDefined
=================================================
*/
	bool  Buffer::LoadOp2::IsDefined () C_NE___
	{
		return filename.IsDefined() or (not data.empty()) or clear;
	}

/*
=================================================
	constructor
=================================================
*/
	Buffer::Buffer (Strong<BufferID>	id,
					const BufferDesc &	desc,
					Bytes				elemSize,
					LoadOp				loadOp,
					ShaderStructName	typeName,
					Renderer &			renderer,
					RC<DynamicUInt>		dynCount,
					StringView			dbgName) __Th___ :
		IResource{ renderer },
		_id{ RVRef(id) },
		_typeName{ typeName },
		_elemSize{ elemSize },
		_bufDesc{ desc },
		_dynCount{ RVRef(dynCount) },
		_loadOp{ RVRef(loadOp) },
		_dbgName{ dbgName }
	{
		ASSERT( (_elemSize > 0_b) == (_dynCount != null) );

		if ( not _loadOp.IsDefined() )
		{
			_uploadStatus.store( EUploadStatus::Complete );

			FrameGraph().GetStateTracker().AddResource( _id.Get() );
		}
		else
		{
			_uploadStatus.store( EUploadStatus::InProgress );

			if ( _loadOp.filename.IsDefined() )
			{
				CHECK_THROW( GetVFS().Open( OUT _loadOp.file, _loadOp.filename ));

				_loadOp.request = _loadOp.file->ReadRemaining();
				CHECK_THROW( _loadOp.request );
			}

			_ResQueue().EnqueueForUpload( GetRC() );
		}
	}
	
/*
=================================================
	destructor
=================================================
*/
	Buffer::~Buffer ()
	{
		if ( _loadOp.file )		_loadOp.file->CancelAllRequests();
		if ( _loadOp.request )	_loadOp.request->Cancel();
		_loadOp = Default;

		{
			auto	id = _id.Release();
			FrameGraph().GetStateTracker().ReleaseResource( id );
		}
	}
	
/*
=================================================
	Resize
=================================================
*/
	bool  Buffer::Resize (TransferCtx_t &ctx) __Th___
	{
		if ( not _dynCount )
			return true;

		uint	count	= uint(ArraySize());

		if_likely( not _dynCount->IsChanged( INOUT count ) or count == 0 )
			return false;

		BufferDesc	desc = _bufDesc.Read();
		desc.size = Max( count, 1u ) * _elemSize;
		
		{
			auto&	res_mngr = RenderTaskScheduler().GetResourceManager();

			auto	buf = res_mngr.CreateBuffer( desc, _dbgName, _GfxDynamicAllocator() );
			CHECK_ERR( buf );

			_bufDesc.Write( desc );
			
			FrameGraph().GetStateTracker().AddResource( buf.Get(),
														EResourceState::_InvalidState,	// current is not used
														EResourceState::General,		// default
														ctx.GetCommandBatchRC() );

			auto	old_buf	= _id.Attach( RVRef(buf) );
			res_mngr.ReleaseResource( old_buf );	// release dummy resource
		}
		
		if ( _loadOp.clear )
			ctx.FillBuffer( _id.Get(), 0_b, desc.size, 0 ); 

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
		
		EXLOCK( _loadOpGuard );
		ASSERT( _loadOp.IsDefined() );
		
		if ( _loadOp.clear )
		{
			ctx.FillBuffer( _id.Get(), 0_b, _bufDesc->size, 0 );
			
			_uploadStatus.store( EUploadStatus::Complete );
			return EUploadStatus::Complete;
		}

		if ( not _loadOp.data.empty() )
		{
			CHECK_ERR( ctx.UploadBuffer( _id.Get(), 0_b, _loadOp.data, EStagingHeapType::Dynamic ), EUploadStatus::InProgress );
			_loadOp.data = {};

			_uploadStatus.store( EUploadStatus::Complete );
			return EUploadStatus::Complete;
		}

		if_unlikely( not _loadOp.request or _loadOp.request->IsCancelled() )
		{
			_uploadStatus.store( EUploadStatus::Canceled );
			return EUploadStatus::Canceled;
		}

		if ( not _loadOp.request->IsCompleted() )
			return EUploadStatus::InProgress;

		if_unlikely( not _loadOp.stream.IsInitialized() )
		{
			_loadOp.stream = BufferStream{ _id.Get(), 0_b, _bufDesc->size, 0_b, EStagingHeapType::Dynamic };
		}
		ASSERT( _loadOp.stream.Buffer() == _id );

		BufferMemView	dst_mem;
		ctx.UploadBuffer( _loadOp.stream, OUT dst_mem );

		if ( not dst_mem.Empty() )
		{
			Unused( dst_mem.Copy( _loadOp.request->GetResult().AsArray<ubyte>().section( usize(_loadOp.stream.pos), UMax )));
		}
		
		if ( _loadOp.stream.IsCompleted() )
		{
			_loadOp = Default;
			_uploadStatus.store( EUploadStatus::Complete );
		}

		return _uploadStatus.load();
	}
	
/*
=================================================
	Readback
=================================================
*/
	IResource::EUploadStatus  Buffer::Readback (TransferCtx_t &)
	{
		EXLOCK( _loadOpGuard );

		return EUploadStatus::Canceled;
	}
	
/*
=================================================
	Cancel
=================================================
*/
	void  Buffer::Cancel ()
	{
		EXLOCK( _loadOpGuard );
	}


} // AE::ResEditor

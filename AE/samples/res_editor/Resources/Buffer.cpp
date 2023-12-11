// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{
namespace
{
    static constexpr auto   c_DevAddrUsage =    EBufferUsage::ShaderAddress | EBufferUsage::ShaderBindingTable |
                                                EBufferUsage::ASBuild_ReadOnly  | EBufferUsage::ASBuild_Scratch;
}

/*
=================================================
    LoadOp2::IsDefined
=================================================
*/
    bool  Buffer::LoadOp2::IsDefined () C_NE___
    {
        return filename.IsDefined() or (not data.empty()) or clear;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    Buffer::Buffer (IDs_t               ids,
                    const BufferDesc &  desc,
                    Bytes               elemSize,
                    LoadOp              loadOp,
                    ShaderStructName    typeName,
                    Renderer &          renderer,
                    RC<DynamicUInt>     dynCount,
                    StringView          dbgName,
                    EBufferFlags        flags,
                    Array<RC<Buffer>>   refBuffers) __Th___ :
        IResource{ renderer },
        _typeName{ typeName },
        _elemSize{ elemSize },
        _bufDesc{ desc },
        _dynCount{ RVRef(dynCount) },
        _loadOp{ RVRef(loadOp) },
        _flags{ flags },
        _dbgName{ dbgName },
        _refBuffers{ RVRef(refBuffers) }
    {
        ASSERT( (_elemSize > 0_b) == (_dynCount != null) );

        for (usize i = 0; i < ids.size(); ++i) {
            Unused( _ids[i].Attach( RVRef(ids[i]) ));
        }

        if ( HasHistory() )
        {
            CHECK_THROW( not _dynCount );
        }

        if ( not _loadOp.IsDefined() )
        {
            _uploadStatus.store( EUploadStatus::Completed );

            auto&   res_mngr = RenderGraph().GetStateTracker();
            for (auto& id : _ids) {
                res_mngr.AddResourceIfNotTracked( id.Get() );
            }
        }
        else
        {
            _uploadStatus.store( EUploadStatus::InProgress );

            if ( _loadOp.filename.IsDefined() )
            {
                CHECK_THROW( GetVFS().Open( OUT _loadOp.file, _loadOp.filename ));

                _loadOp.request = _loadOp.file->ReadRemaining( 0_b );   // TODO: optimize?
                CHECK_THROW( _loadOp.request );
            }

            _DtTrQueue().EnqueueForUpload( GetRC() );
        }

        if ( AnyBits( desc.usage, c_DevAddrUsage ))
        {
            auto&   res_mngr = GraphicsScheduler().GetResourceManager();
            for (usize i = 0; i < _ids.size(); ++i) {
                _address[i] = BitCast<ulong>(res_mngr.GetResource( _ids[i].Get() )->GetDeviceAddress());
            }
        }
    }

/*
=================================================
    destructor
=================================================
*/
    Buffer::~Buffer ()
    {
        if ( _loadOp.file )     _loadOp.file->CancelAllRequests();
        if ( _loadOp.request )  _loadOp.request->Cancel();
        _loadOp = Default;

        {
            auto&   res_mngr = RenderGraph().GetStateTracker();
            for (auto& a_id : _ids)
            {
                auto    id = a_id.Release();
                res_mngr.ReleaseResource( id );
            }
        }
    }

/*
=================================================
    RequireResize
=================================================
*/
    bool  Buffer::RequireResize () C_Th___
    {
        if ( not _dynCount )
            return true;

        uint    count   = uint(ArraySize());

        if_likely( not _dynCount->IsChanged( INOUT count ) or count == 0 )
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
        if ( not _dynCount )
            return true;

        ASSERT( not HasHistory() );

        uint    count   = uint(ArraySize());

        if_likely( not _dynCount->IsChanged( INOUT count ) or count == 0 )
            return false;

        BufferDesc  desc = _bufDesc.Read();
        desc.size = Max( count, 1u ) * _elemSize;

        auto&   res_mngr    = GraphicsScheduler().GetResourceManager();
        auto&   rs_track    = RenderGraph().GetStateTracker();

        auto    buf = res_mngr.CreateBuffer( desc, _dbgName, _GfxDynamicAllocator() );
        CHECK_ERR( buf );

        _bufDesc.Write( desc );

        rs_track.AddResource( buf.Get(),
                              EResourceState::_InvalidState,    // current is not used
                              EResourceState::General,          // default
                              ctx.GetCommandBatchRC() );

        for (auto& id : _ids)
        {
            auto    old_buf = id.Attach( res_mngr.AcquireResource( buf ));
            res_mngr.ReleaseResource( old_buf );    // release dummy resource
        }

        if ( AnyBits( desc.usage, c_DevAddrUsage ))
        {
            for (usize i = 0; i < _ids.size(); ++i) {
                _address[i] = BitCast<ulong>(res_mngr.GetResource( _ids[i].Get() )->GetDeviceAddress());
            }
        }

        if ( _loadOp.clear )
            ctx.FillBuffer( buf, 0_b, desc.size, 0 );

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

        const auto  CopyHistory = [this, &ctx] (Bytes size)
        {{
            if ( this->HasHistory() )
            {
                for (usize i = 1; i < _ids.size(); ++i)
                {
                    BufferCopy  copy;
                    copy.srcOffset  = 0_b;
                    copy.dstOffset  = 0_b;
                    copy.size       = size;

                    ctx.CopyBuffer( _ids[0].Get(), _ids[i].Get(), {copy} );
                }
            }
        }};

        ASSERT( _loadOp.IsDefined() );

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

        if_unlikely( not _loadOp.stream.IsInitialized() )
        {
            _loadOp.stream = BufferStream{ _ids[0].Get(), 0_b, _bufDesc->size, 0_b, EStagingHeapType::Dynamic };
        }
        ASSERT( _loadOp.stream.BufferId() == _ids[0] );

        BufferMemView   dst_mem;
        ctx.UploadBuffer( _loadOp.stream, OUT dst_mem );

        if ( not dst_mem.Empty() )
        {
            Unused( dst_mem.CopyFrom( _loadOp.request->GetResult().AsArray<ubyte>().section( usize(_loadOp.stream.pos), UMax )));

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
        return EUploadStatus::Canceled;
    }


} // AE::ResEditor

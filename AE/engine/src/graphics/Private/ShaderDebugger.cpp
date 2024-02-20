// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ShaderDebugger.h"
#include "graphics/Public/GraphicsImpl.h"

namespace AE::Graphics
{

/*
=================================================
    IsValid
=================================================
*/
    bool  ShaderDebugger::Result::IsValid () C_NE___
    {
        return  _ds         != Default  and
                _deviceBuf  != Default  and
                _hostBuf    != Default  and
                _ppln       != null     and
                _fn         != null;
    }

/*
=================================================
    destructor
=================================================
*/
    ShaderDebugger::~ShaderDebugger ()
    {
        DRC_EXLOCK( _drCheck );

        auto&   res_mngr = GraphicsScheduler().GetResourceManager();

        {
            auto    buffers = _buffers.WriteNoLock();
            EXLOCK( buffers );

            for (auto& buf : *buffers) {
                res_mngr.ImmediatelyReleaseResources( buf.dbgTraceBuffer, buf.readbackBuffer );
            }
            buffers->clear();
        }
        {
            auto    ds_arr = _dsArray.WriteNoLock();
            EXLOCK( ds_arr );

            for (auto& ds : *ds_arr) {
                res_mngr.ImmediatelyRelease( ds );
            }
            ds_arr->clear();
        }
    }

/*
=================================================
    _InitDS
=================================================
*/
    bool  ShaderDebugger::_InitDS (const Result &info) const
    {
        DescriptorUpdater   updater;
        CHECK_ERR( updater.Set( info._ds, EDescUpdateMode::Partialy ));

        updater.BindBuffer( UniformName{"dbg_ShaderTrace"}, info._deviceBuf, info._offset, info._size );
        CHECK_ERR( updater.Flush() );

        return true;
    }

/*
=================================================
    _AllocStorage
=================================================
*/
    bool  ShaderDebugger::_AllocStorage (Bytes size, INOUT Result &result)
    {
        ASSERT( result._ppln != null );
        ASSERT( result._fn != null );
        ASSERT( result._ds and result._dsIndex != UMax );
        ASSERT( result._state != Default );

        CHECK_ERR( size > _TraceHeaderSize );

        // find in existing
        {
            auto    buffers = _buffers.WriteNoLock();
            EXLOCK( buffers );

            for (auto& buf : *buffers)
            {
                const Bytes  offset = AlignUp( buf.size, _OffsetAlign );

                if ( offset + size <= buf.capacity )
                {
                    result._deviceBuf   = buf.dbgTraceBuffer;
                    result._hostBuf     = buf.readbackBuffer;
                    result._offset      = offset;
                    result._size        = size;

                    _pending->push_back( result );
                    buf.size = offset + size;

                    return _InitDS( result );
                }
            }
        }

        auto&   res_mngr = GraphicsScheduler().GetResourceManager();

        // create allocator
        GfxMemAllocatorPtr  gfx_alloc = _gfxAlloc.load();
        if_unlikely( gfx_alloc == null )
        {
            GfxMemAllocatorPtr  new_alloc = res_mngr.CreateLinearGfxMemAllocator( _blockSize * 4 );
            if ( _gfxAlloc.CAS_Loop( INOUT gfx_alloc, new_alloc ))
                gfx_alloc = new_alloc;
        }

        // create new buffer
        {
            Buffer      buf;
            EMemoryType host_mem_type = EMemoryType::HostCachedCoherent;
            if ( not res_mngr.IsSupported( host_mem_type ))
            {
                host_mem_type = EMemoryType::HostCached;
                CHECK_ERR( res_mngr.IsSupported( host_mem_type ));
            }

            buf.dbgTraceBuffer = res_mngr.CreateBuffer( BufferDesc{ _blockSize,
                                                            EBufferUsage::Transfer | EBufferUsage::Storage,
                                                            EBufferOpt::VertexPplnStore | EBufferOpt::FragmentPplnStore,
                                                            Default,
                                                            EMemoryType::DeviceLocal }, "debug storage buffer", gfx_alloc );
            buf.readbackBuffer = res_mngr.CreateBuffer( BufferDesc{ _blockSize,
                                                            EBufferUsage::TransferDst,
                                                            Default,
                                                            Default,
                                                            host_mem_type }, "readback dbg buffer", gfx_alloc );
            CHECK_ERR( buf.dbgTraceBuffer and buf.readbackBuffer );

            result._deviceBuf   = buf.dbgTraceBuffer;
            result._hostBuf     = buf.readbackBuffer;
            result._offset      = 0_b;
            result._size        = size;

            buf.size            = size;

            auto    pending = _pending.WriteNoLock();
            auto    buffers = _buffers.WriteNoLock();
            EXLOCK( pending, buffers );

            pending->push_back( result );
            buffers->push_back( RVRef(buf) );
        }
        return _InitDS( result );
    }

/*
=================================================
    ParseShaderTrace
=================================================
*/
namespace {
    template <typename PplnType>
    static bool  ParseShaderTrace (const void* ppln, const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result)
    {
    #ifdef AE_ENABLE_VULKAN
        return Cast<PplnType>( ppln )->ParseShaderTrace( ptr, maxSize, format, OUT result );
    #else
        Unused( ppln, ptr, maxSize, format, result );
        return false;
    #endif
    }
}
/*
=================================================
    _Get***Pipeline
=================================================
*/
    bool  ShaderDebugger::_GetComputePipeline (ComputePipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        result = Default;
        result._state = EResourceState::ShaderStorage_RW | EResourceState::ComputeShader;

        return _GetPipeline( ppln, dsName, OUT result );
    }

    bool  ShaderDebugger::_GetRayTracingPipeline (RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        result = Default;
        result._state = EResourceState::ShaderStorage_RW | EResourceState::RayTracingShaders;

        return _GetPipeline( ppln, dsName, OUT result );
    }

    bool  ShaderDebugger::_GetGraphicsPipeline (GraphicsPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        result = Default;
        result._state = EResourceState::ShaderStorage_RW | EResourceState::AllGraphicsShaders;

        return _GetPipeline( ppln, dsName, OUT result );
    }

    bool  ShaderDebugger::_GetGraphicsPipeline (MeshPipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        result = Default;
        result._state = EResourceState::ShaderStorage_RW | EResourceState::AllGraphicsShaders;

        return _GetPipeline( ppln, dsName, OUT result );
    }

    bool  ShaderDebugger::_GetGraphicsPipeline (TilePipelineID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        result = Default;
        result._state = EResourceState::ShaderStorage_RW | EResourceState::TileShader;

        return _GetPipeline( ppln, dsName, OUT result );
    }

    template <typename PplnID>
    bool  ShaderDebugger::_GetPipeline (PplnID ppln, DescriptorSetName::Ref dsName, OUT Result &result)
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();

        if ( auto* res = res_mngr.GetResource( ppln ))
        {
            result._ppln    = res;
            result._fn      = &ParseShaderTrace< RemoveAllQualifiers< decltype(*res) >>;

            auto [ds, idx]   = res_mngr.CreateDescriptorSet( ppln, dsName );
            CHECK_ERR( ds );

            result._ds      = ds;
            result._dsIndex = CheckCast<ushort>(idx.vkIndex);

            _dsArray->push_back( RVRef(ds) );
            return true;
        }
        return false;
    }

/*
=================================================
    _Parse
=================================================
*/
    Array<String>  ShaderDebugger::_Parse (ArrayView<ubyte> view, const void* ppln, ParseTraceFn_t fn, ELogFormat format)
    {
        Array<String>   result;
        Unused( fn( ppln, view.data(), ArraySizeOf(view), format, OUT result ));
        return result;
    }

/*
=================================================
    _Merge
=================================================
*/
    Promise<Array<String>>  ShaderDebugger::_Merge (Array< Promise< Array<String> >> tasks)
    {
        return MakePromiseFromArray( RVRef(tasks) );
    }

/*
=================================================
    Reset
=================================================
*/
    void  ShaderDebugger::Reset ()
    {
        auto    buffers     = _buffers.WriteNoLock();
        auto    ds_arr      = _dsArray.WriteNoLock();
        auto    pending     = _pending.WriteNoLock();

        EXLOCK( buffers, ds_arr, pending );

        ASSERT( pending->empty() );
        pending->clear();

        for (auto& buf : *buffers) {
            buf.size = 0_b;
        }

        if ( not ds_arr->empty() )
        {
            auto&   res_mngr = GraphicsScheduler().GetResourceManager();

            for (auto& ds : *ds_arr) {
                res_mngr.DelayedRelease( ds );
            }
            ds_arr->clear();
        }
    }

/*
=================================================
    AllocForCompute
=================================================
*/
    bool  ShaderDebugger::AllocForCompute (OUT Result &result, ITransferContext &ctx, ComputePipelineID ppln, const uint3 &globalID, DescriptorSetName::Ref dsName, Bytes size) __Th___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( not _GetComputePipeline( ppln, dsName, OUT result ))
            return false;

        if_unlikely( not _AllocStorage( size, OUT result ))
            return false;

        const uint  data[4] = { globalID.x, globalID.y, globalID.z, 0 };
        StaticAssert( _TraceHeaderSize == sizeof(data) );

        _FillBuffer( result, ctx, Sizeof(data), data );
        return true;
    }

    bool  ShaderDebugger::AllocForCompute (OUT Result &result, ITransferContext &ctx, ComputePipelineID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
    {
        return AllocForCompute( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
    }

/*
=================================================
    AllocForRayTracing
=================================================
*/
    bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, const uint3 &launchID, DescriptorSetName::Ref dsName, Bytes size) __Th___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( not _GetRayTracingPipeline( ppln, dsName, OUT result ))
            return false;

        if_unlikely( not _AllocStorage( size, OUT result ))
            return false;

        const uint  data[4] = { launchID.x, launchID.y, launchID.z, 0 };
        StaticAssert( _TraceHeaderSize == sizeof(data) );

        _FillBuffer( result, ctx, Sizeof(data), data );
        return true;
    }

    bool  ShaderDebugger::AllocForRayTracing (OUT Result &result, ITransferContext &ctx, RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, Bytes size) __Th___
    {
        return AllocForRayTracing( OUT result, ctx, ppln, uint3{~0u}, dsName, size );
    }

/*
=================================================
    _FillBuffer
=================================================
*/
    void  ShaderDebugger::_FillBuffer (const Result &result, ITransferContext &ctx, Bytes headerSize, const void* headerData) const
    {
        ctx.FillBuffer( result._deviceBuf, result._offset + headerSize, result._size - headerSize, 0 );
        ctx.UpdateBuffer( result._deviceBuf, result._offset, headerSize, headerData );

        ctx.BufferBarrier( result._deviceBuf, EResourceState::CopyDst, result._state );
        ctx.CommitBarriers();
    }

/*
=================================================
    Read
=================================================
*/
    Promise<Array<String>>  ShaderDebugger::Read (ITransferContext &ctx, const Result &request, ELogFormat format) __Th___
    {
        DRC_EXLOCK( _drCheck );

        BufferCopy  range;
        range.srcOffset = request._offset;
        range.dstOffset = request._offset;
        range.size      = request._size;

        ctx.BufferBarrier( request._deviceBuf, request._state, EResourceState::CopySrc );
        ctx.CommitBarriers();

        ctx.CopyBuffer( request._deviceBuf, request._hostBuf, {range} );

        ctx.BufferBarrier( request._hostBuf, EResourceState::CopyDst, EResourceState::Host_Read );
        ctx.CommitBarriers();

        return ctx.ReadHostBuffer( request._hostBuf, request._offset, request._size )
                .Then( [ppln = request._ppln, fn = request._fn, format] (const ArrayView<ubyte> &view)
                        {
                            return _Parse( view, ppln, fn, format );
                        });
    }

/*
=================================================
    ReadAll
=================================================
*/
    Promise<Array<String>>  ShaderDebugger::ReadAll (ITransferContext &ctx, ELogFormat format) __Th___
    {
        DRC_EXLOCK( _drCheck );

        auto    pending = _pending.WriteNoLock();
        EXLOCK( pending );

        if ( pending->empty() )
            return Default;

        for (auto& res : *pending) {
            ctx.BufferBarrier( res._deviceBuf, res._state, EResourceState::CopySrc );
        }
        ctx.CommitBarriers();

        for (auto& res : *pending)
        {
            BufferCopy  range;
            range.srcOffset = res._offset;
            range.dstOffset = res._offset;
            range.size      = res._size;

            ctx.CopyBuffer( res._deviceBuf, res._hostBuf, {range} );
        }

        ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
        ctx.CommitBarriers();

        Array< Promise< Array<String> >>    temp;
        temp.reserve( 32 );

        for (auto& res : *pending)
        {
            temp.push_back(
                ctx.ReadHostBuffer( res._hostBuf, res._offset, res._size )
                    .Then( [ppln = res._ppln, fn = res._fn, format] (const ArrayView<ubyte> &view)
                            {
                                return _Parse( view, ppln, fn, format );
                            }));

            if_unlikely( temp.size() >= 32 )
            {
                auto    task = _Merge( RVRef(temp) );
                temp.clear();
                temp.reserve( 32 );
                temp.push_back( task );
            }
        }
        pending->clear();

        if ( temp.size() == 1 )
            return temp[0];

        return _Merge( RVRef(temp) );
    }


} // AE::Graphics

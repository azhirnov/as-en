// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/Queue.h"

namespace AE::Graphics
{

    using RmGraphicsPipelineID      = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  1 >;
    using RmMeshPipelineID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  2 >;
    using RmComputePipelineID       = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  3 >;
    using RmRayTracingPipelineID    = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  4 >;
    using RmTilePipelineID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  5 >;
    using RmPipelineCacheID         = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  6 >;
    using RmPipelinePackID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  7 >;
    using RmDescriptorSetID         = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  8 >;
    using RmDescriptorSetLayoutID   = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  9 >;

    using RmBufferID                = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 10 >;
    using RmImageID                 = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 11 >;
    using RmBufferViewID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 12 >;
    using RmImageViewID             = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 13 >;
    using RmRTGeometryID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 14 >;       // bottom-level AS
    using RmRTSceneID               = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 15 >;       // top-level AS
    using RmRTShaderBindingID       = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 16 >;       // shader binding table
    using RmMemoryID                = HandleTmpl< 32, 32, Graphics::_hidden_::RemoteIDs_Start + 17 >;

    // for internal usage
    //using RmSamplerID             = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 18 >;
    //using RmPipelineLayoutID      = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 19 >;
    //using RmRenderPassID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 20 >;

    using RmVideoSessionID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 40 >;
    using RmVideoBufferID           = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 41 >;
    using RmVideoImageID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 42 >;





    struct RemoteMemoryObjInfo
    {
        void*   mappedPtr       = 0;
    };


    struct RemoteSemaphore
    {
        ulong   semaphoreId     = 0;
    };

    struct RemoteCmdBatchDependency
    {
        ulong   semaphoreId     = 0;

        ND_ explicit operator bool ()   C_NE___ { return false; }
    };

} // AE::Graphics
//-----------------------------------------------------------------------------


namespace AE::RemoteGraphics
{
    using namespace AE::Graphics;

/*
=================================================
    IDCast
=================================================
*/
    ND_ inline RmBufferID           IDCast (BufferID            id)     { return BitCast<RmBufferID>(id); }
    ND_ inline BufferID             IDCast (RmBufferID          id)     { return BitCast<BufferID>(id); }

    ND_ inline RmImageID            IDCast (ImageID             id)     { return BitCast<RmImageID>(id); }
    ND_ inline ImageID              IDCast (RmImageID           id)     { return BitCast<ImageID>(id); }

    ND_ inline RmBufferViewID       IDCast (BufferViewID        id)     { return BitCast<RmBufferViewID>(id); }
    ND_ inline BufferViewID         IDCast (RmBufferViewID      id)     { return BitCast<BufferViewID>(id); }

    ND_ inline RmImageViewID        IDCast (ImageViewID         id)     { return BitCast<RmImageViewID>(id); }
    ND_ inline ImageViewID          IDCast (RmImageViewID       id)     { return BitCast<ImageViewID>(id); }

    ND_ inline RmRTGeometryID       IDCast (RTGeometryID        id)     { return BitCast<RmRTGeometryID>(id); }
    ND_ inline RTGeometryID         IDCast (RmRTGeometryID      id)     { return BitCast<RTGeometryID>(id); }

    ND_ inline RmRTSceneID          IDCast (RTSceneID           id)     { return BitCast<RmRTSceneID>(id); }
    ND_ inline RTSceneID            IDCast (RmRTSceneID         id)     { return BitCast<RTSceneID>(id); }

    ND_ inline RmRTShaderBindingID  IDCast (RTShaderBindingID   id)     { return BitCast<RmRTShaderBindingID>(id); }
    ND_ inline RTShaderBindingID    IDCast (RmRTShaderBindingID id)     { return BitCast<RTShaderBindingID>(id); }

    ND_ inline RmMemoryID           IDCast (MemoryID            id)     { return BitCast<RmMemoryID>(id); }
    ND_ inline MemoryID             IDCast (RmMemoryID          id)     { return BitCast<MemoryID>(id); }

    ND_ inline RmPipelineCacheID    IDCast (PipelineCacheID     id)     { return BitCast<RmPipelineCacheID>(id); }
    ND_ inline PipelineCacheID      IDCast (RmPipelineCacheID   id)     { return BitCast<PipelineCacheID>(id); }


} // AE::RemoteGraphics

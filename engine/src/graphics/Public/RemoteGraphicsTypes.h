// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/IDs.h"
# include "graphics/Public/Queue.h"

namespace AE::Graphics
{

    using R_GraphicsPipelineID      = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  1 >;
    using R_MeshPipelineID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  2 >;
    using R_ComputePipelineID       = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  3 >;
    using R_RayTracingPipelineID    = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  4 >;
    using R_TilePipelineID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  5 >;
    using R_PipelineCacheID         = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  6 >;
    using R_PipelinePackID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  7 >;
    using R_DescriptorSetID         = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  8 >;
    using R_DescriptorSetLayoutID   = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  9 >;

    using R_BufferID                = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 10 >;
    using R_ImageID                 = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 11 >;
    using R_BufferViewID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 12 >;
    using R_ImageViewID             = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 13 >;
    using R_RTGeometryID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 14 >;       // bottom-level AS
    using R_RTSceneID               = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 15 >;       // top-level AS
    using R_RTShaderBindingID       = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 16 >;       // shader binding table

    // for internal usage
    using R_SamplerID               = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 17 >;
    using R_PipelineLayoutID        = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 18 >;
    using R_RenderPassID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 19 >;
    using R_MemoryID                = HandleTmpl< 32, 32, Graphics::_hidden_::RemoteIDs_Start + 20 >;

    //using R_MaterialID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 20 >;
    using R_ImageInAtlasID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 30 >;

    using R_VideoSessionID          = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 40 >;
    using R_VideoBufferID           = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 41 >;
    using R_VideoImageID            = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 42 >;


    struct RemoteMemoryObjInfo
    {};


    struct RemoteSemaphore
    {
        ulong   semaphoreId;
    };

    struct RemoteCmdBatchDependency
    {
        ulong   semaphoreId;

        ND_ explicit operator bool ()   C_NE___;
    };


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Input surface can be:
        - camera
        - video stream
*/

#pragma once

#include "platform/Public/Common.h"
#include "graphics/Public/GraphicsImpl.h"

namespace AE::App
{
    using Graphics::ImageID;
    using Graphics::ImageViewID;
    using Graphics::ImageLayer;
    using Graphics::EResourceState;
    using Graphics::EPixelFormat;
    using Graphics::EPixelFormatExternal;
    using Graphics::EColorSpace;
    using Graphics::CommandBatchPtr;



    //
    // Input Surface interface
    //

    class IInputSurface
    {
    // types
    public:
        static constexpr uint   MaxSampledImages    = 2;

        struct SampledImage
        {
        // variables
            ImageID                 imageId;
            ImageViewID             viewId;         // 2D with single mipmap, shared 2D array with specified 'layer'

            RectI                   region;         // for texture atlas
            ImageLayer              layer;

            EResourceState          initialState    = Default;
            EResourceState          finalState      = Default;

            EColorSpace             colorSpace      = Default;
            EPixelFormat            format          = Default;
            EPixelFormatExternal    extFormat       = Default;

            // extra:
            uint                    frameId         = 0;    // example: number of camera/video frame


        // methods
            ND_ uint2   RegionSize ()           C_NE___ { return RegionSizePxu(); }
            ND_ uint2   RegionSizePxu ()        C_NE___ { return uint2(region.Size()); }
            ND_ int2    RegionSizePxi ()        C_NE___ { return region.Size(); }
            ND_ float2  RegionSizePxf ()        C_NE___ { return float2(region.Size()); }
        };
        using SampledImages_t = FixedArray< SampledImage, MaxSampledImages >;


    // interface
    public:
        virtual ~IInputSurface ()           __NE___ {}


        // Set samplers which can be used for resource creation.
        //
        // Vulkan:
        //   Ycbcr format requires to use same YcbcrConversion for immutable sampler in descriptor set layout
        //   and for image view creation. But 'IInputSurface' creates only image view and does not create pipelines (and DSL).
        //   So 'SetSamplerCache()' is only way to use exactly the same YcbcrConversion for pipeline and image view.
        //   Note:  format or external format for each sampler must be unique,
        //          otherwise 'IInputSurface' can not match external image with one sampler.
        //
            virtual void  SetSamplerCache (ArrayView<Graphics::SamplerName>)                                                    __NE___ = 0;


        // Begin ...
        // 'beginCmdBatch'  - first batch where images will be sampled.
        // 'endCmdBatch'    - last batch where images was sampled.
        // 'deps'           - list of tasks which must be executed before.
        //
        ND_ virtual AsyncTask  Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)    __NE___ = 0;


        // Get sampled images.
        // Must be used between 'Begin()' / 'End()'.
        //
            virtual bool  GetImages (OUT SampledImages_t &)                                                                     C_NE___ = 0;


        // End ...
        // 'deps'   - list of tasks which must be executed before, 'CmdBatchOnSubmit{endCmdBatch}' is implicitly added.
        //
        ND_ virtual AsyncTask  End (ArrayView<AsyncTask> deps)                                                                  __NE___ = 0;
    };


} // AE::App

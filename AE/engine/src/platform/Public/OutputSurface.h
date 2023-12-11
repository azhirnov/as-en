// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Output surface can be:
        - window / display
        - video stream
        - render target
        - VR device

    Thread-safe: yes
*/

#pragma once

#include "platform/Public/Projection.h"

#include "graphics/Public/IDs.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/MultiSamples.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/GraphicsImpl.h"

namespace AE::App
{
    using Graphics::ImageID;
    using Graphics::ImageViewID;
    using Graphics::ImageLayer;
    using Graphics::EResourceState;
    using Graphics::MultiSamples;
    using Graphics::EPixelFormat;
    using Graphics::EImageUsage;
    using Graphics::EImageOpt;
    using Graphics::EPresentMode;
    using Graphics::EColorSpace;
    using Graphics::CommandBatchPtr;
    using Graphics::SurfaceFormat;



    //
    // Output Surface
    //

    class IOutputSurface
    {
    // types
    public:
        enum class ESurfaceType : ubyte
        {
            Unknown     = 0,
            Screen,
            CurvedScreen,
            VR,
            // multiple screens?
        };

        static constexpr uint   MaxOutputTargets    = 8;


        struct RenderTargetInfo
        {
            uint2                   dimension;
            float                   pixToMm;        // pixels to millimeters, used for touch screen, should not be used for VR

            RenderTargetInfo ()                         __NE___ {}
            RenderTargetInfo (uint2 dim, float pixToMm) __NE___ : dimension{dim}, pixToMm{pixToMm} {}
        };


        //
        // Render Target
        //
        struct RenderTarget
        {
        // variables
            ImageID                 imageId;
            ImageViewID             viewId;         // 2D with single mipmap, shared 2D array with specified 'layer'

            RectI                   region;         // for texture atlas
            ImageLayer              layer;

            float                   pixToMm;        // pixels to millimeters, used for touch screen, should not be used for VR

            EResourceState          initialState    = Default;
            EResourceState          finalState      = Default;

            EColorSpace             colorSpace      = Default;
            EPixelFormat            format          = Default;

            // Projection can be null.
            // Access is thread-safe only between 'Begin()' / 'End()'.
            Ptr<const IProjection>  projection;


        // methods
            ND_ uint2   RegionSize ()           C_NE___ { return RegionSizePxu(); }
            ND_ uint2   RegionSizePxu ()        C_NE___ { return uint2(region.Size()); }
            ND_ int2    RegionSizePxi ()        C_NE___ { return region.Size(); }
            ND_ float2  RegionSizePxf ()        C_NE___ { return float2(region.Size()); }
            ND_ float2  RegionSizeMm ()         C_NE___ { return float2(region.Size()) * pixToMm; }
        };
        using RenderTargets_t = FixedArray< RenderTarget, MaxOutputTargets >;


        //
        // Render Pass Info
        //
        struct RenderPassInfo
        {
        // types
            struct Attachment
            {
                EPixelFormat    format      = Default;
                MultiSamples    samples;
            };
            using Attachments_t = FixedArray< Attachment, MaxOutputTargets >;

        // variables
            Attachments_t   attachments;
        };

        using SurfaceFormats_t  = FixedArray< SurfaceFormat, 16 >;
        using PresentModes_t    = FixedArray< EPresentMode, 8 >;
        using TargetInfos_t     = FixedArray< RenderTargetInfo, MaxOutputTargets >;


        //
        // Surface Info
        //
        struct SurfaceInfo : SurfaceFormat
        {
        // variables
            ESurfaceType    type            = Default;
            EPresentMode    presentMode     = Default;

        // methods
            SurfaceInfo&  operator = (const SurfaceFormat &rhs) __NE___ { SurfaceFormat::operator = (rhs);  return *this; }
            SurfaceInfo&  operator = (const SurfaceInfo &rhs)   __NE___ = default;
        };


    // interface
    public:
        virtual ~IOutputSurface ()                                                                                              __NE___ {}


        // Returns 'true' if surface is initialized.
        //
        ND_ virtual bool  IsInitialized ()                                                                                      C_NE___ = 0;


        // Returns attachment parameters for render pass.
        //
        ND_ virtual RenderPassInfo  GetRenderPassInfo ()                                                                        C_NE___ = 0;


        // Begin rendering.
        // Returns image acquire task which is implicitly synchronized with present/blit task which returned by 'End()', returns 'null' on error.
        // 'beginCmdBatch'  - batch where render targets will be rendered.
        // 'endCmdBatch'    - batch where render targets was rendered.
        // 'deps'           - list of tasks which must be executed before.
        //
        ND_ virtual AsyncTask  Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)    __NE___ = 0;


        // Get render targets.
        // Must be used between 'Begin()' / 'End()'.
        //
            virtual bool  GetTargets (OUT RenderTargets_t &targets)                                                             C_NE___ = 0;


        // End rendering and present frame.
        // Returns present/blit task, returns 'null' on error.
        // 'deps'   - list of tasks which must be executed before, 'CmdBatchOnSubmit{endCmdBatch}' is implicitly added.
        //
        ND_ virtual AsyncTask  End (ArrayView<AsyncTask> deps)                                                                  __NE___ = 0;


        // Returns current surface sizes.
        // Size can be changed at any moment, so result may be outdated.
        // If not changed then result is equal to 'RenderTarget::RegionSize()' and 'RenderTarget::pixToMm' which returns by 'GetTargets()'.
        //
        ND_ virtual TargetInfos_t  GetTargetInfo ()                                                                             C_NE___ = 0;


        // Returns all supported color formats and color spaces.
        //
        ND_ virtual SurfaceFormats_t  GetSurfaceFormats ()                                                                      C_NE___ = 0;


        // Returns all supported present modes.
        //
        ND_ virtual PresentModes_t  GetPresentModes ()                                                                          C_NE___ = 0;


        // Returns current mode.
        //
        ND_ virtual SurfaceInfo  GetSurfaceInfo ()                                                                              C_NE___ = 0;


        // Set color format, color space and present mode.
        //
        ND_ virtual bool  SetSurfaceMode (const SurfaceInfo &)                                                                  __NE___ = 0;
    };


} // AE::App

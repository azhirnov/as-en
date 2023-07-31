// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Output surface can be:
        - window / display
        - video stream
        - render target
        - VR device
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
        };

        static constexpr uint   MaxOutputTargets    = 8;


        //
        // Render Target
        //
        struct RenderTarget
        {
            ImageID                 imageId;
            ImageViewID             viewId;         // 2D with single mipmap, shared 2D array with specified 'layer'

            RectI                   region;         // for texture atlas
            ImageLayer              layer;

            float2                  pixToMm;        // pixels to millimeters, used for touch screen, should not be used for VR

            EResourceState          initialState    = Default;
            EResourceState          finalState      = Default;

            EColorSpace             colorSpace      = Default;
            EPixelFormat            format          = Default;

            // Projection can be null.
            // Access is thread-safe only between 'Begin()' / 'End()'.
            Ptr<const IProjection>  projection;

            ND_ uint2   RegionSize ()           C_NE___ { return RegionSizePxu(); }
            ND_ uint2   RegionSizePxu ()        C_NE___ { return uint2(region.Size()); }
            ND_ int2    RegionSizePxi ()        C_NE___ { return region.Size(); }
            ND_ float2  RegionSizePxf ()        C_NE___ { return float2(region.Size()); }
            ND_ float2  RegionSizeMm ()         C_NE___ { return float2(region.Size()) * pixToMm; }
        };
        using RenderTargets_t = FixedArray< RenderTarget, MaxOutputTargets >;


        //
        // All Images which is used by surface
        //
        struct AllImages_t
        {
            FixedArray< ImageID, 16 >   images;
            EResourceState              initialState    = Default;
            EResourceState              finalState      = Default;
        };


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
        using TargetSizes_t     = FixedArray< uint2, MaxOutputTargets >;


        //
        // Surface Info
        //
        struct SurfaceInfo
        {
            ESurfaceType        type            = Default;
            SurfaceFormat       format;
            EPresentMode        presentMode     = Default;
        };


    // interface
    public:
        virtual ~IOutputSurface ()                                                                                              __NE___ {}


        // Returns 'true' if surface is initialized.
        //   Thread safe: yes
        //
        ND_ virtual bool  IsInitialized ()                                                                                      C_NE___ = 0;


        // Returns attachment parameters for render pass.
        //   Thread safe: yes
        //
        ND_ virtual RenderPassInfo  GetRenderPassInfo ()                                                                        C_NE___ = 0;

        // Begin rendering.
        // Returns image acquire task which is implicitly synchronized with present/blit task which returned by 'End()', returns 'null' on error.
        // 'beginCmdBatch'  - batch where render targets will be rendered.
        // 'endCmdBatch'    - batch where render targets was rendered.
        // 'deps'           - list of tasks which must be executed before.
        //   Thread safe: yes
        //
        ND_ virtual AsyncTask  Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)    __NE___ = 0;


        // Get render targets.
        // Must be used between 'Begin()' / 'End()'.
        //   Thread safe: yes
        //
            virtual bool  GetTargets (OUT RenderTargets_t &targets)                                                             C_NE___ = 0;


        // End rendering and present frame.
        // Returns present/blit task, returns 'null' on error.
        // 'deps'       - list of tasks which must be executed before, 'CmdBatchOnSubmit{endCmdBatch}' is implicitly added.
        //   Thread safe: yes
        //
        ND_ virtual AsyncTask  End (ArrayView<AsyncTask> deps)                                                                  __NE___ = 0;


        // Returns all images which is created by surface.
        // Can be used outside of 'Begin()/End()' scope.
        // Images can be deleted at any moment, so result may be deprecated.
        // If not changed then result is equal to 'RenderTarget::imageId' which returns by 'GetTargets()'.
        //   Thread safe: yes
        //
        ND_ virtual AllImages_t  GetAllImages ()                                                                                C_NE___ = 0;


        // Returns current surface sizes.
        // Size can be changed at any moment, so result may be deprecated.
        // If not changed then result is equal to 'RenderTarget::RegionSize()' which returns by 'GetTargets()'.
        //   Thread safe: yes
        //
        ND_ virtual TargetSizes_t  GetTargetSizes ()                                                                            C_NE___ = 0;


        // Returns all supported color formats and color spaces.
        //   Thread safe: yes
        //
        ND_ virtual SurfaceFormats_t  GetSurfaceFormats ()                                                                      C_NE___ = 0;


        // Returns all supported present modes.
        //   Thread safe: yes
        //
        ND_ virtual PresentModes_t  GetPresentModes ()                                                                          C_NE___ = 0;


        // Returns current mode.
        //   Thread safe: yes
        //
        ND_ virtual SurfaceInfo  GetSurfaceInfo ()                                                                              C_NE___ = 0;


        // Set color format, color space and present mode.
        //   Thread safe: yes
        //
        ND_ virtual bool  SetSurfaceMode (const SurfaceInfo &)                                                                  __NE___ = 0;
    };


} // AE::App

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Graphics.pch.h"

namespace AE::Graphics
{
    using namespace AE::Base;
    using namespace AE::Math;

    using AE::Threading::Promise;
    using AE::Threading::Atomic;
    using AE::Threading::BytesAtomic;
    using AE::Threading::DeferExLock;
    using AE::Threading::DeferSharedLock;
    using AE::Threading::EMemoryOrder;
    using AE::Threading::MemoryBarrier;
    using AE::Threading::Synchronized;

#   if AE_ENABLE_DATA_RACE_CHECK
    using AE::Threading::RWDataRaceCheck;
#   endif

    class RenderTask;
    class DrawTask;


    //
    // Graphics Config
    //
    struct GraphicsConfig final : Noninstanceable
    {
        // Values may be greater than current limit, used only to reserve memory.

        // buffer
        static constexpr uint   MaxVertexBuffers        = 8;
        static constexpr uint   MaxVertexAttribs        = 16;

        // render pass
        static constexpr uint   MaxViewports            = 8;
        static constexpr uint   MaxColorAttachments     = 8;
        static constexpr uint   MaxAttachments          = MaxColorAttachments + 1;  // color + depth_stencil
        static constexpr uint   MaxSamples              = 32;
        static constexpr uint   MaxMultiViews           = 2;
        static constexpr uint   MaxSubpasses            = 8;
        static constexpr uint   MaxSubpassDeps          = MaxSubpasses * 2;

        // pipeline
        static constexpr uint   MaxDescriptorSets       = 8;
        static constexpr uint   MaxPushConstants        = 8;
        static constexpr uint   MaxSpecConstants        = 8;

        static constexpr uint   MinFrames               = 2;
        static constexpr uint   MaxFrames               = 4;
        static constexpr uint   MaxCmdBuffersPerPool    = 16;
        static constexpr uint   MaxCmdPoolsPerQueue     = 8;                        // == max render threads

        static constexpr uint   MaxCmdBufPerBatch       = 32;

        static constexpr uint   MaxPendingCmdBatches    = 15;
    };



    //
    // Debug Label
    //
    struct DebugLabel
    {
        struct ColorTable
        {
            static constexpr RGBA8u     Undefined           = RGBA8u{255, 255, 255, 0};     // transparent white

            static constexpr RGBA8u     GraphicsQueue       = HtmlColor::Red;
            static constexpr RGBA8u     AsyncComputeQueue   = HtmlColor::Orange;
            static constexpr RGBA8u     AsyncTransfersQueue = HtmlColor::Violet;

            static constexpr RGBA8u     AsyncDrawBatch      = HtmlColor::Yellow;

            static constexpr RGBA8u     GraphicsCtx         = HtmlColor::Red;
            static constexpr RGBA8u     ComputeCtx          = HtmlColor::Orange;
            static constexpr RGBA8u     TransferCtx         = HtmlColor::Violet;
            static constexpr RGBA8u     DrawCtx             = HtmlColor::Yellow;
            static constexpr RGBA8u     AccelStructBuiltCtx = HtmlColor::Lime;
            static constexpr RGBA8u     RayTracingCtx       = HtmlColor::Blue;
        };


    // variables
        StringView      label;
        RGBA8u          color   = ColorTable::Undefined;


    // methods
        constexpr DebugLabel (Base::_hidden_::DefaultType)      __NE___                             {}
        constexpr DebugLabel (StringView label)                 __NE___ : label{label}              {}
        constexpr DebugLabel (StringView label, RGBA8u color)   __NE___ : label{label}, color{color}{}

        ND_ explicit operator bool ()                           C_NE___ { return not label.empty(); }
    };


} // AE::Graphics

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/Queue.h"

namespace AE::Graphics
{
    //
    // Surface Format
    //

    struct SurfaceFormat
    {
        EPixelFormat    colorFormat = Default;
        EColorSpace     colorSpace  = Default;

        SurfaceFormat ()                                                __NE___ = default;
        explicit SurfaceFormat (EPixelFormat fmt, EColorSpace space)    __NE___ : colorFormat{fmt}, colorSpace{space} {}

        ND_ bool  operator == (const SurfaceFormat &rhs)                C_NE___ { return colorFormat == rhs.colorFormat and colorSpace == rhs.colorSpace; }
    };


    //
    // Swapchain description
    //

    struct SwapchainDesc : SurfaceFormat
    {
        EPresentMode    presentMode     = EPresentMode::FIFO;
        ubyte           minImageCount   = 2;
        EImageUsage     usage           = EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;   // TODO: keep ColorAttachment only
        EImageOpt       options         = EImageOpt::BlitDst;


        SwapchainDesc () __NE___ : SurfaceFormat{ Default, EColorSpace::sRGB_nonlinear } {}
    };


} // AE::Graphics


namespace AE::Base
{
    template <> struct TTriviallySerializable< Graphics::SurfaceFormat >    { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< Graphics::SwapchainDesc >    { static constexpr bool  value = true; };
}

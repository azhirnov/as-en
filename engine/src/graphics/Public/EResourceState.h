// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    EResourceState contains information about pipeline stage,
    memory access mask, data invalidation and other.
*/

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

    //
    // Resource State
    //

    enum class EResourceState : ushort
    {
        Unknown                                 = 0,
        Preserve                                = 0x01,             // keep content and previous state

        ShaderStorage_Read                      = 0x02,
        ShaderStorage_Write                     = 0x03,
        ShaderStorage_ReadWrite                 = 0x04,
        ShaderStorage_RW                        = ShaderStorage_ReadWrite,

        // buffer device address
        ShaderAddress_Read                      = ShaderStorage_Read,
        ShaderAddress_Write                     = ShaderStorage_Write,
        ShaderAddress_RW                        = ShaderStorage_RW,

        ShaderUniform                           = 0x05,
        UniformRead                             = ShaderUniform,
        ShaderSample                            = 0x06,             // sampled image or uniform texel buffer

        CopySrc                                 = 0x07,
        CopyDst                                 = 0x08,

        ClearDst                                = 0x09,

        BlitSrc                                 = 0x0A,
        BlitDst                                 = 0x0B,

        InputColorAttachment                    = 0x0C,             // for fragment or tile shader
        InputColorAttachment_ReadWrite          = 0x0D,
        InputColorAttachment_RW                 = InputColorAttachment_ReadWrite,

        ColorAttachment_Write                   = 0x0E,
        ColorAttachment_ReadWrite               = 0x0F,
        ColorAttachment_RW                      = ColorAttachment_ReadWrite,

        DepthStencilAttachment_Read             = 0x10,             // only for depth test
        DepthStencilTest                        = DepthStencilAttachment_Read,
        DepthStencilAttachment_Write            = 0x11,
        DepthStencilAttachment_ReadWrite        = 0x12,             // depth test and write
        DepthStencilAttachment_RW               = DepthStencilAttachment_ReadWrite,
        DepthRead_StencilReadWrite              = 0x13,
        DepthTest_StencilRW                     = DepthRead_StencilReadWrite,
        DepthReadWrite_StencilRead              = 0x14,
        DepthRW_StencilTest                     = DepthReadWrite_StencilRead,

        InputDepthStencilAttachment             = 0x15,
        InputDepthStencilAttachment_ReadWrite   = 0x16,
        InputDepthStencilAttachment_RW          = InputDepthStencilAttachment_ReadWrite,

        // readonly depth stencil attachment with depth or stencil test  +  read/sample in fragment shader
        DepthStencilTest_ShaderSample           = 0x17,
        DepthTest_DepthSample_StencilRW         = 0x18,

        Host_Read                               = 0x19,
        Host_Write                              = 0x1A,
        Host_ReadWrite                          = 0x1B,
        Host_RW                                 = Host_ReadWrite,

        PresentImage                            = 0x1C,

        IndirectBuffer                          = 0x1D,
        IndexBuffer                             = 0x1E,
        VertexBuffer                            = 0x1F,

        // only for BuildRTASContext
        CopyRTAS_Read                           = 0x20,             // AS & src buffer
        CopyRTAS_Write                          = 0x21,             // AS & dst buffer
        BuildRTAS_Read                          = 0x22,
        BuildRTAS_Write                         = 0x23,
        BuildRTAS_ReadWrite                     = 0x24,
        BuildRTAS_RW                            = BuildRTAS_ReadWrite,
        BuildRTAS_ScratchBuffer                 = BuildRTAS_ReadWrite,
        BuildRTAS_IndirectBuffer                = 0x25,

        ShaderRTAS_Read                         = 0x26,             // use RTScene in shader, for RT pipeline and ray query
        RTShaderBindingTable                    = 0x27,

        ShadingRateImage                        = 0x28,

        General                                 = 0x29,             // all stages & all access types

        _AccessCount,
        _AccessMask                             = 0x3F,

        // flags
        DSTestBeforeFS                          = 0x0080,
        DSTestAfterFS                           = 0x0100,
        Invalidate                              = 0x0200,       // only for image
        _FlagsMask                              = DSTestBeforeFS | DSTestAfterFS | Invalidate,

        // shader bits
        MeshTaskShader                          = 0x0400,       // AMD: executed in compute queue
        VertexProcessingShaders                 = 0x0800,       // executed only in graphics queue (VS, TCS, TES, GS, Mesh)
        TileShader                              = 0x1000,
        FragmentShader                          = 0x2000,
        ComputeShader                           = 0x4000,
        RayTracingShaders                       = 0x8000,
        PreRasterizationShaders                 = MeshTaskShader | VertexProcessingShaders,
        PostRasterizationShaders                = TileShader | FragmentShader,
        AllGraphicsShaders                      = PreRasterizationShaders | PostRasterizationShaders,
        AllShaders                              = PreRasterizationShaders | PostRasterizationShaders | ComputeShader | RayTracingShaders,

        _InvalidState                           = 0xFFFF,
    };

    STATIC_ASSERT( EResourceState::_AccessCount < EResourceState::_AccessMask );


    ND_ forceinline constexpr EResourceState  operator | (EResourceState lhs, EResourceState rhs)
    {
        DBG_CHECK_MSG( uint(AnyBits( lhs, EResourceState::_AccessMask )) + uint(AnyBits( rhs, EResourceState::_AccessMask )) < 2,
                    "only one operand must have access bits" );

        return EResourceState( uint(lhs) | uint(rhs) );
    }

    ND_ forceinline constexpr EResourceState  operator & (EResourceState lhs, EResourceState rhs)
    {
        return EResourceState( uint(lhs) & uint(rhs) );
    }

    forceinline constexpr EResourceState&  operator |= (EResourceState &lhs, EResourceState rhs)
    {
        return (lhs = (lhs | rhs));
    }

    forceinline constexpr EResourceState&  operator &= (EResourceState &lhs, EResourceState rhs)
    {
        return (lhs = (lhs & rhs));
    }

    ND_ forceinline constexpr EResourceState  operator ~ (EResourceState x)
    {
        return EResourceState(~uint(x));
    }

/*
=================================================
    EResourceState_IsSameStates
=================================================
*/
    ND_ forceinline constexpr bool  EResourceState_IsSameStates (EResourceState srcState, EResourceState dstState) __NE___
    {
        constexpr auto  mask = EResourceState::_AccessMask | EResourceState::Invalidate;
        return (srcState & mask) == (dstState & mask);
    }


} // AE::Graphics

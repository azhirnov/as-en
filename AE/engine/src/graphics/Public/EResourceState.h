// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    EResourceState contains information about pipeline stage,
    memory access mask, data invalidation and other.
*/

#pragma once

#include "graphics/Public/ShaderEnums.h"

namespace AE::Graphics
{

    //
    // Resource State
    //

    struct _EResState
    {
        enum EState : uint
        {
            Unknown                     = 0,
            Preserve,

            // image layout outside of shader stages
            CopySrc,
            CopyDst,
            ClearDst,
            BlitSrc,
            BlitDst,
            ColorAttachment,
            DepthStencilTest,
            DepthStencilAttachment_RW,
            DepthTest_StencilRW,
            DepthRW_StencilTest,
            PresentImage,
            ShadingRateImage,
            General,

            // image layout in shader stages
            ShaderStorage_Read,
            ShaderStorage_Write,
            ShaderStorage_RW,
            ShaderUniform,
            ShaderSample,
            InputColorAttachment,
            InputColorAttachment_RW,
            InputDepthStencilAttachment,
            InputDepthStencilAttachment_RW,
            DepthStencilTest_ShaderSample,
            DepthTest_DepthSample_StencilRW,
            Host_Read,
            Host_Write,

            // buffer state in shader stages
            ShaderRTAS,
            //ShaderAddress = ShaderStorage,

            // buffer state outside of shader stages
            IndirectBuffer,
            IndexBuffer,
            VertexBuffer,
            CopyRTAS_Read,
            CopyRTAS_Write,
            BuildRTAS_Read,
            BuildRTAS_RW,
            BuildRTAS_IndirectBuffer,
            RTShaderBindingTable,

            _AccessCount,
        };
        static constexpr uint   AccessMask                  = 0xFF;

        // access type to speed up barrier checks
        static constexpr uint   Read                        = 1 << 8;           // buffer|color|depth
        static constexpr uint   Write                       = 1 << 9;           // buffer|color|depth
        static constexpr uint   ReadWrite                   = Read | Write;
        static constexpr uint   DepthTest                   = Read;
        static constexpr uint   DepthWrite                  = Write;
        static constexpr uint   DepthRW                     = Read | Write;
        static constexpr uint   StencilTest                 = 1 << 10;
        static constexpr uint   StencilWrite                = 1 << 11;
        static constexpr uint   StencilRW                   = StencilTest | StencilWrite;
        static constexpr uint   AllReadWriteBits            = ReadWrite | StencilRW;
        static constexpr uint   AllReadBits                 = Read | StencilTest;
        static constexpr uint   AllWriteBits                = Write | StencilWrite;

        // flags
        static constexpr uint   DSTestBeforeFS              = 1 << 12;
        static constexpr uint   DSTestAfterFS               = 1 << 13;
        static constexpr uint   Invalidate                  = 1 << 14;

        // shader bits
        static constexpr uint   MeshTaskShader              = 1 << 15;
        static constexpr uint   VertexProcessingShaders     = 1 << 16;
        static constexpr uint   TileShader                  = 1 << 17;
        static constexpr uint   FragmentShader              = 1 << 18;
        static constexpr uint   ComputeShader               = 1 << 19;
        static constexpr uint   RayTracingShaders           = 1 << 20;

        STATIC_ASSERT( uint(_AccessCount) < Read );
    };


    enum class EResourceState : uint
    {
        Unknown                                 = _EResState::Unknown,
        Preserve                                = _EResState::Preserve,                                     // keep content and previous state

        ShaderStorage_Read                      = _EResState::ShaderStorage_Read | _EResState::Read,
        ShaderStorage_Write                     = _EResState::ShaderStorage_Write | _EResState::Write,
        ShaderStorage_ReadWrite                 = _EResState::ShaderStorage_RW | _EResState::ReadWrite,
        ShaderStorage_RW                        = ShaderStorage_ReadWrite,

        ShaderAddress_Read                      = ShaderStorage_Read,                                       // buffer device address
        ShaderAddress_Write                     = ShaderStorage_Write,
        ShaderAddress_RW                        = ShaderStorage_RW,

        ShaderUniform                           = _EResState::ShaderUniform | _EResState::Read,
        UniformRead                             = ShaderUniform,
        ShaderSample                            = _EResState::ShaderSample | _EResState::Read,              // sampled image or uniform texel buffer

        CopySrc                                 = _EResState::CopySrc | _EResState::Read,
        CopyDst                                 = _EResState::CopyDst | _EResState::Write,

        ClearDst                                = _EResState::ClearDst | _EResState::Write,

        BlitSrc                                 = _EResState::BlitSrc | _EResState::Read,
        BlitDst                                 = _EResState::BlitDst | _EResState::Write,

        InputColorAttachment                    = _EResState::InputColorAttachment | _EResState::Read,      // for fragment or tile shader
        InputColorAttachment_ReadWrite          = _EResState::InputColorAttachment_RW | _EResState::Write,  // for programmable blending
        InputColorAttachment_RW                 = InputColorAttachment_ReadWrite,

        ColorAttachment                         = _EResState::ColorAttachment | _EResState::Write,
        ColorAttachment_Blend                   = _EResState::ColorAttachment | _EResState::ReadWrite,      // for blending

        DepthStencilAttachment_Read             = _EResState::DepthStencilTest | _EResState::DepthTest | _EResState::StencilTest,               // only for depth test
        DepthStencilTest                        = DepthStencilAttachment_Read,
        DepthStencilAttachment_Write            = _EResState::DepthStencilAttachment_RW | _EResState::DepthWrite | _EResState::StencilWrite,
        DepthStencilAttachment_ReadWrite        = _EResState::DepthStencilAttachment_RW | _EResState::DepthRW | _EResState::StencilRW,          // depth test and write
        DepthStencilAttachment_RW               = DepthStencilAttachment_ReadWrite,
        DepthRead_StencilReadWrite              = _EResState::DepthTest_StencilRW | _EResState::DepthTest | _EResState::StencilRW,
        DepthTest_StencilRW                     = DepthRead_StencilReadWrite,
        DepthReadWrite_StencilRead              = _EResState::DepthRW_StencilTest | _EResState::DepthRW | _EResState::StencilTest,
        DepthRW_StencilTest                     = DepthReadWrite_StencilRead,

        InputDepthStencilAttachment             = _EResState::InputDepthStencilAttachment | _EResState::DepthTest | _EResState::StencilTest,
        InputDepthStencilAttachment_ReadWrite   = _EResState::InputDepthStencilAttachment_RW | _EResState::DepthRW | _EResState::StencilRW,
        InputDepthStencilAttachment_RW          = InputDepthStencilAttachment_ReadWrite,

        // readonly depth stencil attachment with depth or stencil test  +  read/sample in fragment shader
        DepthStencilTest_ShaderSample           = _EResState::DepthStencilTest_ShaderSample | _EResState::DepthTest | _EResState::StencilTest,
        DepthTest_DepthSample_StencilRW         = _EResState::DepthTest_DepthSample_StencilRW | _EResState::DepthTest | _EResState::StencilRW,

        Host_Read                               = _EResState::Host_Read | _EResState::Read,
        Host_Write                              = _EResState::Host_Write | _EResState::Write,

        PresentImage                            = _EResState::PresentImage | _EResState::Read,

        IndirectBuffer                          = _EResState::IndirectBuffer | _EResState::Read,
        IndexBuffer                             = _EResState::IndexBuffer | _EResState::Read,
        VertexBuffer                            = _EResState::VertexBuffer | _EResState::Read,

        // only for BuildRTASContext
        CopyRTAS_Read                           = _EResState::CopyRTAS_Read | _EResState::Read,             // AS & src buffer
        CopyRTAS_Write                          = _EResState::CopyRTAS_Write | _EResState::Write,           // AS & dst buffer
        BuildRTAS_Read                          = _EResState::BuildRTAS_Read | _EResState::Read,
        BuildRTAS_Write                         = _EResState::BuildRTAS_RW | _EResState::Write,
        BuildRTAS_ReadWrite                     = _EResState::BuildRTAS_RW | _EResState::ReadWrite,
        BuildRTAS_RW                            = BuildRTAS_ReadWrite,
        BuildRTAS_ScratchBuffer                 = BuildRTAS_ReadWrite,
        BuildRTAS_IndirectBuffer                = _EResState::BuildRTAS_IndirectBuffer | _EResState::Read,

        ShaderRTAS                              = _EResState::ShaderRTAS | _EResState::Read,                // use RTScene in shader, for RT pipeline and ray query
        RTShaderBindingTable                    = _EResState::RTShaderBindingTable | _EResState::Read,

        ShadingRateImage                        = _EResState::ShadingRateImage | _EResState::Read,

        General                                 = _EResState::General | _EResState::ReadWrite,              // all stages & all access types


        // flags
        DSTestBeforeFS                          = _EResState::DSTestBeforeFS,           // depth stencil test before fragment shader (best performance)
        DSTestAfterFS                           = _EResState::DSTestAfterFS,            // depth stencil test after fragment shader (low performance)
        Invalidate                              = _EResState::Invalidate,               // only for image

        // shader bits
        MeshTaskShader                          = _EResState::MeshTaskShader,           // AMD: executed in compute queue
        VertexProcessingShaders                 = _EResState::VertexProcessingShaders,  // executed only in graphics queue (VS, TCS, TES, GS, Mesh)
        TileShader                              = _EResState::TileShader,
        FragmentShader                          = _EResState::FragmentShader,
        ComputeShader                           = _EResState::ComputeShader,
        RayTracingShaders                       = _EResState::RayTracingShaders,
        PreRasterizationShaders                 = MeshTaskShader | VertexProcessingShaders,
        PostRasterizationShaders                = TileShader | FragmentShader,
        AllGraphicsShaders                      = PreRasterizationShaders | PostRasterizationShaders,
        AllShaders                              = PreRasterizationShaders | PostRasterizationShaders | ComputeShader | RayTracingShaders,

        _InvalidState                           = ~0u,
    };

    STATIC_ASSERT( sizeof(EResourceState) == sizeof(_EResState::EState) );


/*
=================================================
    EResourceState operators
=================================================
*/
    ND_ constexpr EResourceState  operator | (EResourceState lhs, EResourceState rhs) __NE___
    {
        DBG_CHECK_MSG( uint(AnyBits( lhs, _EResState::AccessMask )) + uint(AnyBits( rhs, _EResState::AccessMask )) < 2,
                    "only one operand must have access bits" );

        return EResourceState( uint(lhs) | uint(rhs) );
    }

    ND_ constexpr EResourceState  operator & (EResourceState lhs, EResourceState rhs) __NE___
    {
        return EResourceState( uint(lhs) & uint(rhs) );
    }

    constexpr EResourceState&  operator |= (EResourceState &lhs, EResourceState rhs) __NE___
    {
        return (lhs = (lhs | rhs));
    }

    constexpr EResourceState&  operator &= (EResourceState &lhs, EResourceState rhs) __NE___
    {
        return (lhs = (lhs & rhs));
    }

    ND_ constexpr EResourceState  operator ~ (EResourceState x) __NE___
    {
        return EResourceState(~uint(x));
    }


    ND_ constexpr _EResState::EState  ToEResState (EResourceState state) __NE___
    {
        return _EResState::EState(uint(state) & _EResState::AccessMask);
    }


} // AE::Graphics

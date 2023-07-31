// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/RenderPassDesc.h"
# include "graphics/Vulkan/VCommon.h"
# include "Packer/RenderPassPack.h"

namespace AE::Graphics
{
    using AE::PipelineCompiler::SerializableRenderPassInfo;
    using AE::PipelineCompiler::SerializableVkRenderPass;



    //
    // Vulkan Render Pass
    //

    class VRenderPass final
    {
    // types
    public:
        using ShaderIOArr_t         = PipelineCompiler::SubpassShaderIO::ShaderIOArr_t;
        using AttachmentStates_t    = SerializableVkRenderPass::AttachmentStates_t;

        struct SubpassInfo
        {
            ShaderIOArr_t   colorAttachments;
            ShaderIOArr_t   inputAttachments;
        };

        struct AttachmentIdx
        {
            ubyte   _data   = UMax;

            AttachmentIdx (uint idx, bool clear) : _data{ubyte( (idx & 0xF) | (clear ? 0x10 : 0) )} {}

            ND_ uint    Index ()    C_NE___ { return _data & 0x0F; }
            ND_ bool    IsClear ()  C_NE___ { return !!(_data & 0x10); }
        };

    private:
        using AttachmentMap_t       = FixedMap< AttachmentName::Optimized_t, AttachmentIdx, GraphicsConfig::MaxAttachments >;
        using SPNameToIdx_t         = FixedMap< SubpassName::Optimized_t, ubyte, GraphicsConfig::MaxSubpasses >;
        using Subpasses_t           = FixedArray< SubpassInfo, GraphicsConfig::MaxSubpasses >;
        using AttachmentPixFormat_t = StaticArray< EPixelFormat, GraphicsConfig::MaxAttachments >;


    // variables
    private:
        VkRenderPass                _renderPass     = Default;
        Strong<RenderPassID>        _compatibleRP;  // this RP will be used to create pipelines and framebuffers

        AttachmentMap_t             _attachmentMap;
        AttachmentStates_t          _attStates;
        SPNameToIdx_t               _subpassMap;
        Subpasses_t                 _subpasses;
        AttachmentPixFormat_t       _pixFormats;

        DEBUG_ONLY(
            SubpassName::Optimized_t _firstSPName;
            DebugName_t              _debugName;
        )
        DRC_ONLY(   RWDataRaceCheck _drCheck;)


    // methods
    public:
        VRenderPass ()                                                                      __NE___ {}
        ~VRenderPass ()                                                                     __NE___;

        ND_ bool  Create (VResourceManager&, const SerializableRenderPassInfo &compatInfo,
                          const SerializableVkRenderPass &vkInfo, RenderPassID compatId,
                          StringView dbgName)                                               __NE___;
            void  Destroy (VResourceManager &)                                              __NE___;

        ND_ bool  GetMaxTileWorkgroupSize (const VDevice &dev, OUT uint2 &tileSize)         C_NE___;
        ND_ uint2 GetTileSizeGranularity (const VDevice &dev)                               C_NE___;

        ND_ VkRenderPass                Handle ()                                           C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _renderPass; }
        ND_ RenderPassID                CompatibpleRP ()                                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _compatibleRP; }

        ND_ AttachmentMap_t const&      AttachmentMap ()                                    C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _attachmentMap; }
        ND_ AttachmentStates_t const&   AttachmentStates ()                                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _attStates; }
        ND_ SPNameToIdx_t const&        SubpassMap ()                                       C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _subpassMap; }
        ND_ ArrayView<SubpassInfo>      Subpasses ()                                        C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _subpasses; }

        ND_ EPixelFormat                GetPixelFormat (const AttachmentName &name)         C_NE___;
        ND_ EPixelFormat                GetPixelFormat (uint attachmentIdx)                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _pixFormats[attachmentIdx]; }

        DEBUG_ONLY(
            ND_ StringView                  GetDebugName ()                                 C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _debugName; }
            ND_ SubpassName::Optimized_t    GetFirstSubpassName ()                          C_NE___ { DRC_SHAREDLOCK( _drCheck );  return _firstSPName; }
        )
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

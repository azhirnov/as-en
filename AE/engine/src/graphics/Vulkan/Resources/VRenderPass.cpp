// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VRenderPass.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VRenderPass::~VRenderPass () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( _renderPass == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VRenderPass::Create (VResourceManager& resMngr, const SerializableRenderPassInfo &compatInfo, const SerializableVkRenderPass &vkInfo,
                               RenderPassID compatId, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _renderPass == Default );
        CHECK_ERR( not _compatibleRP );

        if ( compatId != Default )
        {
            _compatibleRP = resMngr.AcquireResource( compatId );
            CHECK_ERR( compatId.IsValid() == _compatibleRP.IsValid() ); // will fail if compatible RP is not alive
        }

        const VkRenderPassCreateInfo2*  rp_ci = vkInfo.operator->();

        auto&   dev = resMngr.GetDevice();
        VK_CHECK_ERR( dev.vkCreateRenderPass2KHR( dev.GetVkDevice(), rp_ci, null, OUT &_renderPass ));

        _subpasses.resize( compatInfo.subpasses.size() );

        for (auto [name, sp] : compatInfo.subpasses)
        {
            SubpassInfo&    sp_info     = _subpasses[ sp.subpassIndex ];
            sp_info.colorAttachments    = sp.colorAttachments;
            sp_info.inputAttachments    = sp.inputAttachments;

            _subpassMap.emplace( name, sp.subpassIndex );

            DEBUG_ONLY(
                if ( sp.subpassIndex == 0 )
                    _firstSPName = name;
            )
        }

        _attStates = vkInfo.AttachmentStates();

        for (auto [name, idx] : compatInfo.attachments)
        {
            ASSERT( idx < rp_ci->attachmentCount );

            _pixFormats[idx] = AEEnumCast( rp_ci->pAttachments[idx].format );
            _attachmentMap.emplace( name, AttachmentIdx{ idx, (rp_ci->pAttachments[idx].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR) });
        }

        dev.SetObjectName( _renderPass, dbgName, VK_OBJECT_TYPE_RENDER_PASS );

        DEBUG_ONLY( _debugName = dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VRenderPass::Destroy (VResourceManager& resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        resMngr.ImmediatelyRelease( INOUT _compatibleRP );

        if ( _renderPass != Default )
        {
            auto&   dev = resMngr.GetDevice();
            dev.vkDestroyRenderPass( dev.GetVkDevice(), _renderPass, null );
        }

        _attachmentMap.clear();
        _subpassMap.clear();
        _subpasses.clear();
        _pixFormats.fill( Default );

        _renderPass     = Default;
        _compatibleRP   = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    GetMaxTileWorkgroupSize
=================================================
*/
    bool  VRenderPass::GetMaxTileWorkgroupSize (const VDevice &dev, OUT uint2 &tileSize) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        CHECK_ERR( _renderPass != Default );
        CHECK_ERR( dev.GetVExtensions().subpassShadingHW );

        VkExtent2D  size {};
        VK_CHECK_ERR( dev.vkGetDeviceSubpassShadingMaxWorkgroupSizeHUAWEI( dev.GetVkDevice(), _renderPass, OUT &size ));

        tileSize.x = size.width;
        tileSize.y = size.height;

        return true;
    }

/*
=================================================
    GetTileSizeGranularity
=================================================
*/
    uint2  VRenderPass::GetTileSizeGranularity (const VDevice &dev) C_NE___
    {
        // TODO: define tile size if create info and just check granularity in ctor
        // Mali GPU use 4x4 and 16x16 blocks for some HW optimizations.
        // Adreno have a large tile sime and can switch off the TBDR.

        VkExtent2D  granularity {};
        dev.vkGetRenderAreaGranularity( dev.GetVkDevice(), _renderPass, OUT &granularity );

        return uint2{ granularity.width, granularity.height };
    }

/*
=================================================
    GetPixelFormat
=================================================
*/
    EPixelFormat  VRenderPass::GetPixelFormat (const AttachmentName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto    it = _attachmentMap.find( name );
        if_likely( it != _attachmentMap.end() )
        {
            return _pixFormats[ it->second.Index() ];
        }
        return Default;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

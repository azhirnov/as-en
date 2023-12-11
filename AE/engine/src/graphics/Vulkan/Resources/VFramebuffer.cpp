// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/EnumToString.h"
# include "graphics/Vulkan/Resources/VFramebuffer.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
    CalcHash
=================================================
*/
    HashVal  VFramebuffer::Key::CalcHash () C_NE___
    {
        return HashOf( dimension ) + HashOf( renderPassId ) + HashOf( attachments );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    destructor
=================================================
*/
    VFramebuffer::~VFramebuffer () __NE___
    {
        CHECK( _framebuffer == Default );
    }

/*
=================================================
    IsAllResourcesAlive
=================================================
*/
    bool  VFramebuffer::IsAllResourcesAlive (const VResourceManager &resMngr) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        for (auto& att : _attachments)
        {
            if ( not resMngr.IsAlive( att ))
                return false;
        }
        return true;
    }

/*
=================================================
    Create
=================================================
*/
    bool  VFramebuffer::Create (VResourceManager &resMngr, const RenderPassDesc &rpDesc, RenderPassID rpId, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( not _framebuffer );
        CHECK_ERR( not rpDesc.attachments.empty() );

        _renderPassId = resMngr.AcquireResource( rpId );
        CHECK_ERR( _renderPassId );

        VRenderPass const*  ren_pass = resMngr.GetResource( _renderPassId );
        CHECK_ERR( ren_pass != null );

        const auto&     rp_attach   = ren_pass->AttachmentMap();
        VDevice const&  dev         = resMngr.GetDevice();

        CHECK_ERR( rp_attach.size() == rpDesc.attachments.size() );
        CHECK_ERR( rpDesc.area.IsValid() );
        CHECK_ERR( rpDesc.layerCount.Get() > 0 );

        FixedArray< VkImageView, GraphicsConfig::MaxAttachments >   image_views;
        BitSet< GraphicsConfig::MaxAttachments >                    assigned;

        image_views.resize( rpDesc.attachments.size() );
        _attachments.resize( rpDesc.attachments.size() );
        _images.resize( rpDesc.attachments.size() );

        for (auto [name, att] : rpDesc.attachments)
        {
            auto    iter = rp_attach.find( name );
            if_unlikely( iter == rp_attach.end() )
            {
            #ifdef AE_DEBUG
                String  str {"Attachment '"};
                str << resMngr.HashToName( name ) << "' is not exists in render pass '"
                    << ren_pass->GetDebugName() << "'.\nRequired attachments: ";

                for (auto [n, a] : rp_attach) {
                    str << "'" << resMngr.HashToName( n ) << "', ";
                }
                str.pop_back();
                str.pop_back();
                RETURN_ERR( str );
            #else
                RETURN_ERR( "Attachment is not exists in render pass" );
            #endif
            }

            auto*   view = resMngr.GetResource( att.imageView );
            CHECK_ERR( view != null );

            const uint  idx = iter->second.Index();

            CHECK_ERR( not assigned.test( idx ));   // already assigned
            assigned.set( idx );

            image_views[ idx ]  = view->Handle();
            _attachments[ idx ] = att.imageView;
            _images[ idx ]      = view->ImageId();

            #ifdef AE_DEBUG
            {
                const auto  att_fmt = resMngr.GetDescription( view->ImageId() ).format;
                const auto  rp_fmt  = ren_pass->GetPixelFormat( idx );

                if ( att_fmt != rp_fmt )
                {
                    String  str {"Attachment '"};
                    str << resMngr.HashToName( name ) << "' has different pixel format than in render pass '"
                        << ren_pass->GetDebugName() << "'\n" << ToString( att_fmt ) << " != " << ToString( rp_fmt );
                    RETURN_ERR( str );
                }
            }
            #endif
        }

        _dimension  = uint2{rpDesc.area.Size()};    // TODO: check
        _layers     = rpDesc.layerCount;

        // create framebuffer
        VkFramebufferCreateInfo     fb_info = {};

        fb_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.renderPass      = ren_pass->Handle();
        fb_info.attachmentCount = uint(image_views.size());
        fb_info.pAttachments    = image_views.data();
        fb_info.width           = _dimension.x;
        fb_info.height          = _dimension.y;
        fb_info.layers          = _layers.Get();

        VK_CHECK_ERR( dev.vkCreateFramebuffer( dev.GetVkDevice(), &fb_info, null, OUT &_framebuffer ));

        dev.SetObjectName( _framebuffer, dbgName, VK_OBJECT_TYPE_FRAMEBUFFER );
        DEBUG_ONLY( _debugName = dbgName; )

        return true;
    }

/*
=================================================
    SetCachePtr
=================================================
*/
    bool  VFramebuffer::SetCachePtr (CachePtr_t ptr) __NE___
    {
        CHECK_ERR( _framebuffer != Default );
        CHECK_ERR( _cachePtr == Default );

        _cachePtr = ptr;
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VFramebuffer::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( _framebuffer != Default )
        {
            auto&   dev = resMngr.GetDevice();
            dev.vkDestroyFramebuffer( dev.GetVkDevice(), _framebuffer, null );
        }

        resMngr.ImmediatelyRelease( INOUT _renderPassId );

        if ( _cachePtr != Default )
            resMngr.RemoveFramebufferCache( _cachePtr );

        _framebuffer    = Default;
        _renderPassId   = Default;
        _dimension      = Default;
        _layers         = Default;
        _cachePtr       = Default;

        _images.clear();
        _attachments.clear();

        DEBUG_ONLY( _debugName.clear(); )
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

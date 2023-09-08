// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VImageView.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VImageView::~VImageView () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _imageView == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VImageView::Create (VResourceManager &resMngr, const ImageViewDesc &desc, ImageID imageId, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _imageView == Default );

        const VImage*   image = resMngr.GetResource( imageId, True{"incRef"} );
        CHECK_ERR( image != null );

        _imageId = Strong<ImageID>{imageId};
        _canBeDestroyed = true;

        const auto& img_desc = image->Description();

        _desc = desc;
        _desc.Validate( img_desc );
        GRES_CHECK( image->IsSupported( resMngr, _desc ));

        VkImageViewUsageCreateInfo  ext_usage_info = {};
        ext_usage_info.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO;

        VkImageViewCreateInfo   info = {};
        info.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.flags      = 0;
        info.image      = image->Handle();
        info.viewType   = VEnumCast( _desc.viewType );
        info.format     = VEnumCast( _desc.format );
        info.components = VEnumCast( _desc.swizzle );

        info.subresourceRange.aspectMask        = VEnumCast( _desc.aspectMask );
        info.subresourceRange.baseMipLevel      = _desc.baseMipmap.Get();
        info.subresourceRange.levelCount        = _desc.mipmapCount;
        info.subresourceRange.baseArrayLayer    = _desc.baseLayer.Get();
        info.subresourceRange.layerCount        = _desc.layerCount;

        auto&   dev = resMngr.GetDevice();
        if ( desc.extUsage   != Default         and
             dev.GetVExtensions().maintenance2  )
        {
            ext_usage_info.usage = VEnumCast( desc.extUsage, img_desc.memType );
            info.pNext = &ext_usage_info;
        }

        VK_CHECK_ERR( dev.vkCreateImageView( dev.GetVkDevice(), &info, null, OUT &_imageView ));

        dev.SetObjectName( _imageView, dbgName, VK_OBJECT_TYPE_IMAGE_VIEW );

        DEBUG_ONLY( _debugName = dbgName; )
        return true;
    }

/*
=================================================
    Create
=================================================
*/
    bool  VImageView::Create (VResourceManager &resMngr, const VulkanImageViewDesc &desc, ImageID imageId, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _imageView == Default );
        CHECK_ERR( desc.view != Default );

        const VImage*   image = resMngr.GetResource( imageId, True{"incRef"} );
        CHECK_ERR( image != null );

        _imageId = Strong<ImageID>{imageId};

        _desc.viewType      = AEEnumCast( desc.viewType );
        _desc.format        = AEEnumCast( desc.format );
        _desc.aspectMask    = AEEnumCast( VkImageAspectFlagBits( desc.subresourceRange.aspectMask ));
        _desc.baseMipmap    = MipmapLevel{ desc.subresourceRange.baseMipLevel };
        _desc.mipmapCount   = ushort(desc.subresourceRange.levelCount);
        _desc.baseLayer     = ImageLayer{ desc.subresourceRange.baseArrayLayer };
        _desc.layerCount    = ushort(desc.subresourceRange.layerCount);
        // TODO: swizzle
        _canBeDestroyed     = desc.canBeDestroyed;

        _imageView          = desc.view;

        auto&   dev = resMngr.GetDevice();
        dev.SetObjectName( _imageView, dbgName, VK_OBJECT_TYPE_IMAGE_VIEW );

        DEBUG_ONLY( _debugName = dbgName; )
        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VImageView::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto&   dev = resMngr.GetDevice();

        if ( _canBeDestroyed and _imageView != Default )
            dev.vkDestroyImageView( dev.GetVkDevice(), _imageView, null );

        resMngr.ImmediatelyRelease( INOUT _imageId );

        _imageId    = Default;
        _imageView  = Default;
        _desc       = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }

} // AE::Graphics

#endif // AE_ENABLE_VULKAN

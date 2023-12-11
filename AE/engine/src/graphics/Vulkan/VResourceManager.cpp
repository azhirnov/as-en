// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/Vulkan/VEnumCast.h"

# include "graphics/Vulkan/Allocators/VUniMemAllocator.h"
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"

# include "graphics/Vulkan/Descriptors/VDefaultDescriptorAllocator.h"

namespace AE::Graphics
{
#   include "graphics/Private/ResourceManager.cpp.h"

/*
=================================================
    GetMemoryInfo
=================================================
*/
    bool  VResourceManager::GetMemoryInfo (ImageID id, OUT VulkanMemoryObjInfo &info) C_NE___
    {
        auto*   image = GetResource( id );
        CHECK_ERR( image != null );
        return GetMemoryInfo( image->MemoryId(), OUT info );
    }

    bool  VResourceManager::GetMemoryInfo (BufferID id, OUT VulkanMemoryObjInfo &info) C_NE___
    {
        auto*   buffer = GetResource( id );
        CHECK_ERR( buffer != null );
        return GetMemoryInfo( buffer->MemoryId(), OUT info );
    }

    bool  VResourceManager::GetMemoryInfo (MemoryID id, OUT VulkanMemoryObjInfo &info) C_NE___
    {
        auto*   mem = GetResource( id );
        CHECK_ERR( mem != null );
        return mem->GetMemoryInfo( OUT info );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    _DestroyResource
=================================================
*/
    template <>
    void  VResourceManager::_DestroyResource (VFramebufferID id) __NE___
    {
        Unused( _ImmediatelyReleaseResource( id ));
    }

    template <>
    void  VResourceManager::_DestroyResource (VkSwapchainKHR handle) __NE___
    {
        _device.vkDestroySwapchainKHR( _device.GetVkDevice(), handle, null );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    Create*
=================================================
*/
    Strong<MemoryID>  VResourceManager::CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateResource<MemoryID>( "failed when creating memory object", buffer, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
    }

    Strong<MemoryID>  VResourceManager::CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
    {
        return _CreateResource<MemoryID>( "failed when creating memory object", image, desc, _ChooseMemAllocator( RVRef(allocator) ), dbgName );
    }

    Strong<RenderPassID>  VResourceManager::CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, RenderPassID compatId, StringView dbgName) __NE___
    {
        return _CreateResource<RenderPassID>( "failed when creating render pass", *this, info, vkInfo, compatId, dbgName );
    }

/*
=================================================
    CreateDescriptorSets
=================================================
*/
    bool  VResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
                                                  PipelinePackID packId, const DSLayoutName &dslName,
                                                  DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        auto*   pack = GetResource( packId ? packId : _defaultPack.Get() );
        CHECK_ERR( pack );

        auto    layout_id = pack->GetDSLayout( dslName );
        CHECK_ERR( layout_id );

        return CreateDescriptorSets( OUT dst, count, layout_id, RVRef(allocator), dbgName );
    }

    bool  VResourceManager::CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
                                                  DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName) __NE___
    {
        CHECK_ERR( dst != null and count > 0 );
        CHECK_ERR( layoutId );

        allocator = _ChooseDescAllocator( RVRef(allocator) );

        usize   i       = 0;
        bool    created = true;

        for (; created & (i < count); ++i)
        {
            dst[i]  = _CreateResource<DescriptorSetID>( "failed when creating descriptor set", *this, layoutId, allocator, dbgName );
            created = (dst[i].IsValid());
        }

        if ( not created ) {
            for (usize j = 0; j < i; ++j) {
                ImmediatelyRelease( INOUT dst[j] );
            }
        }

        return created;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    CreateSampler
=================================================
*/
    Strong<SamplerID>  VResourceManager::CreateSampler (const VkSamplerCreateInfo &info, const VkSamplerYcbcrConversionCreateInfo* ycbcrInfo, StringView dbgName) __NE___
    {
        return _CreateResource<SamplerID>( "failed when creating sampler", *this, info, ycbcrInfo, dbgName );
    }

/*
=================================================
    GetVkSampler
=================================================
*/
    VkSampler  VResourceManager::GetVkSampler (const SamplerName &name) C_NE___
    {
        auto*   res = GetResource( GetSampler( name ));

        if_unlikely( res == null )
            res = GetResource( _defaultSampler );

        return res->Handle();
    }

/*
=================================================
    _CreateDefaultSampler
=================================================
*/
    bool  VResourceManager::_CreateDefaultSampler () __NE___
    {
        CHECK_ERR( _defaultSampler == Default );

        VkSamplerCreateInfo     info = {};
        info.sType                  = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.flags                  = 0;
        info.magFilter              = VK_FILTER_NEAREST;
        info.minFilter              = VK_FILTER_NEAREST;
        info.mipmapMode             = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        info.addressModeU           = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV           = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW           = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.mipLodBias             = 0.0f;
        info.anisotropyEnable       = VK_FALSE;
        info.maxAnisotropy          = 0.0f;
        info.compareEnable          = VK_FALSE;
        info.compareOp              = VK_COMPARE_OP_NEVER;
        info.minLod                 = 0.0f;
        info.maxLod                 = 0.0f;
        info.borderColor            = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        info.unnormalizedCoordinates= VK_FALSE;

        _defaultSampler = CreateSampler( info, null, "Default" );
        CHECK_ERR( _defaultSampler );

        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    CreateFramebuffer
=================================================
*/
    VFramebufferID  VResourceManager::CreateFramebuffer (const RenderPassDesc &desc) __NE___
    {
        RenderPassID    rp_id = GetCompatibleRenderPass( desc.packId, desc.renderPassName );
        CHECK_ERR( rp_id );

        VFramebuffer::Key   key{ StructView<ImageViewID>{ desc.attachments.GetValueArray(), &RenderPassDesc::Attachment::imageView },
                                 rp_id,
                                 uint3{ desc.area.Width(), desc.area.Height(), desc.layerCount.Get() }};

        // find in cache
        {
            std::shared_lock    rlock{ _resPool.fbCacheGuard };

            auto    iter = _resPool.fbCache.find( key );
            if_likely( iter != _resPool.fbCache.end() )
            {
                // increase ref counter to safely use fb in current frame
                auto    id = AcquireResource( iter->second );
                CHECK_ERR( id );
                rlock.unlock();

                // decrease ref counter when current frame is complete execution on the GPU side
                _DelayedReleaseResource2( iter->second );
                return id.Release();
            }
        }

        // create new framebuffer
        VFramebufferID  raw_id;
        CHECK_ERR( _Assign( OUT raw_id ));

        auto&   data = _GetResourcePool( raw_id )[ raw_id.Index() ];
        Replace( data );

        if_unlikely( not data.Create( *this, desc, rp_id  DEBUG_ONLY(, HashToName( desc.renderPassName )) ))
        {
            data.Destroy( *this );
            _Unassign( raw_id );
            RETURN_ERR( "failed when creating framebuffer from render pass '"s << HashToName( desc.renderPassName ) << "'" );
        }

        data.AddRef();  // 'raw_id' rc == 1


        // add to cache
        Strong<VFramebufferID>  actual_id;
        {
            EXLOCK( _resPool.fbCacheGuard );
            auto [iter, inserted] = _resPool.fbCache.emplace( key, raw_id );

            if_likely( inserted )
            {
                CHECK_ERR( data.Data().SetCachePtr( iter ));
                actual_id.Attach( raw_id );
            }
            else
            {
                actual_id = AcquireResource( iter->second );
                CHECK( actual_id );
            }
        }

        // framebuffer already added to cache
        if_unlikely( *actual_id != raw_id )
        {
            CHECK( _ImmediatelyReleaseResource( raw_id ) == 0 );
        }

        // decrease ref counter when current frame is complete execution on the GPU side
        _DelayedReleaseResource2( VFramebufferID{actual_id} );
        return actual_id.Release();
    }
//-----------------------------------------------------------------------------


/*
=================================================
    LoadPipelineCache
=================================================
*/
    Strong<PipelineCacheID>  VResourceManager::LoadPipelineCache (RC<RStream> stream) __NE___
    {
        PipelineCacheID     id;
        CHECK_ERR( _Assign( OUT id ));

        auto&   data = _GetResourcePool( id )[ id.Index() ];
        Replace( data );

        if_unlikely( not data.Create( *this, Default, RVRef(stream) ))
        {
            data.Destroy( *this );
            _Unassign( id );
            RETURN_ERR( "failed when loading pipeline cache" );
        }

        data.AddRef();
        return Strong<PipelineCacheID>{ id };
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _InitReleaseResourceByID
=================================================
*/
    struct VResourceManager::_InitReleaseResourceByID
    {
        VResourceManager&   resMngr;

        template <typename ID>
        static void  _Release (VResourceManager &resMngr, VExpiredResource::IDValue_t id) __NE___
        {
            resMngr._DestroyResource( UnsafeBitCast<ID>( id ));
        }

        template <typename ID, uint I>
        void operator () () __NE___
        {
            resMngr._releaseResIDs[I] = &_Release<ID>;
        }
    };

/*
=================================================
    _InitReleaseResourceByIDFns
=================================================
*/
    void  VResourceManager::_InitReleaseResourceByIDFns () __NE___
    {
        AllVkResources_t::Visit( _InitReleaseResourceByID{*this} );
    }

/*
=================================================
    ForceReleaseResources
=================================================
*/
    bool  VResourceManager::ForceReleaseResources () __NE___
    {
        bool    non_empty = false;

        for (auto& list : _expiredResources.All())
        {
            EXLOCK( list.guard );
            list.frameId = Default;

            non_empty |= not list.resources.empty();

            for (auto& res : list.resources)
            {
                _releaseResIDs[ res.type ]( *this, res.id );
            }
            list.resources.clear();

            // TODO: trim memory
        }

        return non_empty;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Create***Allocator
=================================================
*/
    GfxMemAllocatorPtr  VResourceManager::CreateLinearGfxMemAllocator (Bytes pageSize) C_NE___
    {
        return MakeRC<VLinearMemAllocator>( pageSize );
    }

    GfxMemAllocatorPtr  VResourceManager::CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize) C_NE___
    {
        return MakeRC<VBlockMemAllocator>( blockSize, pageSize );
    }

    GfxMemAllocatorPtr  VResourceManager::CreateUnifiedGfxMemAllocator (Bytes pageSize) C_NE___
    {
        return MakeRC<VUniMemAllocator>( pageSize );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    ExpiredResources ctor
=================================================
*/
    VResourceManager::VExpiredResources::VExpiredResources () __Th___
    {
        for (auto& item : _list) {
            item.resources.reserve( 1u << 10 );
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    VReleaseExpiredResourcesTask::Run
=================================================
*/
    void  VResourceManager::VReleaseExpiredResourcesTask::Run ()
    {
        auto&   res_mngr = GraphicsScheduler().GetResourceManager();
        auto&   list     = res_mngr._expiredResources.Get( _frameId );
        EXLOCK( list.guard );

        for (auto& res : list.resources)
        {
            res_mngr._releaseResIDs[ res.type ]( res_mngr, res.id );
        }
        list.resources.clear();
    }



} // AE::Graphics

#endif // AE_ENABLE_VULKAN

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Video/VVideoBuffer.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/Video/VVideoUtils.cpp.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VVideoBuffer::~VVideoBuffer () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _bufferId == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VVideoBuffer::Create (VResourceManager &resMngr, const VideoBufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _buffer == Default );
        CHECK_ERR( _bufferId == Default );
        CHECK_ERR( allocator );

        _desc = desc;
        GRES_CHECK( IsSupported( resMngr, _desc ));

        auto&   dev = resMngr.GetDevice();

        VkBufferCreateInfo          buffer_ci    = {};
        VkVideoProfileListInfoKHR   prof_list    = {};
        VkVideoProfileInfoKHR       profile_info = {};

        CHECK_ERR( WithVideoProfile( dev, _desc.profile,
            [this, &profile_info] (const VkVideoProfileInfoKHR &profileInfo, const VkVideoCapabilitiesKHR &capabilities) -> bool
            {
                profile_info    = profileInfo;
                _minOffsetAlign = POTBytes{ capabilities.minBitstreamBufferOffsetAlignment };
                _minSizeAlign   = POTBytes{ capabilities.minBitstreamBufferSizeAlignment };

                _desc.size      = AlignUp( _desc.size, _minSizeAlign );

                return true;
            }));

        prof_list.sType         = VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
        prof_list.profileCount  = 1;
        prof_list.pProfiles     = &profile_info;

        buffer_ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_ci.pNext = &prof_list;
        buffer_ci.flags = 0;
        buffer_ci.usage = VEnumCast( _desc.usage ) | VEnumCast( _desc.videoUsage );
        buffer_ci.size  = VkDeviceSize( _desc.size );

        if_unlikely( EMemoryType_IsNonCoherent( desc.memType ))
            buffer_ci.size = AlignUp( buffer_ci.size, dev.GetDeviceProperties().res.minNonCoherentAtomSize );

        VQueueFamilyIndices_t   queue_family_indices;

        // setup sharing mode
        if ( _desc.queues != Default )
        {
            dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

            buffer_ci.sharingMode           = VK_SHARING_MODE_CONCURRENT;
            buffer_ci.pQueueFamilyIndices   = queue_family_indices.data();
            buffer_ci.queueFamilyIndexCount = uint(queue_family_indices.size());
        }

        // reset to exclusive mode
        if ( buffer_ci.queueFamilyIndexCount <= 1 )
        {
            buffer_ci.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
            buffer_ci.pQueueFamilyIndices   = null;
            buffer_ci.queueFamilyIndexCount = 0;
        }

        VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buffer_ci, null, OUT &_buffer ));

        VulkanBufferDesc        vk_desc;
        vk_desc.buffer          = _buffer;
        vk_desc.usage           = VkBufferUsageFlagBits(buffer_ci.usage);
        vk_desc.size            = Bytes{buffer_ci.size};
        vk_desc.queues          = _desc.queues;
        vk_desc.memFlags        = VEnumCast( _desc.memType );
        vk_desc.canBeDestroyed  = false;
        vk_desc.allocMemory     = true;

        _bufferId = resMngr.CreateBuffer( vk_desc, dbgName );
        CHECK_ERR( _bufferId );

        DEBUG_ONLY( _debugName = dbgName; )
        return false;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VVideoBuffer::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        resMngr.ImmediatelyRelease( INOUT _bufferId );

        _buffer = Default;
        _desc   = Default;

        DEBUG_ONLY( _debugName.clear(); )
    }

/*
=================================================
    IsSupported
=================================================
*/
    bool  VVideoBuffer::IsSupported (const VResourceManager &, const VideoBufferDesc &) __NE___
    {
        // TODO
        return true;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

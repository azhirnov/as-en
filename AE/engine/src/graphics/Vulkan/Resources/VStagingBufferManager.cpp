// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/EnumToString.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Vulkan/Resources/VStagingBufferManager.h"
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{
#   include "graphics/Private/StagingBufferManager.cpp.h"


/*
=================================================
    AcquireMappedMemory
=================================================
*/
    void  VStagingBufferManager::AcquireMappedMemory (FrameUID frameId, VkDeviceMemory memory, Bytes offset, Bytes size) __NE___
    {
        ASSERT( _frameId.load() == frameId );

        if_unlikely( size == 0_b )
            return;

        ASSERT( memory != Default );

        VkMappedMemoryRange range;
        range.sType     = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range.pNext     = null;
        range.memory    = memory;
        range.offset    = VkDeviceSize(offset);
        range.size      = VkDeviceSize(size);

        EXLOCK( _memRanges.guard );
        _memRanges.ranges[ frameId.Index() ].push_back( range );
    }

/*
=================================================
    _CheckHostVisibleMemory
=================================================
*
    bool  VStagingBufferManager::_CheckHostVisibleMemory (OUT Bytes& totalHostMem) const
    {
        auto&   dev     = _resMngr.GetDevice();
        auto&   props   = dev.GetVProperties().memoryProperties;

        VkMemoryRequirements    mem_req = {};
        {
            VkBuffer            id;
            VkBufferCreateInfo  info = {};
            info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            info.flags          = 0;
            info.usage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            info.size           = 64 << 10;
            info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

            VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &info, null, OUT &id ));
            dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), id, OUT &mem_req );
            dev.vkDestroyBuffer( dev.GetVkDevice(), id, null );
        }

        BitSet<VK_MAX_MEMORY_HEAPS>     cached_heaps;
        BitSet<VK_MAX_MEMORY_HEAPS>     coherent_heaps;

        for (uint i = 0; i < props.memoryTypeCount; ++i)
        {
            auto&   mt = props.memoryTypes[i];

            if ( AllBits( mem_req.memoryTypeBits, 1u << i ))
            {
                if ( AllBits( mt.propertyFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT ))
                    cached_heaps[ mt.heapIndex ] = true;
                else
                if ( AllBits( mt.propertyFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
                    coherent_heaps[ mt.heapIndex ] = true;
            }
        }

        totalHostMem = 0_b;
        for (uint i = 0; i < props.memoryHeapCount; ++i)
        {
            if ( cached_heaps[i] or coherent_heaps[i] )
                totalHostMem += props.memoryHeaps[i].size;      // TOD: separate read/write ?
        }

        return true;
    }

/*
=================================================
    _CreateStaticBuffers
=================================================
*/
    bool  VStagingBufferManager::_CreateStaticBuffers (const GraphicsCreateInfo &info)
    {
        auto&       dev     = _resMngr.GetDevice();
        const auto  q_mask  = dev.GetAvailableQueues();

        _static.writeSize   = info.staging.writeStaticSize;
        _static.readSize    = info.staging.readStaticSize;

        _static.buffersForWrite.resize( info.maxFrames * _QueueCount );
        _static.buffersForRead.resize( info.maxFrames * _QueueCount );

        VkBufferCreateInfo      buf_info = {};
        buf_info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_info.flags          = 0;
        buf_info.usage          = VEnumCast( EBufferUsage::Transfer );
        buf_info.size           = 1 << 10;
        buf_info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

        VkMemoryRequirements    mem_req = {};
        {
            VkBuffer    buf = Default;
            VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_info, null, OUT &buf ));

            dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buf, &mem_req );
            dev.vkDestroyBuffer( dev.GetVkDevice(), buf, null );
        }

        Bytes   total_write_size;
        Bytes   total_read_size;
        void*   mapped_write_mem    = null;
        void*   mapped_read_mem     = null;
        uint    write_mem_idx       = UMax;
        uint    read_mem_idx        = UMax;
        {
            for (uint i = 0; i < _QueueCount; ++i)
            {
                if ( not AllBits( q_mask, EQueueMask(1u << i) ))
                    continue;

                total_write_size = AlignUp( total_write_size + _static.writeSize[i], mem_req.alignment );
                total_read_size  = AlignUp( total_read_size  + _static.readSize[i],  mem_req.alignment );
            }
            total_write_size *= info.maxFrames;
            total_read_size  *= info.maxFrames;

            VkMemoryAllocateInfo    alloc_info = {};
            alloc_info.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize   = VkDeviceSize( total_write_size );
            CHECK_ERR( dev.GetMemoryTypeIndex( mem_req.memoryTypeBits,
                                               NAMED_ARG( includeFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),  NAMED_ARG( optIncludeFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                               NAMED_ARG( excludeFlags, Zero ),                                 NAMED_ARG( optExcludeFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),
                                               OUT write_mem_idx ));
            alloc_info.memoryTypeIndex = write_mem_idx;

            VK_CHECK_ERR( dev.vkAllocateMemory( dev.GetVkDevice(), &alloc_info, null, OUT &_static.memoryForWrite ));
            VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), _static.memoryForWrite, 0, alloc_info.allocationSize, 0, OUT &mapped_write_mem ));

            // host to device memory must be coherent, because flush memory is not used
            auto    mem_flags_for_write = VkMemoryPropertyFlagBits( dev.GetVProperties().memoryProperties.memoryTypes[alloc_info.memoryTypeIndex].propertyFlags );
            CHECK_ERR( AllBits( mem_flags_for_write, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) or
                       AllBits( mem_flags_for_write, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ));

            alloc_info.allocationSize   = VkDeviceSize( total_read_size );
            CHECK_ERR( dev.GetMemoryTypeIndex( mem_req.memoryTypeBits,
                                               NAMED_ARG( includeFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ),  NAMED_ARG( optIncludeFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT ),
                                               NAMED_ARG( excludeFlags, Zero ),                                 NAMED_ARG( optExcludeFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),
                                               OUT read_mem_idx ));
            alloc_info.memoryTypeIndex = read_mem_idx;

            VK_CHECK_ERR( dev.vkAllocateMemory( dev.GetVkDevice(), &alloc_info, null, OUT &_static.memoryForRead ));
            VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), _static.memoryForRead, 0, alloc_info.allocationSize, 0, OUT &mapped_read_mem ));
            _static.memoryFlagsForRead = VkMemoryPropertyFlagBits( dev.GetVProperties().memoryProperties.memoryTypes[alloc_info.memoryTypeIndex].propertyFlags );
        }

        if ( total_write_size == 0_b and total_read_size == 0_b )
            return true;

        const auto  CreateStaticBuffer = [this, &dev, &buf_info, &mem_req, q_mask]
                                         (OUT StaticBuffer& sb, INOUT Bytes& offset, Bytes size, VkDeviceMemory mem, void* mapped, usize idx, uint memIdx, const char* name) -> bool
        {{
            const auto  q_idx = (idx % _QueueCount);

            if ( not AllBits( q_mask, EQueueMask(1u << q_idx) ) or size == 0 )
                return true;    // skip

            VkBuffer    buf = Default;
            buf_info.size = VkDeviceSize(size);
            VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_info, null, OUT &buf ));

            offset = AlignUp( offset, mem_req.alignment );
            VK_CHECK_ERR( dev.vkBindBufferMemory( dev.GetVkDevice(), buf, mem, VkDeviceSize(offset) ));

            VulkanBufferDesc    desc;
            desc.buffer         = buf;
            desc.size           = size;
            desc.usage          = VkBufferUsageFlagBits( buf_info.usage );
            desc.memFlags       = VkMemoryPropertyFlagBits( dev.GetVProperties().memoryProperties.memoryTypes[memIdx].propertyFlags );
            desc.canBeDestroyed = true;

            sb.buffer = _resMngr.CreateBuffer( desc, String{name} << " {f:" << ToString(idx / _QueueCount) << "} {q:" << ToString(EQueueType(q_idx)) << "}" );
            CHECK_ERR( sb.buffer );

            sb.size.store( 0_b );
            sb.capacity     = size;
            sb.memOffset    = offset;
            sb.bufferHandle = _resMngr.GetResource( sb.buffer )->Handle();
            sb.mapped       = mapped + offset;

            offset += size;
            return true;
        }};

        Bytes   write_mem_offset;
        Bytes   read_mem_offset;

        for (usize i = 0; i < _static.buffersForWrite.size(); ++i)
        {
            if ( total_write_size > 0 )
                CHECK_ERR( CreateStaticBuffer( OUT _static.buffersForWrite[i], INOUT write_mem_offset, _static.writeSize[i % _QueueCount], _static.memoryForWrite, mapped_write_mem, i, write_mem_idx, "SSWB" ));

            if ( total_read_size > 0 )
                CHECK_ERR( CreateStaticBuffer( OUT _static.buffersForRead[i],  INOUT read_mem_offset,  _static.readSize[i % _QueueCount],  _static.memoryForRead,  mapped_read_mem,  i, read_mem_idx, "SSRB" ));
        }

        ASSERT( total_write_size == write_mem_offset );
        ASSERT( total_read_size  == read_mem_offset  );

        return true;
    }

/*
=================================================
    _InitVertexStream
=================================================
*/
    bool  VStagingBufferManager::_InitVertexStream (const GraphicsCreateInfo &info)
    {
        if ( info.staging.vstreamSize == 0 )
            return true;

        auto&   dev = _resMngr.GetDevice();

        VkBufferCreateInfo      buf_info = {};
        buf_info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buf_info.flags          = 0;
        buf_info.usage          = VEnumCast( EBufferUsage::Vertex | EBufferUsage::Index );
        buf_info.size           = 1 << 10;
        buf_info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

        VkMemoryRequirements    mem_req = {};
        {
            VkBuffer    buf = Default;
            VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_info, null, OUT &buf ));

            dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buf, &mem_req );
            dev.vkDestroyBuffer( dev.GetVkDevice(), buf, null );
        }

        const Bytes             per_frame_size  = AlignUp( info.staging.vstreamSize, Bytes{mem_req.alignment} );
        VkMemoryAllocateInfo    alloc_info      = {};
        alloc_info.sType            = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize   = VkDeviceSize( per_frame_size * info.maxFrames );

        // search for unified memory
        if ( not dev.GetMemoryTypeIndex( mem_req.memoryTypeBits,
                                         NAMED_ARG( includeFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),
                                         NAMED_ARG( optIncludeFlags, Zero ), NAMED_ARG( excludeFlags, Zero ), NAMED_ARG( optExcludeFlags, Zero ),
                                         OUT alloc_info.memoryTypeIndex ))
        {
            // search for host coherent memory
            CHECK_ERR( dev.GetMemoryTypeIndex( mem_req.memoryTypeBits,
                                               NAMED_ARG( includeFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ),   NAMED_ARG( optIncludeFlags, Zero ),
                                               NAMED_ARG( excludeFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ),                                          NAMED_ARG( optExcludeFlags, Zero ),
                                               OUT alloc_info.memoryTypeIndex ));
        }
        VK_CHECK_ERR( dev.vkAllocateMemory( dev.GetVkDevice(), &alloc_info, null, OUT &_vstream.memory ));

        void*   mapped = null;
        VK_CHECK_ERR( dev.vkMapMemory( dev.GetVkDevice(), _vstream.memory, 0, alloc_info.allocationSize, 0, OUT &mapped ));

        const auto  prop_flags = dev.GetVProperties().memoryProperties.memoryTypes[ alloc_info.memoryTypeIndex ].propertyFlags;
        Bytes       offset;

        for (uint i = 0; i < info.maxFrames; ++i)
        {
            auto&       vb  = _vstream.buffers[i];
            VkBuffer    buf = Default;

            buf_info.size = VkDeviceSize(per_frame_size);
            VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_info, null, OUT &buf ));

            VK_CHECK_ERR( dev.vkBindBufferMemory( dev.GetVkDevice(), buf, _vstream.memory, VkDeviceSize(offset) ));

            VulkanBufferDesc    desc;
            desc.buffer         = buf;
            desc.size           = per_frame_size;
            desc.usage          = VkBufferUsageFlagBits( buf_info.usage );
            desc.memFlags       = VkMemoryPropertyFlagBits( prop_flags );
            desc.canBeDestroyed = true;

            vb.buffer = _resMngr.CreateBuffer( desc, "vstream" );
            CHECK_ERR( vb.buffer );

            vb.size.store( 0_b );
            vb.capacity     = per_frame_size;
            vb.memOffset    = offset;
            vb.bufferHandle = _resMngr.GetResource( vb.buffer )->Handle();
            vb.mapped       = mapped + offset;

            offset += per_frame_size;
        }
        return true;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

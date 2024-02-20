// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Private/EnumToString.h"
# include "graphics/Private/EnumUtils.h"
# include "graphics/Vulkan/Resources/VStagingBufferManager.h"
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
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

        ASSERT( IsMultipleOf( size, _memSizeAlign ));
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
    _CreateStaticBuffers
=================================================
*/
    bool  VStagingBufferManager::_CreateStaticBuffers (const GraphicsCreateInfo &info)
    {
        auto&       dev         = _resMngr.GetDevice();
        auto&       mem_types   = dev.GetResourceFlags().memTypes;
        const auto  q_mask      = dev.GetAvailableQueues();

        _static.writeSize   = info.staging.writeStaticSize;
        _static.readSize    = info.staging.readStaticSize;

        _static.buffersForWrite.resize( info.maxFrames * _QueueCount );
        _static.buffersForRead.resize( info.maxFrames * _QueueCount );

        Bytes   mem_align;
        {
            VkBufferCreateInfo      buf_info = {};
            buf_info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buf_info.flags          = 0;
            buf_info.usage          = VEnumCast( EBufferUsage::Transfer );
            buf_info.size           = 1 << 10;
            buf_info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

            VkMemoryRequirements2   mem_req2 = {};
            mem_req2.sType  = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

            CHECK_ERR( dev.GetMemoryRequirements( buf_info, OUT &mem_req2 ));

            mem_align = Bytes{mem_req2.memoryRequirements.alignment};
        }

        Bytes   total_write_size;
        Bytes   total_read_size;

        for (uint i = 0; i < _QueueCount; ++i)
        {
            if ( not AllBits( q_mask, EQueueMask(1u << i) ))
                continue;

            total_write_size = AlignUp( total_write_size + _static.writeSize[i], mem_align );
            total_read_size  = AlignUp( total_read_size  + _static.readSize[i],  mem_align );
        }
        total_write_size *= info.maxFrames;
        total_read_size  *= info.maxFrames;

        if ( total_write_size == 0_b and total_read_size == 0_b )
            return true;

        auto        w_allocator = MakeRC<VLinearMemAllocator>( total_write_size );
        auto        r_allocator = MakeRC<VLinearMemAllocator>( total_read_size );
        EMemoryType mem_type;

        const auto  CreateStaticBuffer = [this, q_mask, &mem_type]
                                         (OUT StaticBuffer& sb, INOUT VkDeviceMemory &mem, Bytes size, usize idx, GfxMemAllocatorPtr alloc, const char* name) -> bool
        {{
            const auto  q_idx = (idx % _QueueCount);

            if ( not AllBits( q_mask, EQueueMask(1u << q_idx) ) or size == 0 )
                return true;    // skip

            BufferDesc  desc;
            desc.usage      = EBufferUsage::Transfer;
            desc.size       = size;
            desc.memType    = mem_type;

            String  dbg_name;
            DEBUG_ONLY( dbg_name = String{name} << " {f:" << ToString(idx / _QueueCount) << "} {q:" << ToString(EQueueType(q_idx)) << "}";)
            Unused( name );

            sb.buffer = _resMngr.CreateBuffer( desc, dbg_name, RVRef(alloc) );
            CHECK_ERR( sb.buffer );

            auto*   buf = _resMngr.GetResource( sb.buffer );

            VulkanMemoryObjInfo mem_info;
            CHECK_ERR( _resMngr.GetMemoryInfo( buf->MemoryId(), OUT mem_info ));

            sb.size.store( 0_b );
            sb.capacity     = size;
            sb.memOffset    = mem_info.offset;
            sb.bufferHandle = buf->Handle();
            sb.mapped       = mem_info.mappedPtr;

            if ( mem == Default )
                mem = mem_info.memory;
            else
                CHECK( mem == mem_info.memory );

            return true;
        }};

        if ( total_write_size > 0 )
        {
            // host to device memory must be coherent, because memory flush is not used
            mem_type = EMemoryType::HostCoherent;
            CHECK( mem_types.contains( EMemoryType::HostCoherent ));

            for (usize i = 0; i < _static.buffersForWrite.size(); ++i)
                CHECK_ERR( CreateStaticBuffer( OUT _static.buffersForWrite[i], INOUT _static.memoryForWrite, _static.writeSize[i % _QueueCount], i, w_allocator, "SSWB" ));
        }

        if ( total_read_size > 0 )
        {
            if ( mem_types.contains( EMemoryType::HostCached ))
                mem_type = EMemoryType::HostCached; // better performance for read access on host
            else
            if ( mem_types.contains( EMemoryType::HostCachedCoherent ))
                mem_type = EMemoryType::HostCachedCoherent;
            else
                mem_type = EMemoryType::HostCoherent;

            for (usize i = 0; i < _static.buffersForWrite.size(); ++i)
                CHECK_ERR( CreateStaticBuffer( OUT _static.buffersForRead[i],  INOUT _static.memoryForRead,  _static.readSize[i % _QueueCount],  i, r_allocator, "SSRB" ));
        }

        CHECK( w_allocator->GetStatistic().pageCount == 1 );
        CHECK( r_allocator->GetStatistic().pageCount == 1 );

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

        auto&       dev     = _resMngr.GetDevice();
        BufferDesc  desc;
        desc.usage  = EBufferUsage::Vertex | EBufferUsage::Index;

        if ( dev.GetResourceFlags().memTypes.contains( EMemoryType::Unified ))
            desc.memType = EMemoryType::Unified;    // better performance for read access on device
        else
            desc.memType = EMemoryType::HostCoherent;

        {
            VkBufferCreateInfo      buf_info = {};
            buf_info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buf_info.flags          = 0;
            buf_info.usage          = VEnumCast( desc.usage );
            buf_info.size           = 1 << 10;
            buf_info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

            VkMemoryRequirements2   mem_req2 = {};
            mem_req2.sType  = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;

            CHECK_ERR( dev.GetMemoryRequirements( buf_info, OUT &mem_req2 ));

            desc.size = AlignUp( info.staging.vstreamSize, Bytes{mem_req2.memoryRequirements.alignment} );
        }

        auto    allocator = MakeRC<VLinearMemAllocator>( desc.size * info.maxFrames );

        for (uint i = 0; i < info.maxFrames; ++i)
        {
            auto&   vb  = _vstream.buffers[i];

            vb.buffer = _resMngr.CreateBuffer( desc, "vstream", allocator );
            CHECK_ERR( vb.buffer );

            auto*   buf = _resMngr.GetResource( vb.buffer );

            VulkanMemoryObjInfo mem_info;
            CHECK_ERR( _resMngr.GetMemoryInfo( buf->MemoryId(), OUT mem_info ));

            vb.size.store( 0_b );
            vb.capacity     = desc.size;
            vb.memOffset    = mem_info.offset;
            vb.bufferHandle = buf->Handle();
            vb.mapped       = mem_info.mappedPtr;
        }

        CHECK( allocator->GetStatistic().pageCount == 1 );
        return true;
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

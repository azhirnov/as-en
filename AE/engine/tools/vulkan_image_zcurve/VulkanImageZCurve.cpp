// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Private/EnumUtils.h"
#include "graphics/Private/EnumToString.h"

#include "graphics/Vulkan/VDevice.h"
#include "graphics/Vulkan/VEnumCast.h"

using namespace AE;
using namespace AE::Graphics;

template <typename T>
static void  PrintImageZCurve (const VDevice &dev, EPixelFormat fmt, bool printSrcPattern = false)
{
    // create command pool and buffer
    VQueuePtr       vqueue      = dev.GetQueue( EQueueType::Graphics );
    VkQueue         cmd_queue   = vqueue->handle;
    VkCommandPool   cmd_pool    = VK_NULL_HANDLE;
    VkCommandBuffer cmd_buffer  = VK_NULL_HANDLE;
    {
        VkCommandPoolCreateInfo     pool_info = {};
        pool_info.sType             = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex  = uint(vqueue->familyIndex);
        pool_info.flags             = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        VK_CHECK( dev.vkCreateCommandPool( dev.GetVkDevice(), &pool_info, null, OUT &cmd_pool ));

        VkCommandBufferAllocateInfo info = {};
        info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        info.pNext              = null;
        info.commandPool        = cmd_pool;
        info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        info.commandBufferCount = 1;
        VK_CHECK( dev.vkAllocateCommandBuffers( dev.GetVkDevice(), &info, OUT &cmd_buffer ));
    }

    // check format
    const auto&     fmt_info        = EPixelFormat_GetInfo( fmt );
    CHECK_ERRV( not fmt_info.IsCompressed() );
    CHECK_ERRV( fmt_info.BytesPerPixel() == SizeOf<T> );
    STATIC_ASSERT( IsUnsignedInteger<T> );
    STATIC_ASSERT( sizeof(T) >= sizeof(ushort) );

    // create staging buffer
    VkBuffer        staging_buffer  = VK_NULL_HANDLE;
    VkDeviceMemory  staging_memory  = VK_NULL_HANDLE;
    T *             mapped          = null;
    const uint2     img_dim         { 64, 64 };
    const uint      pixel_count     = Area( img_dim );
    {
        VkBufferCreateInfo  info = {};
        info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.flags          = 0;
        info.size           = pixel_count * sizeof(*mapped);
        info.usage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK( dev.vkCreateBuffer( dev.GetVkDevice(), &info, null, OUT &staging_buffer ));

        VkMemoryRequirements    mem_req;
        dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), staging_buffer, OUT &mem_req );

        VkMemoryAllocateInfo    alloc = {};
        alloc.sType             = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc.allocationSize    = mem_req.size;
        CHECK_ERRV( dev.GetMemoryTypeIndex( mem_req.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Zero,   // include
                                            Zero, Zero,                                                                         // exclude
                                            OUT alloc.memoryTypeIndex ));

        VK_CHECK( dev.vkAllocateMemory( dev.GetVkDevice(), &alloc, null, OUT &staging_memory ));

        VK_CHECK( dev.vkBindBufferMemory( dev.GetVkDevice(), staging_buffer, staging_memory, 0 ));

        VK_CHECK( dev.vkMapMemory( dev.GetVkDevice(), staging_memory, 0, info.size, 0, OUT BitCast<void**>(&mapped) ));

        for (uint y = 0, i = 0; y < img_dim.y; ++y)
        for (uint x = 0; x < img_dim.x; ++x, ++i)
        {
            T   val = CheckCast<T>( x | (y << 8) ); // 16bit

            if constexpr( sizeof(T) >= 4 )
                val = CheckCast<T>( (~((x << 8) | y) << 16) | val );    // 32bit

            CHECK( VecToLinear( uint2{x,y}, img_dim ) == i );

            mapped[i] = val;
        }
    }


    VkImage         image           = VK_NULL_HANDLE;
    VkBuffer        buffer          = VK_NULL_HANDLE;
    VkDeviceMemory  shared_mem      = VK_NULL_HANDLE;
    VkDeviceSize    total_size      = 0;
    uint            mem_type_bits   = 0;

    // create image
    {
        VkImageCreateInfo   info = {};
        info.sType          = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.flags          = 0;
        info.imageType      = VK_IMAGE_TYPE_2D;
        info.format         = VEnumCast( fmt );
        info.extent         = { img_dim.x, img_dim.y, 1 };
        info.mipLevels      = 1;
        info.arrayLayers    = 1;
        info.samples        = VK_SAMPLE_COUNT_1_BIT;
        info.tiling         = VK_IMAGE_TILING_OPTIMAL;
        info.usage          = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;

        VK_CHECK( dev.vkCreateImage( dev.GetVkDevice(), &info, null, OUT &image ));

        VkMemoryRequirements    mem_req;
        dev.vkGetImageMemoryRequirements( dev.GetVkDevice(), image, OUT &mem_req );

        total_size       = mem_req.size;
        mem_type_bits   |= mem_req.memoryTypeBits;
    }

    // create buffer
    {
        VkBufferCreateInfo  info = {};
        info.sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        info.flags          = 0;
        info.size           = pixel_count * sizeof(*mapped);
        info.usage          = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        info.sharingMode    = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK( dev.vkCreateBuffer( dev.GetVkDevice(), &info, null, OUT &buffer ));

        VkMemoryRequirements    mem_req;
        dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buffer, OUT &mem_req );

        total_size      = Max( total_size, mem_req.size );
        mem_type_bits   |= mem_req.memoryTypeBits;
    }

    // allocate memory
    {
        VkMemoryAllocateInfo    info = {};
        info.sType              = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        info.allocationSize     = total_size;
        CHECK_ERRV( dev.GetMemoryTypeIndex( mem_type_bits,
                                            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Zero,
                                            Zero, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                            OUT info.memoryTypeIndex ));

        VK_CHECK( dev.vkAllocateMemory( dev.GetVkDevice(), &info, null, OUT &shared_mem ));

        VK_CHECK( dev.vkBindImageMemory( dev.GetVkDevice(), image, shared_mem, 0 ));
        VK_CHECK( dev.vkBindBufferMemory( dev.GetVkDevice(), buffer, shared_mem, 0 ));
    }


    // update resources
    {
        VkCommandBufferBeginInfo    begin_info = {};
        begin_info.sType    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        VK_CHECK( dev.vkBeginCommandBuffer( cmd_buffer, &begin_info ));

        // undefined -> transfer_dst
        VkImageMemoryBarrier    barrier = {};
        barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image               = image;
        barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.srcAccessMask       = 0;
        barrier.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        dev.vkCmdPipelineBarrier( cmd_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, null, 0, null, 1, &barrier );

        VkBufferImageCopy   img_copy = {};
        img_copy.bufferOffset       = 0;
        img_copy.bufferRowLength    = img_dim.x;
        img_copy.bufferImageHeight  = img_dim.y;
        img_copy.imageSubresource   = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        img_copy.imageOffset        = { 0, 0, 0 };
        img_copy.imageExtent        = { img_dim.x, img_dim.y, 1 };
        dev.vkCmdCopyBufferToImage( cmd_buffer, staging_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &img_copy );

        // read after write
        barrier.oldLayout       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout       = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.srcAccessMask   = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dev.vkCmdPipelineBarrier( cmd_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, null, 0, null, 1, &barrier );

        VkBufferCopy    buf_copy = {};
        buf_copy.srcOffset  = 0;
        buf_copy.dstOffset  = 0;
        buf_copy.size       = pixel_count * sizeof(*mapped);
        dev.vkCmdCopyBuffer( cmd_buffer, buffer, staging_buffer, 1, &buf_copy );

        VK_CHECK( dev.vkEndCommandBuffer( cmd_buffer ));

        VkSubmitInfo        submit_info = {};
        submit_info.sType               = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount  = 1;
        submit_info.pCommandBuffers     = &cmd_buffer;
        VK_CHECK( dev.vkQueueSubmit( cmd_queue, 1, &submit_info, VK_NULL_HANDLE ));
    }
    VK_CHECK( dev.vkQueueWaitIdle( cmd_queue ));


    // find block size
    uint2   tile_size {0};
    {
        enum class E : uint { _Count    = 64 * 64 };
        EnumBitSet<E>   bits;
        CHECK_ERRV( bits.size() == pixel_count );

        for (uint i = 0; i < pixel_count; ++i)
        {
            uint2   coord = (uint2(mapped[i], mapped[i] >> 8) & 0xFFu);

            bits.insert( E( coord.x + coord.y * img_dim.x ));

            if constexpr( sizeof(T) >= 4 )
            {
                uint2   coord2 = (~uint2(mapped[i] >> 24, mapped[i] >> 16)) & 0xFFu;
                CHECK( All( coord == coord2 ));
            }

            if ( tile_size.y > 1                        and
                 tile_size.x == tile_size.y             and
                 (tile_size.x+1) * (tile_size.y+1) == i )
            {
                bool    all_bits_set = true;
                for (uint y = 0; y < tile_size.y; ++y) {
                    all_bits_set &= bits.AllInRange( E(y * img_dim.x), E(y * img_dim.x + tile_size.x) );
                }
                if ( all_bits_set )
                    break;
            }
            tile_size = Max( tile_size, coord );
        }
        tile_size += 1;
    }

    // get pattern
    const uint2 max_coord   = Max( tile_size.x, tile_size.y ) < 8 ? tile_size * 2 : tile_size;
    const uint  block_size  = max_coord.x * max_coord.y;

    //if ( block_size != pixel_count )
    {
        String  str;
        str << "\nDevice: '" << dev.GetDeviceName() << "'";
        str << "\nImage with format: "s << ToString(fmt) << ", tile size: " << ToString( tile_size ) << ", z-order pattern:\n";

        const uint2 align = uint2{IntLog10(max_coord.x), IntLog10(max_coord.y)} + 1u;

        for (uint i = 0; i < block_size and i < pixel_count; ++i)
        {
            uint2   coord = (uint2(mapped[i], mapped[i] >> 8) & 0xFFu);

            str << '[' << FormatAlignedI<10>( coord.x, align.x, ' ' ) << ','
                << FormatAlignedI<10>( coord.y, align.y, ' ' ) << ']'
                << (i and ((i+1) % max_coord.x == 0) ? '\n' : ' ');
        }

        if ( printSrcPattern )
        {
            str << "Without reordering:\n";

            for (uint y = 0; y < max_coord.y; ++y)
            {
                for (uint x = 0; x < max_coord.x; ++x)
                {
                    uint2   coord { x, y };

                    str << '[' << FormatAlignedI<10>( coord.x, align.x, ' ' ) << ','
                        << FormatAlignedI<10>( coord.y, align.y, ' ' ) << "] ";
                }
                str << "\n";
            }
        }

        AE_LOGI( str );
    }


    // destroy resources
    dev.vkUnmapMemory( dev.GetVkDevice(), staging_memory );
    dev.vkFreeMemory( dev.GetVkDevice(), staging_memory, null );
    dev.vkFreeMemory( dev.GetVkDevice(), shared_mem, null );
    dev.vkDestroyImage( dev.GetVkDevice(), image, null );
    dev.vkDestroyBuffer( dev.GetVkDevice(), buffer, null );
    dev.vkDestroyBuffer( dev.GetVkDevice(), staging_buffer, null );
    dev.vkFreeCommandBuffers( dev.GetVkDevice(), cmd_pool, 1, &cmd_buffer );
    dev.vkDestroyCommandPool( dev.GetVkDevice(), cmd_pool, null );
}


int  main ()
{
    AE::Base::StaticLogger::LoggerDbgScope  log{};

    VDeviceInitializer  dev {False{"no log"}};

    VDeviceInitializer::InstanceCreateInfo  inst_ci;
    inst_ci.appName         = "VulkanImageZCurve";
    inst_ci.instanceLayers  = dev.GetRecommendedInstanceLayers();
    CHECK_FATAL( dev.CreateInstance( inst_ci ));

    CHECK_FATAL( dev.ChooseHighPerformanceDevice() );
    CHECK_FATAL( dev.CreateDefaultQueue() );
    CHECK_FATAL( dev.CreateLogicalDevice() );

    PrintImageZCurve<uint>( dev, EPixelFormat::RGBA8_UNorm, true );
    PrintImageZCurve<uint>( dev, EPixelFormat::RG16F );

    PrintImageZCurve<ushort>( dev, EPixelFormat::RG8_UNorm );

    PrintImageZCurve<ulong>( dev, EPixelFormat::RGBA16F );

    CHECK_FATAL( dev.DestroyLogicalDevice() );
    CHECK_FATAL( dev.DestroyInstance() );
    return 0;
}

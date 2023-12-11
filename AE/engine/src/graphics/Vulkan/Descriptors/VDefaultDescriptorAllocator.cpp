// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Descriptors/VDefaultDescriptorAllocator.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{
namespace
{
    static constexpr uint   c_MaxPoolAllocFails     = 4;
    static constexpr uint   c_MaxDescriptorPoolSize = 1u << 11;
    static constexpr uint   c_MaxDescriptorSets     = 1u << 10;
    static constexpr uint   c_MaxChunks             = 1024;
    static constexpr uint   c_MaxPoolAllocPerDS     = 1;
    static constexpr uint   c_MaxDSAllocFails       = 30;
}

/*
=================================================
    destructor
=================================================
*/
    VDefaultDescriptorAllocator::~VDefaultDescriptorAllocator () __NE___
    {
        VDevice const&  dev     = GraphicsScheduler().GetDevice();
        Chunk*          chunk   = &_firstChunk;

        for (; chunk != null;)
        {
            for (auto& pool : chunk->pools)
            {
                DeferExLock lock{ pool.guard };
                CHECK( lock.try_lock() );

                if ( pool.handle != Default )
                {
                    dev.vkDestroyDescriptorPool( dev.GetVkDevice(), pool.handle, null );
                    pool.handle = Default;
                }
            }

            auto*   prev = chunk;
            chunk   = chunk->next.exchange( null );

            if ( prev != &_firstChunk )
                delete prev;
        }
    }

/*
=================================================
    Allocate
=================================================
*/
    bool  VDefaultDescriptorAllocator::Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds) __NE___
    {
        const auto&     res_mngr    = GraphicsScheduler().GetResourceManager();
        VDevice const&  dev         = res_mngr.GetDevice();
        const auto*     ds_layout   = res_mngr.GetResource( layoutId );
        CHECK_ERR( ds_layout != null );

        const VkDescriptorSetLayout layout = ds_layout->Handle();

        VkDescriptorSetAllocateInfo     info = {};
        info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorSetCount = 1;
        info.pSetLayouts        = &layout;

        // search in chunks
        Chunk*  chunk           = &_firstChunk;
        auto*   next            = &chunk->next;
        uint    new_pool_count  = 0;
        uint    fail_count      = 0;

        for (uint chunk_idx = 0; chunk_idx < c_MaxChunks; ++chunk_idx)
        {
            ASSERT( chunk != null );
            next = &chunk->next;

            PoolBits_t  bits;
            for (; not bits.all();)
            {
                for (usize i = 0; i < chunk->pools.size(); ++i)
                {
                    if ( bits.test( i ))
                        continue;

                    auto&   pool = chunk->pools[i];

                    if_unlikely( pool.allocFails.load() > c_MaxPoolAllocFails )
                    {
                        bits.set( i );
                        continue;
                    }

                    DeferExLock lock{ pool.guard };
                    if_unlikely( not lock.try_lock() )
                    {
                        ThreadUtils::Pause();
                        continue;
                    }

                    bits.set( i );

                    if_unlikely( pool.handle == Default )
                    {
                        CHECK_ERR( new_pool_count < c_MaxPoolAllocPerDS );
                        CHECK_ERR( _CreateDSPool( dev, c_MaxDescriptorPoolSize, c_MaxDescriptorSets, OUT pool.handle ));
                        ++new_pool_count;
                    }

                    info.descriptorPool = pool.handle;

                    auto    err = dev.vkAllocateDescriptorSets( dev.GetVkDevice(), &info, OUT &ds.handle );
                    switch ( err )
                    {
                        case_likely VK_SUCCESS :
                            ds.data.Ref<uint>() = chunk_idx * PoolsPerChunk + uint(i);
                            return true;

                        case VK_ERROR_OUT_OF_POOL_MEMORY :
                            fail_count += 10u - Min( pool.allocFails.fetch_add(1), 10u );   // 0 fails -> +10
                            break;

                        default :
                            VK_CHECK( err );    break;
                    }
                }
            }

            CHECK_ERR_MSG( fail_count < c_MaxDSAllocFails, "too mush attempts to allocate DS" );

            chunk = next->load();
            if_likely( chunk != null )
                continue;

            // allocate new chunk
            auto*   new_chunk = new Chunk{};

            if_likely( next->CAS_Loop( INOUT chunk, new_chunk ))
                chunk = new_chunk;
            else
                delete new_chunk;
        }
        return false;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VDefaultDescriptorAllocator::Deallocate (DescriptorSetLayoutID, INOUT Storage &ds) __NE___
    {
        CHECK_ERRV( ds.handle != Default );

        auto&       dev         = GraphicsScheduler().GetDevice();
        Chunk*      chunk       = &_firstChunk;
        const uint  chunk_idx   = ds.data.Ref<uint>() / PoolsPerChunk;

        for (uint c = 0; (c < c_MaxChunks) & (chunk != null); ++c)
        {
            if_likely( c != chunk_idx )
            {
                chunk = chunk->next.load();
                continue;
            }

            auto&   pool = chunk->pools[ ds.data.Ref<uint>() % PoolsPerChunk ];
            EXLOCK( pool.guard );

            VK_CHECK( dev.vkFreeDescriptorSets( dev.GetVkDevice(), pool.handle, 1, &ds.handle ));   // TODO: delayed destroy

            pool.allocFails.store( 0 ); // allow to try allocate
            ds.handle = Default;
            return;
        }

        ASSERT( chunk == null );    // search is not complete
        DBG_WARNING( "failed to deallocate" );
    }

/*
=================================================
    _CreateDSPool
=================================================
*/
    bool  VDefaultDescriptorAllocator::_CreateDSPool (const VDevice &dev, uint descCount, uint maxDS, OUT VkDescriptorPool &dsPool)
    {
        const auto&     desc_types = dev.GetResourceFlags().descrTypes;

        FixedMap< VkDescriptorType, VkDescriptorPoolSize, 32 >  pool_sizes;

        for (uint t = 0; t < uint(EDescriptorType::_Count); ++t)
        {
            if ( not desc_types.contains( EDescriptorType(t) ))
                continue;

            VkDescriptorPoolSize    size;
            size.descriptorCount    = descCount;

            BEGIN_ENUM_CHECKS();
            switch ( EDescriptorType(t) )
            {
                case EDescriptorType::UniformBuffer :
                    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
                    pool_sizes.emplace( size.type, size );

                    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    size.descriptorCount *= 4;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::StorageBuffer :
                    size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
                    pool_sizes.emplace( size.type, size );

                    size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    size.descriptorCount *= 4;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::UniformTexelBuffer :
                    size.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::StorageTexelBuffer :
                    size.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::StorageImage :
                    size.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                    size.descriptorCount *= 2;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::SampledImage :
                    size.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    size.descriptorCount *= 4;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::CombinedImage :
                    size.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    size.descriptorCount *= 4;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::SubpassInput :
                    size.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                    size.descriptorCount /= 2;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::Sampler :
                    size.type = VK_DESCRIPTOR_TYPE_SAMPLER;
                    size.descriptorCount /= 2;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::RayTracingScene :
                    size.type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
                    size.descriptorCount /= 2;
                    pool_sizes.emplace( size.type, size );
                    break;

                case EDescriptorType::CombinedImage_ImmutableSampler :
                case EDescriptorType::ImmutableSampler :
                case EDescriptorType::_Count :
                case EDescriptorType::Unknown :     break;
            }
            END_ENUM_CHECKS();
        }

        VkDescriptorPoolCreateInfo  info = {};
        info.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.poolSizeCount  = uint(pool_sizes.size());
        info.pPoolSizes     = pool_sizes.GetValueArray().data();
        info.maxSets        = maxDS;
        info.flags          = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VK_CHECK_ERR( dev.vkCreateDescriptorPool( dev.GetVkDevice(), &info, null, OUT &dsPool ));
        return true;
    }

} // AE::Graphics

#endif // AE_ENABLE_VULKAN

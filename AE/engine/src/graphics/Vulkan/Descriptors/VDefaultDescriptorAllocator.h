// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/DescriptorSet.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

    //
    // Vulkan Default Descriptor Allocator
    //

    class VDefaultDescriptorAllocator final : public IDescriptorAllocator
    {
    // types
    private:
        static constexpr uint   PoolsPerChunk   = 8;

        struct DSPool
        {
            SpinLock            guard;
            Atomic<uint>        allocFails  {0};
            VkDescriptorPool    handle      = Default;
        };

        struct alignas(AE_CACHE_LINE) Chunk
        {
            StaticArray< DSPool, PoolsPerChunk >    pools;
            Atomic< Chunk *>                        next    {null};
        };

        using PoolBits_t = BitSet< PoolsPerChunk >;


    // variables
    private:
        Chunk       _firstChunk;


    // methods
    public:
        VDefaultDescriptorAllocator ()                                          __NE___ {}
        ~VDefaultDescriptorAllocator ()                                         __NE_OV;

        bool  Allocate (DescriptorSetLayoutID layoutId, OUT Storage &ds)        __NE_OV;
        void  Deallocate (DescriptorSetLayoutID layoutId, INOUT Storage &ds)    __NE_OV;

    private:
        static bool  _CreateDSPool (const VDevice &dev, uint descCount, uint maxDS, OUT VkDescriptorPool &dsPool);
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

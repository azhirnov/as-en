// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VDedicatedMemAllocator.h"
# include "graphics/Vulkan/Allocators/VBlockMemAllocator.h"
# include "graphics/Vulkan/Allocators/VLinearMemAllocator.h"
#endif

namespace
{
    ND_ static bool  TestBufferAllocation (GfxMemAllocatorPtr allocator, EMemoryType memType)
    {
        CHECK_ERR( allocator );

        auto&   res_mngr = GraphicsScheduler().GetResourceManager();

        auto    buf_id = res_mngr.CreateBuffer( BufferDesc{ 1_Kb, EBufferUsage::Storage }.SetMemory( memType ), "", allocator );
        CHECK_ERR( buf_id );

        res_mngr.ReleaseResource( buf_id );
        return true;
    }


    ND_ static bool  TestAllocators (GfxMemAllocatorPtr allocator)
    {
        CHECK_ERR( TestBufferAllocation( allocator, EMemoryType::DeviceLocal ));
        CHECK_ERR( TestBufferAllocation( allocator, EMemoryType::HostCoherent ));
        CHECK_ERR( TestBufferAllocation( allocator, EMemoryType::HostCached ));
        CHECK_ERR( TestBufferAllocation( allocator, EMemoryType::Unified ));
        return true;
    }


#ifdef AE_ENABLE_VULKAN
    ND_ static bool  TestCustomAllocators ()
    {
        CHECK_ERR( TestAllocators( MakeRC<VDedicatedMemAllocator>() ));
        CHECK_ERR( TestAllocators( MakeRC<VBlockMemAllocator>( 2_Kb, 64_Kb )));
        CHECK_ERR( TestAllocators( MakeRC<VLinearMemAllocator>( 64_Kb )));
        return true;
    }
#endif

#ifdef AE_ENABLE_METAL
    ND_ static bool  TestCustomAllocators ()
    {
        // TODO
        return true;
    }
#endif

#ifdef AE_ENABLE_REMOTE_GRAPHICS
    ND_ static bool  TestCustomAllocators ()
    {
        // TODO
        return true;
    }
#endif

} // namespace


bool RGTest::Test_Allocator ()
{
    bool    result = true;

    RG_CHECK( TestCustomAllocators() );

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}

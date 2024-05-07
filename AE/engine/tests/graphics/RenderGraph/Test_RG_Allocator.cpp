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

		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto	buf_id = res_mngr.CreateBuffer( BufferDesc{ 1_Kb, EBufferUsage::Storage }.SetMemory( memType ), "", allocator );
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


	ND_ static bool  TestAllocators ()
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();
		{
			auto	alloc = res_mngr.CreateLinearGfxMemAllocator( 64_Kb );
			CHECK_ERR( alloc );
			CHECK_ERR( TestAllocators( alloc ));
		}{
			auto	alloc = res_mngr.CreateBlockGfxMemAllocator( 4_Kb, 64_Kb );
			CHECK_ERR( alloc );
			CHECK_ERR( TestAllocators( alloc ));
		}{
			auto	alloc = res_mngr.CreateUnifiedGfxMemAllocator( 64_Kb );
			CHECK_ERR( alloc );
			CHECK_ERR( TestAllocators( alloc ));
		}{
			auto	alloc = res_mngr.GetDefaultGfxMemAllocator();
			CHECK_ERR( alloc );
			CHECK_ERR( TestAllocators( alloc ));
		}
		return true;
	}


	ND_ static bool  TestCustomAllocators ()
	{
	#ifdef AE_ENABLE_VULKAN
		CHECK_ERR( TestAllocators( MakeRC<VDedicatedMemAllocator>() ));
		return true;
	#else
		return true;
	#endif
	}

} // namespace


bool RGTest::Test_Allocator ()
{
	bool	result = true;

	RG_CHECK( TestAllocators() );
	RG_CHECK( TestCustomAllocators() );

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}

// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VDevice.h"
using namespace AE::Graphics;

namespace
{
	struct DSPoolLimitsTestConfig
	{
		struct {
			VkDescriptorType		descType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			uint					arraySize		= 128;
		}						layout;
		struct {
			VkDescriptorType		descType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			uint					descCount		= 128;
			uint					maxDS			= 128;
		}						pool;
	};


	static bool  TestDSPoolLimits (VDevice const &dev, const DSPoolLimitsTestConfig &cfg, OUT usize &outDSCount)
	{
		// create layout
		VkDescriptorSetLayout	dsLayout = Default;
		{
			Array<VkDescriptorSetLayoutBinding>		bindings;

			{
				auto&	bind = bindings.emplace_back();
				bind.descriptorType		= cfg.layout.descType;
				bind.stageFlags			= VK_SHADER_STAGE_COMPUTE_BIT;
				bind.binding			= uint(bindings.size()-1);
				bind.descriptorCount	= cfg.layout.arraySize;
			}

			VkDescriptorSetLayoutCreateInfo	info = {};
			info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.pBindings		= bindings.data();
			info.bindingCount	= uint(bindings.size());
			VK_CHECK_ERR( dev.vkCreateDescriptorSetLayout( dev.GetVkDevice(), &info, null, OUT &dsLayout ));
		}

		// create pool
		VkDescriptorPool	dsPool = Default;
		{
			Array<VkDescriptorPoolSize>		pool_sizes;
			{
				auto&	size = pool_sizes.emplace_back();
				size.type				= cfg.pool.descType;
				size.descriptorCount	= cfg.pool.descCount;
			}

			// reserve space for single DS
			if ( cfg.layout.descType != cfg.pool.descType )
			{
				auto&	size = pool_sizes.emplace_back();
				size.type				= cfg.layout.descType;
				size.descriptorCount	= cfg.layout.arraySize;
			}

			VkDescriptorPoolCreateInfo	info = {};
			info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			info.poolSizeCount	= uint(pool_sizes.size());
			info.pPoolSizes		= pool_sizes.data();
			info.maxSets		= cfg.pool.maxDS;
			info.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

			VK_CHECK_ERR( dev.vkCreateDescriptorPool( dev.GetVkDevice(), &info, null, OUT &dsPool ));
		}

		// create descriptor sets
		{
			Array<VkDescriptorSet>	desc_sets;

			VkDescriptorSetAllocateInfo	info = {};
			info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			info.descriptorSetCount	= 1;
			info.pSetLayouts		= &dsLayout;
			info.descriptorPool		= dsPool;

			for (uint i = 0; i < cfg.pool.maxDS * 100; ++i)
			{
				VkDescriptorSet	ds = Default;

				auto	err = dev.vkAllocateDescriptorSets( dev.GetVkDevice(), &info, OUT &ds );
				if ( err == VK_SUCCESS )
				{
					desc_sets.push_back( ds );
					continue;
				}

				//VK_CHECK( err );
				break;
			}

			outDSCount = desc_sets.size();

			if ( cfg.layout.descType != cfg.pool.descType )
				--outDSCount;
		}

		VK_CHECK( dev.vkResetDescriptorPool( dev.GetVkDevice(), dsPool, 0 ));
		dev.vkDestroyDescriptorPool( dev.GetVkDevice(), dsPool, null );
		dev.vkDestroyDescriptorSetLayout( dev.GetVkDevice(), dsLayout, null );
		return true;
	}


	static void  TestDSPoolLimits1 (VDevice const &dev)
	{
		const uint	scale = 8;

		DSPoolLimitsTestConfig	cfg;
		cfg.layout.descType		= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		cfg.layout.arraySize	= 128;
		cfg.pool.descCount		= 128 * scale;

		const auto	Print = [] (usize count, StringView name)
		{{
			AE_LOGI( "Allocated descriptor count with "s << name << ": " << ToString( count ) << ", rate per StorageBuffer: " << ToString( float(scale) / float(count) ));
		}};

		// how many storage buffers can allocate
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "StorageBuffer" );
		}

		// how many uniform buffers can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "UniformBuffer" );
		}

		// how many images can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "StorageImage" );
		}

		// how many sampled images can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "SampledImage" );
		}

		// how many combined images can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "CombinedImage" );
		}

		// how many samplers can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_SAMPLER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "Sampler" );
		}

		// how many dynamic uniform buffers can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "DynamicUniformBuffer" );
		}

		// how many dynamic storage buffers can allocate on storage buffer
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "DynamicStorageBuffer" );
		}

		AE_LOGI( "----" );
	}


	static void  TestDSPoolLimits2 (VDevice const &dev)
	{
		const uint	scale = 8;

		DSPoolLimitsTestConfig	cfg;
		cfg.layout.descType		= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		cfg.layout.arraySize	= 128;
		cfg.pool.descCount		= 128 * scale;

		const auto	Print = [] (usize count, StringView name)
		{{
			AE_LOGI( "Allocated descriptor count with "s << name << ": " << ToString( count ) << ", rate per StorageImage: " << ToString( float(scale) / float(count) ));
		}};

		// how many storage images can allocate
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "StorageImage" );
		}

		// how many samplers can allocate on storage image
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_SAMPLER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "Sampler" );
		}

		// how many storage buffers can allocate on storage image
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "StorageBuffer" );
		}

		// how many combined images can allocate on storage image
		{
			cfg.pool.descType	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

			usize	count = 0;
			TestDSPoolLimits( dev, cfg, OUT count );
			Print( count, "CombinedImage" );
		}

		AE_LOGI( "----" );
	}

} // namespace


extern void  VkTest_DescriptorPool (VDevice const &dev)
{
	TestDSPoolLimits1( dev );
	TestDSPoolLimits2( dev );
}

#endif // AE_ENABLE_VULKAN

// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VDevice.h"
using namespace AE::Graphics;

namespace
{
	struct MutableDescTypeConfig
	{
		struct {
			Array<VkDescriptorType>	mutableTypes;
			uint					arraySize		= 128;
		}						layout;
		struct {
			uint					descCount		= 128;
			uint					maxDS			= 128;
		}						pool;
	};


	struct TempResources
	{
		VkImage			img2d		= Default;
		VkImageView		view2d		= Default;

		VkImage			img3d		= Default;
		VkImageView		view3d		= Default;

		VkImage			imgCube		= Default;
		VkImageView		viewCube	= Default;

		VkBuffer		buf			= Default;

		Array<VkDeviceMemory>	memory;

		bool  Create (VDevice const &);
		void  Destroy (VDevice const &);
	};


	bool  TempResources::Create (VDevice const &dev)
	{
		const auto	AllocMem = [this, &dev] (VkMemoryRequirements &memReq, VkDeviceMemory &mem) -> bool
		{{
			VkMemoryAllocateInfo	mem_alloc = {};
			mem_alloc.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			mem_alloc.allocationSize	= memReq.size;

			memReq.memoryTypeBits &= dev.GetMemoryTypeBits( EMemoryType::DeviceLocal );
			CHECK_ERR( memReq.memoryTypeBits != 0 );

			for (uint type_idx : BitIndexIterate( memReq.memoryTypeBits ))
			{
				mem_alloc.memoryTypeIndex = type_idx;

				if_likely( dev.AllocateMemory( mem_alloc, OUT mem ) == VK_SUCCESS )
				{
					this->memory.push_back( mem );
					return true;
				}
			}
			return false;
		}};

		const auto	CreateImage = [&dev, &AllocMem] (VkImageViewType type, OUT VkImage &img, OUT VkImageView &view) -> bool
		{{
			{
				VkImageCreateInfo	info = {};
				info.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
				info.imageType		= type == VK_IMAGE_VIEW_TYPE_3D ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
				info.flags			= type == VK_IMAGE_VIEW_TYPE_CUBE ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
				info.format			= VK_FORMAT_R8G8B8A8_UNORM;
				info.extent.width	= 128;
				info.extent.height	= 128;
				info.extent.depth	= type == VK_IMAGE_VIEW_TYPE_3D ? 128 : 1;
				info.mipLevels		= 1;
				info.arrayLayers	= type == VK_IMAGE_VIEW_TYPE_CUBE ? 6 :
									  type == VK_IMAGE_VIEW_TYPE_2D_ARRAY ? 128 : 1;
				info.samples		= VK_SAMPLE_COUNT_1_BIT;
				info.tiling			= VK_IMAGE_TILING_OPTIMAL;
				info.usage			= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
				info.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
				info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

				VK_CHECK_ERR( dev.vkCreateImage( dev.GetVkDevice(), &info, null, OUT &img ));
			}{
				VkMemoryRequirements	mem_req;
				dev.vkGetImageMemoryRequirements( dev.GetVkDevice(), img, OUT &mem_req );

				VkDeviceMemory	mem;
				CHECK_ERR( AllocMem( mem_req, OUT mem ));

				VK_CHECK_ERR( dev.vkBindImageMemory( dev.GetVkDevice(), img, mem, 0 ));
			}{
				VkImageViewCreateInfo	info = {};
				info.sType		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				info.image		= img;
				info.viewType	= type;
				info.format		= VK_FORMAT_R8G8B8A8_UNORM;

				info.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				info.subresourceRange.baseMipLevel		= 0;
				info.subresourceRange.levelCount		= VK_REMAINING_MIP_LEVELS;
				info.subresourceRange.baseArrayLayer	= 0;
				info.subresourceRange.layerCount		= VK_REMAINING_ARRAY_LAYERS;

				VK_CHECK_ERR( dev.vkCreateImageView( dev.GetVkDevice(), &info, null, OUT &view ));
			}
			return true;
		}};

		CHECK_ERR( CreateImage( VK_IMAGE_VIEW_TYPE_2D, img2d, view2d ));
		CHECK_ERR( CreateImage( VK_IMAGE_VIEW_TYPE_3D, img3d, view3d ));
		CHECK_ERR( CreateImage( VK_IMAGE_VIEW_TYPE_CUBE, imgCube, viewCube ));

		{
			VkBufferCreateInfo	info = {};
			info.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.flags	= 0;
			info.usage	= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			info.size	= 1024;

			VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &info, null, OUT &buf ));

			VkMemoryRequirements	mem_req;
			dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buf, OUT &mem_req );

			VkDeviceMemory	mem;
			CHECK_ERR( AllocMem( mem_req, OUT mem ));

			VK_CHECK_ERR( dev.vkBindBufferMemory( dev.GetVkDevice(), buf, mem, 0 ));
		}
		return true;
	}


	void  TempResources::Destroy (VDevice const &dev)
	{
		const auto	DestroyImg = [&dev] (VkImage &img, VkImageView &view)
		{{
			if ( view != Default )
				dev.vkDestroyImageView( dev.GetVkDevice(), view, null );

			if ( img != Default )
				dev.vkDestroyImage( dev.GetVkDevice(), img, null );

			img		= Default;
			view	= Default;
		}};

		DestroyImg( img2d, view2d );
		DestroyImg( img3d, view3d );
		DestroyImg( imgCube, viewCube );

		if ( buf != Default )
			dev.vkDestroyBuffer( dev.GetVkDevice(), buf, null );
		buf = Default;

		for (auto& mem : memory) {
			dev.vkFreeMemory( dev.GetVkDevice(), mem, null );
		}
		memory.clear();
	}


	static bool  TestMutableDescType (VDevice const &dev, const MutableDescTypeConfig &cfg, OUT usize &outDSCount)
	{
		// create layout
		VkDescriptorSetLayout	dsLayout = Default;
		{
			Array<VkDescriptorSetLayoutBinding>		bindings;

			VkMutableDescriptorTypeListVALVE		mutable_type_list = {};
			mutable_type_list.descriptorTypeCount = uint(cfg.layout.mutableTypes.size());
			mutable_type_list.pDescriptorTypes    = cfg.layout.mutableTypes.data();

			VkMutableDescriptorTypeCreateInfoEXT	mutable_type_info = {};
			mutable_type_info.sType								= VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT;
			mutable_type_info.mutableDescriptorTypeListCount	= 1;
			mutable_type_info.pMutableDescriptorTypeLists		= &mutable_type_list;

			{
				auto&	bind = bindings.emplace_back();
				bind.descriptorType		= VK_DESCRIPTOR_TYPE_MUTABLE_EXT;
				bind.stageFlags			= VK_SHADER_STAGE_COMPUTE_BIT;
				bind.binding			= uint(bindings.size()-1);
				bind.descriptorCount	= cfg.layout.arraySize;
			}

			VkDescriptorSetLayoutCreateInfo	info = {};
			info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			info.pNext			= &mutable_type_info;
			info.pBindings		= bindings.data();
			info.bindingCount	= uint(bindings.size());

			VkDescriptorSetLayoutSupport	support = {};
			support.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;

			dev.vkGetDescriptorSetLayoutSupport( dev.GetVkDevice(), &info, OUT &support );

			if ( support.supported == VK_FALSE )
			{
				AE_LOGW( "DSLayout is not supported" );
				return false;
			}
			VK_CHECK_ERR( dev.vkCreateDescriptorSetLayout( dev.GetVkDevice(), &info, null, OUT &dsLayout ));
		}

		// create pool
		VkDescriptorPool	dsPool = Default;
		{
			Array<VkDescriptorPoolSize>		pool_sizes;
			{
				auto&	size = pool_sizes.emplace_back();
				size.type				= VK_DESCRIPTOR_TYPE_MUTABLE_EXT;
				size.descriptorCount	= cfg.pool.descCount;
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
		Array<VkDescriptorSet>	desc_sets;
		{
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
		}

		// update descriptors
		if ( not desc_sets.empty() )
		{
			TempResources				res;
			usize						offset	= 0;
			Array<VkWriteDescriptorSet>	descriptors;
			LinearAllocator<>			alloc;

			if ( res.Create( dev ))
			{
				// 2D image
				if ( offset < cfg.layout.arraySize )
				{
					uint	count	= cfg.layout.arraySize / 4;
					auto*	images	= alloc.Allocate<VkDescriptorImageInfo>( count );
					if ( images != null )
					{
						for (uint i = 0; i < count; ++i) {
							images[i] = { Default, res.view2d, VK_IMAGE_LAYOUT_GENERAL };
						}

						VkWriteDescriptorSet&	wds = descriptors.emplace_back();
						wds.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						wds.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
						wds.dstArrayElement	= uint(offset);
						wds.descriptorCount	= count;
						wds.dstBinding		= 0;
						wds.dstSet			= desc_sets.back();
						wds.pImageInfo		= images;

						offset += count;
					}
				}

				// 3D image
				if ( offset < cfg.layout.arraySize )
				{
					uint	count	= cfg.layout.arraySize / 4;
					auto*	images	= alloc.Allocate<VkDescriptorImageInfo>( count );
					if ( images != null )
					{
						for (uint i = 0; i < count; ++i) {
							images[i] = { Default, res.view3d, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
						}

						VkWriteDescriptorSet&	wds = descriptors.emplace_back();
						wds.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						wds.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
						wds.dstArrayElement	= uint(offset);
						wds.descriptorCount	= count;
						wds.dstBinding		= 0;
						wds.dstSet			= desc_sets.back();
						wds.pImageInfo		= images;

						offset += count;
					}
				}

				// image cube
				if ( offset < cfg.layout.arraySize )
				{
					uint	count	= cfg.layout.arraySize / 4;
					auto*	images	= alloc.Allocate<VkDescriptorImageInfo>( count );
					if ( images != null )
					{
						for (uint i = 0; i < count; ++i) {
							images[i] = { Default, res.viewCube, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
						}

						VkWriteDescriptorSet&	wds = descriptors.emplace_back();
						wds.sType			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						wds.descriptorType	= VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
						wds.dstArrayElement	= uint(offset);
						wds.descriptorCount	= count;
						wds.dstBinding		= 0;
						wds.dstSet			= desc_sets.back();
						wds.pImageInfo		= images;

						offset += count;
					}
				}

				dev.vkUpdateDescriptorSets( dev.GetVkDevice(), uint(descriptors.size()), descriptors.data(), 0, null );

				res.Destroy( dev );
			}
		}

		desc_sets.clear();
		VK_CHECK( dev.vkResetDescriptorPool( dev.GetVkDevice(), dsPool, 0 ));
		dev.vkDestroyDescriptorPool( dev.GetVkDevice(), dsPool, null );
		dev.vkDestroyDescriptorSetLayout( dev.GetVkDevice(), dsLayout, null );
		return true;
	}


	static void  MutableDescTypeTest1 (VDevice const &dev)
	{
		MutableDescTypeConfig	cfg;
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER );

		usize	count = 0;
		if ( TestMutableDescType( dev, cfg, OUT count ))
		{
			AE_LOGI( "Allocated "s << ToString( count ) << " mutable descriptors" );
		}
	}

	static void  MutableDescTypeTest2 (VDevice const &dev)
	{
		MutableDescTypeConfig	cfg;
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER );

		usize	count = 0;
		if ( TestMutableDescType( dev, cfg, OUT count ))
		{
			AE_LOGI( "Allocated "s << ToString( count ) << " mutable descriptors" );
		}
	}

	static void  MutableDescTypeTest3 (VDevice const &dev)
	{
		MutableDescTypeConfig	cfg;
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_IMAGE );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_STORAGE_BUFFER );
		cfg.layout.mutableTypes.push_back( VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER );

		usize	count = 0;
		if ( TestMutableDescType( dev, cfg, OUT count ))
		{
			AE_LOGI( "Allocated "s << ToString( count ) << " mutable descriptors" );
		}
	}

} // namespace


extern void  VkTest_MutableDescriptorType (VDevice const &dev)
{
	if ( not dev.GetVExtensions().mutableDescType )
	{
		AE_LOGW( "'VK_EXT_mutable_descriptor_type' is not supported" );
		return;
	}

	MutableDescTypeTest1( dev );
	MutableDescTypeTest2( dev );
	MutableDescTypeTest3( dev );
}

#endif // AE_ENABLE_VULKAN

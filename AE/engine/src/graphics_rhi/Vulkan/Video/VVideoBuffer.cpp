// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Video/VVideoBuffer.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"
# include "graphics_rhi/Vulkan/Utils/NextChain.h"
# include "graphics_rhi/Vulkan/Video/VVideoUtils.cpp.h"

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
	bool  VVideoBuffer::Create (ResourceManager &resMngr, const VideoBufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _buffer == Default );
		CHECK_ERR( _bufferId == Default );
		CHECK_ERR( desc.videoUsage != Default );
		CHECK_ERR( not desc.profiles.empty() );
		CHECK_ERR( allocator );

		_desc = desc;
		GRES_CHECK( IsSupported( resMngr, _desc ));

		auto&	dev = resMngr.GetDevice();

		InPlaceLinearAllocator<1024>	alloc;
		{
			VkVideoProfileInfoKHR	profile_info;
			VkVideoCapabilitiesKHR	capabilities;
			VkDeviceSize			min_bitstream_buffer_offset_alignment	= UMax;
			VkDeviceSize			min_bitstream_buffer_size_alignment		= UMax;

			// get capabilities per profile
			for (auto& prof : _desc.profiles)
			{
				if_unlikely( not GetProfileWithCapabilities( dev, prof, alloc, OUT profile_info, OUT capabilities ))
					return false;

				min_bitstream_buffer_offset_alignment	= Min( min_bitstream_buffer_offset_alignment, capabilities.minBitstreamBufferOffsetAlignment );
				min_bitstream_buffer_size_alignment		= Min( min_bitstream_buffer_size_alignment, capabilities.minBitstreamBufferSizeAlignment );

				alloc.Discard();
			}

			_minOffsetAlign	= POTBytes{ min_bitstream_buffer_offset_alignment };
			_minSizeAlign	= POTBytes{ min_bitstream_buffer_size_alignment };
			_desc.size		= AlignUp( _desc.size, _minSizeAlign );
		}

		VkBufferCreateInfo					buffer_ci	 = {};
		VkVideoProfileListInfoKHR			prof_list	 = {};
		VkBufferUsageFlags2CreateInfoKHR	flags2_ci	 = {};
		VNextChain							p_next		 {buffer_ci};

		prof_list.sType			= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
		prof_list.profileCount	= uint(_desc.profiles.size());
		p_next.Add( prof_list );

		CHECK_ERR( ConvertProfiles( dev, _desc.profiles, alloc, OUT prof_list.pProfiles ));

		flags2_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_USAGE_FLAGS_2_CREATE_INFO;
		flags2_ci.usage		= VEnumCast( _desc.usage ) | VEnumCast( _desc.videoUsage );

		buffer_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_ci.flags		= 0;
		buffer_ci.usage		= VkBufferUsageFlags( flags2_ci.usage );
		buffer_ci.size		= VkDeviceSize( _desc.size );

		if_unlikely( EMemoryType_IsNonCoherent( desc.memType ))
			buffer_ci.size = AlignUp( buffer_ci.size, dev.GetDeviceProperties().res.minNonCoherentAtomSize );

		VQueueFamilyIndices_t	queue_family_indices;

		// setup sharing mode
		if ( _desc.queues != Default )
		{
			dev.GetQueueFamilies( _desc.queues, OUT queue_family_indices );

			buffer_ci.sharingMode			= VK_SHARING_MODE_CONCURRENT;
			buffer_ci.pQueueFamilyIndices	= queue_family_indices.data();
			buffer_ci.queueFamilyIndexCount	= uint(queue_family_indices.size());
		}

		// reset to exclusive mode
		if ( buffer_ci.queueFamilyIndexCount <= 1 )
		{
			buffer_ci.sharingMode			= VK_SHARING_MODE_EXCLUSIVE;
			buffer_ci.pQueueFamilyIndices	= null;
			buffer_ci.queueFamilyIndexCount	= 0;
		}

		if ( dev.GetVExtensions().maintenance5 ){
			p_next.Add( flags2_ci );
		}else{
			CHECK_ERR_MSG( flags2_ci.usage == buffer_ci.usage,
				"Some buffer usage flags requires 'maintenance5' extension" );
		}

		VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buffer_ci, null, OUT &_buffer ));

		VulkanBufferDesc		vk_desc;
		vk_desc.buffer			= _buffer;
		vk_desc.usage			= VkBufferUsageFlagBits(buffer_ci.usage);
		vk_desc.size			= Bytes{buffer_ci.size};
		vk_desc.queues			= _desc.queues;
		vk_desc.memFlags		= VEnumCast( _desc.memType );
		vk_desc.canBeDestroyed	= false;
		vk_desc.allocMemory		= true;

		_bufferId = resMngr.CreateBuffer( vk_desc, dbgName, RVRef(allocator) );
		CHECK_ERR( _bufferId );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VVideoBuffer::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		resMngr.ImmediatelyRelease( INOUT _bufferId );

		_buffer	= Default;
		_desc	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  VVideoBuffer::IsSupported (const ResourceManager &, const VideoBufferDesc &) __NE___
	{
		// TODO
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

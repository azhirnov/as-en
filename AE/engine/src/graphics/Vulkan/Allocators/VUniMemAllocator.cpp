// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Allocators/VUniMemAllocator.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

# define VMA_STATIC_VULKAN_FUNCTIONS		0
# define VMA_RECORDING_ENABLED				0
# define VMA_DEDICATED_ALLOCATION			1	// TODO: set 0 to avoid crash on Intel
# define VMA_DEBUG_INITIALIZE_ALLOCATIONS	0
# define VMA_DEBUG_ALWAYS_DEDICATED_MEMORY	0
# ifdef AE_DEBUG
#	define VMA_DEBUG_DETECT_CORRUPTION		1
# else
#	define VMA_DEBUG_DETECT_CORRUPTION		0
# endif
# define VMA_DEBUG_GLOBAL_MUTEX				0	// will be externally synchronized
# define VMA_EXTERNAL_MEMORY				0
# define VMA_BIND_MEMORY2					0
# define VMA_MEMORY_BUDGET					0
# define VMA_BUFFER_DEVICE_ADDRESS			1
# define VMA_MEMORY_PRIORITY				0

# define VMA_USE_STL_SHARED_MUTEX			1

# define VMA_IMPLEMENTATION					1
# define VMA_ASSERT							ASSERT

# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4701)
#	pragma warning (disable: 4703)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wmissing-field-initializers"
#	pragma clang diagnostic ignored "-Wunused-variable"
#	pragma clang diagnostic ignored "-Wcast-align"
#	pragma clang diagnostic ignored "-Wunused-private-field"
#	pragma clang diagnostic ignored "-Wnullability-completeness"
#	pragma clang diagnostic ignored "-Wunused-parameter"
# endif
# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#	pragma GCC diagnostic ignored "-Wunused-variable"
#	pragma GCC diagnostic ignored "-Wcast-align"
# endif

# include "vk_mem_alloc.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
#endif
#ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
#endif

# define USE_AE_MEM_SELECTOR		1


namespace AE::Graphics
{
namespace {
/*
=================================================
	ConvertToCreateFlags
=================================================
*/
	ND_ static VmaAllocationCreateFlags  ConvertToCreateFlags (EMemoryType memType) __NE___
	{
		VmaAllocationCreateFlags	result = 0;

		if ( AllBits( memType, EMemoryType::Dedicated ))
			result |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		if ( AllBits( memType, EMemoryType::Transient ))
			result |= VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT;

	#if USE_AE_MEM_SELECTOR
		if ( EMemoryType_IsHostVisible( memType ))
			result |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

	#else
		if ( EMemoryType_IsHostVisible( memType ))
		{
			result |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

			if ( EMemoryType_IsNonCoherent( memType ))
				result |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
			else
				result |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
		}
	#endif
		return result;
	}

/*
=================================================
	ConvertToMemoryUsage
=================================================
*/
	ND_ static VmaMemoryUsage  ConvertToMemoryUsage (EMemoryType memType) __NE___
	{
	#if USE_AE_MEM_SELECTOR

		if ( AllBits( memType, EMemoryType::Transient ))
			return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;

		return VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN;
	#else

		if ( AllBits( memType, EMemoryType::Unified ))
			return VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN;	// use 'requiredFlags'

		if ( AllBits( memType, EMemoryType::Transient ))
			return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_LAZILY_ALLOCATED;

		if ( AllBits( memType, EMemoryType::HostCoherent ))
			return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_TO_CPU;

		if ( AllBits( memType, EMemoryType::HostCached ))
			return VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_TO_GPU;

		return VmaMemoryUsage::VMA_MEMORY_USAGE_GPU_ONLY;
	#endif
	}

/*
=================================================
	RequireBufferDeviceAddress
=================================================
*/
	ND_ inline bool  RequireBufferDeviceAddress (const BufferDesc &desc) __NE___
	{
		constexpr auto	mask =	EBufferUsage::ShaderBindingTable | EBufferUsage::ASBuild_ReadOnly |
								EBufferUsage::ASBuild_Scratch | EBufferUsage::ShaderAddress;
		return AnyBits( desc.usage, mask );
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VUniMemAllocator::VUniMemAllocator (Bytes pageSize) __NE___ :
		_device{ GraphicsScheduler().GetDevice() },
		_allocator{ null }
	{
		if ( pageSize == 0 )
			pageSize = _DefaultPageSize;

		EXLOCK( _guard );
		CHECK( _CreateAllocator( pageSize, OUT _allocator ));
	}

/*
=================================================
	destructor
=================================================
*/
	VUniMemAllocator::~VUniMemAllocator () __NE___
	{
		EXLOCK( _guard );
		if ( _allocator ) {
			vmaDestroyAllocator( _allocator );
		}
	}

/*
=================================================
	AllocForImage
=================================================
*/
	bool  VUniMemAllocator::AllocForImage (VkImage image, const ImageDesc &desc, OUT Storage_t &data) __NE___
	{
		CHECK_ERR( image != Default );
		CHECK_ERR( desc.memType != Default );

		VmaAllocationCreateInfo		info = {};
		info.flags			= ConvertToCreateFlags( desc.memType );
		info.usage			= ConvertToMemoryUsage( desc.memType );
		info.requiredFlags	= 0;
		info.preferredFlags	= 0;
		info.memoryTypeBits	= 0;
		info.pool			= Default;
		info.pUserData		= null;
		info.priority		= 0.f;
		StaticAssert64( sizeof(info) == 48 );

	  #if USE_AE_MEM_SELECTOR
		info.memoryTypeBits = GraphicsScheduler().GetDevice().GetMemoryTypeBits( desc.memType );
	  #else
		info.requiredFlags	= VEnumCast( desc.memType );
	  #endif

		EXLOCK( _guard );

		VmaAllocation	mem	= null;
		VK_CHECK_ERR( vmaAllocateMemoryForImage( _allocator, image, &info, OUT &mem, null ));

		VK_CHECK_ERR( vmaBindImageMemory( _allocator, mem, image ));

		DEBUG_ONLY(
			VmaAllocationInfo	alloc_info	= {};
			vmaGetAllocationInfo( _allocator, mem, OUT &alloc_info );

			Bytes	align = VImage::GetMemoryAlignment( _device, desc );
			ASSERT( IsMultipleOf( alloc_info.offset, align ));
		)

		_CastStorage( data ).allocation = mem;
		return true;
	}

/*
=================================================
	AllocForBuffer
=================================================
*/
	bool  VUniMemAllocator::AllocForBuffer (VkBuffer buffer, const BufferDesc &desc, OUT Storage_t &data) __NE___
	{
		CHECK_ERR( buffer != Default );
		CHECK_ERR( desc.memType != Default );

	  #if not VMA_BUFFER_DEVICE_ADDRESS
		CHECK_ERR( not RequireBufferDeviceAddress( desc ));
	  #endif

		VmaAllocationCreateInfo		info = {};
		info.flags			= ConvertToCreateFlags( desc.memType );
		info.usage			= ConvertToMemoryUsage( desc.memType );
		info.requiredFlags	= 0;
		info.preferredFlags	= 0;
		info.memoryTypeBits	= 0;
		info.pool			= Default;
		info.pUserData		= null;
		info.priority		= 0.f;
		StaticAssert64( sizeof(info) == 48 );

	  #if USE_AE_MEM_SELECTOR
		info.memoryTypeBits = GraphicsScheduler().GetDevice().GetMemoryTypeBits( desc.memType );
	  #else
		info.requiredFlags	= VEnumCast( desc.memType );
	  #endif

		EXLOCK( _guard );

		VmaAllocation	mem	= null;
		VK_CHECK_ERR( vmaAllocateMemoryForBuffer( _allocator, buffer, &info, OUT &mem, null ));

		VK_CHECK_ERR( vmaBindBufferMemory( _allocator, mem, buffer ));

		DEBUG_ONLY(
			VmaAllocationInfo	alloc_info	= {};
			vmaGetAllocationInfo( _allocator, mem, OUT &alloc_info );

			Bytes	align = VBuffer::GetMemoryAlignment( _device, desc );
			ASSERT( IsMultipleOf( alloc_info.offset, align ));
		)

		_CastStorage( data ).allocation = mem;
		return true;
	}

/*
=================================================
	Dealloc
=================================================
*/
	bool  VUniMemAllocator::Dealloc (INOUT Storage_t &data) __NE___
	{
		VmaAllocation&	mem = _CastStorage( data ).allocation;

		if_likely( mem != null )
		{
			EXLOCK( _guard );

			vmaFreeMemory( _allocator, mem );
			mem = null;
			return true;
		}
		return false;
	}

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  VUniMemAllocator::GetInfo (const Storage_t &data, OUT VulkanMemoryObjInfo &outInfo) C_NE___
	{
		VmaAllocation		mem = _CastStorage( data ).allocation;
		CHECK_ERR( mem != null );

		SHAREDLOCK( _guard );

		VmaAllocationInfo	alloc_info	= {};
		vmaGetAllocationInfo( _allocator, mem, OUT &alloc_info );

		const auto&		mem_props = _device.GetVProperties().memoryProperties;
		CHECK_ERR( alloc_info.memoryType < mem_props.memoryTypeCount );

		outInfo.memory		= alloc_info.deviceMemory;
		outInfo.flags		= VkMemoryPropertyFlagBits(mem_props.memoryTypes[ alloc_info.memoryType ].propertyFlags);
		outInfo.offset		= Bytes(alloc_info.offset);
		outInfo.size		= Bytes(alloc_info.size);
		outInfo.mappedPtr	= alloc_info.pMappedData;
		return true;
	}

/*
=================================================
	_CreateAllocator
=================================================
*/
	bool  VUniMemAllocator::_CreateAllocator (Bytes pageSize, OUT VmaAllocator &alloc) C_NE___
	{
		VkDevice				dev		= _device.GetVkDevice();
		VmaVulkanFunctions		funcs	= {};

		funcs.vkGetInstanceProcAddr					= _var_vkGetInstanceProcAddr;
		funcs.vkGetDeviceProcAddr					= _var_vkGetDeviceProcAddr;
		funcs.vkGetPhysicalDeviceProperties			= _var_vkGetPhysicalDeviceProperties;
		funcs.vkGetPhysicalDeviceMemoryProperties	= _var_vkGetPhysicalDeviceMemoryProperties;
		funcs.vkAllocateMemory						= _device._GetVkTable()->_var_vkAllocateMemory;
		funcs.vkFreeMemory							= _device._GetVkTable()->_var_vkFreeMemory;
		funcs.vkMapMemory							= _device._GetVkTable()->_var_vkMapMemory;
		funcs.vkUnmapMemory							= _device._GetVkTable()->_var_vkUnmapMemory;
		funcs.vkFlushMappedMemoryRanges				= _device._GetVkTable()->_var_vkFlushMappedMemoryRanges;
		funcs.vkInvalidateMappedMemoryRanges		= _device._GetVkTable()->_var_vkInvalidateMappedMemoryRanges;
		funcs.vkBindBufferMemory					= _device._GetVkTable()->_var_vkBindBufferMemory;
		funcs.vkBindImageMemory						= _device._GetVkTable()->_var_vkBindImageMemory;
		funcs.vkGetBufferMemoryRequirements			= _device._GetVkTable()->_var_vkGetBufferMemoryRequirements;
		funcs.vkGetImageMemoryRequirements			= _device._GetVkTable()->_var_vkGetImageMemoryRequirements;
		funcs.vkGetBufferMemoryRequirements2KHR		= _device._GetVkTable()->_var_vkGetBufferMemoryRequirements2KHR;
		funcs.vkGetImageMemoryRequirements2KHR		= _device._GetVkTable()->_var_vkGetImageMemoryRequirements2KHR;
		funcs.vkCreateBuffer						= _device._GetVkTable()->_var_vkCreateBuffer;
		funcs.vkDestroyBuffer						= _device._GetVkTable()->_var_vkDestroyBuffer;
		funcs.vkCreateImage							= _device._GetVkTable()->_var_vkCreateImage;
		funcs.vkDestroyImage						= _device._GetVkTable()->_var_vkDestroyImage;
		funcs.vkCmdCopyBuffer						= _device._GetVkTable()->_var_vkCmdCopyBuffer;
		funcs.vkBindBufferMemory2KHR				= _device._GetVkTable()->_var_vkBindBufferMemory2KHR;
		funcs.vkBindImageMemory2KHR					= _device._GetVkTable()->_var_vkBindImageMemory2KHR;
		funcs.vkGetPhysicalDeviceMemoryProperties2KHR = _var_vkGetPhysicalDeviceMemoryProperties2KHR;
		funcs.vkGetDeviceBufferMemoryRequirements	= null;
		funcs.vkGetDeviceImageMemoryRequirements	= null;

		VmaAllocatorCreateInfo	info = {};
		info.flags			= VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
		info.physicalDevice	= _device.GetVkPhysicalDevice();
		info.device			= dev;

	  #if VMA_DEDICATED_ALLOCATION
		if ( _device.GetVExtensions().dedicatedAllocation )
			info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	  #endif
	  #if VMA_MEMORY_BUDGET
		if ( _device.GetVExtensions().memoryBudget )
			info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
	  #endif
	  #if VMA_BUFFER_DEVICE_ADDRESS
		if ( _device.GetVExtensions().bufferDeviceAddress )
			info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	  #endif
	  #if VMA_MEMORY_PRIORITY
		if ( _device.GetVExtensions().memoryPriority )
			info.flags |= VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;
	  #endif

		info.preferredLargeHeapBlockSize	= VkDeviceSize(pageSize);
		info.pAllocationCallbacks			= null;
		info.pDeviceMemoryCallbacks			= null;
		//info.frameInUseCount	// ignore
		info.pHeapSizeLimit					= null;		// TODO
		info.pVulkanFunctions				= &funcs;
		info.instance						= _device.GetVkInstance();
		info.vulkanApiVersion				= VK_MAKE_VERSION( _device.GetDeviceVersion().major, _device.GetDeviceVersion().minor, 0 );

		VK_CHECK_ERR( vmaCreateAllocator( &info, OUT &alloc ));
		return true;
	}
/*
=================================================
	MaxAllocationSize
=================================================
*/
	Bytes  VUniMemAllocator::MaxAllocationSize () C_NE___
	{
		return _device.GetVExtensions().maintenance3 ?
					Bytes{_device.GetVProperties().maintenance3Props.maxMemoryAllocationSize} :
					UMax;
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

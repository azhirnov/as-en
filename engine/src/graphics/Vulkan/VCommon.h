// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "base/Algorithms/Cast.h"
# include "base/Containers/Ptr.h"
# include "base/Containers/FixedArray.h"
# include "base/Containers/FixedString.h"
# include "base/Containers/FixedTupleArray.h"
# include "base/Containers/StructView.h"
# include "base/Utils/Helpers.h"
# include "base/Memory/LinearAllocator.h"
# include "base/Memory/StackAllocator.h"

# include "threading/Primitives/Atomic.h"
# include "threading/TaskSystem/TaskScheduler.h"
# include "threading/TaskSystem/Promise.h"
# include "threading/Memory/GlobalLinearAllocator.h"
# include "threading/Memory/FrameAllocator.h"

# include "graphics/Public/Common.h"
# include "graphics/Public/IDs.h"
# include "graphics/Public/FrameUID.h"
# include "graphics/Public/GraphicsCreateInfo.h"
# include "graphics/Public/GraphicsProfiler.h"

# include "graphics/Vulkan/VulkanLoader.h"
# include "graphics/Vulkan/VulkanCheckError.h"


#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX)
#	define AE_VK_TIMELINE_SEMAPHORE	1
#else
#	define AE_VK_TIMELINE_SEMAPHORE	0
#endif

namespace AE::Graphics
{
	using AE::Threading::SpinLock;
	using AE::Threading::RWSpinLock;
	using AE::Threading::SpinLockRelaxed;
	using AE::Threading::AsyncTask;
	using AE::Threading::GlobalLinearAllocatorRef;
	using AE::Threading::GraphicsFrameAllocatorRef;
	using AE::Threading::BitAtomic;
	using AE::Threading::FAtomic;
	using AE::Threading::ETaskQueue;

	class VDevice;
	class VResourceManager;
	class VRenderPass;
	class VFramebuffer;
	class VCommandPoolManager;
	class VCommandBatch;
	class VRenderTaskScheduler;

	DEBUG_ONLY(
		using DebugName_t = FixedString<64>;
	)

	// Used for temporary allocations.
	// thread-safe: no
	using VTempLinearAllocator	= LinearAllocator< UntypedAllocator, 8, false >;	// TODO: use fast block allocator
	using VTempStackAllocator	= StackAllocator<  UntypedAllocator, 8, false >;

	
	using VFramebufferID		= HandleTmpl< 16, 16, Graphics::_hidden_::VulkanIDs_Start + 1 >;


	//
	// Vulkan Config
	//
	struct VConfig final : Noninstancable
	{
		static constexpr uint	MaxQueues		= uint(EQueueType::_Count) + 1;

		static constexpr uint	MaxVideoMaxReq	= 8;
		
		// for query manager
		static constexpr uint	TimestampQueryPerFrame			= 1000;
		static constexpr uint	PipelineStatQueryPerFrame		= 1000;
		static constexpr uint	PerformanceQueryPerFrame		= 1000;

		static constexpr uint	ASCompactedSizeQueryPerFrame	= 1000;
		static constexpr uint	ASSerializationQueryPerFrame	= 1000;
	};

	
	// debugger can't show enum names for VkFlags, so use enum instead
#define VULKAN_ENUM_BIT_OPERATORS( _type_ )																																	\
			inline constexpr _type_&  operator |= (_type_ &lhs, _type_ rhs) __NE___	{ return lhs = _type_( AE::Math::ToNearUInt( lhs ) | AE::Math::ToNearUInt( rhs )); }	\
		ND_ inline constexpr _type_   operator |  (_type_  lhs, _type_ rhs)	__NE___	{ return _type_( AE::Math::ToNearUInt( lhs ) | AE::Math::ToNearUInt( rhs )); }			\
			inline constexpr _type_&  operator &= (_type_ &lhs, _type_ rhs) __NE___	{ return lhs = _type_( AE::Math::ToNearUInt( lhs ) & AE::Math::ToNearUInt( rhs )); }	\
		ND_ inline constexpr _type_   operator &  (_type_  lhs, _type_ rhs)	__NE___	{ return _type_( AE::Math::ToNearUInt( lhs ) & AE::Math::ToNearUInt( rhs )); }			\
		ND_ inline constexpr _type_   operator ~  (_type_ value)			__NE___	{ return _type_( ~AE::Math::ToNearUInt( value )); }										\

	VULKAN_ENUM_BIT_OPERATORS( VkDependencyFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkImageAspectFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkStencilFaceFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkShaderStageFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkImageCreateFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkQueueFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkImageUsageFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkBufferUsageFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkSampleCountFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkMemoryPropertyFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkSubgroupFeatureFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkPipelineCreateFlagBits );
	VULKAN_ENUM_BIT_OPERATORS( VkGeometryFlagBitsKHR );
	VULKAN_ENUM_BIT_OPERATORS( VkGeometryInstanceFlagBitsKHR );
	VULKAN_ENUM_BIT_OPERATORS( VkBuildAccelerationStructureFlagBitsKHR );
	VULKAN_ENUM_BIT_OPERATORS( VkSamplerCreateFlagBits );
#undef VULKAN_ENUM_BIT_OPERATORS

} // AE::Graphics

#endif // AE_ENABLE_VULKAN


# ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_ENABLE_VULKAN
#	pragma detect_mismatch( "AE_ENABLE_VULKAN", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_VULKAN", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

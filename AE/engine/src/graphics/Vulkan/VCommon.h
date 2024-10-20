// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	docs:
		[Vulkan spec](https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html)
		[GLSL ref](https://registry.khronos.org/OpenGL-Refpages/gl4/)
		[GLSL ext](https://github.com/KhronosGroup/GLSL/extensions)
		[GLSL spec](https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf)
		[SPIRV spec](https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html)

	[supported features](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/VulkanFeatures.md)
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/Common.h"
# include "graphics/Public/IDs.h"
# include "graphics/Public/GraphicsCreateInfo.h"
# include "graphics/Public/GraphicsProfiler.h"

# include "graphics/Private/Defines.h"

# include "graphics/Vulkan/VulkanLoader.h"
# include "graphics/Vulkan/VulkanCheckError.h"

namespace AE::Graphics
{
	using AE::Threading::SpinLock;
	using AE::Threading::RWSpinLock;
	using AE::Threading::SpinLockRelaxed;
	using AE::Threading::GlobalLinearAllocatorRef;
	using AE::Threading::GraphicsFrameAllocatorRef;
	using AE::Threading::StructAtomic;
	using AE::Threading::FAtomic;
	using AE::Threading::ETaskQueue;
	using AE::Threading::Synchronized;

	class VDevice;
	class VRenderPass;
	class VFramebuffer;
	class VCommandPoolManager;
	class VCommandBatch;

	class ResourceManager;
	class RenderTaskScheduler;
	using VResourceManager = ResourceManager;

	GFX_DBG_ONLY(
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
	struct VConfig final : Noninstanceable
	{
		static constexpr uint	MaxVideoMemReq	= 8;

		// for query manager
		static constexpr uint	TimestampQueryPerFrame			= 1000;
		static constexpr uint	PipelineStatQueryPerFrame		= 1000;
		static constexpr uint	PerformanceQueryPerFrame		= 1000;

		static constexpr uint	ASCompactedSizeQueryPerFrame	= 1000;
		static constexpr uint	ASSerializationQueryPerFrame	= 1000;
	};


	// debugger can't show enum names for VkFlags, so use enum instead
#define VULKAN_ENUM_BIT_OPERATORS( _type_ )																															\
			inline constexpr _type_&  operator |= (_type_ &lhs, _type_ rhs) __NE___	{ return lhs = _type_( Math::ToNearUInt( lhs ) | Math::ToNearUInt( rhs )); }	\
		ND_ inline constexpr _type_   operator |  (_type_  lhs, _type_ rhs)	__NE___	{ return _type_( Math::ToNearUInt( lhs ) | Math::ToNearUInt( rhs )); }			\
			inline constexpr _type_&  operator &= (_type_ &lhs, _type_ rhs) __NE___	{ return lhs = _type_( Math::ToNearUInt( lhs ) & Math::ToNearUInt( rhs )); }	\
		ND_ inline constexpr _type_   operator &  (_type_  lhs, _type_ rhs)	__NE___	{ return _type_( Math::ToNearUInt( lhs ) & Math::ToNearUInt( rhs )); }			\
		ND_ inline constexpr _type_   operator ~  (_type_ value)			__NE___	{ return _type_( ~Math::ToNearUInt( value )); }									\

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

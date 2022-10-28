// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/Queue.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{
	enum class EQueueFamily : uint
	{
		_Count		= VConfig::MaxQueues,
		External	= VK_QUEUE_FAMILY_EXTERNAL,
		Foreign		= VK_QUEUE_FAMILY_FOREIGN_EXT,
		Ignored		= VK_QUEUE_FAMILY_IGNORED,
		Unknown		= Ignored,
	};

	using VQueuePtr				= Ptr< const struct VQueue >;
	using VQueueFamilyIndices_t	= FixedArray< uint, uint(EQueueType::_Count) >;



	//
	// Vulkan Queue
	//

	struct VQueue
	{
	// variables
		mutable RecursiveMutex		guard;				// use when call vkQueueSubmit, vkQueueWaitIdle, vkQueueBindSparse, vkQueuePresentKHR,
														// warning: don't use vkDeviceWaitIdle because it implicitly use all queues, so 'guard' must be locked for all queues.
		VkQueue						handle				= Default;
		EQueueType					type				= Default;
		EQueueFamily				familyIndex			= Default;
		VkQueueFlagBits				familyFlags			= Zero;
		uint						queueIndex			= UMax;
		float						priority			= 0.0f;
		VkQueueGlobalPriorityEXT	globalPriority		= Zero;
		VkPipelineStageFlagBits2	supportedStages		= Zero;		// all supported pipeline stages, except HOST and ALL
		VkAccessFlagBits2			supportedAccess		= Zero;		// all supported memory access types, except HOST and ALL
		packed_uint3				minImageTransferGranularity;
		uint						timestampValidBits	= 0;
		FixedString<64>				debugName;
		

	// methods
		VQueue () {}

		VQueue (VQueue &&other) :
			handle{other.handle}, type{other.type}, familyIndex{other.familyIndex}, familyFlags{other.familyFlags},
			queueIndex{other.queueIndex}, priority{other.priority}, globalPriority{other.globalPriority},
			supportedStages{other.supportedStages}, supportedAccess{other.supportedAccess},
			minImageTransferGranularity{other.minImageTransferGranularity}, timestampValidBits{other.timestampValidBits},
			debugName{other.debugName}
		{}
		
		VQueue (const VQueue &other) :
			handle{other.handle}, type{other.type}, familyIndex{other.familyIndex}, familyFlags{other.familyFlags},
			queueIndex{other.queueIndex}, priority{other.priority}, globalPriority{other.globalPriority},
			supportedStages{other.supportedStages}, supportedAccess{other.supportedAccess},
			minImageTransferGranularity{other.minImageTransferGranularity}, timestampValidBits{other.timestampValidBits},
			debugName{other.debugName}
		{}

		ND_ bool  HasUnsupportedStages (VkPipelineStageFlagBits2 stages)	const	{ return AnyBits( stages, ~supportedStages ); }
		ND_ bool  HasUnsupportedAccess (VkAccessFlagBits2 access)			const	{ return AnyBits( access, ~supportedAccess ); }
	};
	


	//
	// Vulkan Pipeline Scope Helper
	//

	struct VPipelineScope final : Noninstancable
	{
	private:
		//	synchronization scope :						first	second
		// VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT			NONE	ALL
		// VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT		ALL		NONE

		static constexpr auto	_HostStageMask =
			VK_PIPELINE_STAGE_2_HOST_BIT;

		static constexpr auto	_TransferStageMask =
			//VK_PIPELINE_STAGE_2_ALL_TRANSFER_BIT |	// same as TRANSFER
			//VK_PIPELINE_STAGE_2_TRANSFER_BIT |		// same as COPY | CLEAR | BLIT | RESOLVE
			VK_PIPELINE_STAGE_2_COPY_BIT;
		
		static constexpr auto	_TransferComputeStageMask =
			_TransferStageMask |
			VK_PIPELINE_STAGE_2_CLEAR_BIT;				// color only
		
		static constexpr auto	_TransferGraphicsStageMask =
			_TransferComputeStageMask |
			VK_PIPELINE_STAGE_2_RESOLVE_BIT |
			VK_PIPELINE_STAGE_2_BLIT_BIT |
			VK_PIPELINE_STAGE_2_CLEAR_BIT;				// color & depth-stencil

		static constexpr auto	_ComputeStageMask =
			VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT |
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

		static constexpr auto	_RTAccelStructBuildStageMask =
			VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT |
			VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		
		static constexpr auto	_RayTracingStageMask =
			VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT |
			VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;

		static constexpr auto	_GraphicsStageMask = 
			//VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT |
			VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT |
			VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT |
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT |
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT |
			VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT |
			VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT |
			VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_NV |
			VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_NV |
			VK_PIPELINE_STAGE_2_PRE_RASTERIZATION_SHADERS_BIT |
			VK_PIPELINE_STAGE_2_FRAGMENT_DENSITY_PROCESS_BIT_EXT |
			VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR |
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
			VK_PIPELINE_STAGE_2_SUBPASS_SHADING_BIT_HUAWEI |
			VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		static constexpr auto	_VideoStageMask =
			VK_PIPELINE_STAGE_2_VIDEO_DECODE_BIT_KHR |
			VK_PIPELINE_STAGE_2_VIDEO_ENCODE_BIT_KHR;
		#else
		static constexpr auto	_VideoStageMask = 0;
		#endif
	

		static constexpr auto	_HostAccessMask =
			VK_ACCESS_2_HOST_READ_BIT |
			VK_ACCESS_2_HOST_WRITE_BIT;

		static constexpr auto	_TransferAccessMask =
			VK_ACCESS_2_TRANSFER_READ_BIT |
			VK_ACCESS_2_TRANSFER_WRITE_BIT;

		static constexpr auto	_ShaderAccessMask =
			//VK_ACCESS_2_SHADER_READ_BIT |		// same as UNIFORM_READ | SHADER_SAMPLED_READ | SHADER_STORAGE_READ
			//VK_ACCESS_2_SHADER_WRITE_BIT |	// same as STORAGE_WRITE
			VK_ACCESS_2_UNIFORM_READ_BIT |
			VK_ACCESS_2_SHADER_SAMPLED_READ_BIT |
			VK_ACCESS_2_SHADER_STORAGE_READ_BIT |
			VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT |
			VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR;

		static constexpr auto	_DeviceCommandAccessMask =
			VK_ACCESS_2_COMMAND_PREPROCESS_READ_BIT_NV |
			VK_ACCESS_2_COMMAND_PREPROCESS_WRITE_BIT_NV |
			VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;

		static constexpr auto	_ComputeAccessMask =
			VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT |
			_ShaderAccessMask;

		static constexpr auto	_RayTracingAccessMask =
			VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT |
			VK_ACCESS_2_SHADER_STORAGE_READ_BIT |		// SBT
			_ShaderAccessMask;
		
		static constexpr auto	_RTAccelStructBuildAccessMask =
			VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR |	// acceleration structure, vertex, index, transform, AABB, instance buffers
			VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;	// acceleration structure and scratch buffer

		static constexpr auto	_GraphicsAccessMask =
			VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT |
			VK_ACCESS_2_INDEX_READ_BIT |
			VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT |
			VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT |
			_ShaderAccessMask |
			VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR |
			VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
		
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		static constexpr auto	_VideoAccessMask =
			VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR |
			VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR |
			VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR |
			VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR;
		#else
		static constexpr auto	_VideoAccessMask = 0;
		#endif
		
		static constexpr auto	_AllAccessMask =
			VK_ACCESS_2_MEMORY_READ_BIT |
			VK_ACCESS_2_MEMORY_WRITE_BIT |
			_HostAccessMask |
			_TransferAccessMask |
			_ComputeAccessMask |
			_RayTracingAccessMask |
			_RTAccelStructBuildAccessMask |
			_GraphicsAccessMask |
			_VideoAccessMask;


		static constexpr auto	_ReadOnlyAccessMask =
			VK_ACCESS_2_HOST_READ_BIT |
			VK_ACCESS_2_MEMORY_READ_BIT |
			VK_ACCESS_2_TRANSFER_READ_BIT |
			VK_ACCESS_2_UNIFORM_READ_BIT |
			VK_ACCESS_2_SHADER_SAMPLED_READ_BIT |
			VK_ACCESS_2_SHADER_STORAGE_READ_BIT |
			VK_ACCESS_2_ACCELERATION_STRUCTURE_READ_BIT_KHR |
			VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT |
			VK_ACCESS_2_INDEX_READ_BIT |
			VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT |
			VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR |
			VK_ACCESS_2_FRAGMENT_DENSITY_MAP_READ_BIT_EXT
		#ifdef VK_ENABLE_BETA_EXTENSIONS
			| VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR | VK_ACCESS_2_VIDEO_ENCODE_READ_BIT_KHR
		#endif
			;
		static constexpr auto	_WriteOnlyAccessMask =
			VK_ACCESS_2_HOST_WRITE_BIT |
			VK_ACCESS_2_MEMORY_WRITE_BIT |
			VK_ACCESS_2_TRANSFER_WRITE_BIT |
			VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT |
			VK_ACCESS_2_ACCELERATION_STRUCTURE_WRITE_BIT_KHR |
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
		#ifdef VK_ENABLE_BETA_EXTENSIONS
			| VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR | VK_ACCESS_2_VIDEO_ENCODE_WRITE_BIT_KHR
		#endif
			;
		
		static constexpr VkPipelineStageFlagBits2	_StageScopes  [] = {
			_HostStageMask,								// Host
			_GraphicsStageMask,							// Graphics
			_ComputeStageMask,							// Compute
			_TransferGraphicsStageMask,					// Transfer_Graphics
			_TransferStageMask,							// Transfer_Copy
			_RayTracingStageMask,						// RayTracing
			_RTAccelStructBuildStageMask,				// RTAS_Build
			_VideoStageMask,							// Video
			VK_PIPELINE_STAGE_2_NONE,					// None
			VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT		// All
		};
		static constexpr VkAccessFlagBits2			_AccessScopes [] = {
			_HostAccessMask,							// Host
			_GraphicsAccessMask,						// Graphics
			_ComputeAccessMask,							// Compute
			_TransferAccessMask,						// Transfer_Graphics
			_TransferAccessMask,						// Transfer_Copy
			_RayTracingAccessMask,						// RayTracing
			_RTAccelStructBuildAccessMask,				// RTAS_Build
			_VideoAccessMask,							// Video
			VK_ACCESS_2_NONE,							// None
			_AllAccessMask								// All
		};

		STATIC_ASSERT( CountOf(_StageScopes)  == uint(EPipelineScope::_Count) );
		STATIC_ASSERT( CountOf(_AccessScopes) == uint(EPipelineScope::_Count) );

	public:
		ND_ static constexpr auto	GetStages (EPipelineScope scope)	{ return _StageScopes[uint(scope)]; }
		ND_ static constexpr auto	GetAccess (EPipelineScope scope)	{ return _AccessScopes[uint(scope)]; }
		
		ND_ static constexpr auto	GetReadAccess (EPipelineScope scope)	{ return _AccessScopes[uint(scope)] & _ReadOnlyAccessMask; }
		ND_ static constexpr auto	GetWriteAccess (EPipelineScope scope)	{ return _AccessScopes[uint(scope)] & _WriteOnlyAccessMask; }

		ND_ static constexpr auto	GetStages (std::initializer_list<EPipelineScope> scopes)
		{
			VkPipelineStageFlagBits2	result = 0;
			for (auto scope : scopes)
				result |= _StageScopes[uint(scope)];
			return result;
		}

		ND_ static constexpr auto	GetAccess (std::initializer_list<EPipelineScope> scopes)
		{
			VkAccessFlagBits2	result = 0;
			for (auto scope : scopes)
				result |= _AccessScopes[uint(scope)];
			return result;
		}
	};

} // AE::Graphics

#endif // AE_ENABLE_VULKAN

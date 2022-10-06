// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Pipeline
	//

	class VRayTracingPipeline final
	{
	// types
	public:
		struct CreateInfo
		{
			PipelineCompiler::SerializableRayTracingPipeline const&	templCI;
			RayTracingPipelineDesc const&							specCI;
			VPipelineLayoutID										layoutId;
			ArrayView< VPipelinePack::ShaderModuleRef >				shaders;
			PipelineCacheID											cacheId;
			VPipelinePack::Allocator_t *							allocator		= null;
			VTempLinearAllocator *									tempAllocator	= null;
		};

	private:
		template <typename K, typename V>
		using THashMap = FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, VPipelinePack::Allocator_t* >>;

		using NameToHandleAlloc_t	= StdAllocatorRef< Pair<const RayTracingGroupName::Optimized_t, uint>, VPipelinePack::Allocator_t* >;
		using NameToHandle_t		= THashMap< RayTracingGroupName::Optimized_t, uint >;


	// variables
	private:
		VkPipeline					_handle				= Default;
		VkPipelineLayout			_layout				= Default;
		
		EPipelineDynamicState		_dynamicState		= Default;
		EPipelineOpt				_options			= Default;

		Strong<VPipelineLayoutID>	_layoutId;
		InPlace<NameToHandle_t>		_nameToHandle;
		ArrayView<ulong>			_groupHandles;		// allocated by pipeline pack linear allocator
		
		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRayTracingPipeline () {}
		~VRayTracingPipeline ();

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci);
			void  Destroy (VResourceManager &);

		ND_ VkPipeline				Handle ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _handle; }
		ND_ VkPipelineLayout		Layout ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layout; }
		ND_ VkPipelineBindPoint		BindPoint ()		const	{ return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR; }
		ND_ VPipelineLayoutID		LayoutID ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _layoutId; }
		ND_ EPipelineDynamicState	DynamicState ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _dynamicState; }
		
		DEBUG_ONLY(  ND_ StringView  GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN

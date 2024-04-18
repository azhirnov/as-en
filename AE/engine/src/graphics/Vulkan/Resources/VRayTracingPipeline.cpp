// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VRayTracingPipeline.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "VPipelineHelper.inl.h"

namespace AE::Graphics
{
/*
=================================================
	destructor
=================================================
*/
	VRayTracingPipeline::~VRayTracingPipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _handle );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRayTracingPipeline::Create (VResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( (ci.specCI.dynamicState & ~EPipelineDynamicState::RayTracingPipelineMask) == Zero );
		CHECK_ERR( not _handle and not _layout );
		CHECK_ERR( ci.allocator != null );
		CHECK_ERR( ci.tempAllocator != null );
		CHECK_ERR( resMngr.GetFeatureSet().rayTracingPipeline == EFeature::RequireTrue );

		auto*	ppln_layout = resMngr.GetResource( ci.layoutId, True{"incRef"} );
		CHECK_ERR( ppln_layout != null );

		_layout = ppln_layout->Handle();
		_layoutId.Attach( ci.layoutId );

		AutoreleasePplnCache	cache_ptr	{ resMngr, ci.cacheId };
		VkPipelineCache			ppln_cache	= cache_ptr ? cache_ptr->Handle() : Default;

		VkRayTracingPipelineCreateInfoKHR		pipeline_info		= {};
		VkPipelineDynamicStateCreateInfo		dynamic_state_info	= {};

		// TODO: VkPipelineCreateFlags2CreateInfoKHR (VK_KHR_maintenance5)

		const uint	group_count	= uint(ci.templCI.generalShaders.size() + ci.templCI.triangleGroups.size() + ci.templCI.proceduralGroups.size());
		auto*		groups		= ci.tempAllocator->Allocate<VkRayTracingShaderGroupCreateInfoKHR>( group_count );
		CHECK_ERR( groups != null );

		pipeline_info.sType			= VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
		pipeline_info.layout		= _layout;
		pipeline_info.flags			= VEnumCast( ci.specCI.options );
		pipeline_info.groupCount	= group_count;
		pipeline_info.pGroups		= groups;
		pipeline_info.pDynamicState	= &dynamic_state_info;

		pipeline_info.maxPipelineRayRecursionDepth	= ci.specCI.maxRecursionDepth;
		pipeline_info.basePipelineHandle			= Default;
		pipeline_info.basePipelineIndex				= -1;

		CHECK_ERR( SetDynamicState( OUT dynamic_state_info, ci.specCI.dynamicState, false, *ci.tempAllocator ));
		CHECK_ERR( SetShaderStages( OUT pipeline_info.pStages, OUT pipeline_info.stageCount, ci.shaders, ci.specCI.specialization, *ci.tempAllocator ));

		TRY{
			const auto	ToGroupIndex = [] (auto* lhs, auto* rhs)
			{{
				ssize	d = Distance( lhs, rhs );
				return CheckCast<uint>( d );
			}};

			_nameToHandle.CreateTh( NameToHandleAlloc_t{ ci.allocator });	// throw
			_nameToHandle->reserve( group_count );							// throw

			for (auto& gen : ci.templCI.generalShaders)
			{
				GRES_CHECK( gen.shader == VK_SHADER_UNUSED_KHR  or  gen.shader < ci.shaders.size() );

				_nameToHandle->emplace( gen.name, ToGroupIndex( pipeline_info.pGroups, groups ));

				auto&	dst = *(groups++);
				dst.sType				= VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				dst.pNext				= null;
				dst.type				= VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
				dst.generalShader		= gen.shader;
				dst.closestHitShader	= VK_SHADER_UNUSED_KHR;
				dst.anyHitShader		= VK_SHADER_UNUSED_KHR;
				dst.intersectionShader	= VK_SHADER_UNUSED_KHR;
				dst.pShaderGroupCaptureReplayHandle = null;
			}

			for (auto& tri : ci.templCI.triangleGroups)
			{
				GRES_CHECK( tri.closestHit	== VK_SHADER_UNUSED_KHR  or  tri.closestHit	< ci.shaders.size() );
				GRES_CHECK( tri.anyHit		== VK_SHADER_UNUSED_KHR  or  tri.anyHit		< ci.shaders.size() );

				_nameToHandle->emplace( tri.name, ToGroupIndex( pipeline_info.pGroups, groups ));

				auto&	dst = *(groups++);
				dst.sType				= VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				dst.pNext				= null;
				dst.type				= VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
				dst.generalShader		= VK_SHADER_UNUSED_KHR;
				dst.closestHitShader	= tri.closestHit;
				dst.anyHitShader		= tri.anyHit;
				dst.intersectionShader	= VK_SHADER_UNUSED_KHR;
				dst.pShaderGroupCaptureReplayHandle = null;
			}

			for (auto& proc : ci.templCI.proceduralGroups)
			{
				GRES_CHECK( proc.closestHit		== VK_SHADER_UNUSED_KHR  or  proc.closestHit	< ci.shaders.size() );
				GRES_CHECK( proc.anyHit			== VK_SHADER_UNUSED_KHR  or  proc.anyHit		< ci.shaders.size() );
				GRES_CHECK( proc.intersection	== VK_SHADER_UNUSED_KHR  or  proc.intersection	< ci.shaders.size() );

				_nameToHandle->emplace( proc.name, ToGroupIndex( pipeline_info.pGroups, groups ));

				auto&	dst = *(groups++);
				dst.sType				= VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
				dst.pNext				= null;
				dst.type				= VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
				dst.generalShader		= VK_SHADER_UNUSED_KHR;
				dst.closestHitShader	= proc.closestHit;
				dst.anyHitShader		= proc.anyHit;
				dst.intersectionShader	= proc.intersection;
				dst.pShaderGroupCaptureReplayHandle = null;
			}
			GRES_CHECK( groups == pipeline_info.pGroups + pipeline_info.groupCount );
		}
		CATCH_ALL(
			RETURN_ERR( "failed to allocate '_nameToHandle' hash map" );
		)

		auto&	dev	= resMngr.GetDevice();
		VK_CHECK_ERR( dev.vkCreateRayTracingPipelinesKHR( dev.GetVkDevice(), Default, ppln_cache, 1, &pipeline_info, null, OUT &_handle ));

		// get shader group handles
		{
			const Bytes	data_size	= Bytes{dev.GetVProperties().rayTracingPipelineProps.shaderGroupHandleSize} * pipeline_info.groupCount;
			auto*		dst			= ci.allocator->Allocate( SizeAndAlign{ data_size, SizeOf<ulong> });
			CHECK_ERR( dst != null );

			VK_CHECK_ERR( dev.vkGetRayTracingShaderGroupHandlesKHR( dev.GetVkDevice(), _handle, 0, pipeline_info.groupCount, usize(data_size), dst ));
			_groupHandles = ArrayView<ulong>{ Cast<ulong>(dst), usize(data_size) };
		}

		dev.SetObjectName( _handle, ci.specCI.dbgName, VK_OBJECT_TYPE_PIPELINE );

		_dynamicState	= ci.specCI.dynamicState;
		_options		= ci.specCI.options;

		CopyShaderTrace( ci.shaders, ci.allocator, OUT _dbgTrace );

		DEBUG_ONLY( _debugName = ci.specCI.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VRayTracingPipeline::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _handle != Default )
			dev.vkDestroyPipeline( dev.GetVkDevice(), _handle, null );

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_nameToHandle.Destroy();

		_handle			= Default;
		_layout			= Default;
		_layoutId		= Default;
		_dynamicState	= Default;
		_options		= Default;
		_groupHandles	= Default;
		_dbgTrace		= Default;

		DEBUG_ONLY( _debugName.clear(); )
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  VRayTracingPipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		result.clear();
		DRC_SHAREDLOCK( _drCheck );

		for (auto& trace : _dbgTrace)
		{
			Array<String>	temp;
			CHECK_ERR( trace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT temp ));
			result.insert( result.end(), temp.begin(), temp.end() );
		}
		return true;
	}

/*
=================================================
	CopyHandle
=================================================
*/
	bool  VRayTracingPipeline::CopyHandle (const VDevice &dev, RayTracingGroupName::Ref name, OUT void* dst, Bytes dstSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const Bytes		handle_size	{dev.GetVProperties().rayTracingPipelineProps.shaderGroupHandleSize};
		CHECK_ERR( dstSize <= handle_size );

		auto	it = _nameToHandle->find( name );
		if ( it != _nameToHandle->end() )
		{
			const Bytes		offset = it->second * handle_size;
			GRES_CHECK( offset + handle_size <= ArraySizeOf(_groupHandles) );

			MemCopy( OUT dst, _groupHandles.data() + offset, handle_size );
			return true;
		}
		else
		{
			// zero handle makes shader inactive
			ZeroMem( OUT dst, handle_size );
			return false;
		}
	}

	bool  VRayTracingPipeline::CopyHandle (const VDevice &dev, uint index, OUT void* dst, Bytes dstSize) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const Bytes		handle_size	{dev.GetVProperties().rayTracingPipelineProps.shaderGroupHandleSize};
		const Bytes		offset		= index * handle_size;

		CHECK_ERR( dstSize <= handle_size );

		if ( offset + handle_size <= ArraySizeOf(_groupHandles) )
		{
			MemCopy( OUT dst, _groupHandles.data() + offset, handle_size );
			return true;
		}
		else
		{
			// zero handle makes shader inactive
			ZeroMem( OUT dst, handle_size );
			return false;
		}
	}

/*
=================================================
	GetShaderGroupStackSize
=================================================
*/
	Bytes  VRayTracingPipeline::GetShaderGroupStackSize (const VDevice &dev, RayTracingGroupName::Ref name, VkShaderGroupShaderKHR type) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto	it = _nameToHandle->find( name );
		CHECK_ERR( it != _nameToHandle->end() );

		// TODO: 'type' is not validated, this is not safe!
		return Bytes{ dev.vkGetRayTracingShaderGroupStackSizeKHR( dev.GetVkDevice(), _handle, it->second, type )};
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

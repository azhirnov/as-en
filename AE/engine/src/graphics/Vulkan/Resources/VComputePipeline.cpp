// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VComputePipeline.h"
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
	VComputePipeline::~VComputePipeline () __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK( not _handle );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VComputePipeline::Create (VResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( ci.shader.IsValid() );
		CHECK_ERR( (ci.specCI.dynamicState & ~EPipelineDynamicState::ComputePipelineMask) == Zero );
		CHECK_ERR( not _handle and not _layout );

		auto*	ppln_layout = resMngr.GetResource( ci.layoutId, True{"incRef"} );
		CHECK_ERR( ppln_layout != null );

		_layout = ppln_layout->Handle();
		_layoutId.Attach( ci.layoutId );

		auto&					dev			= resMngr.GetDevice();
		AutoreleasePplnCache	cache_ptr	{ resMngr, ci.cacheId };
		VkPipelineCache			ppln_cache	= cache_ptr ? cache_ptr->Handle() : Default;

		_localSize = ushort3{
			ci.templCI.localSizeSpec.x == UMax or ci.specCI.localSize.x == UMax ? ci.templCI.defaultLocalSize.x : ci.specCI.localSize.x,
			ci.templCI.localSizeSpec.y == UMax or ci.specCI.localSize.y == UMax ? ci.templCI.defaultLocalSize.y : ci.specCI.localSize.y,
			ci.templCI.localSizeSpec.z == UMax or ci.specCI.localSize.z == UMax ? ci.templCI.defaultLocalSize.z : ci.specCI.localSize.z };
		CHECK_ERR( All( _localSize > Zero ));

		VkComputePipelineCreateInfo		pipeline_info = {};

		// TODO: VkPipelineCreateFlags2CreateInfoKHR (VK_KHR_maintenance5)

		pipeline_info.sType			= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipeline_info.layout		= _layout;
		pipeline_info.flags			= VEnumCast( ci.specCI.options );

		pipeline_info.stage.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipeline_info.stage.flags	= 0;
		pipeline_info.stage.stage	= VK_SHADER_STAGE_COMPUTE_BIT;
		pipeline_info.stage.module	= ci.shader.module;
		pipeline_info.stage.pName	= ci.shader.Entry();

		pipeline_info.basePipelineHandle= Default;
		pipeline_info.basePipelineIndex	= -1;

		const auto	AddCustomSpec = [&ci, this] (VkShaderStageFlagBits, VkSpecializationMapEntry* entryArr, uint* dataArr, OUT uint &count)
		{{
			count = Sum<uint>( ci.templCI.localSizeSpec != UMax );

			if ( entryArr != null )
			{
				uint	idx = 0;

				if ( ci.templCI.localSizeSpec.x != UMax )
				{
					auto&	entry = entryArr[idx];
					entry.constantID	= ci.templCI.localSizeSpec.x;
					entry.offset		= sizeof(uint) * idx;
					entry.size			= sizeof(uint);
					dataArr[idx]		= _localSize.x;
					++idx;
				}
				if ( ci.templCI.localSizeSpec.y != UMax )
				{
					auto&	entry = entryArr[idx];
					entry.constantID	= ci.templCI.localSizeSpec.y;
					entry.offset		= sizeof(uint) * idx;
					entry.size			= sizeof(uint);
					dataArr[idx]		= _localSize.y;
					++idx;
				}
				if ( ci.templCI.localSizeSpec.z != UMax )
				{
					auto&	entry = entryArr[idx];
					entry.constantID	= ci.templCI.localSizeSpec.z;
					entry.offset		= sizeof(uint) * idx;
					entry.size			= sizeof(uint);
					dataArr[idx]		= _localSize.z;
					++idx;
				}

				ASSERT( count == idx );
			}
		}};

		VTempLinearAllocator	allocator;
		CHECK_ERR( AddSpecialization( OUT pipeline_info.stage.pSpecializationInfo,
									  ci.specCI.specialization, *ci.shader.shaderConstants, allocator,
									  VK_SHADER_STAGE_COMPUTE_BIT, AddCustomSpec ));

		VK_CHECK_ERR( dev.vkCreateComputePipelines( dev.GetVkDevice(), ppln_cache, 1, &pipeline_info, null, OUT &_handle ));

		dev.SetObjectName( _handle, ci.specCI.dbgName, VK_OBJECT_TYPE_PIPELINE );

		_options	= ci.specCI.options;
		_dbgTrace	= ci.shader.dbgTrace;

		DEBUG_ONLY( _debugName = ci.specCI.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VComputePipeline::Destroy (VResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		auto&	dev = resMngr.GetDevice();

		if ( _handle != Default )
			dev.vkDestroyPipeline( dev.GetVkDevice(), _handle, null );

		resMngr.ImmediatelyRelease( INOUT _layoutId );

		_handle		= Default;
		_layout		= Default;
		_layoutId	= Default;
		_localSize	= Default;
		_options	= Default;
		_dbgTrace	= null;

		DEBUG_ONLY( _debugName.clear(); )
	}

/*
=================================================
	ParseShaderTrace
=================================================
*/
	bool  VComputePipeline::ParseShaderTrace (const void* ptr, Bytes maxSize, ShaderDebugger::ELogFormat format, OUT Array<String> &result) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		return _dbgTrace and _dbgTrace->ParseShaderTrace( ptr, maxSize, ConvertLogFormat(format), OUT result );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Resources/VRTShaderBindingTable.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VRTShaderBindingTable::~VRTShaderBindingTable () __NE___
	{
		ASSERT( _bufferId == Default );
		ASSERT( _pipelineId == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRTShaderBindingTable::Create (ResourceManager &resMngr, const CreateInfo &ci) __NE___
	{
		CHECK_ERR( _bufferId == Default and _pipelineId == Default );
		CHECK_ERR( resMngr.GetFeatureSet().rayTracingPipeline == FeatureSet::EFeature::RequireTrue );

		_sbt			= ci.sbt;
		_bufferId		= resMngr.AcquireResource( ci.bufferId );
		_pipelineId		= resMngr.AcquireResource( ci.pipelineId );
		_desc			= ci.desc;

		CHECK_ERR( _bufferId != Default and _pipelineId != Default );

		GFX_DBG_ONLY( _debugName = ci.dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VRTShaderBindingTable::Destroy (ResourceManager &resMngr) __NE___
	{
		resMngr.ImmediatelyRelease( INOUT _bufferId );
		resMngr.ImmediatelyRelease( INOUT _pipelineId );

		_sbt = Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Vulkan/Resources/VPipelinePack.h"

namespace AE::Graphics
{

	//
	// Vulkan Ray Tracing Shader Binding Table
	//

	class VRTShaderBindingTable final
	{
	// types
	public:
		struct CreateInfo
		{
			RTShaderBindingTable		sbt;
			BufferID					bufferId;
			RayTracingPipelineID		pipelineId;
			RTShaderBindingDesc			desc;
			StringView					dbgName;
		};


	// variables
	private:
		RTShaderBindingTable			_sbt;
		Strong<BufferID>				_bufferId;
		Strong<RayTracingPipelineID>	_pipelineId;

		RTShaderBindingDesc				_desc;

		GFX_DBG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(		RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		VRTShaderBindingTable ()									__NE___	{}
		~VRTShaderBindingTable ()									__NE___;

		ND_ bool  Create (VResourceManager &, const CreateInfo &ci)	__NE___;
			void  Destroy (VResourceManager &)						__NE___;

		ND_ RTShaderBindingTable const&	GetSBT ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _sbt; }
		ND_ BufferID					BufferId ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _bufferId; }
		ND_ RayTracingPipelineID		RTPipelineId ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pipelineId; }

		ND_ RTShaderBindingDesc const&	Description ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _desc; }

		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()				C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

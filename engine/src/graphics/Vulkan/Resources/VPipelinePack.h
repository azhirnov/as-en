// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "base/Containers/InPlace.h"
# include "base/Memory/LinearAllocator.h"
# include "serializing/Common.h"

# include "threading/Primitives/DataRaceCheck.h"
# include "threading/Memory/LfLinearAllocator.h"

# include "graphics/Public/ResourceManager.h"

# include "PipelineCompilerImpl.h"

# include "graphics/Vulkan/VCommon.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"

namespace AE::Graphics
{
	class VComputePipeline;
	class VGraphicsPipeline;
	class VMeshPipeline;
	class VTilePipeline;
	class VRayTracingPipeline;
	class VPipelineLayout;
	class VRTShaderBindingTable;

	using ShaderTracePtr = Ptr< const PipelineCompiler::ShaderTrace >;


	//
	// Vulkan Pipeline Pack
	//

	class VPipelinePack
	{
	// types
	public:
		struct ShaderModuleRef
		{
			VkShaderStageFlagBits									stage			= Zero;
			VkShaderModule											module			= Default;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			ShaderTracePtr											dbgTrace;
			
			ND_ bool		IsValid ()	C_NE___	{ return module != Default and shaderConstants != null; }
			ND_ const char*	Entry ()	C_NE___	{ return "Main"; }
		};
		
	private:

		//
		// Shader Module
		//
		struct alignas(AE_CACHE_LINE) ShaderModule
		{
			Threading::RWSpinLock										guard;		// protects 'module', 'dbgTrace', 'constants'
			Bytes32u													offset;
			Bytes32u													dataSize;
			ubyte														shaderTypeIdx	= UMax;
			mutable VkShaderModule										module			= Default;
			mutable Unique< PipelineCompiler::ShaderTrace >				dbgTrace;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );


		#include "graphics/Private/PipelinePackDecl.h"


	// variables
	private:
		GfxMemAllocatorPtr		_sbtAllocator;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

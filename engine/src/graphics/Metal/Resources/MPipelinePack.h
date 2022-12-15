// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/Containers/InPlace.h"
# include "base/Memory/LinearAllocator.h"
# include "serializing/Common.h"

# include "threading/Primitives/DataRaceCheck.h"
# include "threading/Memory/LfLinearAllocator.h"

# include "graphics/Public/ResourceManager.h"

# include "PipelineCompilerImpl.h"

# include "graphics/Metal/MCommon.h"
# include "graphics/Metal/Resources/MSampler.h"
//# include "graphics/Metal/Resources/MRenderPass.h"

namespace AE::Graphics
{
	class MComputePipeline;
	class MGraphicsPipeline;
	class MMeshPipeline;
	class MTilePipeline;
	class MRayTracingPipeline;
	class MPipelineLayout;
	class MRTShaderBindingTable;


	//
	// Metal Pipeline Pack
	//

	class MPipelinePack final
	{
	// types
	public:
		struct ShaderModuleRef
		{
			EShader													type			= Default;
			MetalLibrary											lib;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			
			ND_ bool		IsValid ()	C_NE___	{ return bool{lib} and shaderConstants != null; }
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
			mutable MetalLibraryRC										lib;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );
		

		#include "graphics/Private/PipelinePackDecl.h"
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL

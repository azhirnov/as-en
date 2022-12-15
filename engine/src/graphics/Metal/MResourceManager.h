// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "threading/Containers/LfIndexedPool3.h"
# include "threading/Containers/LfStaticIndexedPool.h"

# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Metal/MDevice.h"
# include "graphics/Metal/MSwapchain.h"

# include "graphics/Metal/Resources/MBuffer.h"
# include "graphics/Metal/Resources/MBufferView.h"
# include "graphics/Metal/Resources/MImage.h"
# include "graphics/Metal/Resources/MImageView.h"
# include "graphics/Metal/Resources/MSampler.h"
# include "graphics/Metal/Resources/MMemoryObject.h"
# include "graphics/Metal/Resources/MComputePipeline.h"
# include "graphics/Metal/Resources/MGraphicsPipeline.h"
# include "graphics/Metal/Resources/MTilePipeline.h"
# include "graphics/Metal/Resources/MMeshPipeline.h"
# include "graphics/Metal/Resources/MRayTracingPipeline.h"
# include "graphics/Metal/Resources/MPipelinePack.h"
# include "graphics/Metal/Resources/MDescriptorSet.h"
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"
# include "graphics/Metal/Resources/MPipelineLayout.h"
# include "graphics/Metal/Resources/MRenderPass.h"
# include "graphics/Metal/Resources/MRTGeometry.h"
# include "graphics/Metal/Resources/MRTScene.h"
# include "graphics/Metal/Resources/MPipelineCache.h"
# include "graphics/Metal/Resources/MRTShaderBindingTable.h"

# include "graphics/Metal/Resources/MStagingBufferManager.h"
# include "graphics/Metal/Resources/MQueryManager.h"

# include "graphics/Metal/Descriptors/MDefaultDescriptorAllocator.h"

namespace AE::Graphics
{

	//
	// Metal Resource Manager
	//

	class MResourceManager final : public IResourceManager
	{
		#include "graphics/Private/ResourceManagerDecl.h"

	// methods
	public:
		ND_ bool	CreateBufferAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalBufferRC &buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)			__NE___;
		ND_ bool	CreateImageAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalImageRC &image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)				__NE___;
		ND_ bool	CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ bool	CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)		__NE___;

		ND_ Strong<PipelineCacheID>	LoadPipelineCache (const Path &filename) 						__NE___;
		
		ND_ MetalSampler			GetMtlSampler (const SamplerName &name)							C_NE___;
		ND_ Strong<MSamplerID>		CreateSampler (const SamplerDesc &desc, StringView dbgName)		__NE___;
		
		ND_ Strong<MRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableMtlRenderPass &mtlInfo)	__NE___;
	};
	
	
#	include "graphics/Private/ResourceManagerImpl.h"

} // AE::Graphics

#endif // AE_ENABLE_METAL

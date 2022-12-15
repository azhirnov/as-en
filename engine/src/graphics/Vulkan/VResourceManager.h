// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/ResourceManager.h"
# include "graphics/Private/ResourceBase.h"
# include "graphics/Vulkan/VDevice.h"
# include "graphics/Vulkan/VSwapchain.h"

# include "graphics/Vulkan/Resources/VBuffer.h"
# include "graphics/Vulkan/Resources/VBufferView.h"
# include "graphics/Vulkan/Resources/VDescriptorSet.h"
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"
# include "graphics/Vulkan/Resources/VFramebuffer.h"
# include "graphics/Vulkan/Resources/VImage.h"
# include "graphics/Vulkan/Resources/VImageView.h"
# include "graphics/Vulkan/Resources/VPipelineLayout.h"
# include "graphics/Vulkan/Resources/VComputePipeline.h"
# include "graphics/Vulkan/Resources/VGraphicsPipeline.h"
# include "graphics/Vulkan/Resources/VRayTracingPipeline.h"
# include "graphics/Vulkan/Resources/VMeshPipeline.h"
# include "graphics/Vulkan/Resources/VTilePipeline.h"
# include "graphics/Vulkan/Resources/VPipelinePack.h"
# include "graphics/Vulkan/Resources/VPipelineCache.h"
# include "graphics/Vulkan/Resources/VRenderPass.h"
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/Resources/VRTGeometry.h"
# include "graphics/Vulkan/Resources/VRTScene.h"
# include "graphics/Vulkan/Resources/VMemoryObject.h"
# include "graphics/Vulkan/Resources/VRTShaderBindingTable.h"

# include "graphics/Vulkan/Resources/VStagingBufferManager.h"
# include "graphics/Vulkan/Resources/VQueryManager.h"

# include "threading/Containers/LfIndexedPool3.h"
# include "threading/Containers/LfStaticIndexedPool.h"

namespace AE::Graphics
{

	//
	// Vulkan Resource Manager
	//

	class VResourceManager final : public IResourceManager
	{
		#include "graphics/Private/ResourceManagerDecl.h"

	// methods
	public:
		ND_ Strong<VMemoryID>		CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ Strong<VMemoryID>		CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;

		ND_ Strong<PipelineCacheID>	LoadPipelineCache (RC<RStream> stream)						__NE___;

		ND_ Strong<VSamplerID>		CreateSampler (const VkSamplerCreateInfo &info, StringView dbgName = Default)	__NE___;
		ND_ VkSampler				GetVkSampler (const SamplerName &name)											C_NE___;
		
		ND_ Strong<VRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, VRenderPassID compatId)	__NE___;
		ND_ VFramebufferID			CreateFramebuffer (const RenderPassDesc &desc)					__NE___;
			void					RemoveFramebufferCache (VFramebuffer::CachePtr_t iter)			__NE___;	// call from VFramebuffer

		template <typename ID>
		void  DelayedDestroy (ID res)																__NE___;

	private:

		// resource api
			void  _DestroyVkResource (const VExpiredResource &res)									__NE___;
	};

	
#	include "graphics/Private/ResourceManagerImpl.h"

/*
=================================================
	DelayedDestroy
=================================================
*/
	template <typename ID>
	void  VResourceManager::DelayedDestroy (ID id) __NE___
	{
		STATIC_ASSERT( AllVkResources_t::HasType<ID> );
		ASSERT( id != Default );

		VExpiredResource	res;
		res.id		= UnsafeBitCast< VExpiredResource::IDValue_t >( id );
		res.type	= uint( AllVkResources_t::Index<ID> );

		auto&	dst = _expiredResources.GetCurrent();
		EXLOCK( dst.guard );

		dst.resources.push_back( res );
	}
	
/*
=================================================
	RemoveFramebufferCache
=================================================
*/
	inline void  VResourceManager::RemoveFramebufferCache (VFramebuffer::CachePtr_t iter) __NE___
	{
		EXLOCK( _resPool.fbCacheGuard );
		_resPool.fbCache.erase( iter );
	}

/*
=================================================
	Release Expired Resources Task
=================================================
*/
	class VResourceManager::VReleaseExpiredResourcesTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID	_frameId;

	public:
		explicit VReleaseExpiredResourcesTask (FrameUID frameId) __NE___ :
			IAsyncTask{ETaskQueue::Renderer}, _frameId{frameId}
		{}
			
		void  Run () override;

		StringView  DbgName () C_NE_OV { return "ReleaseExpiredResources"; }
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

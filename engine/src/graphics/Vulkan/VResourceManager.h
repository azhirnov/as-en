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
		friend class VRenderTaskScheduler;

	// types
	public:
		class ReleaseExpiredResourcesTask;

	private:
		template <typename T, typename ID, usize ChunkSize, usize MaxChunks>
		using PoolTmpl		= Threading::LfIndexedPool3< ResourceBase<T>, typename ID::Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

		template <typename T, typename ID, usize PoolSize>
		using StPoolTmpl	= Threading::LfStaticIndexedPool< ResourceBase<T>, typename ID::Index_t, PoolSize, GlobalLinearAllocatorRef >;

		// chunk size
		static constexpr uint	MaxImages		= 1u << 10;
		static constexpr uint	MaxBuffers		= 1u << 10;
		static constexpr uint	MaxMemoryObjs	= 1u << 10;
		static constexpr uint	MaxDSLayouts	= 1u << 9;
		static constexpr uint	MaxDescSets		= 1u << 9;
		static constexpr uint	MaxRTObjects	= 1u << 9;
		static constexpr uint	MaxPipelines	= 1u << 10;
		
		using MemObjPool_t			= PoolTmpl< VMemoryObject,			VMemoryID,				MaxMemoryObjs,	64 >;
		using BufferPool_t			= PoolTmpl< VBuffer,				BufferID,				MaxBuffers,		32 >;
		using BufferViewPool_t		= PoolTmpl< VBufferView,			BufferViewID,			MaxBuffers,		32 >;
		using ImagePool_t			= PoolTmpl< VImage,					ImageID,				MaxImages,		32 >;
		using ImageViewPool_t		= PoolTmpl< VImageView,				ImageViewID,			MaxImages,		32 >;
		using DSLayoutPool_t		= PoolTmpl<	VDescriptorSetLayout,	DescriptorSetLayoutID,	MaxDSLayouts,	 8 >;
		using PplnLayoutPool_t		= PoolTmpl<	VPipelineLayout,		VPipelineLayoutID,		MaxDSLayouts,	 8 >;
		using GPipelinePool_t		= PoolTmpl< VGraphicsPipeline,		GraphicsPipelineID,		MaxPipelines,	32 >;
		using CPipelinePool_t		= PoolTmpl< VComputePipeline,		ComputePipelineID,		MaxPipelines,	32 >;
		using MPipelinePool_t		= PoolTmpl< VMeshPipeline,			MeshPipelineID,			MaxPipelines,	32 >;
		using RTPipelinePool_t		= PoolTmpl< VRayTracingPipeline,	RayTracingPipelineID,	MaxPipelines,	 8 >;
		using TPipelinePool_t		= PoolTmpl< VTilePipeline,			TilePipelineID,			MaxPipelines,	 8 >;
		using DescSetPool_t			= PoolTmpl< VDescriptorSet,			DescriptorSetID,		MaxDescSets,	 8 >;
		using RTGeomPool_t			= PoolTmpl< VRTGeometry,			RTGeometryID,			MaxRTObjects,	 8 >;
		using RTScenePool_t			= PoolTmpl< VRTScene,				RTSceneID,				MaxRTObjects,	 8 >;
		using SamplerPool_t			= StPoolTmpl< VSampler,				VSamplerID,				4096 >;
		using RenderPassPool_t		= StPoolTmpl< VRenderPass,			VRenderPassID,			1024 >;
		using PipelineCachePool_t	= StPoolTmpl< VPipelineCache,		PipelineCacheID,		  64 >;
		using FramebufferPool_t		= StPoolTmpl< VFramebuffer,			VFramebufferID,			 128 >;
		using PipelinePackPool_t	= StPoolTmpl< VPipelinePack,		PipelinePackID,			  32 >;
		using FramebufferCache_t	= VFramebuffer::FramebufferCache_t;

		struct ResourceDestructor;
		using AllResourceIDs_t		= TypeList< BufferViewID, ImageViewID, BufferID, ImageID,
												PipelineCacheID, DescriptorSetLayoutID, VPipelineLayoutID,
												VSamplerID, VFramebufferID, VRenderPassID, 
												GraphicsPipelineID, ComputePipelineID, MeshPipelineID, RayTracingPipelineID, TilePipelineID,
												RTGeometryID, RTSceneID,
												VMemoryID	// must be in  the end
											>;

		//---- Expired resources ----//
		using AllVkResources_t		= TypeList< VkBuffer, VkBufferView, VkImage, VkImageView, VkPipeline, VkAccelerationStructureKHR, VMemoryID,
												// Can be added to release list to decrease reference counter
												VFramebufferID >;
		struct ExpiredResource
		{
			static constexpr usize	IDSize = AllVkResources_t::ForEach_Max< TypeListUtils::GetTypeSize >();

			using IDValue_t = BitSizeToUInt< IDSize * 8 >;

			IDValue_t	id		= UMax;		// type of AllVkResources_t[type]
			uint		type	= UMax;		// index in AllVkResources_t
		};

		struct alignas(AE_CACHE_LINE) ExpiredResArray
		{
			SpinLock					guard;
			FrameUID					frameId;
			Array< ExpiredResource >	resources;
		};
		using ExpiredResources_t	= StaticArray< ExpiredResArray, GraphicsConfig::MaxFrames*2 >;

		struct ExpiredResources
		{
			AtomicFrameUID			_currentFrameId;
			ExpiredResources_t		_list;

			ExpiredResources () __TH___;

			ND_ ExpiredResArray&	Get (FrameUID id)	__NE___	{ return _list[ uint(id.Unique()) % _list.size() ]; }
			ND_ ExpiredResArray&	GetCurrent ()		__NE___	{ return Get( GetFrameId() ); }
			ND_ FrameUID			GetFrameId ()		C_NE___	{ return _currentFrameId.load(); }
			ND_ ExpiredResources_t&	All ()				__NE___	{ return _list; }
		};


	// variables
	private:
		VDevice const&			_device;

		struct {
			BufferPool_t			buffers;
			ImagePool_t				images;
			
			BufferViewPool_t		bufferViews;
			ImageViewPool_t			imageViews;

			SamplerPool_t			samplers;

			PipelinePackPool_t		pipelinePacks;
			PipelineCachePool_t		pipelineCache;

			DSLayoutPool_t			dsLayouts;
			PplnLayoutPool_t		pplnLayouts;
			DescSetPool_t			descSet;

			GPipelinePool_t			graphicsPpln;
			CPipelinePool_t			computePpln;
			MPipelinePool_t			meshPpln;
			RTPipelinePool_t		raytracePpln;
			TPipelinePool_t			tilePpln;
			
			RTGeomPool_t			rtGeom;
			RTScenePool_t			rtScene;

			RenderPassPool_t		renderPass;
			FramebufferPool_t		framebuffers;
			
			RWSpinLock				fbCacheGuard;
			FramebufferCache_t		fbCache;

			MemObjPool_t			memObjs;
		}						_resPool;
		
		GfxMemAllocatorPtr		_defaultMemAlloc;
		DescriptorAllocatorPtr	_defaultDescAlloc;

		VStagingBufferManager	_stagingMngr;
		VQueryManager			_queryMngr;
		
		FeatureSet				_featureSet;
		ExpiredResources		_expiredResources;

		StrongAtom<PipelinePackID>		_defaultPack;
		Strong<VSamplerID>				_defaultSampler;
		Strong<DescriptorSetLayoutID>	_emptyDSLayout;

		#ifdef AE_DBG_OR_DEV
		 mutable SharedMutex			_hashToNameGuard;
		 PipelineCompiler::HashToName	_hashToName;		// for debugging
		#endif


	// methods
	private:
		explicit VResourceManager (const VDevice &)			__TH___;

		ND_ bool  Initialize (const GraphicsCreateInfo &)	__TH___;
			void  Deinitialize ()							__NE___;
		

	public:
		~VResourceManager ();

		ND_ Strong<VMemoryID>			CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ Strong<VMemoryID>			CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;

		ND_ Strong<PipelineCacheID>		LoadPipelineCache (RC<RStream> stream)						__NE___;

		ND_ Strong<ComputePipelineID>	CreatePipeline (const VComputePipeline::CreateInfo    &ci)	__NE___;
		ND_ Strong<GraphicsPipelineID>	CreatePipeline (const VGraphicsPipeline::CreateInfo   &ci)	__NE___;
		ND_ Strong<MeshPipelineID>		CreatePipeline (const VMeshPipeline::CreateInfo       &ci)	__NE___;
		ND_ Strong<RayTracingPipelineID>CreatePipeline (const VRayTracingPipeline::CreateInfo &ci)	__NE___;
		ND_ Strong<TilePipelineID>		CreatePipeline (const VTilePipeline::CreateInfo       &ci)	__NE___;

		ND_ Strong<DescriptorSetLayoutID>	CreateDescriptorSetLayout (const VDescriptorSetLayout::CreateInfo &ci)	__NE___;
		ND_ Strong<VPipelineLayoutID>		CreatePipelineLayout (const VPipelineLayout::DescriptorSets_t &descSetLayouts, const VPipelineLayout::PushConstants_t &pushConstants, StringView dbgName = Default) __NE___;
		
		ND_ Strong<VSamplerID>		CreateSampler (const VkSamplerCreateInfo &info, StringView dbgName = Default)	__NE___;
		ND_ VSamplerID				GetSampler (const SamplerName &name)											C_NE___;
		ND_ VkSampler				GetVkSampler (const SamplerName &name)											C_NE___;
		
		ND_ Strong<VRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableVkRenderPass &vkInfo, VRenderPassID compatId)	__NE___;
		ND_ VRenderPassID			GetCompatibleRenderPass (const CompatRenderPassName &name)		C_NE___;
		ND_ VRenderPassID			GetCompatibleRenderPass (const RenderPassName &name)			C_NE___;
		ND_ VRenderPassID			GetRenderPass (const RenderPassName &name)						C_NE___;
		ND_ VFramebufferID			CreateFramebuffer (const RenderPassDesc &desc)					__NE___;
			void					RemoveFramebufferCache (VFramebuffer::CachePtr_t iter)			__NE___;	// call from VFramebuffer

		ND_ bool					GetMemoryInfo (VMemoryID id, OUT VulkanMemoryObjInfo &info)		C_NE___;

		ND_ VDevice const&			GetDevice ()			C_NE___	{ return _device; }
		ND_ VStagingBufferManager&	GetStagingManager ()	__NE___	{ return _stagingMngr; }
		ND_ VQueryManager&			GetQueryManager ()		__NE___	{ return _queryMngr; }

		template <typename ID>
		void  DelayedDestroy (ID res) __NE___;

		AE_GLOBALLY_ALLOC

	private:

	// resource api
			void  _DestroyVkResource (const ExpiredResource &res);
		
		template <typename PplnID>
		ND_ DescSetAndBinding_t  _CreateDescriptorSet (const PplnID &pplnId, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName);

	// resource pool
		ND_ auto&  _GetResourcePool (const VPipelineLayoutID &)				__NE___	{ return _resPool.pplnLayouts; }
		ND_ auto&  _GetResourcePool (const VSamplerID &)					__NE___	{ return _resPool.samplers; }
		ND_ auto&  _GetResourcePool (const VRenderPassID &)					__NE___	{ return _resPool.renderPass; }
		ND_ auto&  _GetResourcePool (const VFramebufferID &)				__NE___	{ return _resPool.framebuffers; }
		ND_ auto&  _GetResourcePool (const VMemoryID &)						__NE___	{ return _resPool.memObjs; }
		
		ND_ StringView  _GetResourcePoolName (const VPipelineLayoutID &)	__NE___	{ return "pplnLayouts"; }
		ND_ StringView  _GetResourcePoolName (const VSamplerID &)			__NE___	{ return "samplers"; }
		ND_ StringView  _GetResourcePoolName (const VRenderPassID &)		__NE___	{ return "renderPass"; }
		ND_ StringView  _GetResourcePoolName (const VMemoryID &)			__NE___	{ return "memObjs"; }

	// memory managment
		ND_ GfxMemAllocatorPtr		_ChooseMemAllocator (GfxMemAllocatorPtr userDefined);
		ND_ DescriptorAllocatorPtr	_ChooseDescAllocator (DescriptorAllocatorPtr userDefined);

	// empty descriptor set layout
		ND_ bool  _CreateEmptyDescriptorSetLayout ();
		ND_ auto  _GetEmptyDescriptorSetLayout ()		{ return *_emptyDSLayout; }

		ND_ bool  _CreateDefaultSampler ();

		#include "graphics/Private/ResourceManagerFn.h"
	};

	
#	define RESMNGR	VResourceManager
#	include "graphics/Private/ResourceManager.h"

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

		ExpiredResource	res;
		res.id		= UnsafeBitCast< ExpiredResource::IDValue_t >( id );
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
	class VResourceManager::ReleaseExpiredResourcesTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID	_frameId;

	public:
		explicit ReleaseExpiredResourcesTask (FrameUID frameId) __NE___ :
			IAsyncTask{EThread::Renderer}, _frameId{frameId}
		{}
			
		void  Run () override;

		StringView  DbgName () C_NE_OV { return "ReleaseExpiredResources"; }
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

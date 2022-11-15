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
# include "graphics/Metal/Resources/MStagingBufferManager.h"

# include "graphics/Metal/Descriptors/MDefaultDescriptorAllocator.h"

namespace AE::Graphics
{

	//
	// Metal Resource Manager
	//

	class MResourceManager final : public IResourceManager
	{
		friend class MRenderTaskScheduler;
		
	// types
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
		
		using MemObjPool_t			= PoolTmpl< MMemoryObject,			MMemoryID,				MaxMemoryObjs,	64 >;
		using BufferPool_t			= PoolTmpl< MBuffer,				BufferID,				MaxBuffers,		32 >;
		using BufferViewPool_t		= PoolTmpl< MBufferView,			BufferViewID,			MaxBuffers,		32 >;
		using ImagePool_t			= PoolTmpl< MImage,					ImageID,				MaxImages,		32 >;
		using ImageViewPool_t		= PoolTmpl< MImageView,				ImageViewID,			MaxImages,		32 >;
		using GPipelinePool_t		= PoolTmpl< MGraphicsPipeline,		GraphicsPipelineID,		MaxPipelines,	32 >;
		using CPipelinePool_t		= PoolTmpl< MComputePipeline,		ComputePipelineID,		MaxPipelines,	32 >;
		using TPipelinePool_t		= PoolTmpl< MTilePipeline,			TilePipelineID,			MaxPipelines,	32 >;
		using MPipelinePool_t		= PoolTmpl< MMeshPipeline,			MeshPipelineID,			MaxPipelines,	32 >;
		using RTPipelinePool_t		= PoolTmpl< MRayTracingPipeline,	RayTracingPipelineID,	MaxPipelines,	 8 >;
		using DSLayoutPool_t		= PoolTmpl<	MDescriptorSetLayout,	DescriptorSetLayoutID,	MaxDSLayouts,	 8 >;
		using PplnLayoutPool_t		= PoolTmpl<	MPipelineLayout,		MPipelineLayoutID,		MaxDSLayouts,	 8 >;
		using DescSetPool_t			= PoolTmpl< MDescriptorSet,			DescriptorSetID,		MaxDescSets,	 8 >;
		using RTGeomPool_t			= PoolTmpl< MRTGeometry,			RTGeometryID,			MaxRTObjects,	 8 >;
		using RTScenePool_t			= PoolTmpl< MRTScene,				RTSceneID,				MaxRTObjects,	 8 >;
		using SamplerPool_t			= StPoolTmpl< MSampler,				MSamplerID,				4096 >;
		using RenderPassPool_t		= StPoolTmpl< MRenderPass,			MRenderPassID,			1024 >;
		using PipelinePackPool_t	= StPoolTmpl< MPipelinePack,		PipelinePackID,			  32 >;
		using PipelineCachePool_t	= StPoolTmpl< MPipelineCache,		PipelineCacheID,		  64 >;
		
		struct ResourceDestructor;
		using AllResourceIDs_t		= TypeList< BufferViewID, ImageViewID, BufferID, ImageID,
												PipelineCacheID, DescriptorSetLayoutID, MPipelineLayoutID,
												MSamplerID, MRenderPassID, 
												GraphicsPipelineID, ComputePipelineID, MeshPipelineID, RayTracingPipelineID, TilePipelineID,
												RTGeometryID, RTSceneID,
												MMemoryID	// must be in the end
											>;


	// variables
	private:
		MDevice const&			_device;
		
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

			MemObjPool_t			memObjs;
		}						_resPool;
		
		FeatureSet				_featureSet;
		
		DescriptorAllocatorPtr	_defaultDescAlloc;
		
		MStagingBufferManager	_stagingMngr;

		StrongAtom<PipelinePackID>		_defaultPack;
		Strong<MSamplerID>				_defaultSampler;

		#ifdef AE_DBG_OR_DEV
		 mutable SharedMutex			_hashToNameGuard;
		 PipelineCompiler::HashToName	_hashToName;		// for debugging
		#endif
		

	// methods
	private:
		explicit MResourceManager (const MDevice &)			__TH___;

		ND_ bool  Initialize (const GraphicsCreateInfo &)	__TH___;
			void  Deinitialize ()							__NE___;
		

	public:
		~MResourceManager ();
		
		ND_ bool				OnSurfaceCreated (const MSwapchain &sw)	__NE___;

		ND_ bool				CreateBufferAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalBufferRC &buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)			__NE___;
		ND_ bool				CreateImageAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalImageRC &image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)				__NE___;
		ND_ bool				CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)	__NE___;
		ND_ bool				CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)		__NE___;

		Strong<PipelineCacheID>	LoadPipelineCache (const Path &filename) __NE___;

		ND_ Strong<ComputePipelineID>	CreatePipeline (const MComputePipeline::CreateInfo    &ci)	__NE___;
		ND_ Strong<GraphicsPipelineID>	CreatePipeline (const MGraphicsPipeline::CreateInfo   &ci)	__NE___;
		ND_ Strong<MeshPipelineID>		CreatePipeline (const MMeshPipeline::CreateInfo       &ci)	__NE___;
		ND_ Strong<RayTracingPipelineID>CreatePipeline (const MRayTracingPipeline::CreateInfo &ci)	__NE___;
		ND_ Strong<TilePipelineID>		CreatePipeline (const MTilePipeline::CreateInfo       &ci)	__NE___;
		
		ND_ Strong<MPipelineLayoutID>	CreatePipelineLayout (const MPipelineLayout::DescriptorSets_t &descSetLayouts, const MPipelineLayout::PushConstants_t &pushConstants, StringView dbgName = Default) __NE___;
		
		ND_ MSamplerID				GetSampler (const SamplerName &name)							C_NE___;
		ND_ MetalSampler			GetMtlSampler (const SamplerName &name)							C_NE___;
		ND_ Strong<MSamplerID>		CreateSampler (const SamplerDesc &desc, StringView dbgName)		__NE___;
		
		ND_ Strong<MRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableMtlRenderPass &mtlInfo);
		ND_ MRenderPassID			GetCompatibleRenderPass (const CompatRenderPassName &name)		C_NE___;
		ND_ MRenderPassID			GetCompatibleRenderPass (const RenderPassName &name)			C_NE___;
		ND_ MRenderPassID			GetRenderPass (const RenderPassName &name)						C_NE___;

		ND_ bool					GetMemoryInfo (MMemoryID id, OUT MetalMemoryObjInfo &info)		C_NE___;
		
		ND_ MDevice const&			GetDevice ()			C_NE___	{ return _device; }
		ND_ MStagingBufferManager&	GetStagingManager ()	__NE___	{ return _stagingMngr; }
		
		AE_GLOBALLY_ALLOC

	private:

	// resource api
		template <typename PplnID>
		ND_ DescSetAndBinding_t  _CreateDescriptorSet (const PplnID &pplnId, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName);
		 
	// resource pool
		ND_ auto&  _GetResourcePool (const MSamplerID &)				__NE___	{ return _resPool.samplers; }
		ND_ auto&  _GetResourcePool (const MPipelineLayoutID &)			__NE___	{ return _resPool.pplnLayouts; }
		ND_ auto&  _GetResourcePool (const MMemoryID &)					__NE___	{ return _resPool.memObjs; }
		ND_ auto&  _GetResourcePool (const MRenderPassID &)				__NE___	{ return _resPool.renderPass; }
		
		ND_ StringView  _GetResourcePoolName (const MSamplerID &)		__NE___	{ return "samplers"; }
		ND_ StringView  _GetResourcePoolName (const MPipelineLayoutID &)__NE___	{ return "pplnLayouts"; }
		ND_ StringView  _GetResourcePoolName (const MMemoryID &)		__NE___	{ return "memObjs"; }
		ND_ StringView  _GetResourcePoolName (const MRenderPassID &)	__NE___	{ return "renderPass"; }
		 
	// 
		ND_ bool  _CreateDefaultSampler ();
		
		ND_ DescriptorAllocatorPtr	_ChooseDescAllocator (DescriptorAllocatorPtr userDefined);

		template <typename ID>
		ND_ auto const&  _GetDescription (ID id) const;


		#include "graphics/Private/ResourceManagerFn.inl.h"
	};
	
	
#	define RESMNGR	MResourceManager
#	include "graphics/Private/ResourceManagerHdr.inl.h"

} // AE::Graphics

#endif // AE_ENABLE_METAL

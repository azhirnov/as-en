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
												MMemoryID	// must be in  the end
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
		explicit MResourceManager (const MDevice &);

		ND_ bool  Initialize (const GraphicsCreateInfo &);
			void  Deinitialize ();
		

	public:
		~MResourceManager ();
		
		ND_ bool				OnSurfaceCreated (const MSwapchain &sw);

		bool					InitializeResources (const PipelinePackDesc &desc) override;
		Strong<PipelinePackID>	LoadPipelinePack (const PipelinePackDesc &desc) override;
		Array<RenderTechName>	GetSupportedRenderTechs (PipelinePackID id) const override;
		
		bool					IsSupported (EMemoryType memType)		const override;
		bool					IsSupported (const BufferDesc &desc)	const override;
		bool					IsSupported (const ImageDesc &desc)		const override;
		bool					IsSupported (BufferID buffer, const BufferViewDesc &desc)	const override;
		bool					IsSupported (ImageID image, const ImageViewDesc &desc)		const override;
		
		Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) override;
		Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null) override;
		
		Strong<ImageID>			CreateImage (const MetalImageDesc &desc, StringView dbgName = Default)		override;
		Strong<BufferID>		CreateBuffer (const MetalBufferDesc &desc, StringView dbgName = Default)	override;
		
		Strong<ImageViewID>		CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)		override;
		Strong<BufferViewID>	CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)	override;

		ND_ bool				CreateBufferAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalBufferRC &buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool				CreateImageAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalImageRC &image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool				CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);
		ND_ bool				CreateAccelStructAndMemoryObj (OUT Strong<MMemoryID> &memId, OUT MetalAccelStructRC &as, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName);

		Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)	override;
		Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			override;
		
		RTASBuildSizes			GetRTGeometrySizes (const RTGeometryBuild &desc) override;
		RTASBuildSizes			GetRTSceneSizes (const RTSceneBuild &desc)		 override;

		DescSetAndBinding_t		CreateDescriptorSet (GraphicsPipelineID   ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (MeshPipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (ComputePipelineID    ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (RayTracingPipelineID ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		DescSetAndBinding_t		CreateDescriptorSet (TilePipelineID       ppln, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		
		Strong<DescriptorSetID>	CreateDescriptorSet (PipelinePackID packId, const DSLayoutName &dslName, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		Strong<DescriptorSetID>	CreateDescriptorSet (DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator = null, StringView dbgName = Default) override;
		
		Strong<PipelineCacheID>	CreatePipelineCache () override;
		Strong<PipelineCacheID>	LoadPipelineCache (const Path &filename);
		
		Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	override;
		RenderTechPipelinesPtr			LoadRenderTech      (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)	override;

		Strong<GraphicsPipelineID>		CreateGraphicsPipeline   (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache = Default)	override;
		Strong<MeshPipelineID>			CreateMeshPipeline       (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache = Default)	override;
		Strong<ComputePipelineID>		CreateComputePipeline    (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache = Default)	override;
		Strong<RayTracingPipelineID>	CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache = Default)	override;
		Strong<TilePipelineID>			CreateTilePipeline       (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache = Default)	override;
		
		ND_ Strong<ComputePipelineID>	CreatePipeline (const MComputePipeline::CreateInfo    &ci);
		ND_ Strong<GraphicsPipelineID>	CreatePipeline (const MGraphicsPipeline::CreateInfo   &ci);
		ND_ Strong<MeshPipelineID>		CreatePipeline (const MMeshPipeline::CreateInfo       &ci);
		ND_ Strong<RayTracingPipelineID>CreatePipeline (const MRayTracingPipeline::CreateInfo &ci);
		ND_ Strong<TilePipelineID>		CreatePipeline (const MTilePipeline::CreateInfo       &ci);
		
		ND_ Strong<MPipelineLayoutID>	CreatePipelineLayout (const MPipelineLayout::DescriptorSets_t &descSetLayouts, const MPipelineLayout::PushConstants_t &pushConstants, StringView dbgName = Default);
		
		ND_ MSamplerID				GetSampler (const SamplerName &name) const;
		ND_ MetalSampler			GetMtlSampler (const SamplerName &name) const;
		ND_ Strong<MSamplerID>		CreateSampler (const SamplerDesc &desc, StringView dbgName);
		
		ND_ Strong<MRenderPassID>	CreateRenderPass (const SerializableRenderPassInfo &info, const SerializableMtlRenderPass &mtlInfo);
		ND_ MRenderPassID			GetCompatibleRenderPass (const CompatRenderPassName &name) const;
		ND_ MRenderPassID			GetCompatibleRenderPass (const RenderPassName &name) const;
		ND_ MRenderPassID			GetRenderPass (const RenderPassName &name) const;

		bool	ReleaseResource (Strong<ImageID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ImageViewID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<BufferViewID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelineCacheID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<PipelinePackID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<GraphicsPipelineID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<MeshPipelineID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<ComputePipelineID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RayTracingPipelineID>&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<TilePipelineID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<DescriptorSetID>	&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTGeometryID>		&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		bool	ReleaseResource (Strong<RTSceneID>			&id) override	{ return _ReleaseResource( id.Release() ) == 0; }
		
		template <usize IS, usize GS, uint UID>
		bool	ReleaseResource (Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)	{ return _ReleaseResource( id.Release(), refCount ) == 0; }

		template <typename Arg0, typename ...Args>
		bool	ReleaseResources (Arg0 &arg0, Args& ...args);

		BufferDesc const&		GetDescription (BufferID id)		const override;
		ImageDesc const&		GetDescription (ImageID id)			const override;
		BufferViewDesc const&	GetDescription (BufferViewID id)	const override;
		ImageViewDesc const&	GetDescription (ImageViewID id)		const override;

		NativeBuffer_t			GetBufferHandle (BufferID id)			const override;
		NativeImage_t			GetImageHandle (ImageID id)				const override;
		NativeBufferView_t		GetBufferViewHandle (BufferViewID id)	const override;
		NativeImageView_t		GetImageViewHandle (ImageViewID id)		const override;

			bool			GetMemoryInfo (ImageID id, OUT MetalMemoryObjInfo &info)	const override;
			bool			GetMemoryInfo (BufferID id, OUT MetalMemoryObjInfo &info)	const override;
		ND_ bool			GetMemoryInfo (MMemoryID id, OUT MetalMemoryObjInfo &info)	const;
		
		template <usize IS, usize GS, uint UID>
		ND_ auto			AcquireResource (HandleTmpl<IS, GS, UID> id);

		template <usize IS, usize GS, uint UID>
		ND_ bool			IsAlive (HandleTmpl<IS,GS,UID> id)	const;
		ND_ bool			IsAlive (const SamplerName &name)	const;

		bool				IsResourceAlive (BufferID		 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageID		 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (BufferViewID	 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (ImageViewID	 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (DescriptorSetID id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelineCacheID id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (PipelinePackID	 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (RTGeometryID	 id) const override		{ return IsAlive( id ); }
		bool				IsResourceAlive (RTSceneID		 id) const override		{ return IsAlive( id ); }
		
		template <typename ID>
		ND_ auto const*		GetResource (ID id, Bool incRef = false, Bool quiet = false) const;
		
		template <typename ID>
		ND_ auto const*		GetResource (const Strong<ID> &id, Bool incRef = false, Bool quiet = false) const;

		ND_ MDevice const&			GetDevice ()			const	{ return _device; }
		ND_ MStagingBufferManager&	GetStagingManager ()			{ return _stagingMngr; }
		ND_ FeatureSet const&		GetFeatureSet ()		const	{ return _featureSet; }
		ND_ PipelinePackID			GetDefaultPack ()		const	{ return _defaultPack; }

		bool  ForceReleaseResources () override;
		
		StagingBufferStat  GetStagingBufferFrameStat (FrameUID frameId) const override	{ return _stagingMngr.GetFrameStat( frameId ); }

		void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg);
		void  OnEndFrame (FrameUID frameId);
		
		template <usize Size, uint UID, bool Opt, uint Seed>
		ND_ String  HashToName (const NamedID< Size, UID, Opt, Seed > &name) const;
		
		#ifdef AE_DBG_OR_DEV
		void  AddHashToName (const PipelineCompiler::HashToName &value);
		#endif
		
		AE_GLOBALLY_ALLOC

	private:

	// resource api 
		template <typename ID>
		ND_ int  _ReleaseResource (ID id, uint refCount = 1);

		template <typename ID, typename ...Args>
		ND_ Strong<ID>  _CreateResource (const char* msg, Args&& ...args);

		template <typename PplnID>
		ND_ DescSetAndBinding_t  _CreateDescriptorSet (const PplnID &pplnId, const DescriptorSetName &dsName, DescriptorAllocatorPtr allocator, StringView dbgName);
		 
	// resource pool
		ND_ auto&  _GetResourcePool (const BufferID &)						{ return _resPool.buffers; }
		ND_ auto&  _GetResourcePool (const ImageID &)						{ return _resPool.images; }
		ND_ auto&  _GetResourcePool (const BufferViewID &)					{ return _resPool.bufferViews; }
		ND_ auto&  _GetResourcePool (const ImageViewID &)					{ return _resPool.imageViews; }
		ND_ auto&  _GetResourcePool (const GraphicsPipelineID &)			{ return _resPool.graphicsPpln; }
		ND_ auto&  _GetResourcePool (const ComputePipelineID &)				{ return _resPool.computePpln; }
		ND_ auto&  _GetResourcePool (const MeshPipelineID &)				{ return _resPool.meshPpln; }
		ND_ auto&  _GetResourcePool (const RayTracingPipelineID &)			{ return _resPool.raytracePpln; }
		ND_ auto&  _GetResourcePool (const TilePipelineID &)				{ return _resPool.tilePpln; }
		ND_ auto&  _GetResourcePool (const MSamplerID &)					{ return _resPool.samplers; }
		ND_ auto&  _GetResourcePool (const DescriptorSetID &)				{ return _resPool.descSet; }
		ND_ auto&  _GetResourcePool (const DescriptorSetLayoutID &)			{ return _resPool.dsLayouts; }
		ND_ auto&  _GetResourcePool (const MPipelineLayoutID &)				{ return _resPool.pplnLayouts; }
		ND_ auto&  _GetResourcePool (const PipelineCacheID &)				{ return _resPool.pipelineCache; }
		ND_ auto&  _GetResourcePool (const PipelinePackID &)				{ return _resPool.pipelinePacks; }
		ND_ auto&  _GetResourcePool (const RTGeometryID &)					{ return _resPool.rtGeom; }
		ND_ auto&  _GetResourcePool (const RTSceneID &)						{ return _resPool.rtScene; }
		ND_ auto&  _GetResourcePool (const MMemoryID &)						{ return _resPool.memObjs; }
		ND_ auto&  _GetResourcePool (const MRenderPassID &)					{ return _resPool.renderPass; }

		template <typename ID>
		ND_ const auto&  _GetResourceCPool (const ID &id)	const			{ return const_cast<MResourceManager *>(this)->_GetResourcePool( id ); }
		
		ND_ StringView  _GetResourcePoolName (const BufferID &)				{ return "buffers"; }
		ND_ StringView  _GetResourcePoolName (const ImageID &)				{ return "images"; }
		ND_ StringView  _GetResourcePoolName (const BufferViewID &)			{ return "bufferViews"; }
		ND_ StringView  _GetResourcePoolName (const ImageViewID &)			{ return "imageViews"; }
		ND_ StringView  _GetResourcePoolName (const GraphicsPipelineID &)	{ return "graphicsPpln"; }
		ND_ StringView  _GetResourcePoolName (const ComputePipelineID &)	{ return "computePpln"; }
		ND_ StringView  _GetResourcePoolName (const MeshPipelineID &)		{ return "meshPpln"; }
		ND_ StringView  _GetResourcePoolName (const RayTracingPipelineID &)	{ return "raytracePpln"; }
		ND_ StringView  _GetResourcePoolName (const TilePipelineID &)		{ return "tilePpln"; }
		ND_ StringView  _GetResourcePoolName (const MSamplerID &)			{ return "samplers"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetID &)		{ return "descSet"; }
		ND_ StringView  _GetResourcePoolName (const DescriptorSetLayoutID &){ return "dsLayouts"; }
		ND_ StringView  _GetResourcePoolName (const MPipelineLayoutID &)	{ return "pplnLayouts"; }
		ND_ StringView  _GetResourcePoolName (const PipelineCacheID &)		{ return "pipelineCache"; }
		ND_ StringView  _GetResourcePoolName (const PipelinePackID &)		{ return "pipelinePacks"; }
		ND_ StringView  _GetResourcePoolName (const RTGeometryID &)			{ return "rtGeometry"; }
		ND_ StringView  _GetResourcePoolName (const RTSceneID &)			{ return "rtScene"; }
		ND_ StringView  _GetResourcePoolName (const MMemoryID &)			{ return "memObjs"; }
		ND_ StringView  _GetResourcePoolName (const MRenderPassID &)		{ return "renderPass"; }
		 
	// 
		template <typename ID>	ND_ bool   _Assign (OUT ID &id);
		template <typename ID>		void   _Unassign (ID id);

		ND_ bool  _CreateDefaultSampler ();
		
		ND_ DescriptorAllocatorPtr	_ChooseDescAllocator (DescriptorAllocatorPtr userDefined);

		template <typename ID>
		ND_ auto const&  _GetDescription (ID id) const;
	};
	
	
#	define RESMNGR	MResourceManager
#	define RESMNGR_HEADER
#	include "graphics/Private/ResourceManagerUtils.h"

} // AE::Graphics

#endif // AE_ENABLE_METAL

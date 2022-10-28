// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Private/PipelinePackBase.h"
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


	//
	// Metal Pipeline Pack
	//

	class MPipelinePack
	{
	// types
	public:
		struct ShaderModuleRef
		{
			EShader													type			= Default;
			MetalLibrary											lib				= Default;
			StringView												entry;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			
			ND_ bool  IsValid () const	{ return lib != Default; }
		};

		using Allocator_t = Threading::LfLinearAllocator< usize(SmallAllocationSize * 16) >;
		
	private:

		//
		// Render Pass Refs
		//
		struct RenderPassRefs
		{
			template <typename K, typename V>
			using THashMap = FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, Allocator_t* >>;

			THashMap< RenderPassName::Optimized_t,		 MRenderPassID >	specMap;
			THashMap< CompatRenderPassName::Optimized_t, MRenderPassID >	compatMap;

			explicit RenderPassRefs (Allocator_t *alloc);
		};

		//
		// Shader Module
		//
		struct alignas(AE_CACHE_LINE) ShaderModule
		{
			Threading::RWSpinLock										guard;		// protects 'module', 'dbgTrace', 'constants'
			Bytes32u													offset;
			Bytes32u													dataSize;
			ubyte														shaderTypeIdx	= UMax;
			mutable MetalShaderFunctionRC								module;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );
		

		struct Types
		{
			using Allocator_t			= MPipelinePack::Allocator_t;
			using TempLinearAllocator_t	= MTempLinearAllocator;
			using ResMngr_t				= MResourceManager;
			using PipelineLayoutID_t	= MPipelineLayoutID;
			using PipelineLayout_t		= MPipelineLayout;
			using SamplerID_t			= MSamplerID;
			using RenderPassRefs_t		= RenderPassRefs;
			using ShaderModule_t		= ShaderModule;
			using ShaderModuleRef_t		= ShaderModuleRef;
			using ComputePipeline_t		= MComputePipeline;
			using GraphicsPipeline_t	= MGraphicsPipeline;
			using MeshPipeline_t		= MMeshPipeline;
			using TilePipeline_t		= MTilePipeline;
			using RayTracingPipeline_t	= MRayTracingPipeline;
		};

		//
		// Pipeline Pack implementation
		//
		class PackImpl final : public PipelinePackBase<	Types >
		{
		// variables
		private:
			DEBUG_ONLY( DebugName_t		_debugName; )


		// methods
		public:
			ND_ bool  Create (MResourceManager &, const PipelinePackDesc &desc);
				void  Destroy (MResourceManager &);
			
			ND_ MRenderPassID			GetRenderPass (const RenderPassName &name) const;
			ND_ MRenderPassID			GetRenderPass (const CompatRenderPassName &name) const;
			
			ND_ EPixelFormat			GetSwapchainFmt	()	const	{ DRC_SHAREDLOCK( _drCheck );  return _swapchainFmt; }

			DEBUG_ONLY( ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		private:
			bool  _LoadDepthStencilStates (MResourceManager &, Serializing::Deserializer &) override;
			bool  _LoadRenderPasses (MResourceManager &, Bytes offset, Bytes size) override;

			ShaderModuleRef	_GetShader (const MResourceManager &, PipelineCompiler::ShaderUID uid, EShader type) const override;
			MSamplerID		_CreateSampler (MResourceManager &, const SamplerDesc &desc) override;

			Strong<DescriptorSetLayoutID>  _CreateDescriptorSetLayout (MResourceManager &, const Uniforms_t &, ArrayView<SamplerID_t>, EDescSetUsage, StackAllocator_t &) override;

			ND_ bool  _CreateShader (const MDevice &, INOUT const ShaderModule &shader) const;
		};


	// variables
	private:
		PackImpl	_packImpl;


	// methods
	public:
		MPipelinePack () {}
		~MPipelinePack () {}

		ND_ bool  Create (MResourceManager &, const PipelinePackDesc &desc);
			void  Destroy (MResourceManager &);
		
		ND_ MSamplerID				GetSampler (const SamplerName &name) const;
		ND_ Array<RenderTechName>	GetSupportedRenderTechs () const;

		ND_ MRenderPassID			GetRenderPass (const RenderPassName &name) const;
		ND_ MRenderPassID			GetRenderPass (const CompatRenderPassName &name) const;
		
		ND_ EPixelFormat			GetSwapchainFmt	() const;

		ND_ Strong<GraphicsPipelineID>		CreatePipeline (MResourceManager &, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache) const;
		ND_ Strong<ComputePipelineID>		CreatePipeline (MResourceManager &, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache) const;
		ND_ Strong<MeshPipelineID>			CreatePipeline (MResourceManager &, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<TilePipelineID>			CreatePipeline (MResourceManager &, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<RayTracingPipelineID>	CreatePipeline (MResourceManager &, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache) const;
		
		ND_ DescriptorSetLayoutID			GetDSLayout (const DSLayoutName &name) const;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (MResourceManager &, const RenderTechName &name, PipelineCacheID cache) const;
		ND_ RenderTechPipelinesPtr			LoadRenderTech (MResourceManager &, const RenderTechName &name, PipelineCacheID cache) const;
		
		ND_ auto const&				GetUnsupportedFS ()	const;
		DEBUG_ONLY( ND_ auto const&	GetAllFS ()			const; )
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		const; )
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL

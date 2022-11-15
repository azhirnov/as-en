// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Private/PipelinePackBase.h"
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
			StringView												entry			= "main";		// TODO
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			ShaderTracePtr											dbgTrace;
			
			ND_ bool  IsValid () const	{ return module != Default; }
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

			THashMap< RenderPassName::Optimized_t,		 VRenderPassID >	specMap;
			THashMap< CompatRenderPassName::Optimized_t, VRenderPassID >	compatMap;

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
			mutable VkShaderModule										module			= Default;
			mutable Unique< PipelineCompiler::ShaderTrace >				dbgTrace;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );
		

		struct Types
		{
			using Allocator_t			= VPipelinePack::Allocator_t;
			using TempLinearAllocator_t	= VTempLinearAllocator;
			using ResMngr_t				= VResourceManager;
			using PipelineLayoutID_t	= VPipelineLayoutID;
			using PipelineLayout_t		= VPipelineLayout;
			using SamplerID_t			= VSamplerID;
			using RenderPassRefs_t		= RenderPassRefs;
			using ShaderModule_t		= ShaderModule;
			using ShaderModuleRef_t		= ShaderModuleRef;
			using ComputePipeline_t		= VComputePipeline;
			using GraphicsPipeline_t	= VGraphicsPipeline;
			using MeshPipeline_t		= VMeshPipeline;
			using TilePipeline_t		= VTilePipeline;
			using RayTracingPipeline_t	= VRayTracingPipeline;
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
			ND_ bool  Create (VResourceManager &, const PipelinePackDesc &desc);
				void  Destroy (VResourceManager &);
			
			ND_ VRenderPassID			GetRenderPass (const RenderPassName &name) const;
			ND_ VRenderPassID			GetRenderPass (const CompatRenderPassName &name) const;
			
			ND_ EPixelFormat			GetSwapchainFmt	()	const	{ DRC_SHAREDLOCK( _drCheck );  return _swapchainFmt; }

			DEBUG_ONLY( ND_ StringView  GetDebugName ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		private:
			bool  _LoadDepthStencilStates (VResourceManager &, Serializing::Deserializer &) override;
			bool  _LoadRenderPasses (VResourceManager &, Bytes offset, Bytes size) override;

			ShaderModuleRef	_GetShader (const VResourceManager &, PipelineCompiler::ShaderUID uid, EShader type) const override;
			VSamplerID		_CreateSampler (VResourceManager &, const SamplerDesc &desc, StringView dbgName) override;

			Strong<DescriptorSetLayoutID>  _CreateDescriptorSetLayout (VResourceManager &, const Uniforms_t &, ArrayView<SamplerID_t>, EDescSetUsage, StackAllocator_t &) override;

			ND_ bool  _CreateShader (const VDevice &, INOUT const ShaderModule &shader) const;
		};


	// variables
	private:
		PackImpl	_packImpl;


	// methods
	public:
		VPipelinePack () {}
		~VPipelinePack () {}

		ND_ bool  Create (VResourceManager &, const PipelinePackDesc &desc);
			void  Destroy (VResourceManager &);
		
		ND_ VSamplerID				GetSampler (const SamplerName &name) const;
		ND_ Array<RenderTechName>	GetSupportedRenderTechs () const;

		ND_ VRenderPassID			GetRenderPass (const RenderPassName &name) const;
		ND_ VRenderPassID			GetRenderPass (const CompatRenderPassName &name) const;
		
		ND_ EPixelFormat			GetSwapchainFmt	() const;

		ND_ Strong<GraphicsPipelineID>		CreatePipeline (VResourceManager &, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache) const;
		ND_ Strong<ComputePipelineID>		CreatePipeline (VResourceManager &, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache) const;
		ND_ Strong<MeshPipelineID>			CreatePipeline (VResourceManager &, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<TilePipelineID>			CreatePipeline (VResourceManager &, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<RayTracingPipelineID>	CreatePipeline (VResourceManager &, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache) const;
		
		ND_ DescriptorSetLayoutID			GetDSLayout (const DSLayoutName &name) const;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (VResourceManager &, const RenderTechName &name, PipelineCacheID cache) const;
		ND_ RenderTechPipelinesPtr			LoadRenderTech (VResourceManager &, const RenderTechName &name, PipelineCacheID cache) const;
		
		ND_ auto const&				GetUnsupportedFS ()	const;
		DEBUG_ONLY( ND_ auto const&	GetAllFS ()			const; )
		DEBUG_ONLY( ND_ StringView  GetDebugName ()		const; )
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Public/ResourceManager.h"
# include "graphics/Public/ShaderDebugger.h"

# include "PipelineCompilerImpl.h"

# include "graphics/Remote/RCommon.h"
# include "graphics/Remote/Resources/RSampler.h"

namespace AE::Graphics
{
	class RComputePipeline;
	class RGraphicsPipeline;
	class RMeshPipeline;
	class RTilePipeline;
	class RRayTracingPipeline;
	class RPipelineLayout;
	class RRTShaderBindingTable;

	using ShaderTracePtr = RemoteGraphics::Msg::ShaderTracePtr;



	//
	// Pipeline Pack
	//

	class RPipelinePack
	{
	// types
	public:
		using RmDSLMap_t	= FixedMap< DescriptorSetName::Optimized_t, Pair< DescSetBinding, RmDescriptorSetLayoutID >, GraphicsConfig::MaxDescriptorSets >;
		using DSLMap_t		= FixedMap< DescriptorSetName::Optimized_t, Pair< DescSetBinding, DescriptorSetLayoutID >,   GraphicsConfig::MaxDescriptorSets >;


		//
		// Render Tech
		//
		class RenderTech final : public IRenderTechPipelines
		{
		// types
		private:
			using PipelineID = uint;	// GraphicsPipelineID, MeshPipelineID, TilePipelineID, ComputePipelineID, RayTracingPipelineID

			struct PipelineInfo
			{
				PipelineCompiler::PipelineSpecUID	uid		= Default;
				PipelineID							pplnId;

				template <typename ID>
				ND_ ID  Cast ()		C_NE___	{ return BitCast<ID>( pplnId ); }
			};

			using PplnSpecMap_t	= FlatHashMap< PipelineName::Optimized_t, PipelineInfo >;
			using RTSBTMap_t	= FlatHashMap< RTShaderBindingName::Optimized_t, Strong<RTShaderBindingID> >;
			using Passes_t		= FlatHashMap< RenderTechPassName::Optimized_t, IRenderTechPipelines::PassInfo >;


		// variables
		private:
			RPipelinePack const &			_pack;
			RmRenderTechPipelinesID			_rtechId;
			RenderTechName::Optimized_t		_name;
			PplnSpecMap_t					_pipelines;
			RTSBTMap_t						_rtSbtMap;
			Passes_t						_passes;

			DRC_ONLY( RWDataRaceCheck		_drCheck; )


		// methods
		public:
			RenderTech (const RPipelinePack &pack)														__NE___;
			~RenderTech ()																				__NE_OV;

			ND_ bool  Create (RResourceManager&,
							  const RemoteGraphics::Msg::ResMngr_LoadRenderTech_Response &)				__NE___;
				void  Destroy (RResourceManager &)														__NE___;

			ND_ RenderTechName::Optimized_t	Name ()														C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _name; }
			ND_ RmRenderTechPipelinesID		Handle ()													C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _rtechId; }

			GraphicsPipelineID		GetGraphicsPipeline	 (PipelineName::Ref name)						C_NE_OV;
			MeshPipelineID			GetMeshPipeline		 (PipelineName::Ref name)						C_NE_OV;
			TilePipelineID			GetTilePipeline		 (PipelineName::Ref name)						C_NE_OV;
			ComputePipelineID		GetComputePipeline	 (PipelineName::Ref name)						C_NE_OV;
			RayTracingPipelineID	GetRayTracingPipeline(PipelineName::Ref name)						C_NE_OV;
			RTShaderBindingID		GetRTShaderBinding	 (RTShaderBindingName::Ref name)				C_NE_OV;
			PassInfo				GetPass				 (RenderTechPassName::Ref pass)					C_NE_OV;
			bool					FeatureSetSupported  (FeatureSetName::Ref name)						C_NE_OV;
			EPixelFormat			GetAttachmentFormat  (RenderTechPassName::Ref, AttachmentName::Ref)	C_NE_OV;

		private:
			void  _PrintPipelines (PipelineName::Ref name, PipelineCompiler::PipelineSpecUID mask)		C_NE___;
			void  _PrintSBTs (RTShaderBindingName::Ref reqName)											C_NE___;
		};

	private:
		using FeatureNames_t	= FlatHashSet< FeatureSetName::Optimized_t >;
		using SamplerRefs_t		= FlatHashMap< SamplerName::Optimized_t,		Strong<SamplerID> >;
		using PplnTemplMap_t	= FlatHashMap< PipelineTmplName::Optimized_t,	PipelineCompiler::PipelineTemplUID >;

		using RmToDSLayout_t	= FlatHashMap< RmDescriptorSetLayoutID, Strong< DescriptorSetLayoutID >>;
		using NameToDSLayout_t	= FlatHashMap< DSLayoutName::Optimized_t, DescriptorSetLayoutID >;
		using RmPLToPL_t		= FlatHashMap< RmPipelineLayoutID, Strong< PipelineLayoutID >>;

		using RenderPassMap_t	= FlatHashMap< RenderPassName::Optimized_t, Strong<RenderPassID> >;

		using LinearAllocator_t	= Threading::LfLinearAllocator< usize(SmallAllocationSize * 16), usize{16_b} >;

		using EMarker			= PipelineCompiler::PipelineStorage::EMarker;
		using RTechArr_t		= Synchronized< SharedMutex, Array< RC<RenderTech> >>;


	// variables
	private:
		RmPipelinePackID					_packId;

		RC<IAllocatorTS>					_allocator;

		mutable RTechArr_t					_renTechs;

		FeatureNames_t						_unsupportedFS;
		SamplerRefs_t						_samplerRefs;

		RmToDSLayout_t						_rmToDSLayout;
		NameToDSLayout_t					_nameToDSLayout;
		RmPLToPL_t							_rmToPplnLayout;

		RenderPassMap_t						_renderPassMap;

		Ptr<const RPipelinePack>			_parentPack;
		Strong<PipelinePackID>				_parentPackId;

		DRC_ONLY(		RWDataRaceCheck		_drCheck; )
		GFX_DBG_ONLY(	DebugName_t			_debugName; )


	// methods
	public:
		RPipelinePack ()														__NE___	{}
		~RPipelinePack ()														__NE___	{}

		ND_ RmPipelinePackID			Handle ()								C_NE___	{ return _packId; }
		GFX_DBG_ONLY( ND_ StringView	GetDebugName ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ auto  GetRenderPass (RenderPassName::Ref name)						C_NE___ -> RenderPassID;

		ND_ auto  Cast (RmDescriptorSetLayoutID)								C_NE___ -> DescriptorSetLayoutID;
		ND_ auto  Cast (RmPipelineLayoutID)										C_NE___ -> PipelineLayoutID;

		ND_ bool  ConvertDescSetsLayouts (const RmDSLMap_t &, OUT DSLMap_t &)	C_NE___;

		ND_ bool  Create (RResourceManager &, const PipelinePackDesc &)			__NE___;
			void  Destroy (RResourceManager &)									__NE___;

		ND_ SamplerID	GetSampler (SamplerName::Ref name)						C_NE___;

		ND_ DescriptorSetLayoutID	GetDSLayout (DSLayoutName::Ref name)		C_NE___;

	private:
		ND_ bool  _LoadPipelineBlock (RResourceManager &,
									  const PipelinePackDesc &,
									  ArrayView<RmDescriptorSetLayoutID>,
									  ArrayView<RmPipelineLayoutID>,
									  Bytes baseOffset)							__NE___;

		ND_ bool  _LoadDescrSetLayouts (RResourceManager &,
										ArrayRStream &memStream,
										ArrayView<RmDescriptorSetLayoutID>,
										Bytes blockOffset)						__NE___;

		ND_ bool  _LoadPipelineLayouts (RResourceManager &,
										ArrayRStream &memStream,
										ArrayView<RmDescriptorSetLayoutID>,
										ArrayView<RmPipelineLayoutID>,
										Bytes blockOffset)						__NE___;

		ND_ bool  _CreateRenderPasses (RResourceManager &,
									   ArrayView<RenderPassName::Optimized_t>)	__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/InPlace.h"
#include "base/Memory/LinearAllocator.h"
#include "serializing/Common.h"

#include "threading/Primitives/DataRaceCheck.h"
#include "threading/Memory/LfLinearAllocator.h"

#include "graphics/Public/ResourceManager.h"

#include "PipelineCompilerImpl.h"

namespace AE::Graphics
{
#	if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::RWDataRaceCheck;
#	endif
	using AE::Threading::Mutex;


	//
	// Base Pipeline Pack
	//
	
	template <typename Types>
	class PipelinePackBase
	{
	// types
	protected:
		using Allocator_t			= typename Types::Allocator_t;
		using PipelineLayoutID_t	= typename Types::PipelineLayoutID_t;
		using PipelineLayout_t		= typename Types::PipelineLayout_t;
		using SamplerID_t			= typename Types::SamplerID_t;
		using ResMngr_t				= typename Types::ResMngr_t;
		using RenderPassRefs_t		= typename Types::RenderPassRefs_t;
		using ShaderModule_t		= typename Types::ShaderModule_t;
		using ShaderModuleRef_t		= typename Types::ShaderModuleRef_t;

		template <typename K, typename V>
		using THashMap				= FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, Allocator_t* >>;

		template <typename T>
		using THashSet				= FlatHashSet< T, std::hash<T>, std::equal_to<T>, StdAllocatorRef< T, Allocator_t* >>;

		using PipelineLayoutDesc	= PipelineCompiler::PipelineLayoutDesc;
		using PipelineStorage		= PipelineCompiler::PipelineStorage;
		using EMarker				= PipelineStorage::EMarker;
		using SpecConstants_t		= PipelineCompiler::ShaderBytecode::SpecConstants_t;
		using Uniform_t				= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t			= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		using DSLayouts_t			= Tuple< uint, Strong< DescriptorSetLayoutID > *	>;
		using PplnLayouts_t			= Tuple< uint, Strong< PipelineLayoutID_t > *		>;
		using ShaderModules_t		= Tuple< uint, ShaderModule_t* >;
		
		using SerRStates_t			= Tuple< uint, const PipelineCompiler::SerializableRenderState *>;
		using SerDSStates_t			= Tuple< uint, const PipelineCompiler::SerializableDepthStencilState *>;

		using SerGPplnTempl_t		= Tuple< uint, const PipelineCompiler::SerializableGraphicsPipeline *		>;
		using SerMPplnTempl_t		= Tuple< uint, const PipelineCompiler::SerializableMeshPipeline *			>;
		using SerCPplnTempl_t		= Tuple< uint, const PipelineCompiler::SerializableComputePipeline *		>;
		using SerRTPplnTempl_t		= Tuple< uint, const PipelineCompiler::SerializableRayTracingPipeline *		>;
		using SerTPplnTempl_t		= Tuple< uint, const PipelineCompiler::SerializableTilePipeline *			>;

		using SerGPplnSpec_t		= Tuple< uint, const PipelineCompiler::SerializableGraphicsPipelineSpec *	>;
		using SerMPplnSpec_t		= Tuple< uint, const PipelineCompiler::SerializableMeshPipelineSpec *		>;
		using SerCPplnSpec_t		= Tuple< uint, const PipelineCompiler::SerializableComputePipelineSpec *	>;
		using SerRTPplnSpec_t		= Tuple< uint, const PipelineCompiler::SerializableRayTracingPipelineSpec *	>;
		using SerTPplnSpec_t		= Tuple< uint, const PipelineCompiler::SerializableTilePipelineSpec *		>;

		using StackAllocator_t		= StackAllocator< UntypedAllocator, 16, false >;
		using FeatureNames_t		= THashSet< FeatureSetName::Optimized_t >;
		using SamplerRefs_t			= THashMap< SamplerName::Optimized_t, SamplerID_t >;
		using DSLayoutMap_t			= THashMap< DSLayoutName::Optimized_t, DescriptorSetLayoutID >;
		using PplnTemplMap_t		= THashMap< PipelineTmplName::Optimized_t, PipelineCompiler::PipelineTemplUID >;

	public:

		//
		// Render Tech
		//
		class RenderTech final : public IRenderTechPipelines
		{
		// types
		private:
			using PipelineID = HandleTmpl< 16, 16, 0 >;

			struct PipelineInfo
			{
				PipelineCompiler::PipelineSpecUID	uid		= Default;
				PipelineID							pplnId;

				template <typename ID>
				ND_ ID  Cast ()	const	{ return ID{ pplnId.Index(), pplnId.Generation() }; }
			};

			using Passes_t		= Tuple< uint, PipelineCompiler::SerializableRenderTechnique::Pass * >;
			using PplnSpecMap_t	= THashMap< PipelineName::Optimized_t, PipelineInfo >;


		// variables
		private:
			PipelinePackBase &				_pack;
			RenderTechName::Optimized_t		_name;
			bool							_isSupported	: 1;
			bool							_isLoaded		: 1;
			bool							_isTryLoad		: 1;
			Passes_t						_passes;
			PplnSpecMap_t					_pipelines;
			
			DRC_ONLY( RWDataRaceCheck		_drCheck; )


		// methods
		public:
			explicit RenderTech (PipelinePackBase& pack);
			~RenderTech () override;
			
				bool  Deserialize (ResMngr_t &, Serializing::Deserializer &);
			ND_ bool  Load (ResMngr_t &, PipelineCacheID cache);
				void  Destroy (ResMngr_t &);

			ND_ RenderTechName::Optimized_t	Name ()			const	{ DRC_SHAREDLOCK( _drCheck );  return _name; }
			ND_ bool						IsSupported ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _isSupported; }

			GraphicsPipelineID		GetGraphicsPipeline	 (const PipelineName &name)			C_NE_OV;
			MeshPipelineID			GetMeshPipeline		 (const PipelineName &name)			C_NE_OV;
			TilePipelineID			GetTilePipeline		 (const PipelineName &name)			C_NE_OV;
			ComputePipelineID		GetComputePipeline	 (const PipelineName &name)			C_NE_OV;
			RayTracingPipelineID	GetRayTracingPipeline(const PipelineName &name)			C_NE_OV;
			PassInfo				GetPass				 (const RenderTechPassName &pass)	C_NE_OV;


		private:
			ND_ bool  _PreloadShaders (const ResMngr_t &);

			void  _PrintPipelines (PipelineCompiler::PipelineSpecUID mask) const;
		
			template <PipelineCompiler::PipelineSpecUID  SpecMask,
					  PipelineCompiler::PipelineTemplUID TemplMask,
					  typename SpecType,
					  typename TemplType>
			ND_ Pair< const typename TypeList<SpecType>::template Get<1>, const typename TypeList<TemplType>::template Get<1> >
				_Extract (PipelineCompiler::PipelineSpecUID uid, SpecType &specArr, TemplType &templArr,
						  const FeatureNames_t &unsupportedFS DEBUG_ONLY(, const FeatureNames_t &allFeatureSets ));

			ND_ PipelineID  _CompileGraphicsPipeline   (ResMngr_t &, const PipelineCompiler::SerializableGraphicsPipelineSpec  &, const PipelineCompiler::SerializableGraphicsPipeline  &, PipelineCacheID);
			ND_ PipelineID  _CompileMeshPipeline       (ResMngr_t &, const PipelineCompiler::SerializableMeshPipelineSpec      &, const PipelineCompiler::SerializableMeshPipeline      &, PipelineCacheID);
			ND_ PipelineID  _CompileComputePipeline    (ResMngr_t &, const PipelineCompiler::SerializableComputePipelineSpec   &, const PipelineCompiler::SerializableComputePipeline   &, PipelineCacheID);
			ND_ PipelineID  _CompileRayTracingPipeline (ResMngr_t &, const PipelineCompiler::SerializableRayTracingPipelineSpec&, const PipelineCompiler::SerializableRayTracingPipeline&, PipelineCacheID);
			ND_ PipelineID  _CompileTilePipeline       (ResMngr_t &, const PipelineCompiler::SerializableTilePipelineSpec	   &, const PipelineCompiler::SerializableTilePipeline		&, PipelineCacheID);
		};

	private:
		using RenTechs_t	= Tuple< uint, RenderTech * >;
		using RenTechMap_t	= THashMap< RenderTechName::Optimized_t, ushort >;


	// variables
	protected:
		mutable Mutex				_fileGuard;
		RC<RStream>					_file;

		mutable Allocator_t			_allocator;
		
		InPlace< FeatureNames_t >	_unsupportedFS;

		SerRStates_t				_renderStates;
		SerDSStates_t				_depthStencilStates;

		DSLayouts_t					_dsLayouts;
		PplnLayouts_t				_pplnLayouts;
		
		RenTechs_t					_renTechs;
		InPlace< RenTechMap_t >		_renTechMap;

		SerGPplnTempl_t				_serGPplnTempl;
		SerMPplnTempl_t				_serMPplnTempl;
		SerCPplnTempl_t				_serCPplnTempl;
		SerRTPplnTempl_t			_serRTPplnTempl;
		SerTPplnTempl_t				_serTPplnTempl;
		
		SerGPplnSpec_t				_serGPplnSpec;
		SerMPplnSpec_t				_serMPplnSpec;
		SerCPplnSpec_t				_serCPplnSpec;
		SerRTPplnSpec_t				_serRTPplnSpec;
		SerTPplnSpec_t				_serTPplnSpec;

		InPlace< PplnTemplMap_t >	_pplnTemplMap;
		InPlace< DSLayoutMap_t >	_dsLayoutMap;

		InPlace< SamplerRefs_t >	_samplerRefs;
		InPlace< RenderPassRefs_t >	_renderPassRefs;
		
		ShaderModules_t				_shaders;
		Bytes						_shaderOffset;
		Bytes						_shaderDataSize;

		Strong<PipelinePackID>		_parentPackId;
		EPixelFormat				_swapchainFmt	= Default;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck; )

		DEBUG_ONLY(
		  InPlace<FeatureNames_t>	_allFeatureSets;
		)


	// methods
	public:
		PipelinePackBase () {}
		~PipelinePackBase () {}
		
		ND_ SamplerID_t				GetSampler (const SamplerName &name) const;
		ND_ Array<RenderTechName>	GetSupportedRenderTechs () const;

		ND_ Strong<GraphicsPipelineID>		CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache) const;
		ND_ Strong<ComputePipelineID>		CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache) const;
		ND_ Strong<MeshPipelineID>			CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<TilePipelineID>			CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache) const;
		ND_ Strong<RayTracingPipelineID>	CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache) const;
		
		ND_ DescriptorSetLayoutID			GetDSLayout (const DSLayoutName &name) const;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (ResMngr_t &, const RenderTechName &name, PipelineCacheID cache) const;
		ND_ RenderTechPipelinesPtr			LoadRenderTech (ResMngr_t &, const RenderTechName &name, PipelineCacheID cache) const;

		ND_ FeatureNames_t const&				GetUnsupportedFS ()	const	{ DRC_SHAREDLOCK( _drCheck );  return *_unsupportedFS; }
		DEBUG_ONLY( ND_ FeatureNames_t const&	GetAllFS ()			const	{ DRC_SHAREDLOCK( _drCheck );  return *_allFeatureSets; })
		

	protected:
		ND_ bool  _Create (ResMngr_t &, const PipelinePackDesc &desc);
			void  _Destroy (ResMngr_t &);
		
		ND_ bool  _LoadDepthStencilStatesImpl (Serializing::Deserializer &);

		ND_ virtual bool  _LoadDepthStencilStates (ResMngr_t &, Serializing::Deserializer &) = 0;
		ND_ virtual bool  _LoadRenderPasses (ResMngr_t &, Bytes offset, Bytes size) = 0;

		ND_ virtual SamplerID_t			_CreateSampler (ResMngr_t &, const SamplerDesc &desc, StringView dbgName) = 0;
		ND_ virtual ShaderModuleRef_t	_GetShader (const ResMngr_t &, PipelineCompiler::ShaderUID uid, EShader type) const = 0;
		
		ND_ virtual Strong<DescriptorSetLayoutID>  _CreateDescriptorSetLayout (ResMngr_t &, const Uniforms_t &, ArrayView<SamplerID_t>, EDescSetUsage, StackAllocator_t &) = 0;


	private:
		ND_ bool  _LoadNameMapping (ResMngr_t &, Bytes offset, Bytes size);
		ND_ bool  _LoadFeatureSets (ResMngr_t &, Bytes offset, Bytes size);
		ND_ bool  _LoadSamplers (ResMngr_t &, Bytes offset, Bytes size);
		ND_ bool  _LoadPipelineBlock (ResMngr_t &, Bytes offset, Bytes size);

		ND_ bool  _LoadRenderStates (Serializing::Deserializer &);

		ND_ bool  _LoadDescrSetLayouts (ResMngr_t &, Serializing::Deserializer &, StackAllocator_t &, OUT DSLayoutMap_t &);
		ND_ bool  _LoadPipelineLayouts (ResMngr_t &, Serializing::Deserializer &);
		ND_ bool  _LoadRenderTechniques (ResMngr_t &, Serializing::Deserializer &);
		ND_ bool  _LoadShaders (Serializing::Deserializer &);

		template <typename TName, typename TUID>
		ND_ bool  _LoadPipelineNames (Serializing::Deserializer &des, OUT THashMap<TName, TUID> &);

		template <typename T>
		ND_ bool  _LoadPipelineArray (Serializing::Deserializer &des, OUT Tuple<uint, const T*> &arr);

		ND_ PipelineLayoutID_t	_GetPipelineLayout (PipelineCompiler::PipelineLayoutUID uid) const;

		template <PipelineCompiler::PipelineTemplUID TemplMask, typename TemplType>
		ND_ const typename TypeList<TemplType>::template Get<1>
			_Extract (ResMngr_t &, const PipelineTmplName &name, const TemplType &templArr) const;

		ND_ RenderState const*  _GetRenderState (PipelineCompiler::RenderStateUID uid) const;
	};


} // AE::Graphics

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	All containers inside PipelinePack are initialized in '_Create()' and destroyed in '_Destroy()', in other methods they are immutable.

	Thread-safe:
		- safe to use after creation.
		- safe to use before destruction.
		- unsafe when creating and loading in progress.
*/

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define PPLNPACK			VPipelinePack

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define PPLNPACK			MPipelinePack

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE::Graphics
{

	//
	// Pipeline Pack
	//

	class PPLNPACK
	{
	// types
	#if defined(AE_ENABLE_VULKAN)

	public:
		struct ShaderModuleRef
		{
			VkShaderStageFlagBits									stage			= Zero;
			VkShaderModule											module			= Default;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			ShaderTracePtr											dbgTrace;

			ND_ bool			IsValid ()		C_NE___	{ return module != Default and shaderConstants != null; }
			ND_ const char*		Entry ()		C_NE___	{ return "Main"; }
		};

	private:

		//
		// Shader Module
		//
		struct alignas(AE_CACHE_LINE) ShaderModule
		{
			mutable Threading::RWSpinLock								guard;		// protects 'module', 'dbgTrace', 'constants'
			Bytes32u													offset;
			Bytes32u													dataSize;
			Bytes32u													data2Size;
			ubyte														shaderTypeIdx	= UMax;

			// initialized in '_GetShader()':
			mutable VkShaderModule										module			= Default;
			mutable Unique< PipelineCompiler::ShaderTrace >				dbgTrace;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		StaticAssert( sizeof(ShaderModule) == 128 );

	#elif defined(AE_ENABLE_METAL)

	public:
		struct ShaderModuleRef
		{
			EShader													type			= Default;
			MetalLibrary											lib;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;

			ND_ bool			IsValid ()		C_NE___	{ return bool{lib} and shaderConstants != null; }
			ND_ const char*		Entry ()		C_NE___	{ return "Main"; }
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
		StaticAssert( sizeof(ShaderModule) == 128 );

	#else
	#	error not implemented
	#endif


	private:
		template <typename T>
		using StdAlloc_t			= StdAllocatorRef< T, IAllocatorTS* >;
		using LinearAllocator_t		= Threading::LfLinearAllocator< usize(SmallAllocationSize * 16), usize{16_b} >;

		using Device_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, Device				);
		using ResMngr_t				= AE_PRIVATE_UNITE_RAW( SUFFIX, ResourceManager		);
		using TempLinearAllocator_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, TempLinearAllocator	);

		using PipelineLayout_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineLayout		);
		using ComputePipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, ComputePipeline		);
		using GraphicsPipeline_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, GraphicsPipeline	);
		using MeshPipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, MeshPipeline		);
		using TilePipeline_t		= AE_PRIVATE_UNITE_RAW( SUFFIX, TilePipeline		);
		using RayTracingPipeline_t	= AE_PRIVATE_UNITE_RAW( SUFFIX, RayTracingPipeline	);
		using RTShaderBindingTable_t= AE_PRIVATE_UNITE_RAW( SUFFIX, RTShaderBindingTable);


		//
		// Render Pass Refs
		//
		struct RenderPassRefs
		{
			template <typename K, typename V>
			using THashMap = FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAlloc_t< Pair<const K, V> >>;

			THashMap< RenderPassName::Optimized_t,		 Strong<RenderPassID> >		specMap;
			THashMap< CompatRenderPassName::Optimized_t, Strong<RenderPassID> >		compatMap;

			explicit RenderPassRefs (IAllocatorTS* alloc) __Th___;
		};

		template <typename K, typename V>
		using THashMap				= FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAlloc_t< Pair<const K, V> >>;

		template <typename T>
		using THashSet				= FlatHashSet< T, std::hash<T>, std::equal_to<T>, StdAlloc_t<T> >;

		using PipelineLayoutDesc	= PipelineCompiler::PipelineLayoutDesc;
		using PipelineStorage		= PipelineCompiler::PipelineStorage;
		using EMarker				= PipelineCompiler::PipelineStorage::EMarker;
		using SpecConstants_t		= PipelineCompiler::ShaderBytecode::SpecConstants_t;
		using Uniform_t				= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using UniformOffsets_t		= PipelineCompiler::DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniforms_t			= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		using DSLayouts_t			= MutableArrayView< Strong< DescriptorSetLayoutID > >;
		using PplnLayouts_t			= MutableArrayView< Strong< PipelineLayoutID > >;
		using ShaderModules_t		= MutableArrayView< ShaderModule >;

		using SerRStates_t			= ArrayView< PipelineCompiler::SerializableRenderState >;
		using SerDSStates_t			= ArrayView< PipelineCompiler::SerializableDepthStencilState >;

		using SerGPplnTempl_t		= ArrayView< PipelineCompiler::SerializableGraphicsPipeline >;
		using SerMPplnTempl_t		= ArrayView< PipelineCompiler::SerializableMeshPipeline >;
		using SerCPplnTempl_t		= ArrayView< PipelineCompiler::SerializableComputePipeline >;
		using SerRTPplnTempl_t		= ArrayView< PipelineCompiler::SerializableRayTracingPipeline >;
		using SerTPplnTempl_t		= ArrayView< PipelineCompiler::SerializableTilePipeline >;

		using SerGPplnSpec_t		= ArrayView< PipelineCompiler::SerializableGraphicsPipelineSpec >;
		using SerMPplnSpec_t		= ArrayView< PipelineCompiler::SerializableMeshPipelineSpec >;
		using SerCPplnSpec_t		= ArrayView< PipelineCompiler::SerializableComputePipelineSpec >;
		using SerRTPplnSpec_t		= ArrayView< PipelineCompiler::SerializableRayTracingPipelineSpec >;
		using SerTPplnSpec_t		= ArrayView< PipelineCompiler::SerializableTilePipelineSpec >;

		using SerRTSBTs_t			= ArrayView< PipelineCompiler::SerializableRTShaderBindingTable >;

		using StackAllocator_t		= StackAllocator< UntypedAllocator, 16, false >;
		using FeatureNames_t		= THashSet< FeatureSetName::Optimized_t >;
		using SamplerRefs_t			= THashMap< SamplerName::Optimized_t,		Strong<SamplerID> >;
		using DSLayoutMap_t			= THashMap< DSLayoutName::Optimized_t,		DescriptorSetLayoutID >;
		using PplnTemplMap_t		= THashMap< PipelineTmplName::Optimized_t,	PipelineCompiler::PipelineTemplUID >;

	public:

		//
		// Render Tech
		//
		class RenderTech final : public IRenderTechPipelines
		{
		// types
		private:
			using PipelineID = HandleTmpl< 16, 16, 0 >;	// GraphicsPipelineID, MeshPipelineID, TilePipelineID, ComputePipelineID, RayTracingPipelineID

			struct PipelineInfo
			{
				PipelineCompiler::PipelineSpecUID	uid		= Default;
				PipelineID							pplnId;

				template <typename ID>
				ND_ ID  Cast ()		C_NE___	{ return BitCast<ID>( pplnId ); }
			};

			using Passes_t			= MutableArrayView< PipelineCompiler::SerializableRenderTechnique::Pass >;
			using PplnSpecMap_t		= THashMap< PipelineName::Optimized_t, PipelineInfo >;
			using PplnSpecIter_t	= PplnSpecMap_t::iterator;

			struct SBTInfo
			{
				uint				uid		= UMax;
				RTShaderBindingID	sbtId;
			};
			using RTSBTMap_t	= THashMap< RTShaderBindingName::Optimized_t, SBTInfo >;


		// variables
		private:
			PPLNPACK &						_pack;
			RenderTechName::Optimized_t		_name;
			bool							_isSupported		: 1;
			bool							_isLoaded			: 1;	// TODO: states
			bool							_wasAttemptToLoad	: 1;
			Passes_t						_passes;
			PplnSpecMap_t					_pipelines;
			RTSBTMap_t						_rtSbtMap;

			DRC_ONLY( RWDataRaceCheck		_drCheck; )


		// methods
		public:
			explicit RenderTech (PPLNPACK& pack)																	__NE___;
			~RenderTech ()																							__NE_OV;

			ND_	bool  Deserialize (ResMngr_t &, Serializing::Deserializer &)										__Th___;
			ND_ auto  LoadAsync (ResMngr_t &, PipelineCacheID)														__NE___ -> Promise<RenderTechPipelinesPtr>;
			ND_ bool  Load (ResMngr_t &, PipelineCacheID)															__NE___;
				void  Destroy (ResMngr_t &)																			__NE___;

			ND_ RenderTechName::Optimized_t	Name ()																	C_NE_OV	{ DRC_SHAREDLOCK( _drCheck );  return _name; }
			ND_ bool						IsSupported ()															C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _isSupported; }

			GraphicsPipelineID		GetGraphicsPipeline	 (PipelineName::Ref name)									C_NE_OV;
			MeshPipelineID			GetMeshPipeline		 (PipelineName::Ref name)									C_NE_OV;
			TilePipelineID			GetTilePipeline		 (PipelineName::Ref name)									C_NE_OV;
			ComputePipelineID		GetComputePipeline	 (PipelineName::Ref name)									C_NE_OV;
			RayTracingPipelineID	GetRayTracingPipeline(PipelineName::Ref name)									C_NE_OV;
			RTShaderBindingID		GetRTShaderBinding	 (RTShaderBindingName::Ref name)							C_NE_OV;
			PassInfo				GetPass				 (RenderTechPassName::Ref pass)								C_NE_OV;
			bool					FeatureSetSupported  (FeatureSetName::Ref name)									C_NE_OV;
			EPixelFormat			GetAttachmentFormat  (RenderTechPassName::Ref pass, AttachmentName::Ref)		C_NE_OV;

			// for internal usage
			ND_ PplnSpecMap_t const&	_GetPipelineSpecs ()														C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pipelines; }
			ND_ RTSBTMap_t const&		_GetSBTs ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _rtSbtMap; }
			ND_ Passes_t const&			_GetPasses ()																C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _passes; }


		private:
			ND_ bool  _PreloadShaders (const ResMngr_t &)															__NE___;
			ND_ bool  _CompilePipelines (ResMngr_t &, PipelineCacheID, PplnSpecIter_t begin, PplnSpecIter_t end)	__NE___;
			ND_ bool  _CreateSBTs (ResMngr_t &resMngr)																__NE___;

			void  _PrintPipelines (PipelineName::Ref name, PipelineCompiler::PipelineSpecUID mask)					C_NE___;
			void  _PrintSBTs (RTShaderBindingName::Ref reqName)														C_NE___;

			template <PipelineCompiler::PipelineSpecUID  SpecMask,
					  PipelineCompiler::PipelineTemplUID TemplMask,
					  typename SpecType,
					  typename TemplType>
			ND_ Pair< const typename SpecType::value_type*, const typename TemplType::value_type* >
				_Extract (const ResMngr_t &, PipelineCompiler::PipelineSpecUID uid, SpecType &specArr, TemplType &templArr,
						  const FeatureNames_t &unsupportedFS GFX_DBG_ONLY(, const FeatureNames_t &allFeatureSets )) __NE___;

			ND_ PipelineID  _CompileGraphicsPipeline   (ResMngr_t &, const PipelineCompiler::SerializableGraphicsPipelineSpec  &, const PipelineCompiler::SerializableGraphicsPipeline  &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileMeshPipeline       (ResMngr_t &, const PipelineCompiler::SerializableMeshPipelineSpec      &, const PipelineCompiler::SerializableMeshPipeline      &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileComputePipeline    (ResMngr_t &, const PipelineCompiler::SerializableComputePipelineSpec   &, const PipelineCompiler::SerializableComputePipeline   &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileRayTracingPipeline (ResMngr_t &, const PipelineCompiler::SerializableRayTracingPipelineSpec&, const PipelineCompiler::SerializableRayTracingPipeline&, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileTilePipeline       (ResMngr_t &, const PipelineCompiler::SerializableTilePipelineSpec	   &, const PipelineCompiler::SerializableTilePipeline		&, PipelineCacheID, StringView) __NE___;

			ND_ RTShaderBindingID  _CreateRTShaderBinding (ResMngr_t &, const PipelineCompiler::SerializableRTShaderBindingTable &, StringView) __NE___;
		};

	private:
		using RenTechs_t	= MutableArrayView< StaticRC<RenderTech> >;
		using RenTechMap_t	= THashMap< RenderTechName::Optimized_t, ushort >;


	// variables
	protected:
		mutable Mutex				_fileGuard;
		RC<RStream>					_file;

		RC<IAllocatorTS>			_allocator;

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

		SerRTSBTs_t					_serRTSBTs;

		InPlace< PplnTemplMap_t >	_pplnTemplMap;
		InPlace< DSLayoutMap_t >	_dsLayoutMap;

		InPlace< SamplerRefs_t >	_samplerRefs;
		InPlace< RenderPassRefs >	_renderPassRefs;

		ShaderModules_t				_shaders;
		Bytes						_shaderOffset;
		Bytes						_shaderDataSize;

		Ptr<const PPLNPACK>			_parentPack;
		Strong<PipelinePackID>		_parentPackId;
		EPixelFormat				_surfaceFormat	= Default;

		DRC_ONLY( RWDataRaceCheck	_drCheck; )

		GFX_DBG_ONLY(
		  InPlace<FeatureNames_t>	_allFeatureSets;
			DebugName_t				_debugName;
		)

		#ifdef AE_ENABLE_VULKAN
		GfxMemAllocatorPtr			_sbtAllocator;
		#endif


	// methods
	public:
		PPLNPACK ()																								__NE___	{}
		~PPLNPACK ()																							__NE___	{}

		ND_ bool  Create (ResMngr_t &, const PipelinePackDesc &desc)											__NE___;
			void  Destroy (ResMngr_t &)																			__NE___;

		ND_ RenderPassID					GetRenderPass (RenderPassName::Ref name)							C_NE___;
		ND_ RenderPassID					GetRenderPass (CompatRenderPassName::Ref name)						C_NE___;

		ND_ SamplerID						GetSampler (SamplerName::Ref name)									C_NE___;
		ND_ Array<RenderTechName>			GetSupportedRenderTechs ()											C_NE___;

		ND_ DescriptorSetLayoutID			GetDSLayout (DSLayoutName::Ref name)								C_NE___;

		ND_ Strong<GraphicsPipelineID>		CreatePipeline (ResMngr_t &, PipelineTmplName::Ref, const GraphicsPipelineDesc   &, PipelineCacheID)	C_NE___;
		ND_ Strong<ComputePipelineID>		CreatePipeline (ResMngr_t &, PipelineTmplName::Ref, const ComputePipelineDesc    &, PipelineCacheID)	C_NE___;
		ND_ Strong<MeshPipelineID>			CreatePipeline (ResMngr_t &, PipelineTmplName::Ref, const MeshPipelineDesc       &, PipelineCacheID)	C_NE___;
		ND_ Strong<TilePipelineID>			CreatePipeline (ResMngr_t &, PipelineTmplName::Ref, const TilePipelineDesc       &, PipelineCacheID)	C_NE___;
		ND_ Strong<RayTracingPipelineID>	CreatePipeline (ResMngr_t &, PipelineTmplName::Ref, const RayTracingPipelineDesc &, PipelineCacheID)	C_NE___;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (ResMngr_t &, RenderTechName::Ref name, PipelineCacheID cache)						C_NE___;
		ND_ RenderTechPipelinesPtr			LoadRenderTech (ResMngr_t &, RenderTechName::Ref name, PipelineCacheID cache)							C_NE___;

		ND_ EPixelFormat					GetSurfaceFormat ()													C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _surfaceFormat; }
		GFX_DBG_ONLY( ND_ StringView		GetDebugName ()														C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		// for internal usage
		ND_ PplnLayouts_t					_GetPipelineLayoutIDs ()											C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _pplnLayouts; }
		ND_ RenderPassRefs const&			_GetRenderPassRefs ()												C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_renderPassRefs; }
		ND_ SamplerRefs_t const&			_GetSamplerRefs ()													C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_samplerRefs; }
		ND_ DSLayouts_t						_GetDescriptorSetLayouts ()											C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _dsLayouts; }
		ND_ FeatureNames_t const&			_GetUnsupportedFS ()												C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_unsupportedFS; }
		GFX_DBG_ONLY( ND_ auto const&		_GetAllFS ()														C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_allFeatureSets; })


	private:
		ND_ bool  _Create (ResMngr_t &, const PipelinePackDesc &desc)											__Th___;
			void  _Destroy (ResMngr_t &)																		__NE___;

		ND_ bool  _LoadDepthStencilStatesImpl (Serializing::Deserializer &)										__NE___;

		ND_ bool  _LoadDepthStencilStates (ResMngr_t &, Serializing::Deserializer &)							__NE___;
		ND_ bool  _LoadRenderPasses (ResMngr_t &, Bytes offset, Bytes size)										__Th___;

		ND_ SamplerID		_CreateSampler (ResMngr_t &, const SamplerDesc &,
											const Optional<SamplerYcbcrConversionDesc> &, StringView dbgName)	__NE___;
		ND_ ShaderModuleRef	_GetShader (const ResMngr_t &, PipelineCompiler::ShaderUID uid, EShader type)		C_NE___;
		ND_ bool			_CreateShader (const Device_t &dev, INOUT const ShaderModule &shader)				C_NE___;

		ND_ auto  _CreateDescriptorSetLayout (ResMngr_t &, const Uniforms_t &, ArrayView<SamplerID>,
											  const UniformOffsets_t &, EDescSetUsage,
											  EShaderStages, StackAllocator_t &)								__NE___ -> Strong<DescriptorSetLayoutID>;

		ND_ bool  _LoadNameMapping (ResMngr_t &, Bytes offset, Bytes size)										__Th___;
		ND_ bool  _LoadFeatureSets (ResMngr_t &, Bytes offset, Bytes size)										__Th___;
		ND_ bool  _CopyFeatureSets ()																			__Th___;
		ND_ bool  _LoadSamplers (ResMngr_t &, Bytes offset, Bytes size)											__Th___;
		ND_ bool  _LoadPipelineBlock (ResMngr_t &, Bytes offset, Bytes size)									__Th___;

		ND_ bool  _LoadRenderStates (Serializing::Deserializer &)												__NE___;

		ND_ bool  _LoadDescrSetLayouts (ResMngr_t &, Serializing::Deserializer &,
										StackAllocator_t &, OUT DSLayoutMap_t &)								__Th___;
		ND_ bool  _LoadPipelineLayouts (ResMngr_t &, Serializing::Deserializer &)								__NE___;
		ND_ bool  _LoadRenderTechniques (ResMngr_t &, Serializing::Deserializer &)								__Th___;
		ND_ bool  _LoadRTShaderBindingTable (Serializing::Deserializer &)										__NE___;
		ND_ bool  _LoadShaders (Serializing::Deserializer &)													__NE___;

		template <typename TName, typename TUID>
		ND_ bool  _LoadPipelineNames (Serializing::Deserializer &des, OUT THashMap<TName, TUID> &)				__Th___;

		template <typename T>
		ND_ bool  _LoadPipelineArray (Serializing::Deserializer &des, OUT ArrayView<T> &arr)					__NE___;

		ND_ PipelineLayoutID	_GetPipelineLayout (PipelineCompiler::PipelineLayoutUID uid)					C_NE___;

		template <PipelineCompiler::PipelineTemplUID TemplMask, typename TemplType>
		ND_ const typename TemplType::value_type*
			_Extract (ResMngr_t &, PipelineTmplName::Ref name, const TemplType &templArr)						C_NE___;

		ND_ RenderState const*  _GetRenderState (PipelineCompiler::RenderStateUID uid)							C_NE___;
	};


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef PPLNPACK

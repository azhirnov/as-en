// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX			V
#	define PPLNPACK			VPipelinePack

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX			M
#	define PPLNPACK			MPipelinePack

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX			R
#	define PPLNPACK			RPipelinePack

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
			
			ND_ bool		IsValid ()	C_NE___	{ return module != Default and shaderConstants != null; }
			ND_ const char*	Entry ()	C_NE___	{ return "Main"; }
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
			mutable VkShaderModule										module			= Default;
			mutable Unique< PipelineCompiler::ShaderTrace >				dbgTrace;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );

	#elif defined(AE_ENABLE_METAL)

	public:
		struct ShaderModuleRef
		{
			EShader													type			= Default;
			MetalLibrary											lib;
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			
			ND_ bool		IsValid ()	C_NE___	{ return bool{lib} and shaderConstants != null; }
			ND_ const char*	Entry ()	C_NE___	{ return "Main"; }
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
		STATIC_ASSERT( sizeof(ShaderModule) == 128 );
		
	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		
	public:
		struct ShaderModuleRef
		{
			PipelineCompiler::ShaderBytecode::OptSpecConst_t const*	shaderConstants	= null;
			ShaderTracePtr											dbgTrace;
			
			ND_ bool		IsValid ()	C_NE___;
			ND_ const char*	Entry ()	C_NE___	{ return "Main"; }
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
		//	mutable VkShaderModule										module			= Default;
			mutable Unique< PipelineCompiler::ShaderTrace >				dbgTrace;
			mutable PipelineCompiler::ShaderBytecode::OptSpecConst_t	constants;
		};
		//STATIC_ASSERT( sizeof(ShaderModule) == 128 );

	#else
	#	error not implemented
	#endif


	public:
		using Allocator_t = Threading::LfLinearAllocator< usize(SmallAllocationSize * 16), 16 >;

	private:
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
			using THashMap = FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, Allocator_t* >>;

			THashMap< RenderPassName::Optimized_t,		 RenderPassID >	specMap;
			THashMap< CompatRenderPassName::Optimized_t, RenderPassID >	compatMap;

			explicit RenderPassRefs (Allocator_t *alloc);
		};

		template <typename K, typename V>
		using THashMap				= FlatHashMap< K, V, std::hash<K>, std::equal_to<K>, StdAllocatorRef< Pair<const K, V>, Allocator_t* >>;

		template <typename T>
		using THashSet				= FlatHashSet< T, std::hash<T>, std::equal_to<T>, StdAllocatorRef< T, Allocator_t* >>;

		using PipelineLayoutDesc	= PipelineCompiler::PipelineLayoutDesc;
		using PipelineStorage		= PipelineCompiler::PipelineStorage;
		using EMarker				= PipelineStorage::EMarker;
		using SpecConstants_t		= PipelineCompiler::ShaderBytecode::SpecConstants_t;
		using Uniform_t				= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using UniformOffsets_t		= PipelineCompiler::DescriptorSetLayoutDesc::UniformOffsets_t;
		using Uniforms_t			= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		using DSLayouts_t			= Tuple< uint, Strong< DescriptorSetLayoutID > *>;
		using PplnLayouts_t			= Tuple< uint, Strong< PipelineLayoutID > *		>;
		using ShaderModules_t		= Tuple< uint, ShaderModule* >;
		
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

		using SerRTSBTs_t			= Tuple< uint, const PipelineCompiler::SerializableRTShaderBindingTable *	>;

		using StackAllocator_t		= StackAllocator< UntypedAllocator, 16, false >;
		using FeatureNames_t		= THashSet< FeatureSetName::Optimized_t >;
		using SamplerRefs_t			= THashMap< SamplerName::Optimized_t,		SamplerID >;
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
			using PipelineID = HandleTmpl< 16, 16, 0 >;

			struct PipelineInfo
			{
				PipelineCompiler::PipelineSpecUID	uid		= Default;
				PipelineID							pplnId;

				template <typename ID>
				ND_ ID  Cast ()		C_NE___	{ return ID{ pplnId.Index(), pplnId.Generation() }; }
			};

			using Passes_t		= Tuple< uint, PipelineCompiler::SerializableRenderTechnique::Pass * >;
			using PplnSpecMap_t	= THashMap< PipelineName::Optimized_t, PipelineInfo >;

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
			bool							_isLoaded			: 1;
			bool							_wasAttampToLoad	: 1;
			Passes_t						_passes;
			PplnSpecMap_t					_pipelines;
			RTSBTMap_t						_rtSbtMap;
			
			DRC_ONLY( RWDataRaceCheck		_drCheck; )


		// methods
		public:
			explicit RenderTech (PPLNPACK& pack)																	__Th___;
			~RenderTech ()																							__NE_OV;
			
				bool  Deserialize (ResMngr_t &, Serializing::Deserializer &)										__Th___;
			ND_ bool  Load (ResMngr_t &, PipelineCacheID cache)														__NE___;
				void  Destroy (ResMngr_t &)																			__NE___;
			
			ND_ RenderTechName::Optimized_t	Name ()																	C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _name; }
			ND_ bool						IsSupported ()															C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _isSupported; }

			GraphicsPipelineID		GetGraphicsPipeline	 (const PipelineName &name)									C_NE_OV;
			MeshPipelineID			GetMeshPipeline		 (const PipelineName &name)									C_NE_OV;
			TilePipelineID			GetTilePipeline		 (const PipelineName &name)									C_NE_OV;
			ComputePipelineID		GetComputePipeline	 (const PipelineName &name)									C_NE_OV;
			RayTracingPipelineID	GetRayTracingPipeline(const PipelineName &name)									C_NE_OV;
			RTShaderBindingID		GetRTShaderBinding	 (const RTShaderBindingName &name)							C_NE_OV;
			PassInfo				GetPass				 (const RenderTechPassName &pass)							C_NE_OV;
			bool					FeatureSetSupported  (const FeatureSetName &name)								C_NE_OV;
			EPixelFormat			GetAttachmentFormat  (const RenderTechPassName &pass, const AttachmentName &)	C_NE_OV;


		private:
			ND_ bool  _PreloadShaders (const ResMngr_t &)															__NE___;

			void  _PrintPipelines (const PipelineName &name,
								   PipelineCompiler::PipelineSpecUID mask)											C_NE___;
		
			template <PipelineCompiler::PipelineSpecUID  SpecMask,
					  PipelineCompiler::PipelineTemplUID TemplMask,
					  typename SpecType,
					  typename TemplType>
			ND_ Pair< const typename TypeList<SpecType>::template Get<1>, const typename TypeList<TemplType>::template Get<1> >
				_Extract (const ResMngr_t &, PipelineCompiler::PipelineSpecUID uid, SpecType &specArr, TemplType &templArr,
						  const FeatureNames_t &unsupportedFS DEBUG_ONLY(, const FeatureNames_t &allFeatureSets ))	__NE___;

			ND_ PipelineID  _CompileGraphicsPipeline   (ResMngr_t &, const PipelineCompiler::SerializableGraphicsPipelineSpec  &, const PipelineCompiler::SerializableGraphicsPipeline  &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileMeshPipeline       (ResMngr_t &, const PipelineCompiler::SerializableMeshPipelineSpec      &, const PipelineCompiler::SerializableMeshPipeline      &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileComputePipeline    (ResMngr_t &, const PipelineCompiler::SerializableComputePipelineSpec   &, const PipelineCompiler::SerializableComputePipeline   &, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileRayTracingPipeline (ResMngr_t &, const PipelineCompiler::SerializableRayTracingPipelineSpec&, const PipelineCompiler::SerializableRayTracingPipeline&, PipelineCacheID, StringView) __NE___;
			ND_ PipelineID  _CompileTilePipeline       (ResMngr_t &, const PipelineCompiler::SerializableTilePipelineSpec	   &, const PipelineCompiler::SerializableTilePipeline		&, PipelineCacheID, StringView) __NE___;

			ND_ RTShaderBindingID  _CreateRTShaderBinding (ResMngr_t &, const PipelineCompiler::SerializableRTShaderBindingTable &, StringView) __NE___;
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

		SerRTSBTs_t					_serRTSBTs;

		InPlace< PplnTemplMap_t >	_pplnTemplMap;
		InPlace< DSLayoutMap_t >	_dsLayoutMap;

		InPlace< SamplerRefs_t >	_samplerRefs;
		InPlace< RenderPassRefs >	_renderPassRefs;
		
		ShaderModules_t				_shaders;
		Bytes						_shaderOffset;
		Bytes						_shaderDataSize;

		Strong<PipelinePackID>		_parentPackId;
		EPixelFormat				_surfaceFormat	= Default;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck; )

		DEBUG_ONLY(
		  InPlace<FeatureNames_t>	_allFeatureSets;
		)
		DEBUG_ONLY( DebugName_t		_debugName; )
			
		#if defined(AE_ENABLE_VULKAN)
		GfxMemAllocatorPtr			_sbtAllocator;
		#endif

	// methods
	public:
		PPLNPACK ()																																					__NE___	{}
		~PPLNPACK ()																																				__NE___	{}
		
		ND_ bool  Create (ResMngr_t &, const PipelinePackDesc &desc)																								__NE___;
			void  Destroy (ResMngr_t &)																																__NE___;
			
		ND_ RenderPassID			GetRenderPass (const ResMngr_t &, const RenderPassName &name)																	C_NE___;
		ND_ RenderPassID			GetRenderPass (const ResMngr_t &, const CompatRenderPassName &name)																C_NE___;

		ND_ SamplerID				GetSampler (const SamplerName &name)																							C_NE___;
		ND_ Array<RenderTechName>	GetSupportedRenderTechs ()																										C_NE___;

		ND_ Strong<GraphicsPipelineID>		CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache)	C_NE___;
		ND_ Strong<ComputePipelineID>		CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache)	C_NE___;
		ND_ Strong<MeshPipelineID>			CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache)	C_NE___;
		ND_ Strong<TilePipelineID>			CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache)	C_NE___;
		ND_ Strong<RayTracingPipelineID>	CreatePipeline (ResMngr_t &, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache)	C_NE___;
		
		ND_ DescriptorSetLayoutID			GetDSLayout (const DSLayoutName &name)																					C_NE___;

		ND_ Promise<RenderTechPipelinesPtr>	LoadRenderTechAsync (ResMngr_t &, const RenderTechName &name, PipelineCacheID cache)									C_NE___;
		ND_ RenderTechPipelinesPtr			LoadRenderTech (ResMngr_t &, const RenderTechName &name, PipelineCacheID cache)											C_NE___;
		
		ND_ EPixelFormat						GetSurfaceFormat ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _surfaceFormat; }
		DEBUG_ONLY( ND_ StringView				GetDebugName ()						C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		ND_ FeatureNames_t const&				GetUnsupportedFS ()					C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_unsupportedFS; }
		DEBUG_ONLY( ND_ FeatureNames_t const&	GetAllFS ()							C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return *_allFeatureSets; })
		
			
	private:
		ND_ bool  _Create (ResMngr_t &, const PipelinePackDesc &desc)														__Th___;
			void  _Destroy (ResMngr_t &)																					__NE___;

		ND_ bool  _LoadDepthStencilStatesImpl (Serializing::Deserializer &)													__NE___;

		ND_ bool  _LoadDepthStencilStates (ResMngr_t &, Serializing::Deserializer &)										__NE___;
		ND_ bool  _LoadRenderPasses (ResMngr_t &, Bytes offset, Bytes size)													__Th___;

		ND_ SamplerID		_CreateSampler (ResMngr_t &, const SamplerDesc &,
											const Optional<SamplerYcbcrConversionDesc> &, StringView dbgName)				__NE___;
		ND_ ShaderModuleRef	_GetShader (const ResMngr_t &, PipelineCompiler::ShaderUID uid, EShader type)					C_NE___;
		ND_ bool			_CreateShader (const Device_t &dev, INOUT const ShaderModule &shader)							C_NE___;
		
		ND_ Strong<DescriptorSetLayoutID>  _CreateDescriptorSetLayout (ResMngr_t &, const Uniforms_t &, ArrayView<SamplerID>,
																	   const UniformOffsets_t &, EDescSetUsage,
																	   EShaderStages, StackAllocator_t &)					__NE___;

		ND_ bool  _LoadNameMapping (ResMngr_t &, Bytes offset, Bytes size)													__Th___;
		ND_ bool  _LoadFeatureSets (ResMngr_t &, Bytes offset, Bytes size)													__Th___;
		ND_ bool  _CopyFeatureSets (ResMngr_t &)																			__Th___;
		ND_ bool  _LoadSamplers (ResMngr_t &, Bytes offset, Bytes size)														__Th___;
		ND_ bool  _LoadPipelineBlock (ResMngr_t &, Bytes offset, Bytes size)												__Th___;

		ND_ bool  _LoadRenderStates (Serializing::Deserializer &)															__NE___;

		ND_ bool  _LoadDescrSetLayouts (ResMngr_t &, Serializing::Deserializer &, StackAllocator_t &, OUT DSLayoutMap_t &)	__Th___;
		ND_ bool  _LoadPipelineLayouts (ResMngr_t &, Serializing::Deserializer &)											__NE___;
		ND_ bool  _LoadRenderTechniques (ResMngr_t &, Serializing::Deserializer &)											__Th___;
		ND_ bool  _LoadRTShaderBindingTable (Serializing::Deserializer &)													__NE___;
		ND_ bool  _LoadShaders (Serializing::Deserializer &)																__NE___;

		template <typename TName, typename TUID>
		ND_ bool  _LoadPipelineNames (Serializing::Deserializer &des, OUT THashMap<TName, TUID> &)							__Th___;

		template <typename T>
		ND_ bool  _LoadPipelineArray (Serializing::Deserializer &des, OUT Tuple<uint, const T*> &arr)						__NE___;

		ND_ PipelineLayoutID	_GetPipelineLayout (PipelineCompiler::PipelineLayoutUID uid) const;

		template <PipelineCompiler::PipelineTemplUID TemplMask, typename TemplType>
		ND_ const typename TypeList<TemplType>::template Get<1>
			_Extract (ResMngr_t &, const PipelineTmplName &name, const TemplType &templArr) const;

		ND_ RenderState const*  _GetRenderState (PipelineCompiler::RenderStateUID uid) const;
	};


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef SUFFIX
#undef PPLNPACK

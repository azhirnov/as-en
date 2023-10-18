// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define SUFFIX       V
#   define RESMNGR      VResourceManager

#elif defined(AE_ENABLE_METAL)
#   define SUFFIX       M
#   define RESMNGR      MResourceManager

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   define SUFFIX       R
#   define RESMNGR      RResourceManager

#else
#   error not implemented
#endif
//-----------------------------------------------------------------------------


namespace AE::Graphics
{

    //
    // Resource Manager
    //

    class RESMNGR final : public IResourceManager
    {
        friend class AE_PRIVATE_UNITE_RAW( SUFFIX, RenderTaskScheduler );

    // types
    private:
        template <typename T, typename ID, usize ChunkSize, usize MaxChunks>
        using PoolTmpl      = Threading::LfIndexedPool3< ResourceBase<T>, typename ID::Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

        template <typename T, typename ID, usize PoolSize>
        using StPoolTmpl    = Threading::LfStaticIndexedPool< ResourceBase<T>, typename ID::Index_t, PoolSize, GlobalLinearAllocatorRef >;

        // chunk size
        static constexpr uint   MaxImages       = 1u << 10;
        static constexpr uint   MaxBuffers      = 1u << 10;
        static constexpr uint   MaxMemoryObjs   = 1u << 10;
        static constexpr uint   MaxDSLayouts    = 1u << 9;
        static constexpr uint   MaxDescSets     = 1u << 9;
        static constexpr uint   MaxRTObjects    = 1u << 9;
        static constexpr uint   MaxPipelines    = 1u << 10;

        using Device_t              = AE_PRIVATE_UNITE_RAW( SUFFIX, Device                  );
        using Swapchain_t           = AE_PRIVATE_UNITE_RAW( SUFFIX, Swapchain               );
        using StagingBufferManager_t= AE_PRIVATE_UNITE_RAW( SUFFIX, StagingBufferManager    );
        using QueryManager_t        = AE_PRIVATE_UNITE_RAW( SUFFIX, QueryManager            );

        using ComputePipeline_t     = AE_PRIVATE_UNITE_RAW( SUFFIX, ComputePipeline         );
        using GraphicsPipeline_t    = AE_PRIVATE_UNITE_RAW( SUFFIX, GraphicsPipeline        );
        using MeshPipeline_t        = AE_PRIVATE_UNITE_RAW( SUFFIX, MeshPipeline            );
        using RayTracingPipeline_t  = AE_PRIVATE_UNITE_RAW( SUFFIX, RayTracingPipeline      );
        using TilePipeline_t        = AE_PRIVATE_UNITE_RAW( SUFFIX, TilePipeline            );
        using Buffer_t              = AE_PRIVATE_UNITE_RAW( SUFFIX, Buffer                  );
        using Image_t               = AE_PRIVATE_UNITE_RAW( SUFFIX, Image                   );
        using RTGeometry_t          = AE_PRIVATE_UNITE_RAW( SUFFIX, RTGeometry              );
        using RTScene_t             = AE_PRIVATE_UNITE_RAW( SUFFIX, RTScene                 );
        using DescriptorSetLayout_t = AE_PRIVATE_UNITE_RAW( SUFFIX, DescriptorSetLayout     );
        using PipelineLayout_t      = AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineLayout          );
        using ShaderBindingTable_t  = AE_PRIVATE_UNITE_RAW( SUFFIX, RTShaderBindingTable    );
        using VideoBuffer_t         = AE_PRIVATE_UNITE_RAW( SUFFIX, VideoBuffer             );
        using VideoImage_t          = AE_PRIVATE_UNITE_RAW( SUFFIX, VideoImage              );
        using VideoSession_t        = AE_PRIVATE_UNITE_RAW( SUFFIX, VideoSession            );

        using MemObjPool_t          = PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, MemoryObject      ),      MemoryID,                   MaxMemoryObjs,  64 >;
        using BufferPool_t          = PoolTmpl< Buffer_t,                                               BufferID,                   MaxBuffers,     32 >;
        using BufferViewPool_t      = PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, BufferView        ),      BufferViewID,               MaxBuffers,     32 >;
        using ImagePool_t           = PoolTmpl< Image_t,                                                ImageID,                    MaxImages,      32 >;
        using ImageViewPool_t       = PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, ImageView         ),      ImageViewID,                MaxImages,      32 >;
        using DSLayoutPool_t        = PoolTmpl< DescriptorSetLayout_t,                                  DescriptorSetLayoutID,      MaxDSLayouts,    8 >;
        using GPipelinePool_t       = PoolTmpl< GraphicsPipeline_t,                                     GraphicsPipelineID,         MaxPipelines,   32 >;
        using CPipelinePool_t       = PoolTmpl< ComputePipeline_t,                                      ComputePipelineID,          MaxPipelines,   32 >;
        using MPipelinePool_t       = PoolTmpl< MeshPipeline_t,                                         MeshPipelineID,             MaxPipelines,   32 >;
        using RTPipelinePool_t      = PoolTmpl< RayTracingPipeline_t,                                   RayTracingPipelineID,       MaxPipelines,    8 >;
        using TPipelinePool_t       = PoolTmpl< TilePipeline_t,                                         TilePipelineID,             MaxPipelines,    8 >;
        using DescSetPool_t         = PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, DescriptorSet     ),      DescriptorSetID,            MaxDescSets,     8 >;
        using RTGeomPool_t          = PoolTmpl< RTGeometry_t,                                           RTGeometryID,               MaxRTObjects,    8 >;
        using RTScenePool_t         = PoolTmpl< RTScene_t,                                              RTSceneID,                  MaxRTObjects,    8 >;
        using PplnLayoutPool_t      = PoolTmpl< PipelineLayout_t,                                       PipelineLayoutID,           MaxDSLayouts,    8 >;
        using SBTPool_t             = PoolTmpl< ShaderBindingTable_t,                                   RTShaderBindingID,          1u << 8,         8 >;
        using SamplerPool_t         = StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, Sampler         ),      SamplerID,                  4096 >;
        using RenderPassPool_t      = StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, RenderPass      ),      RenderPassID,               1024 >;
        using PipelineCachePool_t   = StPoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, PipelineCache   ),      PipelineCacheID,              64 >;
        using PipelinePackPool_t    = PoolTmpl< AE_PRIVATE_UNITE_RAW( SUFFIX, PipelinePack  ),          PipelinePackID,             32,             32 >;
        using VideoBufferPool_t     = PoolTmpl< VideoBuffer_t,                                          VideoBufferID,              MaxBuffers,     32 >;
        using VideoImagePool_t      = PoolTmpl< VideoImage_t,                                           VideoImageID,               MaxImages,      32 >;
        using VideoSessionPool_t    = PoolTmpl< VideoSession_t,                                         VideoSessionID,             1u << 8,        32 >;

      #ifdef AE_ENABLE_VULKAN
        using FramebufferPool_t     = StPoolTmpl< VFramebuffer,                                         VFramebufferID,              128 >;
        using FramebufferCache_t    = VFramebuffer::FramebufferCache_t;
      #endif

        struct _ResourceDestructor;
        struct _ResourcePrinter;
        using AllResourceIDs_t      = TypeList< BufferViewID, ImageViewID, BufferID, ImageID, SamplerID,
                                                PipelineCacheID, PipelinePackID, DescriptorSetLayoutID, PipelineLayoutID, DescriptorSetID,
                                              #ifdef AE_ENABLE_VULKAN
                                                VFramebufferID, // Can be added to release list to decrease reference counter
                                              #endif
                                                RenderPassID, GraphicsPipelineID, ComputePipelineID, MeshPipelineID, RayTracingPipelineID, TilePipelineID,
                                                RTGeometryID, RTSceneID, RTShaderBindingID,
                                                VideoBufferID, VideoImageID, VideoSessionID,
                                                MemoryID        // must be in  the end
                                            >;

        //---- Expired resources ----//
    #ifdef AE_ENABLE_VULKAN
    public:
        class VReleaseExpiredResourcesTask;

    private:
        using AllVkResources_t      = TypeListUtils::Merge<
                                            AllResourceIDs_t,
                                            TypeList<   // native handles
                                                VkSwapchainKHR
                                            >>;
        struct VExpiredResource
        {
            static constexpr usize  IDSize = AllVkResources_t::ForEach_Max< TypeListUtils::GetTypeSize >();

            using IDValue_t = ByteSizeToUInt< IDSize >;

            IDValue_t   id      = UMax;     // type of AllVkResources_t [type]
            uint        type    = UMax;     // index in AllVkResources_t
        };

        struct alignas(AE_CACHE_LINE) VExpiredResArray
        {
            SpinLock                    guard;
            FrameUID                    frameId;
            Array< VExpiredResource >   resources;  // TODO: lock-free ?
        };
        static constexpr uint       ExpiredResFrameOffset = 2;
        using VExpiredResources_t   = StaticArray< VExpiredResArray, GraphicsConfig::MaxFrames + ExpiredResFrameOffset >;

        struct VExpiredResources
        {
            AtomicFrameUID          _currentFrameId;
            VExpiredResources_t     _list;

            VExpiredResources ()                            __Th___;

            ND_ VExpiredResArray&       Get (FrameUID id)   __NE___ { return _list[ id.Remap( _list.size() )]; }
            ND_ VExpiredResArray&       GetCurrent ()       __NE___ { return Get( GetFrameId() ); }
            ND_ FrameUID                GetFrameId ()       C_NE___ { return _currentFrameId.load(); }
            ND_ VExpiredResources_t&    All ()              __NE___ { return _list; }
        };
    #endif


    // variables
    private:
        Device_t const&         _device;

        struct {
            BufferPool_t            buffers;
            ImagePool_t             images;

            BufferViewPool_t        bufferViews;
            ImageViewPool_t         imageViews;

            SamplerPool_t           samplers;

            PipelinePackPool_t      pipelinePacks;
            PipelineCachePool_t     pipelineCache;

            DSLayoutPool_t          dsLayouts;
            PplnLayoutPool_t        pplnLayouts;
            DescSetPool_t           descSet;

            GPipelinePool_t         graphicsPpln;
            CPipelinePool_t         computePpln;
            MPipelinePool_t         meshPpln;
            RTPipelinePool_t        raytracePpln;
            TPipelinePool_t         tilePpln;

            RTGeomPool_t            rtGeom;
            RTScenePool_t           rtScene;
            SBTPool_t               rtSBT;

            RenderPassPool_t        renderPass;

          #ifdef AE_ENABLE_VULKAN
            FramebufferPool_t       framebuffers;
            RWSpinLock              fbCacheGuard;
            FramebufferCache_t      fbCache;
          #endif

            VideoBufferPool_t       vbuffers;
            VideoImagePool_t        vimages;
            VideoSessionPool_t      vsessions;

            MemObjPool_t            memObjs;
        }                       _resPool;

        GfxMemAllocatorPtr      _defaultMemAlloc;
        DescriptorAllocatorPtr  _defaultDescAlloc;

        StagingBufferManager_t  _stagingMngr;
        QueryManager_t          _queryMngr;

        FeatureSet              _featureSet;

      #ifdef AE_ENABLE_VULKAN
        VExpiredResources       _expiredResources;
      #endif

        StrongAtom<PipelinePackID>      _defaultPack;
        Strong<SamplerID>               _defaultSampler;
        Strong<DescriptorSetLayoutID>   _emptyDSLayout;

      #ifdef AE_DEBUG
        mutable SharedMutex             _hashToNameGuard;
        PipelineCompiler::HashToName    _hashToName;        // for debugging
      #endif


    // methods
    private:
        explicit RESMNGR (const Device_t &)                                                                                                         __Th___;

        ND_ bool  Initialize (const GraphicsCreateInfo &)                                                                                           __Th___;
            void  Deinitialize ()                                                                                                                   __NE___;


    public:
        ~RESMNGR ()                                                                                                                                 __NE___;

        ND_ bool                    OnSurfaceCreated (const Swapchain_t &sw)                                                                        __NE___;

            bool                    InitializeResources (const PipelinePackDesc &desc)                                                              __NE_OV;
        ND_ Strong<PipelinePackID>  LoadPipelinePack (const PipelinePackDesc &desc)                                                                 __NE_OV;
        ND_ Array<RenderTechName>   GetSupportedRenderTechs (PipelinePackID id)                                                                     C_NE_OV;

        ND_ bool                    IsSupported (EMemoryType memType)                                                                               C_NE_OV;
        ND_ bool                    IsSupported (const BufferDesc &desc)                                                                            C_NE_OV;
        ND_ bool                    IsSupported (const ImageDesc &desc)                                                                             C_NE_OV;
        ND_ bool                    IsSupported (const VideoImageDesc &desc)                                                                        C_NE_OV;
        ND_ bool                    IsSupported (const VideoBufferDesc &desc)                                                                       C_NE_OV;
        ND_ bool                    IsSupported (const VideoSessionDesc &desc)                                                                      C_NE_OV;
        ND_ bool                    IsSupported (BufferID buffer, const BufferViewDesc &desc)                                                       C_NE_OV;
        ND_ bool                    IsSupported (ImageID image, const ImageViewDesc &desc)                                                          C_NE_OV;
        ND_ bool                    IsSupported (const RTGeometryDesc &desc)                                                                        C_NE_OV;
        ND_ bool                    IsSupported (const RTSceneDesc &desc)                                                                           C_NE_OV;

        ND_ Strong<ComputePipelineID>   CreatePipeline (const ComputePipeline_t::CreateInfo    &ci)                                                 __NE___;
        ND_ Strong<GraphicsPipelineID>  CreatePipeline (const GraphicsPipeline_t::CreateInfo   &ci)                                                 __NE___;
        ND_ Strong<MeshPipelineID>      CreatePipeline (const MeshPipeline_t::CreateInfo       &ci)                                                 __NE___;
        ND_ Strong<RayTracingPipelineID>CreatePipeline (const RayTracingPipeline_t::CreateInfo &ci)                                                 __NE___;
        ND_ Strong<TilePipelineID>      CreatePipeline (const TilePipeline_t::CreateInfo       &ci)                                                 __NE___;

        ND_ Strong<RTShaderBindingID>   CreateRTShaderBinding (const ShaderBindingTable_t::CreateInfo &ci)                                          __NE___;

        ND_ Strong<DescriptorSetLayoutID>   CreateDescriptorSetLayout (const DescriptorSetLayout_t::CreateInfo &ci)                                 __NE___;

        ND_ Strong<PipelineLayoutID>    CreatePipelineLayout (const PipelineLayout_t::DescriptorSets_t &descSetLayouts,
                                                              const PipelineLayout_t::PushConstants_t &pushConstants, StringView dbgName = Default) __NE___;

        ND_ Strong<ImageID>         CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)          __NE_OV;
        ND_ Strong<BufferID>        CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)        __NE_OV;

        ND_ Strong<ImageID>         CreateImage (const NativeImageDesc_t &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)  __NE_OV;
        ND_ Strong<BufferID>        CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE_OV;

        ND_ Strong<ImageViewID>     CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)                        __NE_OV;
        ND_ Strong<BufferViewID>    CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)                    __NE_OV;

        ND_ Strong<ImageViewID>     CreateImageView (const NativeImageViewDesc_t &desc, ImageID image, StringView dbgName = Default)                __NE_OV;
        ND_ Strong<BufferViewID>    CreateBufferView (const NativeBufferViewDesc_t &desc, BufferID buffer, StringView dbgName = Default)            __NE_OV;

        ND_ Strong<RTGeometryID>    CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE_OV;
        ND_ Strong<RTSceneID>       CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)      __NE_OV;

        ND_ RTASBuildSizes          GetRTGeometrySizes (const RTGeometryBuild &desc)                                                                __NE_OV;
        ND_ RTASBuildSizes          GetRTSceneSizes (const RTSceneBuild &desc)                                                                      __NE_OV;

        ND_ DeviceAddress           GetDeviceAddress (BufferID      id)                                                                             C_NE_OV;
        ND_ DeviceAddress           GetDeviceAddress (RTGeometryID  id)                                                                             C_NE_OV;

        ND_ bool                    CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                                          GraphicsPipelineID ppln, const DescriptorSetName &dsName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;
        ND_ bool                    CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                                          MeshPipelineID ppln, const DescriptorSetName &dsName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;
        ND_ bool                    CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                                          ComputePipelineID ppln, const DescriptorSetName &dsName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;
        ND_ bool                    CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                                          RayTracingPipelineID ppln, const DescriptorSetName &dsName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;
        ND_ bool                    CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                                          TilePipelineID ppln, const DescriptorSetName &dsName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;

        ND_ bool                    CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count,
                                                          PipelinePackID packId, const DSLayoutName &dslName,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;
        ND_ bool                    CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, DescriptorSetLayoutID layoutId,
                                                          DescriptorAllocatorPtr allocator = null, StringView dbgName = Default)                    __NE_OV;

        ND_ PushConstantIndex       GetPushConstantIndex (GraphicsPipelineID   ppln, const PushConstantName &pcName, const ShaderStructName &typeName, Bytes dataSize)  __NE_OV;
        ND_ PushConstantIndex       GetPushConstantIndex (MeshPipelineID       ppln, const PushConstantName &pcName, const ShaderStructName &typeName, Bytes dataSize)  __NE_OV;
        ND_ PushConstantIndex       GetPushConstantIndex (ComputePipelineID    ppln, const PushConstantName &pcName, const ShaderStructName &typeName, Bytes dataSize)  __NE_OV;
        ND_ PushConstantIndex       GetPushConstantIndex (RayTracingPipelineID ppln, const PushConstantName &pcName, const ShaderStructName &typeName, Bytes dataSize)  __NE_OV;
        ND_ PushConstantIndex       GetPushConstantIndex (TilePipelineID       ppln, const PushConstantName &pcName, const ShaderStructName &typeName, Bytes dataSize)  __NE_OV;

        using IResourceManager::GetPushConstantIndex;

        ND_ NativeBuffer_t          GetBufferHandle (BufferID id)                                                                                   C_NE_OV;
        ND_ NativeImage_t           GetImageHandle (ImageID id)                                                                                     C_NE_OV;
        ND_ NativeBufferView_t      GetBufferViewHandle (BufferViewID id)                                                                           C_NE_OV;
        ND_ NativeImageView_t       GetImageViewHandle (ImageViewID id)                                                                             C_NE_OV;

        ND_ SamplerID               GetSampler (const SamplerName &name)                                                                            C_NE___;

        ND_ RenderPassID            GetCompatibleRenderPass (PipelinePackID packId, const CompatRenderPassName &name)                               C_NE___;
        ND_ RenderPassID            GetCompatibleRenderPass (PipelinePackID packId, const RenderPassName &name)                                     C_NE___;
        ND_ RenderPassID            GetRenderPass (PipelinePackID packId, const RenderPassName &name)                                               C_NE___;

        ND_ Strong<PipelineCacheID>     CreatePipelineCache ()                                                                                      __NE_OV;

        ND_ Strong<VideoSessionID>      CreateVideoSession (const VideoSessionDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)__NE_OV;
        ND_ Strong<VideoBufferID>       CreateVideoBuffer (const VideoBufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)  __NE_OV;
        ND_ Strong<VideoImageID>        CreateVideoImage (const VideoImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)    __NE_OV;

        ND_ AsyncRTechPipelines         LoadRenderTechAsync (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)              __NE_OV;
        ND_ RenderTechPipelinesPtr      LoadRenderTech      (PipelinePackID packId, const RenderTechName &name, PipelineCacheID cache)              __NE_OV;

        ND_ Strong<GraphicsPipelineID>  CreateGraphicsPipeline   (PipelinePackID packId, const PipelineTmplName &name, const GraphicsPipelineDesc   &desc, PipelineCacheID cache = Default) __NE_OV;
        ND_ Strong<MeshPipelineID>      CreateMeshPipeline       (PipelinePackID packId, const PipelineTmplName &name, const MeshPipelineDesc       &desc, PipelineCacheID cache = Default) __NE_OV;
        ND_ Strong<ComputePipelineID>   CreateComputePipeline    (PipelinePackID packId, const PipelineTmplName &name, const ComputePipelineDesc    &desc, PipelineCacheID cache = Default) __NE_OV;
        ND_ Strong<RayTracingPipelineID>CreateRayTracingPipeline (PipelinePackID packId, const PipelineTmplName &name, const RayTracingPipelineDesc &desc, PipelineCacheID cache = Default) __NE_OV;
        ND_ Strong<TilePipelineID>      CreateTilePipeline       (PipelinePackID packId, const PipelineTmplName &name, const TilePipelineDesc       &desc, PipelineCacheID cache = Default) __NE_OV;

            bool    GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)                            C_NE_OV;
            bool    GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)                           C_NE_OV;
            bool    GetMemoryInfo (MemoryID id, OUT NativeMemObjInfo_t &info)                           C_NE___;

            bool    ReleaseResource (INOUT Strong<ImageID>              &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<BufferID>             &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<ImageViewID>          &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<BufferViewID>         &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<PipelineCacheID>      &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<PipelinePackID>       &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<GraphicsPipelineID>   &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<MeshPipelineID>       &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<ComputePipelineID>    &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<RayTracingPipelineID> &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<TilePipelineID>       &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<DescriptorSetID>      &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<RTGeometryID>         &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<RTSceneID>            &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<VideoSessionID>       &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<VideoBufferID>        &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }
            bool    ReleaseResource (INOUT Strong<VideoImageID>         &id)                            __NE_OV { return DelayedRelease( INOUT id ) == 0; }

            template <typename ArrayType>
            void    ReleaseResourceArray (INOUT ArrayType &arr)                                         __NE___ { for (auto& id : arr) {DelayedRelease( INOUT id );} }

            template <usize IS, usize GS, uint UID>
            bool    ReleaseResource (INOUT Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)   __NE___ { return DelayedRelease( INOUT id, refCount ) == 0; }

            template <typename Arg0, typename ...Args>
            void    ReleaseResources (Arg0 &arg0, Args& ...args)                                        __NE___ { DelayedReleaseResources( FwdArg<Arg0&>(arg0), FwdArg<Args&>(args)... ); }

            template <usize IS, usize GS, uint UID>
            int     DelayedRelease (INOUT Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)    __NE___ { return _DelayedReleaseResource( id.Release(), refCount ); }

            template <typename Arg0, typename ...Args>
            void    DelayedReleaseResources (Arg0 &arg0, Args& ...args)                                 __NE___;

            template <usize IS, usize GS, uint UID>
            int     ImmediatelyRelease (INOUT Strong< HandleTmpl< IS, GS, UID >> &id, uint refCount = 1)__NE___ { return _ImmediatelyReleaseResource( id.Release(), refCount ); }

            template <typename Arg0, typename ...Args>
            void    ImmediatelyReleaseResources (Arg0 &arg0, Args& ...args)                             __NE___;

            bool    ForceReleaseResources ()                                                            __NE_OV;

            void    PrintAllResources ()                                                                __NE___;

        ND_ BufferDesc const&           GetDescription (BufferID id)                                    C_NE_OV;
        ND_ ImageDesc const&            GetDescription (ImageID id)                                     C_NE_OV;
        ND_ BufferViewDesc const&       GetDescription (BufferViewID id)                                C_NE_OV;
        ND_ ImageViewDesc const&        GetDescription (ImageViewID id)                                 C_NE_OV;
        ND_ RTShaderBindingDesc const&  GetDescription (RTShaderBindingID id)                           C_NE_OV;
        ND_ RTGeometryDesc const&       GetDescription (RTGeometryID id)                                C_NE_OV;
        ND_ RTSceneDesc const&          GetDescription (RTSceneID id)                                   C_NE_OV;
        ND_ VideoImageDesc const&       GetDescription (VideoImageID id)                                C_NE_OV;
        ND_ VideoBufferDesc const&      GetDescription (VideoBufferID id)                               C_NE_OV;
        ND_ VideoSessionDesc const&     GetDescription (VideoSessionID id)                              C_NE_OV;

        ND_ FeatureSet const&       GetFeatureSet ()                                                    C_NE_OV { return _featureSet; }
        ND_ PipelinePackID          GetDefaultPack ()                                                   C_NE___ { return _defaultPack; }

        template <usize IS, usize GS, uint UID>
        ND_ bool                    IsAlive (HandleTmpl<IS,GS,UID> id)                                  C_NE___;
        ND_ bool                    IsAlive (const SamplerName &name)                                   C_NE___;

        ND_ bool                    IsResourceAlive (BufferID           id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (ImageID            id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (BufferViewID       id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (ImageViewID        id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (DescriptorSetID    id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (PipelineCacheID    id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (PipelinePackID     id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (RTGeometryID       id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (RTSceneID          id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (VideoSessionID     id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (VideoBufferID      id)                             C_NE_OV { return IsAlive( id ); }
        ND_ bool                    IsResourceAlive (VideoImageID       id)                             C_NE_OV { return IsAlive( id ); }

        ND_ Strong<ImageID>         AcquireResource (ImageID            id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<BufferID>        AcquireResource (BufferID           id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<ImageViewID>     AcquireResource (ImageViewID        id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<BufferViewID>    AcquireResource (BufferViewID       id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<DescriptorSetID> AcquireResource (DescriptorSetID    id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<RTGeometryID>    AcquireResource (RTGeometryID       id)                             __NE_OV { return _AcquireResource( id ); }
        ND_ Strong<RTSceneID>       AcquireResource (RTSceneID          id)                             __NE_OV { return _AcquireResource( id ); }

        template <usize IS, usize GS, uint UID>
        ND_ auto            AcquireResource (HandleTmpl<IS, GS, UID> id)                                __NE___ { return _AcquireResource( id ); }

        template <typename ID>
        ND_ auto const*     GetResource (ID id, Bool incRef = False{}, Bool quiet = False{})            C_NE___;

        template <typename ID>
        ND_ auto const*     GetResource (const Strong<ID> &id, Bool incRef = False{}, Bool quiet = False{}) C_NE___;

        template <typename ID>
        ND_ auto*           GetResources (ID id)                                                        C_NE___;

        template <typename ID0, typename ID1, typename ...IDs>
        ND_ auto            GetResources (ID0 id0, ID1 id1, IDs ...ids)                                 C_NE___;

        template <typename ID>
        ND_ auto&           GetResourcesOrThrow (ID id)                                                 C_Th___;

        template <typename ID0, typename ID1, typename ...IDs>
        ND_ auto            GetResourcesOrThrow (ID0 id0, ID1 id1, IDs ...ids)                          C_Th___;

            void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg)                          __NE___;
            void  OnEndFrame (FrameUID frameId)                                                         __NE___;

        template <usize Size, uint UID, bool Opt, uint Seed>
        ND_ String  HashToName (const NamedID< Size, UID, Opt, Seed > &name)                            C_NE___;

        #ifdef AE_DEBUG
            void  AddHashToName (const PipelineCompiler::HashToName &value)                             __NE___;
        #endif

        ND_ Device_t const&         GetDevice ()                                                        C_NE___ { return _device; }
        ND_ StagingBufferManager_t& GetStagingManager ()                                                __NE___ { return _stagingMngr; }
        ND_ QueryManager_t&         GetQueryManager ()                                                  __NE___ { return _queryMngr; }

        ND_ StagingBufferStat       GetStagingBufferFrameStat (FrameUID frameId)                        C_NE_OV { return _stagingMngr.GetFrameStat( frameId ); }

        // memory allocators
        ND_ GfxMemAllocatorPtr      CreateLinearGfxMemAllocator (Bytes pageSize = 0_b)                  C_NE_OV;
        ND_ GfxMemAllocatorPtr      CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize)        C_NE_OV;
        ND_ GfxMemAllocatorPtr      CreateUnifiedGfxMemAllocator (Bytes pageSize = 0_b)                 C_NE_OV;
        ND_ GfxMemAllocatorPtr      GetDefaultGfxMemAllocator ()                                        C_NE_OV { return _defaultMemAlloc; }

        // descriptor allocators
        ND_ DescriptorAllocatorPtr  GetDefaultDescriptorAllocator ()                                    C_NE_OV { return _defaultDescAlloc; }

        AE_GLOBALLY_ALLOC

    private:

    // resource api
            template <typename ID>
        ND_ int  _DelayedReleaseResource (ID id, uint refCount = 1)             __NE___;

            template <typename ID>
            void  _DelayedReleaseResource2 (ID id)                              __NE___;

            template <typename ID>
            void  _DestroyResource (ID id)                                      __NE___;

            template <typename ID>
        ND_ int  _ImmediatelyReleaseResource (ID id, uint refCount = 1)         __NE___;

        template <usize IS, usize GS, uint UID>
        ND_ auto  _AcquireResource (HandleTmpl<IS, GS, UID> id)                 __NE___ -> Strong< HandleTmpl<IS, GS, UID>>;

        template <typename ID, typename ...Args>
        ND_ Strong<ID>  _CreateResource (const char* msg, Args&& ...args)       __NE___;

        template <typename PplnID>
        ND_ bool  _CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count,
                                         const PplnID &pplnId, const DescriptorSetName &dsName,
                                         DescriptorAllocatorPtr allocator, StringView dbgName) __NE___;

        template <typename PplnID>
        ND_ PushConstantIndex  _GetPushConstantIndex (PplnID ppln, const PushConstantName &pcName,
                                                      const ShaderStructName &typeName, Bytes dataSize) __NE___;

    // resource pool
        ND_ auto&       _GetResourcePool (const BufferID &)                     __NE___ { return _resPool.buffers; }
        ND_ auto&       _GetResourcePool (const ImageID &)                      __NE___ { return _resPool.images; }
        ND_ auto&       _GetResourcePool (const BufferViewID &)                 __NE___ { return _resPool.bufferViews; }
        ND_ auto&       _GetResourcePool (const ImageViewID &)                  __NE___ { return _resPool.imageViews; }
        ND_ auto&       _GetResourcePool (const DescriptorSetLayoutID &)        __NE___ { return _resPool.dsLayouts; }
        ND_ auto&       _GetResourcePool (const GraphicsPipelineID &)           __NE___ { return _resPool.graphicsPpln; }
        ND_ auto&       _GetResourcePool (const ComputePipelineID &)            __NE___ { return _resPool.computePpln; }
        ND_ auto&       _GetResourcePool (const MeshPipelineID &)               __NE___ { return _resPool.meshPpln; }
        ND_ auto&       _GetResourcePool (const RayTracingPipelineID &)         __NE___ { return _resPool.raytracePpln; }
        ND_ auto&       _GetResourcePool (const TilePipelineID &)               __NE___ { return _resPool.tilePpln; }
        ND_ auto&       _GetResourcePool (const DescriptorSetID &)              __NE___ { return _resPool.descSet; }
        ND_ auto&       _GetResourcePool (const PipelineCacheID &)              __NE___ { return _resPool.pipelineCache; }
        ND_ auto&       _GetResourcePool (const PipelinePackID &)               __NE___ { return _resPool.pipelinePacks; }
        ND_ auto&       _GetResourcePool (const RTGeometryID &)                 __NE___ { return _resPool.rtGeom; }
        ND_ auto&       _GetResourcePool (const RTSceneID &)                    __NE___ { return _resPool.rtScene; }
        ND_ auto&       _GetResourcePool (const RTShaderBindingID &)            __NE___ { return _resPool.rtSBT; }
        ND_ auto&       _GetResourcePool (const PipelineLayoutID &)             __NE___ { return _resPool.pplnLayouts; }
        ND_ auto&       _GetResourcePool (const SamplerID &)                    __NE___ { return _resPool.samplers; }
        ND_ auto&       _GetResourcePool (const RenderPassID &)                 __NE___ { return _resPool.renderPass; }
        ND_ auto&       _GetResourcePool (const MemoryID &)                     __NE___ { return _resPool.memObjs; }
        ND_ auto&       _GetResourcePool (const VideoBufferID &)                __NE___ { return _resPool.vbuffers; }
        ND_ auto&       _GetResourcePool (const VideoImageID &)                 __NE___ { return _resPool.vimages; }
        ND_ auto&       _GetResourcePool (const VideoSessionID &)               __NE___ { return _resPool.vsessions; }

        template <typename ID>
        ND_ const auto&  _GetResourceCPool (const ID &id)                       C_NE___ { return const_cast< RemoveAllQualifiers<decltype(*this)> &>(*this)._GetResourcePool( id ); }

        ND_ StringView  _GetResourcePoolName (const BufferID &)                 __NE___ { return "buffers"; }
        ND_ StringView  _GetResourcePoolName (const ImageID &)                  __NE___ { return "images"; }
        ND_ StringView  _GetResourcePoolName (const BufferViewID &)             __NE___ { return "bufferViews"; }
        ND_ StringView  _GetResourcePoolName (const ImageViewID &)              __NE___ { return "imageViews"; }
        ND_ StringView  _GetResourcePoolName (const DescriptorSetLayoutID&)     __NE___ { return "dsLayouts"; }
        ND_ StringView  _GetResourcePoolName (const GraphicsPipelineID &)       __NE___ { return "graphicsPpln"; }
        ND_ StringView  _GetResourcePoolName (const ComputePipelineID &)        __NE___ { return "computePpln"; }
        ND_ StringView  _GetResourcePoolName (const MeshPipelineID &)           __NE___ { return "meshPpln"; }
        ND_ StringView  _GetResourcePoolName (const RayTracingPipelineID &)     __NE___ { return "raytracePpln"; }
        ND_ StringView  _GetResourcePoolName (const TilePipelineID &)           __NE___ { return "tilePpln"; }
        ND_ StringView  _GetResourcePoolName (const DescriptorSetID &)          __NE___ { return "descSet"; }
        ND_ StringView  _GetResourcePoolName (const PipelineCacheID &)          __NE___ { return "pipelineCache"; }
        ND_ StringView  _GetResourcePoolName (const PipelinePackID &)           __NE___ { return "pipelinePacks"; }
        ND_ StringView  _GetResourcePoolName (const RTGeometryID &)             __NE___ { return "rtGeometry"; }
        ND_ StringView  _GetResourcePoolName (const RTSceneID &)                __NE___ { return "rtScene"; }
        ND_ StringView  _GetResourcePoolName (const RTShaderBindingID &)        __NE___ { return "rtSBT"; }
        ND_ StringView  _GetResourcePoolName (const PipelineLayoutID &)         __NE___ { return "pplnLayouts"; }
        ND_ StringView  _GetResourcePoolName (const SamplerID &)                __NE___ { return "samplers"; }
        ND_ StringView  _GetResourcePoolName (const RenderPassID &)             __NE___ { return "renderPass"; }
        ND_ StringView  _GetResourcePoolName (const MemoryID &)                 __NE___ { return "memObjs"; }
        ND_ StringView  _GetResourcePoolName (const VideoBufferID &)            __NE___ { return "videoBuffers"; }
        ND_ StringView  _GetResourcePoolName (const VideoImageID &)             __NE___ { return "videoImages"; }
        ND_ StringView  _GetResourcePoolName (const VideoSessionID &)           __NE___ { return "videoSessions"; }

      #ifdef AE_ENABLE_VULKAN
        ND_ auto&       _GetResourcePool (const VFramebufferID &)               __NE___ { return _resPool.framebuffers; }
        ND_ StringView  _GetResourcePoolName (const VFramebufferID &)           __NE___ { return "framebuffers"; }
      #endif

    // 
        template <typename ID>  ND_ bool   _Assign (OUT ID &id)                 __NE___;
        template <typename ID>      void   _Unassign (ID id)                    __NE___;

    // 
        template <typename ID>
        ND_ auto const&         _GetDescription (ID id)                         C_NE___;

    // memory management
        ND_ GfxMemAllocatorPtr      _ChooseMemAllocator (GfxMemAllocatorPtr userDefined)        __NE___;
        ND_ DescriptorAllocatorPtr  _ChooseDescAllocator (DescriptorAllocatorPtr userDefined)   __NE___;

    // default resources
        ND_ bool  _CreateDefaultSampler ()                                      __NE___;

    // empty descriptor set layout
        ND_ bool  _CreateEmptyDescriptorSetLayout ()                            __NE___;
        ND_ auto  _GetEmptyDescriptorSetLayout ()                               __NE___ { return *_emptyDSLayout; }


    //-----------------------------------------------------
    #if defined(AE_ENABLE_VULKAN)

    // variables
    private:
        using ReleaseResourceByID_t     = void (*) (RESMNGR &, VExpiredResource::IDValue_t) __NE___;
        using ReleaseResourceByIDFns_t  = StaticArray< ReleaseResourceByID_t, AllVkResources_t::Count >;
        struct _InitReleaseResourceByID;

        ReleaseResourceByIDFns_t    _releaseResIDs  {};


    // methods
    public:
        ND_ Strong<MemoryID>        CreateMemoryObj (VkBuffer buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___;
        ND_ Strong<MemoryID>        CreateMemoryObj (VkImage image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)    __NE___;

        ND_ Strong<PipelineCacheID> LoadPipelineCache (RC<RStream> stream)                                                  __NE___;

        ND_ Strong<SamplerID>       CreateSampler (const VkSamplerCreateInfo &, const VkSamplerYcbcrConversionCreateInfo *,
                                                   StringView dbgName = Default)                                            __NE___;
        ND_ VkSampler               GetVkSampler (const SamplerName &name)                                                  C_NE___;

        ND_ Strong<RenderPassID>    CreateRenderPass (const SerializableRenderPassInfo &, const SerializableVkRenderPass &,
                                                      RenderPassID compatId, StringView dbgName = Default)                  __NE___;
        ND_ VFramebufferID          CreateFramebuffer (const RenderPassDesc &desc)                                          __NE___;
            void                    RemoveFramebufferCache (VFramebuffer::CachePtr_t iter)                                  __NE___;    // call from VFramebuffer

            void                    DelayedRelease (VkSwapchainKHR handle)                                                  __NE___ { _DelayedReleaseResource2( handle ); }

    private:

        void  _InitReleaseResourceByIDFns ()                                                                                __NE___;

        // resource api
        void  _DestroyVkResource (const VExpiredResource &res)                                                              __NE___;


    //-----------------------------------------------------
    #elif defined(AE_ENABLE_METAL)

    // methods
    public:
        ND_ bool    CreateBufferAndMemoryObj (OUT Strong<MemoryID> &memId, OUT MetalBufferRC &buffer, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)         __NE___;
        ND_ bool    CreateImageAndMemoryObj (OUT Strong<MemoryID> &memId, OUT MetalImageRC &image, const ImageDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)             __NE___;
        ND_ bool    CreateAccelStructAndMemoryObj (OUT Strong<MemoryID> &memId, OUT MetalAccelStructRC &as, const RTGeometryDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)   __NE___;
        ND_ bool    CreateAccelStructAndMemoryObj (OUT Strong<MemoryID> &memId, OUT MetalAccelStructRC &as, const RTSceneDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName)      __NE___;

        ND_ Strong<PipelineCacheID> LoadPipelineCache (const Path &filename)                                                __NE___;

        ND_ MetalSampler            GetMtlSampler (const SamplerName &name)                                                 C_NE___;
        ND_ Strong<SamplerID>       CreateSampler (const SamplerDesc &desc, StringView dbgName)                             __NE___;

        ND_ Strong<RenderPassID>    CreateRenderPass (const SerializableRenderPassInfo &, const SerializableMtlRenderPass &,
                                                      StringView dbgName = Default)                                         __NE___;


    //-----------------------------------------------------
    #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        // TODO

    //-----------------------------------------------------
    #else
    #   error not implemented
    #endif
    };
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN

/*
=================================================
    _DelayedReleaseResource
=================================================
*/
    template <typename ID>
    int  RESMNGR::_DelayedReleaseResource (ID id, uint refCount) __NE___
    {
        ASSERT( refCount > 0 );

        if_unlikely( not id )
            return -1;

        auto&   pool = _GetResourcePool( id );

        if_likely( auto* res = pool.At( id.Index() ))
        {
            if_likely( res->GetGeneration() == id.Generation() )
            {
                int count = res->ReleaseRef( refCount );

                if_unlikely( count == 0 and res->IsCreated() )
                {
                    _DelayedReleaseResource2( id );
                }
                return count;
            }
        }
        return -1;
    }

    template <typename ID>
    void  RESMNGR::_DelayedReleaseResource2 (ID id) __NE___
    {
        STATIC_ASSERT( AllVkResources_t::HasType<ID> );
        ASSERT( id != Default );

        VExpiredResource    expired;
        expired.id      = UnsafeBitCast< VExpiredResource::IDValue_t >( id );
        expired.type    = uint( AllVkResources_t::Index<ID> );

        auto&   dst = _expiredResources.GetCurrent();
        EXLOCK( dst.guard );
        ASSERT( dst.frameId == _expiredResources.GetFrameId() );

        dst.resources.push_back( expired );
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
        const FrameUID  _frameId;

    public:
        explicit VReleaseExpiredResourcesTask (FrameUID frameId) __NE___ :
            IAsyncTask{ETaskQueue::Renderer}, _frameId{frameId}
        {}

        void        Run ()      __Th_OV;
        StringView  DbgName ()  C_NE_OV { return "ReleaseExpiredResources"; }
    };

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_METAL
/*
=================================================
    _DelayedReleaseResource
=================================================
*/
    template <typename ID>
    int  RESMNGR::_DelayedReleaseResource (ID id, uint refCount) __NE___
    {
        // TODO: delayed destruction required for untracked resources

        return _ImmediatelyReleaseResource( id, refCount );
    }

#endif // AE_ENABLE_METAL
//-----------------------------------------------------------------------------



/*
=================================================
    GetResource
=================================================
*/
    template <typename ID>
    auto const*  RESMNGR::GetResource (ID id, Bool incRef, Bool quiet) C_NE___
    {
        auto&   pool = _GetResourceCPool( id );

        using Result_t = typename RemoveReference<decltype(pool)>::Value_t::Resource_t const*;

        if_likely( auto* res = pool.At( id.Index() ))
        {
            if_likely( res->IsCreated() & (res->GetGeneration() == id.Generation()) )
            {
                if_unlikely( incRef )
                    res->AddRef();

                return &res->Data();
            }

            Unused( quiet );
            ASSERT( quiet or res->IsCreated() );
            ASSERT( quiet or res->GetGeneration() == id.Generation() );
        }

        ASSERT( quiet and "resource index is out of range" );
        return static_cast< Result_t >(null);
    }

    template <typename ID>
    auto const*  RESMNGR::GetResource (const Strong<ID> &id, Bool incRef, Bool quiet) C_NE___
    {
        return GetResource( *id, incRef, quiet );
    }

/*
=================================================
    GetDescription
=================================================
*/
    template <typename ID>
    auto const&  RESMNGR::_GetDescription (ID id) C_NE___
    {
        auto*   res = GetResource( id );

        using T = RemoveAllQualifiers< decltype(*res) >;
        static const typename FunctionInfo< decltype(&T::Description) >::result defaultDesc{};

        return res ? res->Description() : defaultDesc;
    }

    inline BufferDesc const&            RESMNGR::GetDescription (BufferID           id) C_NE___ { return _GetDescription( id ); }
    inline ImageDesc const&             RESMNGR::GetDescription (ImageID            id) C_NE___ { return _GetDescription( id ); }
    inline BufferViewDesc const&        RESMNGR::GetDescription (BufferViewID       id) C_NE___ { return _GetDescription( id ); }
    inline ImageViewDesc const&         RESMNGR::GetDescription (ImageViewID        id) C_NE___ { return _GetDescription( id ); }
    inline RTShaderBindingDesc const&   RESMNGR::GetDescription (RTShaderBindingID  id) C_NE___ { return _GetDescription( id ); }
    inline RTGeometryDesc const&        RESMNGR::GetDescription (RTGeometryID       id) C_NE___ { return _GetDescription( id ); }
    inline RTSceneDesc const&           RESMNGR::GetDescription (RTSceneID          id) C_NE___ { return _GetDescription( id ); }
    inline VideoImageDesc const&        RESMNGR::GetDescription (VideoImageID       id) C_NE___ { return _GetDescription( id ); }
    inline VideoBufferDesc const&       RESMNGR::GetDescription (VideoBufferID      id) C_NE___ { return _GetDescription( id ); }
    inline VideoSessionDesc const&      RESMNGR::GetDescription (VideoSessionID     id) C_NE___ { return _GetDescription( id ); }

/*
=================================================
    IsAlive
=================================================
*/
    template <usize IS, usize GS, uint UID>
    bool  RESMNGR::IsAlive (HandleTmpl<IS,GS,UID> id) C_NE___
    {
        auto&   pool = _GetResourceCPool( id );

        if_likely( auto* res = pool.At( id.Index() ))
            return res->GetGeneration() == id.Generation();

        return false;
    }

/*
=================================================
    _AcquireResource
=================================================
*/
    template <usize IS, usize GS, uint UID>
    auto  RESMNGR::_AcquireResource (HandleTmpl<IS, GS, UID> id) __NE___ -> Strong< HandleTmpl<IS, GS, UID>>
    {
        using Unique_t = Strong< HandleTmpl< IS, GS, UID >>;

        ASSERT( id );

        auto&   pool = _GetResourcePool( id );

        if_likely( auto* res = pool.At( id.Index() ))
        {
            if_unlikely( not res->IsCreated() or res->GetGeneration() != id.Generation() )
                return Unique_t{};

            res->AddRef();
            return Unique_t{ id };
        }

        return Unique_t{};
    }

/*
=================================================
    _Assign
----
    acquire free index from cache (cache is local in thread),
    if cache empty then acquire new indices from main pool (internally synchronized),
    if pool is full then error (false) will be returned.
=================================================
*/
    template <typename ID>
    bool  RESMNGR::_Assign (OUT ID &id) __NE___
    {
        auto&   pool    = _GetResourcePool( id );
        auto    index   = pool.Assign();
        CHECK_ERR( index != UMax );

        id = ID{ index, pool[index].GetGeneration() };
        return true;
    }

/*
=================================================
    _Unassign
=================================================
*/
    template <typename ID>
    void  RESMNGR::_Unassign (ID id) __NE___
    {
        ASSERT( id );
        auto&   pool = _GetResourcePool( id );

        pool.Unassign( id.Index() );
    }

/*
=================================================
    _ImmediatelyReleaseResource
=================================================
*/
    template <typename ID>
    int  RESMNGR::_ImmediatelyReleaseResource (ID id, uint refCount) __NE___
    {
        ASSERT( refCount > 0 );

        if_unlikely( not id )
            return -1;

        auto&   pool = _GetResourcePool( id );

        if_likely( auto* res = pool.At( id.Index() ))
        {
            if_likely( res->GetGeneration() == id.Generation() )
            {
                int count = res->ReleaseRef( refCount );

                if_unlikely( count == 0 and res->IsCreated() )
                {
                    res->Destroy( *this );
                    pool.Unassign( id.Index() );
                }
                return count;
            }
        }
        return -1;
    }

/*
=================================================
    ImmediatelyReleaseResources
=================================================
*/
    template <typename Arg0, typename ...Args>
    void  RESMNGR::ImmediatelyReleaseResources (Arg0 &arg0, Args& ...args) __NE___
    {
        ImmediatelyRelease( INOUT arg0 );

        if constexpr( CountOf<Args...>() > 0 )
            return ImmediatelyReleaseResources( FwdArg<Args&>( args )... );
    }

/*
=================================================
    DelayedReleaseResources
=================================================
*/
    template <typename Arg0, typename ...Args>
    void  RESMNGR::DelayedReleaseResources (Arg0 &arg0, Args& ...args) __NE___
    {
        DelayedRelease( INOUT FwdArg<Arg0&>( arg0 ));

        if constexpr( CountOf<Args...>() > 0 )
            return DelayedReleaseResources( FwdArg<Args&>( args )... );
    }

/*
=================================================
    HashToName
=================================================
*/
    template <usize Size, uint UID, bool Opt, uint Seed>
    String  RESMNGR::HashToName (const NamedID< Size, UID, Opt, Seed > &name) C_NE___
    {
        #ifdef AE_DEBUG
            if constexpr( Opt )
            {
                SHAREDLOCK( _hashToNameGuard );
                return _hashToName( name );
            }
            else
            {
                if_unlikely( name.GetName().empty() )
                {
                    SHAREDLOCK( _hashToNameGuard );
                    return _hashToName( name );
                }
                return String{name.GetName()};
            }
        #else
            Unused( name );
            return {};
        #endif
    }

/*
=================================================
    Get***Handle
=================================================
*/
    inline RESMNGR::NativeBuffer_t  RESMNGR::GetBufferHandle (BufferID id) C_NE___
    {
        auto*   buf = GetResource( id );
        CHECK_ERR( buf );

        return buf->Handle();
    }

    inline RESMNGR::NativeImage_t  RESMNGR::GetImageHandle (ImageID id) C_NE___
    {
        auto*   img = GetResource( id );
        CHECK_ERR( img );

        return img->Handle();
    }

    inline RESMNGR::NativeBufferView_t  RESMNGR::GetBufferViewHandle (BufferViewID id) C_NE___
    {
        auto*   view = GetResource( id );
        CHECK_ERR( view );

        return view->Handle();
    }

    inline RESMNGR::NativeImageView_t  RESMNGR::GetImageViewHandle (ImageViewID id) C_NE___
    {
        auto*   view = GetResource( id );
        CHECK_ERR( view );

        return view->Handle();
    }

/*
=================================================
    GetResources
=================================================
*/
    template <typename ID>
    auto*  RESMNGR::GetResources (ID id) C_NE___
    {
        return GetResource( id );
    }

    template <typename ID0, typename ID1, typename ...IDs>
    auto  RESMNGR::GetResources (ID0 id0, ID1 id1, IDs ...ids) C_NE___
    {
        return Tuple{ GetResource( id0 ), GetResource( id1 ), GetResource( ids ) ... };
    }

    template <typename ID>
    auto&  RESMNGR::GetResourcesOrThrow (ID id) C_Th___
    {
        auto*   result = GetResource( id );
        CHECK_THROW( result != null );
        return *result;
    }

    template <typename ID0, typename ID1, typename ...IDs>
    auto  RESMNGR::GetResourcesOrThrow (ID0 id0, ID1 id1, IDs ...ids) C_Th___
    {
        auto    result = TupleRef{ GetResource( id0 ), GetResource( id1 ), GetResource( ids ) ... };
        CHECK_THROW( result.AllNonNull() );
        return result;
    }

} // AE::Graphics
//-----------------------------------------------------------------------------

#undef RESMNGR
#undef SUFFIX

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "VulkanSyncLog.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Containers/FixedMap.h"
#include "graphics/Public/ImageUtils.h"

using namespace AE;
using namespace AE::Graphics;
using namespace AE::Threading;

#define PRINT_ALL_DS    1

namespace
{
#   include "vulkan_loader/vkenum_to_str.h"

    struct DeviceFnTable
    {
#       define VKLOADER_STAGE_FNPOINTER
#        include "vulkan_loader/fn_vulkan_dev.h"
#       undef  VKLOADER_STAGE_FNPOINTER
    };
    STATIC_ASSERT( sizeof(DeviceFnTable) == sizeof(VulkanDeviceFnTable) );



    //
    // Vulkan Logger
    //
    struct VulkanLogger : public VulkanDeviceFn
    {
    // types
    public:
        struct ImageData
        {
            VkImage                 image   = Default;
            String                  name    = {};
            VkImageCreateInfo       info    = {};
        };
        using ImageMap_t = FlatHashMap< VkImage, ImageData >;


        struct ImageViewData
        {
            VkImageView             view    = Default;
            String                  name    = {};
            VkImageViewCreateInfo   info    = {};
        };
        using ImageViewMap_t = FlatHashMap< VkImageView, ImageViewData >;

        struct DeviceAddressRange
        {
            ulong                   address = 0;
            ulong                   size    = 0;
            VkBuffer                buffer  = Default;
        };
        struct DeviceAddressKey
        {
            ulong                   address = 0;

            explicit DeviceAddressKey (VkDeviceAddress addr) : address{addr} {}

            ND_ bool  operator >  (const DeviceAddressRange &rhs) const { return address >= rhs.address + rhs.size; }
            ND_ bool  operator == (const DeviceAddressRange &rhs) const { return address >= rhs.address and address < rhs.address + rhs.size; }
        };
        using DevAddressToBuffer_t  = Array< DeviceAddressRange >;

        struct BufferData
        {
            VkBuffer                buffer  = Default;
            VkDeviceAddress         address = 0;
            String                  name    = {};
            VkBufferCreateInfo      info    = {};
        };
        using BufferMap_t = FlatHashMap< VkBuffer, BufferData >;


        struct BufferViewData
        {
            VkBufferView            view    = Default;
            String                  name    = {};
            VkBufferViewCreateInfo  info    = {};
        };
        using BufferViewMap_t = FlatHashMap< VkBufferView, BufferViewData >;


        struct AccelStructData
        {
            String                  name;
        };
        using AccelStructMap_t = FlatHashMap< VkAccelerationStructureKHR, AccelStructData >;


        struct FramebufferData
        {
            VkFramebuffer           fb      = Default;
            String                  name;
            VkFramebufferCreateInfo info    = {};
            Array<VkImageView>      attachments;
        };
        using FramebufferMap_t = FlatHashMap< VkFramebuffer, FramebufferData >;


        struct RenderPassData
        {
            VkRenderPass                    rp      = Default;
            String                          name;
            VkRenderPassCreateInfo2         info    = {};
            Array<VkAttachmentDescription2> attachments;
            Array<VkSubpassDescription2>    subpasses;
            Array<VkSubpassDependency2>     dependencies;
            Array<VkMemoryBarrier2>         barriers;
            Array<VkAttachmentReference2>   references;
            Array<uint>                     preserve;
        };
        using RenderPassMap_t = FlatHashMap< VkRenderPass, RenderPassData >;


        struct DescSetLayoutData
        {
            VkDescriptorSetLayout               layout      = Default;
            String                              name;
            VkDescriptorSetLayoutCreateInfo     info        = {};
            Array<VkDescriptorSetLayoutBinding> bindings;
        };
        using DescSetLayoutMap_t = FlatHashMap< VkDescriptorSetLayout, DescSetLayoutData >;


        struct DescriptorData
        {
            VkDescriptorType                    descriptorType  = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            Array<VkDescriptorImageInfo>        images;
            Array<VkDescriptorBufferInfo>       buffers;
            Array<VkBufferView>                 texelBuffers;
            Array<VkAccelerationStructureKHR>   accelStructs;
            Array<BitSet<3>>                    processed;
        };

        struct DescriptorSetData
        {
            VkDescriptorSet         ds      = Default;
            String                  name;
            VkDescriptorSetLayout   layout  = Default;
            Array<DescriptorData>   bindings;
        };
        using DescriptorSetMap_t = FlatHashMap< VkDescriptorSet, DescriptorSetData >;

        struct PipelineData
        {
            enum class EType
            {
                Unknown,
                Graphics,
                Compute,
                Mesh,
                RayTracing,
            };

            VkPipeline  pipeline    = Default;
            EType       type        = EType::Unknown;
            String      name;
        };
        using PipelineMap_t = FlatHashMap< VkPipeline, PipelineData >;


        using DescrSetArray_t = Array< VkDescriptorSet >;

        struct CommandBufferData
        {
            enum class EState
            {
                Initial,
                Recording,
                Pending,
            };
            using BindPoints_t = StaticArray< DescrSetArray_t, 3 >;

            VkCommandBuffer     cmdBuffer           = Default;
            EState              state               = EState::Initial;
            VkRenderPass        currentRP           = Default;
            VkFramebuffer       currentFB           = Default;
            uint                subpassIndex        = UMax;
            String              log;
            String              name;
            BindPoints_t        bindPoints;
            uint                queueFamilyIndex    = UMax;
            int                 dbgLabelDepth       = 0;

            void  Clear ();
        };
        using CommandBufferMap_t = FlatHashMap< VkCommandBuffer, CommandBufferData >;


        struct CommandPoolData
        {
            VkCommandPool           cmdPool     = Default;
            VkCommandPoolCreateInfo info        = {};
        };
        using CommandPoolMap_t = FlatHashMap< VkCommandPool, CommandPoolData >;


        using SemaphoreNameMap_t    = FlatHashMap< VkSemaphore, String >;
        using FenceNameMap_t        = FlatHashMap< VkFence,     String >;
        using QueueNameMap_t        = FlatHashMap< VkQueue,     String >;


        struct ResourceStatistic
        {
            usize   bufferCount         = 0;
            usize   imageCount          = 0;
            usize   bufferViewCount     = 0;
            usize   imageViewCount      = 0;
            usize   pipelineCount       = 0;
            usize   framebufferCount    = 0;
            usize   renderPassCount     = 0;
            usize   commandBufferCount  = 0;
            usize   descSetLayoutCount  = 0;
            usize   descSetCount        = 0;
            usize   semaphoreCount      = 0;
            usize   fenceCount          = 0;
        };

        using SyncNameMap_t = FlatHashMap< Pair<VkSemaphore, ulong>, String >;


    // variables
    public:
        RecursiveMutex          guard;
        bool                    enableLog       = false;
        String                  log;

        QueueNameMap_t          queueMap;
        SemaphoreNameMap_t      semaphoreMap;
        FenceNameMap_t          fenceMap;
        CommandPoolMap_t        commandPool;
        CommandBufferMap_t      commandBuffers;
        ImageMap_t              imageMap;
        ImageViewMap_t          imageViewMap;
        BufferMap_t             bufferMap;
        BufferViewMap_t         bufferViewMap;
        DevAddressToBuffer_t    devAddrToBuffer;
        AccelStructMap_t        accelStructMap;
        FramebufferMap_t        framebufferMap;
        RenderPassMap_t         renderPassMap;
        PipelineMap_t           pipelineMap;
        DescSetLayoutMap_t      descSetLayoutMap;
        DescriptorSetMap_t      descSetMap;
        ResourceStatistic       resourceStat;
    private:
        DeviceFnTable           _originDeviceFnTable;

        SyncNameMap_t           _syncNameMap;
        ulong                   _syncNameCount  = 0;


    // methods
    public:
        void  Initialize (INOUT VulkanDeviceFnTable& fnTable, FlatHashMap<VkQueue, String> queueNames);
        void  Deinitialize (OUT VulkanDeviceFnTable& fnTable);

        void  _PrintResourceUsage (CommandBufferData &cmdbuf, VkPipelineBindPoint pipelineBindPoint);

            void    ResetSyncNames ();
        ND_ String  GetSyncName (VkSemaphore sem, ulong val);

        ND_ String  GetBufferName (VkDeviceOrHostAddressConstKHR addr)  { return GetBufferName( addr.deviceAddress ); }
        ND_ String  GetBufferName (VkDeviceOrHostAddressKHR addr)       { return GetBufferName( addr.deviceAddress ); }
        ND_ String  GetBufferName (VkDeviceAddress addr);

        ND_ static VulkanLogger&  Get ()
        {
            static std::aligned_storage_t< sizeof(VulkanLogger), alignof(VulkanLogger) >    logger;
            return *Cast<VulkanLogger>( &logger );
        }
    };


namespace
{
/*
=================================================
    VkPipelineStageToString
=================================================
*/
    ND_ static String  VkPipelineStageToString (VkPipelineStageFlags stages)
    {
        if ( AllBits( VkPipelineStageFlagBits(stages), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT ))
            return VkPipelineStageFlagBitsToString( VK_PIPELINE_STAGE_ALL_COMMANDS_BIT );

        return VkPipelineStageFlagsToString( stages );
    }

/*
=================================================
    VkPipelineStage2ToString
=================================================
*/
    ND_ static String  VkPipelineStage2ToString (VkPipelineStageFlags2 stages)
    {
        if ( AllBits( stages, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT ))
            return VkPipelineStageFlagBits2ToString( VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT );

        return VkPipelineStageFlags2ToString( stages );
    }

/*
=================================================
    Wrap_vkCreateBuffer
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateBuffer (VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateBuffer( device, pCreateInfo, pAllocator, OUT pBuffer );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   buf = logger.bufferMap.insert_or_assign( *pBuffer, VulkanLogger::BufferData{} ).first->second;

        buf.buffer  = *pBuffer;
        buf.info    = *pCreateInfo;
        buf.name    = "buffer-" + ToString( logger.resourceStat.bufferCount );

        ++logger.resourceStat.bufferCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyBuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyBuffer (VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );

            auto    it = logger.bufferMap.find( buffer );
            if ( it != logger.bufferMap.end() )
            {
                usize   idx = LowerBound2( logger.devAddrToBuffer, VulkanLogger::DeviceAddressKey{it->second.address} );
                if ( idx != UMax )
                    logger.devAddrToBuffer.erase( logger.devAddrToBuffer.begin() + idx );

                logger.bufferMap.erase( it );
            }
        }
        logger.vkDestroyBuffer( device, buffer, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateBufferView
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateBufferView (VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateBufferView( device, pCreateInfo, pAllocator, OUT pView );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   view    = logger.bufferViewMap.insert_or_assign( *pView, VulkanLogger::BufferViewData{} ).first->second;
        auto    buf_it  = logger.bufferMap.find( pCreateInfo->buffer );
        CHECK_ERR( buf_it != logger.bufferMap.end(), VK_ERROR_UNKNOWN );

        view.view   = *pView;
        view.info   = *pCreateInfo;
        view.name   = "buffer-view-" + ToString( logger.resourceStat.bufferViewCount ) + " (" + buf_it->second.name + ")";

        ++logger.resourceStat.bufferViewCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyBufferView
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyBufferView (VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.bufferViewMap.erase( bufferView );
        }
        logger.vkDestroyBufferView( device, bufferView, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateImage
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateImage (VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateImage( device, pCreateInfo, pAllocator, OUT pImage );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   img = logger.imageMap.insert_or_assign( *pImage, VulkanLogger::ImageData{} ).first->second;

        img.image   = *pImage;
        img.info    = *pCreateInfo;
        img.name    = "image-" + ToString( logger.resourceStat.imageCount );

        ++logger.resourceStat.imageCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyImage (VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.imageMap.erase( image );
        }
        logger.vkDestroyImage( device, image, pAllocator );
    }

/*
=================================================
    Wrap_vkGetSwapchainImagesKHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkGetSwapchainImagesKHR (VkDevice device, VkSwapchainKHR swapchain, uint* pSwapchainImageCount, VkImage* pSwapchainImages)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkGetSwapchainImagesKHR( device, swapchain, OUT pSwapchainImageCount, OUT pSwapchainImages );
        if_unlikely( res != VK_SUCCESS )
            return res;

        if ( pSwapchainImages != null )
        {
            for (uint i = 0; i < *pSwapchainImageCount; ++i)
            {
                auto&   img = logger.imageMap.insert_or_assign( pSwapchainImages[i], VulkanLogger::ImageData{} ).first->second;

                img.image   = pSwapchainImages[i];
                img.info    = {};
                img.name    = "image-" + ToString( logger.resourceStat.imageCount );

                ++logger.resourceStat.imageCount;
            }
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkCreateImageView
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateImageView (VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateImageView( device, pCreateInfo, pAllocator, OUT pView );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   view     = logger.imageViewMap.insert_or_assign( *pView, VulkanLogger::ImageViewData{} ).first->second;
        auto    image_it = logger.imageMap.find( pCreateInfo->image );
        CHECK_ERR( image_it != logger.imageMap.end(), VK_ERROR_UNKNOWN );

        view.view   = *pView;
        view.info   = *pCreateInfo;
        view.name   = "image-view-" + ToString( logger.resourceStat.imageViewCount ) + " (" + image_it->second.name + ")";

        ++logger.resourceStat.imageViewCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyImageView
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyImageView (VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.imageViewMap.erase( imageView );
        }
        logger.vkDestroyImageView( device, imageView, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateFramebuffer
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateFramebuffer (VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateFramebuffer( device, pCreateInfo, pAllocator, OUT pFramebuffer );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   fb = logger.framebufferMap.insert_or_assign( *pFramebuffer, VulkanLogger::FramebufferData{} ).first->second;

        fb.fb   = *pFramebuffer;
        fb.name = "framebuffer-" + ToString( logger.resourceStat.framebufferCount );
        fb.info = *pCreateInfo;

        fb.attachments.assign( pCreateInfo->pAttachments, pCreateInfo->pAttachments + pCreateInfo->attachmentCount );
        fb.info.pAttachments = fb.attachments.data();

        ++logger.resourceStat.framebufferCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyFramebuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyFramebuffer (VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.framebufferMap.erase( framebuffer );
        }
        logger.vkDestroyFramebuffer( device, framebuffer, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateRenderPass
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass (VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateRenderPass( device, pCreateInfo, pAllocator, OUT pRenderPass );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   rp = logger.renderPassMap.insert_or_assign( *pRenderPass, VulkanLogger::RenderPassData{} ).first->second;

        rp.rp   = *pRenderPass;
        rp.name = "render-pass-" + ToString( logger.resourceStat.renderPassCount );
        ++logger.resourceStat.renderPassCount;

        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkCreateRenderPass2
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateRenderPass2 (VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateRenderPass2KHR( device, pCreateInfo, pAllocator, OUT pRenderPass );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   rp  = logger.renderPassMap.insert_or_assign( *pRenderPass, VulkanLogger::RenderPassData{} ).first->second;

        rp.rp   = *pRenderPass;
        rp.name = "render-pass-" + ToString( logger.resourceStat.renderPassCount );
        ++logger.resourceStat.renderPassCount;

        rp.attachments.assign( pCreateInfo->pAttachments, pCreateInfo->pAttachments + pCreateInfo->attachmentCount );
        rp.dependencies.assign( pCreateInfo->pDependencies, pCreateInfo->pDependencies + pCreateInfo->dependencyCount );
        rp.barriers.resize( pCreateInfo->dependencyCount );
        rp.subpasses.assign( pCreateInfo->pSubpasses, pCreateInfo->pSubpasses + pCreateInfo->subpassCount );

        rp.info                         = *pCreateInfo;
        rp.info.pNext                   = null;
        rp.info.pAttachments            = rp.attachments.data();
        rp.info.pDependencies           = rp.dependencies.data();
        rp.info.pSubpasses              = rp.subpasses.data();
        rp.info.correlatedViewMaskCount = 0;
        rp.info.pCorrelatedViewMasks    = null;

        usize   ref_count       = 0;
        usize   preserve_count  = 0;
        for (auto& sp : rp.subpasses)
        {
            ref_count += sp.colorAttachmentCount + (sp.pResolveAttachments ? sp.colorAttachmentCount : 0) + sp.inputAttachmentCount + (sp.pDepthStencilAttachment != null);
            preserve_count += sp.preserveAttachmentCount;
        }

        rp.references.resize( ref_count );
        ref_count = 0;

        rp.preserve.resize( preserve_count );
        preserve_count = 0;

        for (auto& sp : rp.subpasses)
        {
            sp.pNext = null;

            std::memcpy( rp.references.data() + ref_count, sp.pInputAttachments, sizeof(*sp.pInputAttachments) * sp.inputAttachmentCount );
            sp.pInputAttachments     = rp.references.data() + ref_count;
            ref_count               += sp.inputAttachmentCount;

            std::memcpy( rp.references.data() + ref_count, sp.pColorAttachments, sizeof(*sp.pColorAttachments) * sp.colorAttachmentCount );
            sp.pColorAttachments     = rp.references.data() + ref_count;
            ref_count               += sp.colorAttachmentCount;

            if ( sp.pResolveAttachments != null )
            {
                std::memcpy( rp.references.data() + ref_count, sp.pResolveAttachments, sizeof(*sp.pResolveAttachments) * sp.colorAttachmentCount );
                sp.pResolveAttachments   = rp.references.data() + ref_count;
                ref_count               += sp.colorAttachmentCount;
            }

            if ( sp.pDepthStencilAttachment != null )
            {
                rp.references[ref_count]    = *sp.pDepthStencilAttachment;
                sp.pDepthStencilAttachment  = &rp.references[ref_count];
                ++ref_count;
            }

            std::memcpy( rp.preserve.data() + preserve_count, sp.pPreserveAttachments, sizeof(*sp.pPreserveAttachments) * sp.preserveAttachmentCount );
            sp.pPreserveAttachments  = rp.preserve.data() + preserve_count;
            preserve_count          += sp.preserveAttachmentCount;
        }

        for (auto& ref : rp.references) {
            ref.pNext = null;
        }
        for (auto& att : rp.attachments) {
            att.pNext = null;
        }
        for (usize i = 0; i < rp.dependencies.size(); ++i)
        {
            auto&   dep = rp.dependencies[i];
            auto*   bar = Cast<VkMemoryBarrier2>( dep.pNext );
            CHECK( bar != null );
            CHECK( bar->pNext == null and bar->sType == VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 );

            rp.barriers[i]          = *bar;
            rp.barriers[i].pNext    = null;
            dep.pNext               = &rp.barriers[i];
        }

        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyRenderPass
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyRenderPass (VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.renderPassMap.erase( renderPass );
        }
        logger.vkDestroyRenderPass( device, renderPass, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateGraphicsPipelines
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateGraphicsPipelines (VkDevice device, VkPipelineCache pipelineCache, uint createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos,
                                                                    const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateGraphicsPipelines( device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
        if_unlikely( res != VK_SUCCESS )
            return res;

        for (uint i = 0; i < createInfoCount; ++i)
        {
            auto& ppln      = logger.pipelineMap.insert_or_assign( pPipelines[i], VulkanLogger::PipelineData{} ).first->second;
            ppln.pipeline   = pPipelines[i];
            ppln.type       = VulkanLogger::PipelineData::EType::Graphics;
            ppln.name       = "pipeline-" + ToString( logger.resourceStat.pipelineCount );
            ++logger.resourceStat.pipelineCount;
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkCreateComputePipelines
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateComputePipelines (VkDevice device, VkPipelineCache pipelineCache, uint createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos,
                                                                  const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateComputePipelines( device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, OUT pPipelines );
        if_unlikely( res != VK_SUCCESS )
            return res;

        for (uint i = 0; i < createInfoCount; ++i)
        {
            auto& ppln      = logger.pipelineMap.insert_or_assign( pPipelines[i], VulkanLogger::PipelineData{} ).first->second;
            ppln.pipeline   = pPipelines[i];
            ppln.type       = VulkanLogger::PipelineData::EType::Compute;
            ppln.name       = "pipeline-" + ToString( logger.resourceStat.pipelineCount );
            ++logger.resourceStat.pipelineCount;
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyPipeline
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyPipeline (VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.pipelineMap.erase( pipeline );
        }
        logger.vkDestroyPipeline( device, pipeline, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateCommandPool
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateCommandPool (VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateCommandPool( device, pCreateInfo, pAllocator, OUT pCommandPool );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   cmdpool = logger.commandPool.insert_or_assign( *pCommandPool, VulkanLogger::CommandPoolData{} ).first->second;

        cmdpool.cmdPool = *pCommandPool;
        cmdpool.info    = *pCreateInfo;

        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyCommandPool
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyCommandPool (VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.commandPool.erase( commandPool );
        }
        logger.vkDestroyCommandPool( device, commandPool, pAllocator );
    }

/*
=================================================
    Wrap_vkAllocateCommandBuffers
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkAllocateCommandBuffers (VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult res = logger.vkAllocateCommandBuffers( device, pAllocateInfo, OUT pCommandBuffers );
        if_unlikely( res != VK_SUCCESS )
            return res;

        if ( not logger.enableLog )
            return VK_SUCCESS;

        uint    family_idx  = 0;
        auto    cmdpool_it  = logger.commandPool.find( pAllocateInfo->commandPool );
        if ( cmdpool_it != logger.commandPool.end() )
            family_idx = cmdpool_it->second.info.queueFamilyIndex;

        for (uint i = 0; i < pAllocateInfo->commandBufferCount; ++i)
        {
            auto&       cmdbuf      = logger.commandBuffers.insert_or_assign( pCommandBuffers[i], VulkanLogger::CommandBufferData{} ).first->second;
            cmdbuf.cmdBuffer        = pCommandBuffers[i];
            cmdbuf.state            = VulkanLogger::CommandBufferData::EState::Initial;
            cmdbuf.name             = "command-buffer"; //+ ToString( logger.resourceStat.commandBufferCount );
            cmdbuf.queueFamilyIndex = family_idx;
            ++logger.resourceStat.commandBufferCount;
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkCreateDescriptorSetLayout
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateDescriptorSetLayout (VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo,
                                                                     const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateDescriptorSetLayout( device, pCreateInfo, pAllocator, OUT pSetLayout );
        if_unlikely( res != VK_SUCCESS )
            return res;

        auto&   ds_layout = logger.descSetLayoutMap.insert_or_assign( *pSetLayout, VulkanLogger::DescSetLayoutData{} ).first->second;

        ds_layout.layout    = *pSetLayout;
        ds_layout.name      = "desc-set-layout-" + ToString( logger.resourceStat.descSetLayoutCount );

        ++logger.resourceStat.descSetLayoutCount;
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyDescriptorSetLayout
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyDescriptorSetLayout (VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.descSetLayoutMap.erase( descriptorSetLayout );
        }
        logger.vkDestroyDescriptorSetLayout( device, descriptorSetLayout, pAllocator );
    }

/*
=================================================
    Wrap_vkAllocateDescriptorSets
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkAllocateDescriptorSets (VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets)
    {
        auto&    logger = VulkanLogger::Get();
        VkResult res    = logger.vkAllocateDescriptorSets( device, pAllocateInfo, OUT pDescriptorSets );

        if_unlikely( res != VK_SUCCESS )
            return res;

        EXLOCK( logger.guard );

        for (uint i = 0; i < pAllocateInfo->descriptorSetCount; ++i)
        {
            auto&   desc_set    = logger.descSetMap.insert_or_assign( pDescriptorSets[i], VulkanLogger::DescriptorSetData{} ).first->second;
            desc_set.ds         = pDescriptorSets[i];
            desc_set.layout     = pAllocateInfo->pSetLayouts[i];
            desc_set.name       = "desc-set-" + ToString( logger.resourceStat.descSetCount );
            ++logger.resourceStat.descSetCount;
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkFreeDescriptorSets
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkFreeDescriptorSets (VkDevice device, VkDescriptorPool descriptorPool, uint descriptorSetCount, const VkDescriptorSet* pDescriptorSets)
    {
        auto&    logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            for (uint i = 0; i < descriptorSetCount; ++i) {
                logger.descSetMap.erase( pDescriptorSets[i] );
            }
        }
        return logger.vkFreeDescriptorSets( device, descriptorPool, descriptorSetCount, pDescriptorSets );
    }

/*
=================================================
    Wrap_vkUpdateDescriptorSets
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkUpdateDescriptorSets (VkDevice device, uint descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites,
                                                            uint descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkUpdateDescriptorSets( device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies );

        EXLOCK( logger.guard );

        //if ( not logger.enableLog )
        //  return;

        for (uint j = 0; j < descriptorWriteCount; ++j)
        {
            auto&   write   = pDescriptorWrites[j];
            auto    iter    = logger.descSetMap.find( write.dstSet );

            if ( iter == logger.descSetMap.end() )
                continue;

            iter->second.bindings.resize( Max( write.dstBinding + 1, iter->second.bindings.size() ));

            auto&   dst = iter->second.bindings[ write.dstBinding ];
            dst.descriptorType = write.descriptorType;
            dst.processed.resize( Max( dst.processed.size(), write.dstArrayElement + write.descriptorCount ));

            for (uint i = 0; i < write.descriptorCount; ++i)
                dst.processed[i] = Default;

            BEGIN_ENUM_CHECKS();
            switch ( dst.descriptorType )
            {
                case VK_DESCRIPTOR_TYPE_SAMPLER :
                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :
                case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT :
                {
                    ASSERT( write.pImageInfo != null );
                    dst.images.resize( Max( dst.images.size(), write.dstArrayElement + write.descriptorCount ));
                    for (uint i = 0; i < write.descriptorCount; ++i)
                        dst.images[i] = write.pImageInfo[i];
                    break;
                }
                case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER :
                case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER :
                {
                    ASSERT( write.pTexelBufferView != null );
                    dst.texelBuffers.resize( Max( dst.texelBuffers.size(), write.dstArrayElement + write.descriptorCount ));
                    for (uint i = 0; i < write.descriptorCount; ++i)
                        dst.texelBuffers[i] = write.pTexelBufferView[i];
                    break;
                }
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC :
                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC :
                {
                    ASSERT( write.pBufferInfo != null );
                    dst.buffers.resize( Max( dst.buffers.size(), write.dstArrayElement + write.descriptorCount ));
                    for (uint i = 0; i < write.descriptorCount; ++i)
                        dst.buffers[i] = write.pBufferInfo[i];
                    break;
                }
                case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR :
                {
                    auto*   write_as = Cast<VkWriteDescriptorSetAccelerationStructureKHR>(write.pNext);
                    ASSERT( write_as != null );
                    ASSERT( write_as->sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR );
                    ASSERT( write_as->pAccelerationStructures != null );
                    ASSERT( write_as->accelerationStructureCount == write.descriptorCount );

                    dst.accelStructs.resize( Max( dst.accelStructs.size(), write.dstArrayElement + write_as->accelerationStructureCount ));
                    for (uint i = 0; i < write_as->accelerationStructureCount; ++i)
                        dst.accelStructs[i + write.dstArrayElement] = write_as->pAccelerationStructures[i];
                    break;
                }
                case VK_DESCRIPTOR_TYPE_MAX_ENUM :
                    break;  // may be immutable sampler

                case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT :
                case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV :
                case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE :
                case VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM :
                case VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM :
                default :
                    DBG_WARNING( "unsupported descriptor type" );
                    break;
            }
            END_ENUM_CHECKS();
        }
    }

/*
=================================================
    Wrap_vkCreateFence
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateFence (VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateFence( device, pCreateInfo, pAllocator, OUT pFence );
        if_unlikely( res != VK_SUCCESS )
            return res;

        logger.fenceMap.emplace( *pFence, "fence-" + ToString( logger.resourceStat.fenceCount ));
        ++logger.resourceStat.fenceCount;

        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyFence
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyFence (VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.fenceMap.erase( fence );
        }
        return logger.vkDestroyFence( device, fence, pAllocator );
    }

/*
=================================================
    Wrap_vkCreateSemaphore
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateSemaphore (VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        VkResult    res = logger.vkCreateSemaphore( device, pCreateInfo, pAllocator, OUT pSemaphore );
        if_unlikely( res != VK_SUCCESS )
            return res;

        logger.semaphoreMap.emplace( *pSemaphore, "semaphore-" + ToString( logger.resourceStat.semaphoreCount ));
        ++logger.resourceStat.semaphoreCount;

        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroySemaphore
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroySemaphore (VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator)
    {
        auto&   logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.semaphoreMap.erase( semaphore );
        }
        return logger.vkDestroySemaphore( device, semaphore, pAllocator );
    }

/*
=================================================
    Wrap_vkBeginCommandBuffer
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkBeginCommandBuffer (VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo)
    {
        auto&    logger = VulkanLogger::Get();
        VkResult res    = logger.vkBeginCommandBuffer( commandBuffer, pBeginInfo );

        if_unlikely( res != VK_SUCCESS )
            return res;

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter != logger.commandBuffers.end() )
        {
            auto&   cmdbuf = iter->second;
            ASSERT( cmdbuf.cmdBuffer == commandBuffer );

            cmdbuf.Clear();
            cmdbuf.state = VulkanLogger::CommandBufferData::EState::Recording;
        }
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkEndCommandBuffer
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkEndCommandBuffer (VkCommandBuffer commandBuffer)
    {
        auto&    logger = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );

            auto    iter = logger.commandBuffers.find( commandBuffer );
            if ( iter != logger.commandBuffers.end() )
            {
                auto&   cmdbuf = iter->second;
                ASSERT( cmdbuf.cmdBuffer == commandBuffer );
                ASSERT( cmdbuf.dbgLabelDepth == 0 );

                cmdbuf.state = VulkanLogger::CommandBufferData::EState::Pending;
            }
        }
        return logger.vkEndCommandBuffer( commandBuffer );
    }

/*
=================================================
    Wrap_vkQueueSubmit
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkQueueSubmit (VkQueue queue, uint submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        if ( logger.enableLog )
        {
            String  log;
            log << "==================================================\n"
                << "Batch in queue: '" << logger.queueMap[ queue ] << '\'';

            const auto  PrintQueueFamilyIdx = [&] ()
            {{
                for (uint i = 0; i < submitCount; ++i)
                {
                    auto&   batch = pSubmits[i];
                    for (uint j = 0; j < batch.commandBufferCount; ++j)
                    {
                        auto    cmd_it = logger.commandBuffers.find( batch.pCommandBuffers[j] );
                        if ( cmd_it == logger.commandBuffers.end() )
                            continue;

                        auto&   cmdbuf = cmd_it->second;
                        log << ", (" << ToString( cmdbuf.queueFamilyIndex ) << ')';
                        return;
                    }
                }
            }};
            PrintQueueFamilyIdx();
            log << '\n';

            if ( fence != Default )
            {
                auto    fence_it = logger.fenceMap.find( fence );
                if ( fence_it != logger.fenceMap.end() )
                    log << "Fence: '" << fence_it->second << "'\n";
            }

            for (uint i = 0; i < submitCount; ++i)
            {
                auto&   batch = pSubmits[i];

                const VkTimelineSemaphoreSubmitInfo*    timeline = null;
                for (auto* next = Cast<VkBaseInStructure>(batch.pNext); next != null;)
                {
                    if ( next->sType == VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO )
                    {
                        timeline = Cast<VkTimelineSemaphoreSubmitInfo>(next);
                        break;
                    }
                }

                if ( batch.waitSemaphoreCount > 0 )
                {
                    log << "--------------------------------------------------\n";
                    log << "waitSemaphore = {";
                    for (uint j = 0; j < batch.waitSemaphoreCount; ++j)
                    {
                        #if 1
                            ulong   val = 0;
                            if ( timeline != null ) {
                                ASSERT( j < timeline->waitSemaphoreValueCount );
                                val = timeline->pWaitSemaphoreValues[j];
                            }
                            log << "\n  '"
                                << logger.GetSyncName( batch.pWaitSemaphores[j], val )
                                << "',  stage: " << VkPipelineStageToString( batch.pWaitDstStageMask[j] );
                        #else
                            log << "\n  '";
                            auto    sem_it = logger.semaphoreMap.find( batch.pWaitSemaphores[j] );
                            log << (sem_it != logger.semaphoreMap.end() ? sem_it->second : "<unknown>");
                            log << ", [" << ToString<16>( ulong(batch.pWaitSemaphores[j]) ) << "]";
                            log << "',  stage: " << VkPipelineStageToString( batch.pWaitDstStageMask[j] );

                            if ( timeline != null ) {
                                ASSERT( j < timeline->waitSemaphoreValueCount );
                                log << ",  value: " << ToString( timeline->pWaitSemaphoreValues[j] );
                            }
                        #endif
                    }
                    log << "\n}\n";
                }

                for (uint j = 0; j < batch.commandBufferCount; ++j)
                {
                    auto    cmd_it = logger.commandBuffers.find( batch.pCommandBuffers[j] );
                    if ( cmd_it == logger.commandBuffers.end() )
                        continue;

                    auto&   cmdbuf = cmd_it->second;
                    cmdbuf.state = VulkanLogger::CommandBufferData::EState::Initial;

                    log << "--------------------------------------------------\n";
                    log << "name: '" << cmdbuf.name << "'\n{\n";
                    log << cmdbuf.log;
                    log << "}\n";

                    cmdbuf.log.clear();
                }

                if ( batch.signalSemaphoreCount > 0 )
                {
                    log << "--------------------------------------------------\n";
                    log << "signalSemaphore = {";
                    for (uint j = 0; j < batch.signalSemaphoreCount; ++j)
                    {
                        #if 1
                            ulong   val = 0;
                            if ( timeline != null ) {
                                ASSERT( j < timeline->signalSemaphoreValueCount );
                                val = timeline->pSignalSemaphoreValues[j];
                            }
                            log << "\n  '"
                                << logger.GetSyncName( batch.pSignalSemaphores[j], val ) << "'";
                        #else
                            log << "\n  '";
                            auto    sem_it = logger.semaphoreMap.find( batch.pSignalSemaphores[j] );
                            log << (sem_it != logger.semaphoreMap.end() ? sem_it->second : "<unknown>"s) << "'";
                            //log << ", [" << ToString<16>( ulong(batch.pSignalSemaphores[j]) ) << "]";

                            if ( timeline != null ) {
                                ASSERT( j < timeline->signalSemaphoreValueCount );
                                log << ",  value: " << ToString( timeline->pSignalSemaphoreValues[j] );
                            }
                        #endif
                    }
                    log << "\n}\n";
                }
            }

            logger.log << log
                << "==================================================\n";
        }

        auto    err = logger.vkQueueSubmit( queue, submitCount, pSubmits, fence );
        ASSERT( err == VK_SUCCESS );
        return err;
    }

/*
=================================================
    Wrap_vkQueueSubmit2KHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkQueueSubmit2KHR (VkQueue queue, uint submitCount, const VkSubmitInfo2KHR* pSubmits, VkFence fence)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        if ( logger.enableLog )
        {
            String  log;
            log << "==================================================\n"
                << "Batch in queue: '" << logger.queueMap[ queue ] << '\'';

            const auto  PrintQueueFamilyIdx = [&] ()
            {{
                for (uint i = 0; i < submitCount; ++i)
                {
                    auto&   batch = pSubmits[i];
                    for (uint j = 0; j < batch.commandBufferInfoCount; ++j)
                    {
                        auto&   cmd_info = batch.pCommandBufferInfos[j];
                        ASSERT( cmd_info.pNext == null );

                        auto    cmd_it = logger.commandBuffers.find( cmd_info.commandBuffer );
                        if ( cmd_it == logger.commandBuffers.end() )
                            continue;

                        auto&   cmdbuf = cmd_it->second;
                        log << ", (" << ToString( cmdbuf.queueFamilyIndex ) << ')';
                        return;
                    }
                }
            }};
            PrintQueueFamilyIdx();
            log << '\n';

            if ( fence != Default )
            {
                auto    fence_it = logger.fenceMap.find( fence );
                if ( fence_it != logger.fenceMap.end() )
                    log << "Fence: '" << fence_it->second << "'\n";
            }

            for (uint i = 0; i < submitCount; ++i)
            {
                auto&   batch = pSubmits[i];

                if ( batch.waitSemaphoreInfoCount > 0 )
                {
                    log << "--------------------------------------------------\n";
                    log << "waitSemaphore = {";
                    for (uint j = 0; j < batch.waitSemaphoreInfoCount; ++j)
                    {
                        auto&   sem = batch.pWaitSemaphoreInfos[j];
                        #if 1
                            log << "\n  '"
                                << logger.GetSyncName( sem.semaphore, sem.value )
                                << "',  stage: " << VkPipelineStage2ToString( sem.stageMask );
                        #else
                            log << "\n  '";
                            auto    sem_it = logger.semaphoreMap.find( sem.semaphore );
                            log << (sem_it != logger.semaphoreMap.end() ? sem_it->second : "<unknown>"s);
                            //log << ", [" << ToString<16>( ulong(sem.semaphore) ) << "]";
                            log << "',  stage: " << VkPipelineStage2ToString( sem.stageMask )
                                << ",  value: " << ToString( sem.value );
                        #endif
                    }
                    log << "\n}\n";
                }

                for (uint j = 0; j < batch.commandBufferInfoCount; ++j)
                {
                    auto&   cmd_info = batch.pCommandBufferInfos[j];
                    ASSERT( cmd_info.pNext == null );

                    auto    cmd_it = logger.commandBuffers.find( cmd_info.commandBuffer );
                    if ( cmd_it == logger.commandBuffers.end() )
                        continue;

                    auto&   cmdbuf = cmd_it->second;
                    cmdbuf.state = VulkanLogger::CommandBufferData::EState::Initial;

                    log << "--------------------------------------------------\n";
                    log << "name: '" << cmdbuf.name << "'\n{\n";
                    log << cmdbuf.log;
                    log << "}\n";

                    cmdbuf.log.clear();
                }

                if ( batch.signalSemaphoreInfoCount > 0 )
                {
                    log << "--------------------------------------------------\n";
                    log << "signalSemaphore = {";
                    for (uint j = 0; j < batch.signalSemaphoreInfoCount; ++j)
                    {
                        auto&   sem = batch.pSignalSemaphoreInfos[j];
                        #if 1
                            log << "\n  '"
                                << logger.GetSyncName( sem.semaphore, sem.value )
                                << "',  stage: " << VkPipelineStage2ToString( sem.stageMask );
                        #else
                            log << "\n  '";
                            auto    sem_it = logger.semaphoreMap.find( sem.semaphore );
                            log << (sem_it != logger.semaphoreMap.end() ? sem_it->second : "<unknown>"s);
                            //log << ", [" << ToString<16>( ulong(sem.semaphore) ) << "]";
                            log << "',  stage: " << VkPipelineStage2ToString( sem.stageMask )
                                << ",  value: " << ToString( sem.value );
                        #endif
                    }
                    log << "\n}\n";
                }
            }

            logger.log << log
                << "==================================================\n";
        }

        auto    err = logger.vkQueueSubmit2KHR( queue, submitCount, pSubmits, fence );
        ASSERT( err == VK_SUCCESS );
        return err;
    }

/*
=================================================
    Wrap_vkAcquireNextImageKHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkAcquireNextImageKHR (VkDevice device, VkSwapchainKHR swapchain, ulong timeout, VkSemaphore semaphore, VkFence fence, OUT uint* pImageIndex)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        if ( logger.enableLog )
        {
            String  log;
            log << "==================================================\n"
                << "Acquire swapchain image\n";

            if ( fence != Default )
            {
                auto    fence_it = logger.fenceMap.find( fence );
                if ( fence_it != logger.fenceMap.end() )
                    log << "  fence: '" << fence_it->second << "'\n";
            }

            if ( semaphore != Default )
                log << "  signalSemaphore:  " << logger.GetSyncName( semaphore, 0 ) << '\n';

            logger.log << log
                << "==================================================\n";
        }

        return logger.vkAcquireNextImageKHR( device, swapchain, timeout, semaphore, fence, OUT pImageIndex );
    }

/*
=================================================
    Wrap_vkQueuePresentKHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkQueuePresentKHR (VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        if ( logger.enableLog )
        {
            String  log;
            log << "==================================================\n"
                << "Present in queue: '" << logger.queueMap[ queue ] << "'\n";

            if ( pPresentInfo->waitSemaphoreCount > 0 )
            {
                log << "  waitSemaphore = {";
                for (uint j = 0; j < pPresentInfo->waitSemaphoreCount; ++j)
                {
                    log << "\n    '"
                        << logger.GetSyncName( pPresentInfo->pWaitSemaphores[j], 0 )
                        << "'";
                }
                log << "\n  }\n";
            }

            logger.log << log
                << "==================================================\n";
        }

        return logger.vkQueuePresentKHR( queue, pPresentInfo );
    }

/*
=================================================
    Wrap_vkSetDebugUtilsObjectNameEXT
=================================================
*/
    template <typename To, typename From>
    ND_ To  VBitCast (const From &src)
    {
    #   if AE_PLATFORM_BITS == 64
            return BitCast<To>( src );
    #   else
            return UnsafeBitCast<To>( src );
    #   endif
    }

    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkSetDebugUtilsObjectNameEXT (VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
    {
        auto&   logger = VulkanLogger::Get();
        EXLOCK( logger.guard );

        BEGIN_ENUM_CHECKS();
        switch ( pNameInfo->objectType )
        {
            case VK_OBJECT_TYPE_COMMAND_BUFFER :
            {
                auto    iter = logger.commandBuffers.find( VBitCast<VkCommandBuffer>(pNameInfo->objectHandle) );
                if ( iter != logger.commandBuffers.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_BUFFER :
            {
                auto    iter = logger.bufferMap.find( VBitCast<VkBuffer>(pNameInfo->objectHandle) );
                if ( iter != logger.bufferMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_BUFFER_VIEW :
            {
                auto    iter = logger.bufferViewMap.find( VBitCast<VkBufferView>(pNameInfo->objectHandle) );
                if ( iter != logger.bufferViewMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_IMAGE :
            {
                auto    iter = logger.imageMap.find( VBitCast<VkImage>(pNameInfo->objectHandle) );
                if ( iter != logger.imageMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_IMAGE_VIEW :
            {
                auto    iter = logger.imageViewMap.find( VBitCast<VkImageView>(pNameInfo->objectHandle) );
                if ( iter != logger.imageViewMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_RENDER_PASS :
            {
                auto    iter = logger.renderPassMap.find( VBitCast<VkRenderPass>(pNameInfo->objectHandle) );
                if ( iter != logger.renderPassMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_FRAMEBUFFER :
            {
                auto    iter = logger.framebufferMap.find( VBitCast<VkFramebuffer>(pNameInfo->objectHandle) );
                if ( iter != logger.framebufferMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_PIPELINE :
            {
                auto    iter = logger.pipelineMap.find( VBitCast<VkPipeline>(pNameInfo->objectHandle) );
                if ( iter != logger.pipelineMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT :
            {
                auto    iter = logger.descSetLayoutMap.find( VBitCast<VkDescriptorSetLayout>(pNameInfo->objectHandle) );
                if ( iter != logger.descSetLayoutMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR :
            {
                auto    iter = logger.accelStructMap.find( VBitCast<VkAccelerationStructureKHR>(pNameInfo->objectHandle) );
                if ( iter != logger.accelStructMap.end() )
                    iter->second.name = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_SEMAPHORE :
            {
                logger.semaphoreMap[ VBitCast<VkSemaphore>(pNameInfo->objectHandle) ] = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_FENCE :
            {
                logger.fenceMap[ VBitCast<VkFence>(pNameInfo->objectHandle) ] = pNameInfo->pObjectName;
                break;
            }

            case VK_OBJECT_TYPE_UNKNOWN :
            case VK_OBJECT_TYPE_DEVICE :
            case VK_OBJECT_TYPE_QUEUE :
            case VK_OBJECT_TYPE_INSTANCE :
            case VK_OBJECT_TYPE_PHYSICAL_DEVICE :
            case VK_OBJECT_TYPE_DEVICE_MEMORY :
            case VK_OBJECT_TYPE_EVENT :
            case VK_OBJECT_TYPE_QUERY_POOL :
            case VK_OBJECT_TYPE_SAMPLER :
            case VK_OBJECT_TYPE_DESCRIPTOR_POOL :
            case VK_OBJECT_TYPE_DESCRIPTOR_SET :
            case VK_OBJECT_TYPE_SHADER_MODULE :
            case VK_OBJECT_TYPE_PIPELINE_CACHE :
            case VK_OBJECT_TYPE_PIPELINE_LAYOUT :
            case VK_OBJECT_TYPE_COMMAND_POOL :
            case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION :
            case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE :
            case VK_OBJECT_TYPE_SURFACE_KHR :
            case VK_OBJECT_TYPE_SWAPCHAIN_KHR :
            case VK_OBJECT_TYPE_DISPLAY_KHR :
            case VK_OBJECT_TYPE_DISPLAY_MODE_KHR :
            case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT :
            case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV :
            case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT :
            case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT :
            case VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL :
            case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV :
            case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT_EXT :
            case VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR :
            case VK_OBJECT_TYPE_VIDEO_SESSION_KHR :
            case VK_OBJECT_TYPE_VIDEO_SESSION_PARAMETERS_KHR :
            case VK_OBJECT_TYPE_CU_MODULE_NVX :
            case VK_OBJECT_TYPE_CU_FUNCTION_NVX :
            case VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA :
            case VK_OBJECT_TYPE_MICROMAP_EXT :
            case VK_OBJECT_TYPE_OPTICAL_FLOW_SESSION_NV :
            case VK_OBJECT_TYPE_MAX_ENUM :
                break;
        }
        END_ENUM_CHECKS();

        return logger.vkSetDebugUtilsObjectNameEXT( device, pNameInfo );
    }

/*
=================================================
    Wrap_vkCmdPipelineBarrier
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL  Wrap_vkCmdPipelineBarrier (VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags,
                                                            uint memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers,
                                                            uint bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers,
                                                            uint imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdPipelineBarrier( commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                                     bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  PipelineBarrier\n";
        log << "    stage:           " << VkPipelineStageToString( srcStageMask ) << " ---> " << VkPipelineStageToString( dstStageMask ) << '\n';
        log << "    dependencyFlags: " << VkDependencyFlagsToString( dependencyFlags ) << "\n";

        for (uint i = 0; i < memoryBarrierCount; ++i)
        {
            auto&   barrier = pMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    MemoryBarrier:\n";
            log << "      access: " << VkAccessFlagsToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlagsToString( barrier.dstAccessMask ) << '\n';
        }

        for (uint i = 0; i < bufferMemoryBarrierCount; ++i)
        {
            auto&   barrier = pBufferMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    BufferBarrier:\n";

            auto    buf = logger.bufferMap.find( barrier.buffer );

            if ( buf != logger.bufferMap.end() )
                log << "      name:   '" << buf->second.name << "'\n";

            log << "      access: " << VkAccessFlagsToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlagsToString( barrier.dstAccessMask ) << '\n';

            if ( not (barrier.offset == 0 and barrier.size == VK_WHOLE_SIZE) )
                log << "      range:  [" << ToString( Bytes{barrier.offset} ) << ", " << (barrier.size == VK_WHOLE_SIZE ? "whole" : ToString( Bytes{barrier.offset + barrier.size} )) << ")\n";

            if ( barrier.srcQueueFamilyIndex != barrier.dstQueueFamilyIndex )
            {
                log << "      queue:   " << ToString( barrier.srcQueueFamilyIndex ) << " ---> " << ToString( barrier.dstQueueFamilyIndex )
                    << (cmdbuf.queueFamilyIndex == barrier.srcQueueFamilyIndex ? "  (release)" :
                        cmdbuf.queueFamilyIndex == barrier.dstQueueFamilyIndex ? "  (acquire)" : "") << '\n';
            }
        }

        for (uint i = 0; i < imageMemoryBarrierCount; ++i)
        {
            auto&   barrier = pImageMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    ImageBarrier:\n";

            auto    img = logger.imageMap.find( barrier.image );
            if ( img != logger.imageMap.end() )
                log << "      name:    '" << img->second.name << "'\n";

            log << "      layout:  " << VkImageLayoutToString( barrier.oldLayout );

            if ( barrier.oldLayout != barrier.newLayout )
                log << " ---> " << VkImageLayoutToString( barrier.newLayout );

            log << '\n';
            log << "      access:  " << VkAccessFlagsToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlagsToString( barrier.dstAccessMask ) << '\n';
            log << "      aspect:  " << VkImageAspectFlagsToString( barrier.subresourceRange.aspectMask ) << '\n';

            if ( not (barrier.subresourceRange.baseMipLevel == 0 and barrier.subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS) )
            {
                log << "      mipmaps: [" << ToString( barrier.subresourceRange.baseMipLevel ) << ", " 
                    << (barrier.subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS ? "whole" : ToString( barrier.subresourceRange.baseMipLevel + barrier.subresourceRange.levelCount )) << ")\n";
            }
            if ( not (barrier.subresourceRange.baseArrayLayer == 0 and barrier.subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS) )
            {
                log << "      layers:  [" << ToString( barrier.subresourceRange.baseArrayLayer ) << ", "
                    << (barrier.subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS ? "whole" : ToString( barrier.subresourceRange.baseArrayLayer + barrier.subresourceRange.layerCount )) << ")\n";
            }

            if ( barrier.srcQueueFamilyIndex != barrier.dstQueueFamilyIndex )
            {
                log << "      queue:   " << ToString( barrier.srcQueueFamilyIndex ) << " ---> " << ToString( barrier.dstQueueFamilyIndex )
                    << (cmdbuf.queueFamilyIndex == barrier.srcQueueFamilyIndex ? "  (release)" :
                        cmdbuf.queueFamilyIndex == barrier.dstQueueFamilyIndex ? "  (acquire)" : "") << '\n';
            }
        }

        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdPipelineBarrier2
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL  Wrap_vkCmdPipelineBarrier2 (VkCommandBuffer commandBuffer, const VkDependencyInfo* pInfo)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdPipelineBarrier2KHR( commandBuffer, pInfo );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  PipelineBarrier2\n";

        if ( pInfo == null )
            return;

        log << "    dependencyFlags: " << VkDependencyFlagsToString( pInfo->dependencyFlags ) << "\n";

        for (uint i = 0; i < pInfo->memoryBarrierCount; ++i)
        {
            auto&   barrier = pInfo->pMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    MemoryBarrier:\n";
            log << "      stage:  " << VkPipelineStage2ToString( barrier.srcStageMask ) << " ---> " << VkPipelineStage2ToString( barrier.dstStageMask ) << '\n';
            log << "      access: " << VkAccessFlags2ToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlags2ToString( barrier.dstAccessMask ) << '\n';
        }

        for (uint i = 0; i < pInfo->bufferMemoryBarrierCount; ++i)
        {
            auto&   barrier = pInfo->pBufferMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    BufferBarrier:\n";

            auto    buf = logger.bufferMap.find( barrier.buffer );
            if ( buf != logger.bufferMap.end() )
                log << "      name:   '" << buf->second.name << "'\n";

            log << "      stage:  " << VkPipelineStage2ToString( barrier.srcStageMask ) << " ---> " << VkPipelineStage2ToString( barrier.dstStageMask ) << '\n';
            log << "      access: " << VkAccessFlags2ToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlags2ToString( barrier.dstAccessMask ) << '\n';

            if ( not (barrier.offset == 0 and barrier.size == VK_WHOLE_SIZE) )
                log << "      range:  [" << ToString( Bytes{barrier.offset} ) << ", " << (barrier.size == VK_WHOLE_SIZE ? "whole" : ToString( Bytes{barrier.offset + barrier.size} )) << ")\n";

            if ( barrier.srcQueueFamilyIndex != barrier.dstQueueFamilyIndex )
            {
                log << "      queue:   " << ToString( barrier.srcQueueFamilyIndex ) << " ---> " << ToString( barrier.dstQueueFamilyIndex )
                    << (cmdbuf.queueFamilyIndex == barrier.srcQueueFamilyIndex ? "  (release)" :
                        cmdbuf.queueFamilyIndex == barrier.dstQueueFamilyIndex ? "  (acquire)" : "") << '\n';
            }
        }

        for (uint i = 0; i < pInfo->imageMemoryBarrierCount; ++i)
        {
            auto&   barrier = pInfo->pImageMemoryBarriers[i];
            ASSERT( barrier.pNext == null );
            log << "    ImageBarrier:\n";

            auto    img = logger.imageMap.find( barrier.image );
            if ( img != logger.imageMap.end() )
                log << "      name:    '" << img->second.name << "'\n";

            log << "      stage:   " << VkPipelineStage2ToString( barrier.srcStageMask ) << " ---> " << VkPipelineStage2ToString( barrier.dstStageMask ) << '\n';
            log << "      access:  " << VkAccessFlags2ToString( barrier.srcAccessMask ) << " ---> " << VkAccessFlags2ToString( barrier.dstAccessMask ) << '\n';

            log << "      layout:  " << VkImageLayoutToString( barrier.oldLayout );

            if ( barrier.oldLayout != barrier.newLayout )
                log << " ---> " << VkImageLayoutToString( barrier.newLayout );
            log << '\n';

            if ( not (barrier.subresourceRange.baseMipLevel == 0 and barrier.subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS) )
            {
                log << "      mipmaps: [" << ToString( barrier.subresourceRange.baseMipLevel ) << ", " 
                    << (barrier.subresourceRange.levelCount == VK_REMAINING_MIP_LEVELS ? "whole" : ToString( barrier.subresourceRange.baseMipLevel + barrier.subresourceRange.levelCount )) << ")\n";
            }
            if ( not (barrier.subresourceRange.baseArrayLayer == 0 and barrier.subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS) )
            {
                log << "      layers:  [" << ToString( barrier.subresourceRange.baseArrayLayer ) << ", "
                    << (barrier.subresourceRange.layerCount == VK_REMAINING_ARRAY_LAYERS ? "whole" : ToString( barrier.subresourceRange.baseArrayLayer + barrier.subresourceRange.layerCount )) << ")\n";
            }

            if ( barrier.srcQueueFamilyIndex != barrier.dstQueueFamilyIndex )
            {
                log << "      queue:   " << ToString( barrier.srcQueueFamilyIndex ) << " ---> " << ToString( barrier.dstQueueFamilyIndex )
                    << (cmdbuf.queueFamilyIndex == barrier.srcQueueFamilyIndex ? "  (release)" :
                        cmdbuf.queueFamilyIndex == barrier.dstQueueFamilyIndex ? "  (acquire)" : "") << '\n';
            }
        }

        log << "  ----------\n\n";
    }

/*
=================================================
    GetPreviousLayout
=================================================
*/
    static VkImageLayout  GetPreviousLayout (const VulkanLogger::FramebufferData& framebuffer, const VulkanLogger::RenderPassData& renderPass,
                                             uint subpassIndex, uint attachmentIndex, VkImageLayout currentLasyout)
    {
        Unused( framebuffer );

        struct StackValue
        {
            uint            subpass;
            VkImageLayout   layout;
        };
        StackValue  stack [32];
        uint        stack_size  = 0;

        for (auto& dep : renderPass.dependencies)
        {
            if ( dep.dstSubpass == subpassIndex )
            {
                stack[stack_size].subpass = dep.srcSubpass;

                if ( dep.srcSubpass == VK_SUBPASS_EXTERNAL )
                {
                    stack[stack_size].layout = renderPass.attachments[ attachmentIndex ].initialLayout;
                    ++stack_size;
                }
                else
                {
                    auto&   sp      = renderPass.subpasses[ dep.srcSubpass ];
                    bool    found   = false;

                    for (uint i = 0; !found && i < sp.colorAttachmentCount; ++i)
                    {
                        if ( sp.pColorAttachments[i].attachment == attachmentIndex )
                        {
                            stack[stack_size].layout = sp.pColorAttachments[i].layout;
                            found = true;
                        }
                    }
                    for (uint i = 0; sp.pResolveAttachments && !found && i < sp.colorAttachmentCount; ++i)
                    {
                        if ( sp.pResolveAttachments[i].attachment == attachmentIndex )
                        {
                            stack[stack_size].layout = sp.pResolveAttachments[i].layout;
                            found = true;
                        }
                    }
                    if ( sp.pDepthStencilAttachment && !found && sp.pDepthStencilAttachment->attachment == attachmentIndex )
                    {
                        stack[stack_size].layout = sp.pDepthStencilAttachment->layout;
                        found = true;
                    }
                    for (uint i = 0; !found && sp.inputAttachmentCount; ++i)
                    {
                        if ( sp.pInputAttachments[i].attachment == attachmentIndex )
                        {
                            stack[stack_size].layout = sp.pInputAttachments[i].layout;
                            found = true;
                        }
                    }

                    if ( found )
                        ++stack_size;
                }
            }
        }

        if ( stack_size == 0 )
            return currentLasyout;

        if ( stack_size == 1 )
            return stack[0].layout;

        DBG_WARNING( "TODO" );
        return currentLasyout;
    }

/*
=================================================
    PrintRPImageViewName
=================================================
*/
    ND_ static bool  PrintRPImageViewName (String &log, VkImageView id)
    {
        auto&   logger  = VulkanLogger::Get();

        auto    view_it = logger.imageViewMap.find( id );
        if ( view_it == logger.imageViewMap.end() )
            return false;

        auto    img_it = logger.imageMap.find( view_it->second.info.image );
        if ( img_it == logger.imageMap.end() )
            return false;

        log << "      view:    '" << view_it->second.name << "'\n";
        log << "      image:   '" << img_it->second.name << "'\n";
        return true;
    }

/*
=================================================
    PrintRenderPass
=================================================
*/
    static void  PrintRenderPass (VulkanLogger::CommandBufferData &cmdbuf, uint subpassIndex)
    {
        auto&   logger  = VulkanLogger::Get();
        auto&   log     = cmdbuf.log;

        auto    rp_it = logger.renderPassMap.find( cmdbuf.currentRP );
        if ( rp_it == logger.renderPassMap.end() )
            return;

        auto    fb_it = logger.framebufferMap.find( cmdbuf.currentFB );
        if ( fb_it == logger.framebufferMap.end() )
            return;

        auto&   rp = rp_it->second;
        auto&   fb = fb_it->second;

        if ( subpassIndex == 0 )
        {
            log << "    renderPass:  '" << rp.name << "'\n";
        //  log << "    framebuffer: '" << fb.name << "'\n";
        }

        auto&   pass = rp.info.pSubpasses [cmdbuf.subpassIndex];

        for (uint i = 0; i < pass.colorAttachmentCount; ++i)
        {
            auto&   ref = pass.pColorAttachments[i];
            auto&   at  = rp.info.pAttachments[ref.attachment];

            log << "    color attachment:\n";

            if ( not PrintRPImageViewName( log, fb.attachments[ref.attachment] ))
                return;

            if ( subpassIndex == 0 )
            {
                log << "      layout:  " << VkImageLayoutToString( at.initialLayout );

                if ( at.initialLayout != ref.layout )
                    log << " ---> " << VkImageLayoutToString( ref.layout );

                log << "\n      loadOp:  " << VkAttachmentLoadOpToString( at.loadOp ) << "\n";
            }
            else
            {
                auto    prev = GetPreviousLayout( fb, rp, subpassIndex, ref.attachment, ref.layout );
                log << "      layout:  " << VkImageLayoutToString( prev );

                if ( prev != ref.layout )
                    log << " ---> " << VkImageLayoutToString( ref.layout );
                log << '\n';
            }
        }

        if ( pass.pDepthStencilAttachment != null )
        {
            auto&   ref = *pass.pDepthStencilAttachment;
            auto&   at  = rp.info.pAttachments[ref.attachment];

            log << "    depth-stencil attachment:\n";

            if ( not PrintRPImageViewName( log, fb.attachments[ref.attachment] ))
                return;

            log << "      layout:        " << VkImageLayoutToString( at.initialLayout );

            if ( subpassIndex == 0 )
            {
                if ( at.initialLayout != ref.layout )
                    log << " ---> " << VkImageLayoutToString( ref.layout );
                log << '\n';
                log << "      depthLoadOp:   " << VkAttachmentLoadOpToString( at.loadOp ) << '\n';
                log << "      stencilLoadOp: " << VkAttachmentLoadOpToString( at.stencilLoadOp ) << '\n';
            }
            else
            {
                auto    prev = GetPreviousLayout( fb, rp, subpassIndex, ref.attachment, ref.layout );
                log << "      layout:        " << VkImageLayoutToString( prev );

                if ( prev != ref.layout )
                    log << " ---> " << VkImageLayoutToString( ref.layout );
                log << '\n';
            }
        }

        if ( subpassIndex != 0 and subpassIndex != VK_SUBPASS_EXTERNAL )
        {
            if ( pass.pResolveAttachments != null )
            {
                for (uint i = 0; i < pass.colorAttachmentCount; ++i)
                {
                    auto&   ref  = pass.pResolveAttachments[i];
                    auto    prev = GetPreviousLayout( fb, rp, subpassIndex, ref.attachment, ref.layout );

                    log << "    resolve attachment:\n";

                    if ( not PrintRPImageViewName( log, fb.attachments[ref.attachment] ))
                        return;

                    log << "      layout:  " << VkImageLayoutToString( prev );

                    if ( prev != ref.layout )
                        log << " ---> " << VkImageLayoutToString( ref.layout );

                    log << '\n';
                }
            }

            for (uint i = 0; i < pass.inputAttachmentCount; ++i)
            {
                auto&   ref  = pass.pInputAttachments[i];
                auto    prev = GetPreviousLayout( fb, rp, subpassIndex, ref.attachment, ref.layout );

                log << "    input attachment:\n";

                if ( not PrintRPImageViewName( log, fb.attachments[ref.attachment] ))
                    return;

                log << "      layout:  " << VkImageLayoutToString( prev );

                if ( prev != ref.layout )
                    log << " ---> " << VkImageLayoutToString( ref.layout );

                log << '\n';
            }
        }

        for (usize i = 0; i < rp.dependencies.size(); ++i)
        {
            const auto& dep = rp.dependencies[i];
            const auto& bar = rp.barriers[i];

            if ( dep.dstSubpass == subpassIndex )
            {
                log << "    dependency:\n";
                log << "      subpass: " << (dep.srcSubpass == VK_SUBPASS_EXTERNAL ? "External" : ToString( dep.srcSubpass) ) << " ---> "
                                         << (dep.dstSubpass == VK_SUBPASS_EXTERNAL ? "External" : ToString( dep.dstSubpass) ) << '\n';
                log << "      stage:   " << VkPipelineStage2ToString( bar.srcStageMask ) << " ---> " << VkPipelineStage2ToString( bar.dstStageMask ) << '\n';
                log << "      access:  " << VkAccessFlags2ToString( bar.srcAccessMask ) << " ---> " << VkAccessFlags2ToString( bar.dstAccessMask ) << '\n';
            }
        }

        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdBeginRenderPass
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBeginRenderPass (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdBeginRenderPass( commandBuffer, pRenderPassBegin, contents );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.currentRP    = pRenderPassBegin->renderPass;
        cmdbuf.currentFB    = pRenderPassBegin->framebuffer;
        cmdbuf.subpassIndex = 0;

        if ( not logger.enableLog )
            return;

        if ( not EndsWith( cmdbuf.log, "\n\n" ))
            cmdbuf.log << '\n';

         cmdbuf.log << "  BeginRenderPass\n";
        PrintRenderPass( cmdbuf, cmdbuf.subpassIndex );
    }

/*
=================================================
    Wrap_vkCmdBeginRenderPass2
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBeginRenderPass2 (VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdBeginRenderPass2KHR( commandBuffer, pRenderPassBegin, pSubpassBeginInfo );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.currentRP    = pRenderPassBegin->renderPass;
        cmdbuf.currentFB    = pRenderPassBegin->framebuffer;
        cmdbuf.subpassIndex = 0;

        if ( not logger.enableLog )
            return;

        if ( not EndsWith( cmdbuf.log, "\n\n" ))
            cmdbuf.log << '\n';

         cmdbuf.log << "  BeginRenderPass2\n";
        PrintRenderPass( cmdbuf, cmdbuf.subpassIndex );

        // TODO: pRenderPassBegin->pNext
    }

/*
=================================================
    Wrap_vkCmdNextSubpass
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdNextSubpass (VkCommandBuffer commandBuffer, VkSubpassContents contents)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdNextSubpass( commandBuffer, contents );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );
        cmdbuf.subpassIndex++;

        if ( not logger.enableLog )
            return;

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  NextSubpass\n";
        log << "    index: " << ToString( cmdbuf.subpassIndex ) << '\n';

        PrintRenderPass( cmdbuf, cmdbuf.subpassIndex );
    }

/*
=================================================
    Wrap_vkCmdNextSubpass2
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdNextSubpass2 (VkCommandBuffer commandBuffer, const VkSubpassBeginInfo* pSubpassBeginInfo, const VkSubpassEndInfo* pSubpassEndInfo)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdNextSubpass2KHR( commandBuffer, pSubpassBeginInfo, pSubpassEndInfo );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );
        cmdbuf.subpassIndex++;

        if ( not logger.enableLog )
            return;

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  NextSubpass2\n";
        log << "    index: " << ToString( cmdbuf.subpassIndex ) << '\n';

        PrintRenderPass( cmdbuf, cmdbuf.subpassIndex );
    }

/*
=================================================
    IsDepthStencilFormat
=================================================
*/
    ND_ static bool  IsDepthStencilFormat (VkFormat fmt)
    {
        switch ( fmt )
        {
            case VK_FORMAT_D16_UNORM :
            case VK_FORMAT_X8_D24_UNORM_PACK32 :
            case VK_FORMAT_D32_SFLOAT :
            case VK_FORMAT_S8_UINT :
            case VK_FORMAT_D16_UNORM_S8_UINT :
            case VK_FORMAT_D24_UNORM_S8_UINT :
            case VK_FORMAT_D32_SFLOAT_S8_UINT :
                return true;
        }
        return false;
    }

/*
=================================================
    Wrap_vkCmdEndRenderPass
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdEndRenderPass (VkCommandBuffer commandBuffer)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdEndRenderPass( commandBuffer );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        if ( logger.enableLog )
        {
            cmdbuf.log << "  EndRenderPass\n";
        }

        cmdbuf.currentRP    = Default;
        cmdbuf.currentFB    = Default;
        cmdbuf.subpassIndex = VK_SUBPASS_EXTERNAL;
    }

/*
=================================================
    Wrap_vkCmdEndRenderPass2
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdEndRenderPass2 (VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdEndRenderPass2KHR( commandBuffer, pSubpassEndInfo );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        const auto  old_rp = cmdbuf.currentRP;
        const auto  old_fb = cmdbuf.currentFB;

        cmdbuf.currentRP    = Default;
        cmdbuf.currentFB    = Default;
        cmdbuf.subpassIndex = UMax;

        if ( not logger.enableLog )
            return;

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  EndRenderPass2\n";

        auto    rp_it = logger.renderPassMap.find( old_rp );
        if ( rp_it == logger.renderPassMap.end() )
            return;

        auto    fb_it = logger.framebufferMap.find( old_fb );
        if ( fb_it == logger.framebufferMap.end() )
            return;

        auto&   rp = rp_it->second;
        auto&   fb = fb_it->second;

        for (usize i = 0; i < rp.info.attachmentCount; ++i)
        {
            auto&   at   = rp.info.pAttachments[i];
            auto    prev = GetPreviousLayout( fb, rp, VK_SUBPASS_EXTERNAL, uint(i), at.finalLayout );

            log << "    attachment:\n";

            if ( not PrintRPImageViewName( log, fb.attachments[i] ))
                return;

            log << "      layout:  " << VkImageLayoutToString( prev );

            if ( prev != at.finalLayout )
                log << " ---> " << VkImageLayoutToString( at.finalLayout );

            log << "\n";

            if ( IsDepthStencilFormat( at.format ))
            {
                log << "      depthStoreOp:   " << VkAttachmentStoreOpToString( at.storeOp ) << '\n';
                log << "      stencilStoreOp: " << VkAttachmentStoreOpToString( at.stencilStoreOp ) << '\n';
            }
            else
            {
                log << "      storeOp: " << VkAttachmentStoreOpToString( at.storeOp ) << '\n';
            }
        }

        for (usize i = 0; i < rp.dependencies.size(); ++i)
        {
            const auto& dep = rp.dependencies[i];
            const auto& bar = rp.barriers[i];

            if ( dep.dstSubpass == VK_SUBPASS_EXTERNAL )
            {
                log << "    dependency:\n";
                log << "      subpass: " << (dep.srcSubpass == VK_SUBPASS_EXTERNAL ? "External" : ToString( dep.srcSubpass) ) << " ---> "
                                         << (dep.dstSubpass == VK_SUBPASS_EXTERNAL ? "External" : ToString( dep.dstSubpass) ) << '\n';
                log << "      stage:   " << VkPipelineStage2ToString( bar.srcStageMask ) << " ---> " << VkPipelineStage2ToString( bar.dstStageMask ) << '\n';
                log << "      access:  " << VkAccessFlags2ToString( bar.srcAccessMask ) << " ---> " << VkAccessFlags2ToString( bar.dstAccessMask ) << '\n';
            }
        }

        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyBuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyBuffer (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint regionCount, const VkBufferCopy* pRegions)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyBuffer\n";

        auto    src_it = logger.bufferMap.find( srcBuffer );
        if ( src_it == logger.bufferMap.end() )
            return;

        auto    dst_it = logger.bufferMap.find( dstBuffer );
        if ( dst_it == logger.bufferMap.end() )
            return;

        log << "    src: '" << src_it->second.name << "'\n";
        log << "    dst: '" << dst_it->second.name << "'\n";

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&   reg = pRegions[i];
            log << "      copy [" << ToString( reg.srcOffset ) << ", " << ToString( reg.srcOffset + reg.size )
                << ") ---> ["     << ToString( reg.dstOffset ) << ", " << ToString( reg.dstOffset + reg.size ) << ")\n";
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    SubresourceLayerToString
=================================================
*/
    ND_ static String  SubresourceLayerToString (const VkOffset3D &offset, const VkExtent3D &extent, const VkImageSubresourceLayers &subres)
    {
        return  "{ off:("s << ToString( offset.x ) << ", " << ToString( offset.y ) << ", " << ToString( offset.z )
                << "), ext:(" << ToString( offset.x + extent.width ) << ", " << ToString( offset.y + extent.height ) << ", " << ToString( offset.z + extent.depth )
                << "), mip:" << ToString( subres.mipLevel )
                << ", layers:[" << ToString( subres.baseArrayLayer )
                << ", " << (subres.layerCount == VK_REMAINING_ARRAY_LAYERS ? "Remaining"s : ToString( subres.baseArrayLayer + subres.layerCount )) << "), "
                << VkImageAspectFlagsToString( subres.aspectMask ) << " }";
    }

/*
=================================================
    SubresourceRangeToString
=================================================
*/
    ND_ static String  SubresourceRangeToString (const VkImageSubresourceRange &subres)
    {
        return  "{ mips:["s << ToString( subres.baseMipLevel ) << ", "
                << (subres.levelCount == VK_REMAINING_MIP_LEVELS ? "Remaining"s : ToString( subres.baseMipLevel + subres.levelCount ))
                << "), layers:[" << ToString( subres.baseArrayLayer ) << ", "
                << (subres.layerCount == VK_REMAINING_ARRAY_LAYERS ? "Remaining"s : ToString( subres.baseArrayLayer + subres.layerCount )) << "), "
                << VkImageAspectFlagsToString( subres.aspectMask ) << " }";
    }

/*
=================================================
    GetFormatInfo
=================================================
*/
    struct PixelFormatInfo
    {
        ushort      bitsPerBlock    = 0;        // for color and depth
        ubyte       bitsPerBlock2   = 0;        // for stencil
        ubyte2      blockSize       = {1,1};

        PixelFormatInfo () {}

        explicit PixelFormatInfo (uint bpp) :
            bitsPerBlock{ CheckCast<ushort>( bpp )} {}

        PixelFormatInfo (uint bpp, uint bpp2) :
            bitsPerBlock{ CheckCast<ushort>( bpp )}, bitsPerBlock2{ CheckCast<ubyte>( bpp2 )} {}

        PixelFormatInfo (uint bpp, uint2 block) :
            bitsPerBlock{ CheckCast<ushort>( bpp )}, blockSize{ CheckCast<ubyte2>( block )} {}

        ND_ uint2  BlockSize () const   { return uint2{blockSize}; }
    };

    ND_ static PixelFormatInfo  GetFormatInfo (VkFormat fmt)
    {
        switch ( fmt )
        {
            case VK_FORMAT_R8G8B8A8_UNORM :             return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R4G4_UNORM_PACK8 :           return PixelFormatInfo{ 8 };
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16 :      return PixelFormatInfo{ 16 };
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16 :      return PixelFormatInfo{ 16 };
            case VK_FORMAT_R5G6B5_UNORM_PACK16 :        return PixelFormatInfo{ 16 };
            case VK_FORMAT_B5G6R5_UNORM_PACK16 :        return PixelFormatInfo{ 16 };
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16 :      return PixelFormatInfo{ 16 };
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16 :      return PixelFormatInfo{ 16 };
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16 :      return PixelFormatInfo{ 16 };
            case VK_FORMAT_R8_UNORM :                   return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_SNORM :                   return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_USCALED :                 return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_SSCALED :                 return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_UINT :                    return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_SINT :                    return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8_SRGB :                    return PixelFormatInfo{ 8*1 };
            case VK_FORMAT_R8G8_UNORM :                 return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_SNORM :                 return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_USCALED :               return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_SSCALED :               return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_UINT :                  return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_SINT :                  return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8_SRGB :                  return PixelFormatInfo{ 8*2 };
            case VK_FORMAT_R8G8B8_UNORM :               return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_SNORM :               return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_USCALED :             return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_SSCALED :             return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_UINT :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_SINT :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8_SRGB :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_UNORM :               return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_SNORM :               return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_USCALED :             return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_SSCALED :             return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_UINT :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_SINT :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_B8G8R8_SRGB :                return PixelFormatInfo{ 8*3 };
            case VK_FORMAT_R8G8B8A8_SNORM :             return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R8G8B8A8_USCALED :           return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R8G8B8A8_SSCALED :           return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R8G8B8A8_UINT :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R8G8B8A8_SINT :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_R8G8B8A8_SRGB :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_UNORM :             return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_SNORM :             return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_USCALED :           return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_SSCALED :           return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_UINT :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_SINT :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_B8G8R8A8_SRGB :              return PixelFormatInfo{ 8*4 };
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32 :      return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32 :      return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_UINT_PACK32 :       return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_SINT_PACK32 :       return PixelFormatInfo{ 32 };
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32 :       return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32 :   return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32 :   return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32 : return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32 : return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_UINT_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2R10G10B10_SINT_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32 :   return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32 :   return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32 : return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32 : return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_UINT_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_A2B10G10R10_SINT_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_R16_UNORM :                  return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_SNORM :                  return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_USCALED :                return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_SSCALED :                return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_UINT :                   return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_SINT :                   return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16_SFLOAT :                 return PixelFormatInfo{ 16*1 };
            case VK_FORMAT_R16G16_UNORM :               return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_SNORM :               return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_USCALED :             return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_SSCALED :             return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_UINT :                return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_SINT :                return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16_SFLOAT :              return PixelFormatInfo{ 16*2 };
            case VK_FORMAT_R16G16B16_UNORM :            return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_SNORM :            return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_USCALED :          return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_SSCALED :          return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_UINT :             return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_SINT :             return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16_SFLOAT :           return PixelFormatInfo{ 16*3 };
            case VK_FORMAT_R16G16B16A16_UNORM :         return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_SNORM :         return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_USCALED :       return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_SSCALED :       return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_UINT :          return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_SINT :          return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R16G16B16A16_SFLOAT :        return PixelFormatInfo{ 16*4 };
            case VK_FORMAT_R32_UINT :                   return PixelFormatInfo{ 32*1 };
            case VK_FORMAT_R32_SINT :                   return PixelFormatInfo{ 32*1 };
            case VK_FORMAT_R32_SFLOAT :                 return PixelFormatInfo{ 32*1 };
            case VK_FORMAT_R32G32_UINT :                return PixelFormatInfo{ 32*2 };
            case VK_FORMAT_R32G32_SINT :                return PixelFormatInfo{ 32*2 };
            case VK_FORMAT_R32G32_SFLOAT :              return PixelFormatInfo{ 32*2 };
            case VK_FORMAT_R32G32B32_UINT :             return PixelFormatInfo{ 32*3 };
            case VK_FORMAT_R32G32B32_SINT :             return PixelFormatInfo{ 32*3 };
            case VK_FORMAT_R32G32B32_SFLOAT :           return PixelFormatInfo{ 32*3 };
            case VK_FORMAT_R32G32B32A32_UINT :          return PixelFormatInfo{ 32*4 };
            case VK_FORMAT_R32G32B32A32_SINT :          return PixelFormatInfo{ 32*4 };
            case VK_FORMAT_R32G32B32A32_SFLOAT :        return PixelFormatInfo{ 32*4 };
            case VK_FORMAT_R64_UINT :                   return PixelFormatInfo{ 64*1 };
            case VK_FORMAT_R64_SINT :                   return PixelFormatInfo{ 64*1 };
            case VK_FORMAT_R64_SFLOAT :                 return PixelFormatInfo{ 64*1 };
            case VK_FORMAT_R64G64_UINT :                return PixelFormatInfo{ 64*2 };
            case VK_FORMAT_R64G64_SINT :                return PixelFormatInfo{ 64*2 };
            case VK_FORMAT_R64G64_SFLOAT :              return PixelFormatInfo{ 64*2 };
            case VK_FORMAT_R64G64B64_UINT :             return PixelFormatInfo{ 64*3 };
            case VK_FORMAT_R64G64B64_SINT :             return PixelFormatInfo{ 64*3 };
            case VK_FORMAT_R64G64B64_SFLOAT :           return PixelFormatInfo{ 64*3 };
            case VK_FORMAT_R64G64B64A64_UINT :          return PixelFormatInfo{ 64*4 };
            case VK_FORMAT_R64G64B64A64_SINT :          return PixelFormatInfo{ 64*4 };
            case VK_FORMAT_R64G64B64A64_SFLOAT :        return PixelFormatInfo{ 64*4 };
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32 :    return PixelFormatInfo{ 32 };
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 :     return PixelFormatInfo{ 32 };
            case VK_FORMAT_D16_UNORM :                  return PixelFormatInfo{ 16 };
            case VK_FORMAT_X8_D24_UNORM_PACK32 :        return PixelFormatInfo{ 32 };
            case VK_FORMAT_D32_SFLOAT :                 return PixelFormatInfo{ 32 };
            case VK_FORMAT_S8_UINT :                    return PixelFormatInfo{ 8 };
            case VK_FORMAT_D16_UNORM_S8_UINT :          return PixelFormatInfo{ 16, 8 };
            case VK_FORMAT_D24_UNORM_S8_UINT :          return PixelFormatInfo{ 24, 8 };
            case VK_FORMAT_D32_SFLOAT_S8_UINT :         return PixelFormatInfo{ 32, 8 };
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK :
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK :
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK :
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK :        return PixelFormatInfo{ 64, {4,4} };
            case VK_FORMAT_BC2_UNORM_BLOCK :
            case VK_FORMAT_BC2_SRGB_BLOCK :             return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_BC3_UNORM_BLOCK :
            case VK_FORMAT_BC3_SRGB_BLOCK :             return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_BC4_UNORM_BLOCK :
            case VK_FORMAT_BC4_SNORM_BLOCK :            return PixelFormatInfo{ 64, {4,4} };
            case VK_FORMAT_BC5_UNORM_BLOCK :
            case VK_FORMAT_BC5_SNORM_BLOCK :            return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_BC6H_UFLOAT_BLOCK :
            case VK_FORMAT_BC6H_SFLOAT_BLOCK :          return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_BC7_UNORM_BLOCK :
            case VK_FORMAT_BC7_SRGB_BLOCK :             return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK :
            case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK :
            case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK :
            case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK :   return PixelFormatInfo{ 64, {4,4} };
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK :
            case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK :   return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_EAC_R11_UNORM_BLOCK :
            case VK_FORMAT_EAC_R11_SNORM_BLOCK :        return PixelFormatInfo{ 64, {4,4} };
            case VK_FORMAT_EAC_R11G11_UNORM_BLOCK :
            case VK_FORMAT_EAC_R11G11_SNORM_BLOCK :     return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK :
            case VK_FORMAT_ASTC_4x4_SRGB_BLOCK :        return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_ASTC_5x4_UNORM_BLOCK :
            case VK_FORMAT_ASTC_5x4_SRGB_BLOCK :        return PixelFormatInfo{ 128, {5,4} };
            case VK_FORMAT_ASTC_5x5_UNORM_BLOCK :
            case VK_FORMAT_ASTC_5x5_SRGB_BLOCK :        return PixelFormatInfo{ 128, {5,5} };
            case VK_FORMAT_ASTC_6x5_UNORM_BLOCK :
            case VK_FORMAT_ASTC_6x5_SRGB_BLOCK :        return PixelFormatInfo{ 128, {6,5} };
            case VK_FORMAT_ASTC_6x6_UNORM_BLOCK :
            case VK_FORMAT_ASTC_6x6_SRGB_BLOCK :        return PixelFormatInfo{ 128, {6,6} };
            case VK_FORMAT_ASTC_8x5_UNORM_BLOCK :
            case VK_FORMAT_ASTC_8x5_SRGB_BLOCK :        return PixelFormatInfo{ 128, {8,5} };
            case VK_FORMAT_ASTC_8x6_UNORM_BLOCK :
            case VK_FORMAT_ASTC_8x6_SRGB_BLOCK :        return PixelFormatInfo{ 128, {8,6} };
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK :
            case VK_FORMAT_ASTC_8x8_SRGB_BLOCK :        return PixelFormatInfo{ 128, {8,8} };
            case VK_FORMAT_ASTC_10x5_UNORM_BLOCK :
            case VK_FORMAT_ASTC_10x5_SRGB_BLOCK :       return PixelFormatInfo{ 128, {10,5} };
            case VK_FORMAT_ASTC_10x6_UNORM_BLOCK :
            case VK_FORMAT_ASTC_10x6_SRGB_BLOCK :       return PixelFormatInfo{ 128, {10,6} };
            case VK_FORMAT_ASTC_10x8_UNORM_BLOCK :
            case VK_FORMAT_ASTC_10x8_SRGB_BLOCK :       return PixelFormatInfo{ 128, {10,8} };
            case VK_FORMAT_ASTC_10x10_UNORM_BLOCK :
            case VK_FORMAT_ASTC_10x10_SRGB_BLOCK :      return PixelFormatInfo{ 128, {10,10} };
            case VK_FORMAT_ASTC_12x10_UNORM_BLOCK :
            case VK_FORMAT_ASTC_12x10_SRGB_BLOCK :      return PixelFormatInfo{ 128, {12,10} };
            case VK_FORMAT_ASTC_12x12_UNORM_BLOCK :
            case VK_FORMAT_ASTC_12x12_SRGB_BLOCK :      return PixelFormatInfo{ 128, {12,12} };
            case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {4,4} };
            case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {5,4} };
            case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {5,5} };
            case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {6,5} };
            case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {6,6} };
            case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {8,5} };
            case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {8,6} };
            case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT :  return PixelFormatInfo{ 128, {8,8} };
            case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT : return PixelFormatInfo{ 128, {10,5} };
            case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT : return PixelFormatInfo{ 128, {10,6} };
            case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT : return PixelFormatInfo{ 128, {10,8} };
            case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT: return PixelFormatInfo{ 128, {10,10} };
            case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT: return PixelFormatInfo{ 128, {12,10} };
            case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT: return PixelFormatInfo{ 128, {12,12} };
        }
        RETURN_ERR( "unsupported format" );
    }

/*
=================================================
    CalcMemorySize
=================================================
*/
    ND_ static VkDeviceSize  CalcMemorySize (const VkBufferImageCopy &region, const VkImageCreateInfo &imageCI)
    {
        ASSERT( region.bufferRowLength == 0 or region.bufferRowLength >= region.imageExtent.width );
        ASSERT( region.bufferImageHeight == 0 or region.bufferImageHeight >= region.imageExtent.height );

        const auto&     fmt_info    = GetFormatInfo( imageCI.format );
        const Bytes     row_pitch   = ImageUtils::RowSize( Max( region.bufferRowLength, region.imageExtent.width ), fmt_info.bitsPerBlock, fmt_info.BlockSize() );
        const Bytes     slice_pitch = ImageUtils::SliceSize( Max( region.bufferImageHeight, region.imageExtent.height ), row_pitch, fmt_info.BlockSize() );
        const uint      dim_z       = Max( region.imageSubresource.layerCount, region.imageExtent.depth );

        return VkDeviceSize( dim_z * slice_pitch );
    }

/*
=================================================
    Wrap_vkCmdCopyImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyImage (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
                                                    uint regionCount, const VkImageCopy* pRegions)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdCopyImage( commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyImage\n";

        auto    src_it = logger.imageMap.find( srcImage );
        if ( src_it == logger.imageMap.end() )
            return;

        auto    dst_it = logger.imageMap.find( dstImage );
        if ( dst_it == logger.imageMap.end() )
            return;

        log << "    src:       '" << src_it->second.name << "'\n";
        log << "    srcLayout: " << VkImageLayoutToString( srcImageLayout ) << '\n';
        log << "    dst:       '" << dst_it->second.name << "'\n";
        log << "    dstLayout: " << VkImageLayoutToString( dstImageLayout ) << '\n';

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&   reg = pRegions[i];
            log << "      copy " << SubresourceLayerToString( reg.srcOffset, reg.extent, reg.srcSubresource ) << " ---> "
                << SubresourceLayerToString( reg.dstOffset, reg.extent, reg.dstSubresource ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdBlitImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBlitImage (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
                                                    uint regionCount, const VkImageBlit* pRegions, VkFilter filter)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdBlitImage( commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  BlitImage\n";

        auto    src_it = logger.imageMap.find( srcImage );
        if ( src_it == logger.imageMap.end() )
            return;

        auto    dst_it = logger.imageMap.find( dstImage );
        if ( dst_it == logger.imageMap.end( ))
            return;

        log << "    src:       '" << src_it->second.name << "'\n";
        log << "    srcLayout: " << VkImageLayoutToString( srcImageLayout ) << '\n';
        log << "    dst:       '" << dst_it->second.name << "'\n";
        log << "    dstLayout: " << VkImageLayoutToString( dstImageLayout ) << '\n';

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&       reg     = pRegions[i];
            VkExtent3D  src_ext { uint(reg.srcOffsets[1].x - reg.srcOffsets[0].x), uint(reg.srcOffsets[1].y - reg.srcOffsets[0].y), uint(reg.srcOffsets[1].z - reg.srcOffsets[0].z) };
            VkExtent3D  dst_ext { uint(reg.dstOffsets[1].x - reg.dstOffsets[0].x), uint(reg.dstOffsets[1].y - reg.dstOffsets[0].y), uint(reg.dstOffsets[1].z - reg.dstOffsets[0].z) };

            log << "      blit " << SubresourceLayerToString( reg.srcOffsets[0], src_ext, reg.srcSubresource ) << " ---> "
                << SubresourceLayerToString( reg.dstOffsets[0], dst_ext, reg.dstSubresource ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyBufferToImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyBufferToImage (VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout,
                                                            uint regionCount, const VkBufferImageCopy* pRegions)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdCopyBufferToImage( commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyBufferToImage\n";

        auto    src_it = logger.bufferMap.find( srcBuffer );
        if ( src_it == logger.bufferMap.end() )
            return;

        auto    dst_it = logger.imageMap.find( dstImage );
        if ( dst_it == logger.imageMap.end() )
            return;

        log << "    src:       '" << src_it->second.name << "'\n";
        log << "    dst:       '" << dst_it->second.name << "'\n";
        log << "    dstLayout: " << VkImageLayoutToString( dstImageLayout ) << '\n';

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&   reg = pRegions[i];
            log << "      copy { [" << ToString( reg.bufferOffset ) << ", " << ToString( reg.bufferOffset + CalcMemorySize( reg, dst_it->second.info )) << ") } ---> "
                << SubresourceLayerToString( reg.imageOffset, reg.imageExtent, reg.imageSubresource ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyImageToBuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyImageToBuffer (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer,
                                                            uint regionCount, const VkBufferImageCopy* pRegions)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdCopyImageToBuffer( commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyImageToBuffer\n";

        auto    src_it = logger.imageMap.find( srcImage );
        if ( src_it == logger.imageMap.end() )
            return;

        auto    dst_it = logger.bufferMap.find( dstBuffer );
        if ( dst_it == logger.bufferMap.end() )
            return;

        log << "    src:       '" << src_it->second.name << "'\n";
        log << "    srcLayout: " << VkImageLayoutToString( srcImageLayout ) << '\n';
        log << "    dst:       '" << dst_it->second.name << "'\n";

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&   reg = pRegions[i];
            log << "      copy " << SubresourceLayerToString( reg.imageOffset, reg.imageExtent, reg.imageSubresource )
                << " ---> { [" << ToString( reg.bufferOffset ) << ", " << ToString( reg.bufferOffset + CalcMemorySize( reg, src_it->second.info )) << ") }\n";
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdUpdateBuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdUpdateBuffer (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const void* pData)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdUpdateBuffer( commandBuffer, dstBuffer, dstOffset, dataSize, pData );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  UpdateBuffer\n";

        auto    dst_it = logger.bufferMap.find( dstBuffer );
        if ( dst_it == logger.bufferMap.end() )
            return;

        log << "    dst:     '" << dst_it->second.name << "'\n";
        log << "    offset:  " << ToString( dstOffset ) << '\n';
        log << "    size:    " << ToString( dataSize ) << '\n';
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdFillBuffer
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdFillBuffer (VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint data)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdFillBuffer( commandBuffer, dstBuffer, dstOffset, size, data );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  FillBuffer\n";

        auto    dst_it = logger.bufferMap.find( dstBuffer );
        if ( dst_it == logger.bufferMap.end() )
            return;

        log << "    dst:     '" << dst_it->second.name << "'\n";
        log << "    offset:  " << ToString( dstOffset ) << '\n';
        log << "    size:    " << ToString( size ) << '\n';
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdClearColorImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdClearColorImage (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor,
                                                          uint rangeCount, const VkImageSubresourceRange* pRanges)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdClearColorImage( commandBuffer, image, imageLayout, pColor, rangeCount, pRanges );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto& log = cmdbuf.log;
        log << "  ClearColorImage\n";

        auto    dst_it = logger.imageMap.find( image );
        if ( dst_it == logger.imageMap.end() )
            return;

        log << "    dst:    '" << dst_it->second.name << "'\n";
        log << "    layout: " << VkImageLayoutToString( imageLayout ) << '\n';

        for (uint i = 0; i < rangeCount; ++i) {
            log << "      " << SubresourceRangeToString( pRanges[i] ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdClearDepthStencilImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdClearDepthStencilImage (VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil,
                                                                 uint rangeCount, const VkImageSubresourceRange* pRanges)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdClearDepthStencilImage( commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto& log = cmdbuf.log;
        log << "  ClearDepthStencilImage\n";

        auto    dst_it = logger.imageMap.find( image );
        if ( dst_it == logger.imageMap.end() )
            return;

        log << "    dst:    '" << dst_it->second.name << "'\n";
        log << "    layout: " << VkImageLayoutToString( imageLayout ) << '\n';

        for (uint i = 0; i < rangeCount; ++i) {
            log << "      " << SubresourceRangeToString( pRanges[i] ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdClearAttachments
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdClearAttachments (VkCommandBuffer commandBuffer, uint attachmentCount, const VkClearAttachment* pAttachments, uint rectCount, const VkClearRect* pRects)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdClearAttachments( commandBuffer, attachmentCount, pAttachments, rectCount, pRects );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto& log = cmdbuf.log;
        log << "  ClearAttachments\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdResolveImage
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdResolveImage (VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout,
                                                        uint regionCount, const VkImageResolve* pRegions)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdResolveImage( commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto& log = cmdbuf.log;
        log << "  ResolveImage\n";

        auto    src_it = logger.imageMap.find( srcImage );
        if ( src_it == logger.imageMap.end() )
            return;

        auto    dst_it = logger.imageMap.find( dstImage );
        if ( dst_it == logger.imageMap.end() )
            return;

        log << "    src:       '" << src_it->second.name << "'\n";
        log << "    srcLayout: " << VkImageLayoutToString( srcImageLayout ) << '\n';
        log << "    dst:       '" << dst_it->second.name << "'\n";
        log << "    dstLayout: " << VkImageLayoutToString( dstImageLayout ) << '\n';

        for (uint i = 0; i < regionCount; ++i)
        {
            auto&   reg = pRegions[i];
            log << "      resolve " << SubresourceLayerToString( reg.srcOffset, reg.extent, reg.srcSubresource )
                << " ---> " << SubresourceLayerToString( reg.dstOffset, reg.extent, reg.dstSubresource ) << '\n';
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdDispatch
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDispatch (VkCommandBuffer commandBuffer, uint groupCountX, uint groupCountY, uint groupCountZ)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDispatch( commandBuffer, groupCountX, groupCountY, groupCountZ );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  Dispatch\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE );
    }

/*
=================================================
    Wrap_vkCmdDispatchBase
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDispatchBase (VkCommandBuffer commandBuffer, uint baseGroupX, uint baseGroupY, uint baseGroupZ, uint groupCountX, uint groupCountY, uint groupCountZ)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDispatchBase( commandBuffer, baseGroupX, baseGroupY, baseGroupZ, groupCountX, groupCountY, groupCountZ );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DispatchBase\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE );
    }

/*
=================================================
    Wrap_vkCmdDispatchIndirect
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDispatchIndirect (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDispatchIndirect( commandBuffer, buffer, offset );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DispatchIndirect\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_COMPUTE );
    }

/*
=================================================
    Wrap_vkCmdDraw
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDraw (VkCommandBuffer commandBuffer, uint vertexCount, uint instanceCount, uint firstVertex, uint firstInstance)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDraw( commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  Draw\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawIndexed
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawIndexed (VkCommandBuffer commandBuffer, uint indexCount, uint instanceCount, uint firstIndex, int vertexOffset, uint firstInstance)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawIndexed( commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawIndexed\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawIndirect
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawIndirect (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawIndirect( commandBuffer, buffer, offset, drawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawIndirect\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawIndexedIndirect
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawIndexedIndirect (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawIndexedIndirect( commandBuffer, buffer, offset, drawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawIndexedIndirect\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawIndirectCount
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawIndirectCount (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer,
                                                            VkDeviceSize countBufferOffset, uint maxDrawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawIndirectCountKHR( commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawIndirectCount\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawIndexedIndirectCount
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawIndexedIndirectCount (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer,
                                                                    VkDeviceSize countBufferOffset, uint maxDrawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawIndexedIndirectCountKHR( commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawIndexedIndirectCount\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawMeshTasks
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawMeshTasks (VkCommandBuffer commandBuffer, uint groupCountX, uint groupCountY, uint groupCountZ)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawMeshTasksEXT( commandBuffer, groupCountX, groupCountY, groupCountZ );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawMeshTasks\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawMeshTasksIndirect
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawMeshTasksIndirect (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint drawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawMeshTasksIndirectEXT( commandBuffer, buffer, offset, drawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawMeshTasksIndirect\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdDrawMeshTasksIndirectCount
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdDrawMeshTasksIndirectCount (VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkBuffer countBuffer,
                                                                     VkDeviceSize countBufferOffset, uint maxDrawCount, uint stride)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdDrawMeshTasksIndirectCountEXT( commandBuffer, buffer, offset, countBuffer, countBufferOffset, maxDrawCount, stride );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  DrawMeshTasksIndirectCount\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS );
    }

/*
=================================================
    Wrap_vkCmdTraceRaysKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdTraceRaysKHR (VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
                                                        const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
                                                        const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, uint width, uint height, uint depth)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdTraceRaysKHR( commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable,
                                  pCallableShaderBindingTable, width, height, depth );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  TraceRays\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR );
    }

/*
=================================================
    Wrap_vkCmdTraceRaysIndirectKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdTraceRaysIndirectKHR (VkCommandBuffer commandBuffer, const VkStridedDeviceAddressRegionKHR* pRaygenShaderBindingTable,
                                                                const VkStridedDeviceAddressRegionKHR* pMissShaderBindingTable, const VkStridedDeviceAddressRegionKHR* pHitShaderBindingTable,
                                                                const VkStridedDeviceAddressRegionKHR* pCallableShaderBindingTable, VkDeviceAddress indirectDeviceAddress)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdTraceRaysIndirectKHR( commandBuffer, pRaygenShaderBindingTable, pMissShaderBindingTable, pHitShaderBindingTable,
                                          pCallableShaderBindingTable, indirectDeviceAddress );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        cmdbuf.log << "  TraceRaysIndirect\n";
        logger._PrintResourceUsage( cmdbuf, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR );
    }

/*
=================================================
    Wrap_vkCmdBindDescriptorSets
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBindDescriptorSets (VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                                                             uint firstSet, uint descriptorSetCount, const VkDescriptorSet* pDescriptorSets,
                                                             uint dynamicOffsetCount, const uint* pDynamicOffsets)
    {
        auto&   logger = VulkanLogger::Get();
        logger.vkCmdBindDescriptorSets( commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        VulkanLogger::DescrSetArray_t * ds_array = null;

        BEGIN_ENUM_CHECKS();
        switch ( pipelineBindPoint )
        {
            case VK_PIPELINE_BIND_POINT_GRAPHICS :          ds_array = &cmdbuf.bindPoints[0];   break;
            case VK_PIPELINE_BIND_POINT_COMPUTE :           ds_array = &cmdbuf.bindPoints[1];   break;
            case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR :   ds_array = &cmdbuf.bindPoints[2];   break;

            case VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI :
            case VK_PIPELINE_BIND_POINT_MAX_ENUM :
            default :
                DBG_WARNING( "unknown pipeline bind point" );
                return;
        }
        END_ENUM_CHECKS();

        ds_array->resize( Max( ds_array->size(), firstSet + descriptorSetCount ));

        for (uint i = 0; i < descriptorSetCount; ++i)
        {
            (*ds_array)[firstSet + i] = pDescriptorSets[i];

            ASSERT( logger.descSetMap.contains( pDescriptorSets[i] ));
        }
    }

/*
=================================================
    Wrap_vkCreateAccelerationStructureKHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkCreateAccelerationStructureKHR (VkDevice                                      device,
                                                                          const VkAccelerationStructureCreateInfoKHR*   pCreateInfo,
                                                                          const VkAllocationCallbacks*                  pAllocator,
                                                                          VkAccelerationStructureKHR*                   pAccelerationStructure)
    {
        auto&       logger  = VulkanLogger::Get();
        VkResult    res     = logger.vkCreateAccelerationStructureKHR( device, pCreateInfo, pAllocator, OUT pAccelerationStructure );

        if ( res != VK_SUCCESS )
            return res;

        EXLOCK( logger.guard );

        logger.accelStructMap.insert_or_assign( *pAccelerationStructure, VulkanLogger::AccelStructData{} );
        return VK_SUCCESS;
    }

/*
=================================================
    Wrap_vkDestroyAccelerationStructureKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkDestroyAccelerationStructureKHR (VkDevice                         device,
                                                                        VkAccelerationStructureKHR      accelerationStructure,
                                                                        const VkAllocationCallbacks*    pAllocator)
    {
        auto&       logger  = VulkanLogger::Get();
        {
            EXLOCK( logger.guard );
            logger.accelStructMap.erase( accelerationStructure );
        }

        logger.vkDestroyAccelerationStructureKHR( device, accelerationStructure, pAllocator );
    }

/*
=================================================
    Wrap_vkGetBufferDeviceAddressKHR
=================================================
*/
    VKAPI_ATTR VkDeviceAddress VKAPI_CALL Wrap_vkGetBufferDeviceAddressKHR (VkDevice device, const VkBufferDeviceAddressInfo* pInfo)
    {
        auto&   logger  = VulkanLogger::Get();
        auto    addr    = logger.vkGetBufferDeviceAddressKHR( device, pInfo );

        if ( addr == 0 )
            return addr;

        EXLOCK( logger.guard );
        {
            auto    buf_it = logger.bufferMap.find( pInfo->buffer );
            if ( buf_it != logger.bufferMap.end() )
            {
                buf_it->second.address = addr;

                VulkanLogger::DeviceAddressRange    range;
                range.address   = addr;
                range.size      = buf_it->second.info.size;
                range.buffer    = buf_it->second.buffer;

                usize   idx = LowerBound( ArrayView{logger.devAddrToBuffer}, VulkanLogger::DeviceAddressKey{addr} );
                logger.devAddrToBuffer.insert( logger.devAddrToBuffer.begin() + idx, range );
            }
        }
        return addr;
    }

/*
=================================================
    Wrap_vkCmdInsertDebugUtilsLabelEXT
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdInsertDebugUtilsLabelEXT (VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdInsertDebugUtilsLabelEXT( commandBuffer, pLabelInfo );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  InsertDebugLabel\n";
        log << "    name: " << pLabelInfo->pLabelName;
        log << "\n  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdBeginDebugUtilsLabelEXT
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBeginDebugUtilsLabelEXT (VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdBeginDebugUtilsLabelEXT( commandBuffer, pLabelInfo );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        const int   depth = ++cmdbuf.dbgLabelDepth;

        if ( not logger.enableLog )
            return;

        auto&   log = cmdbuf.log;
        log << "  BeginDebugLabel\n";
        log << "    name:  '" << pLabelInfo->pLabelName << "'\n";
        log << "    depth: " << ToString( depth ) << "\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdEndDebugUtilsLabelEXT
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdEndDebugUtilsLabelEXT (VkCommandBuffer commandBuffer)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdEndDebugUtilsLabelEXT( commandBuffer );

        EXLOCK( logger.guard );

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        const int   depth = cmdbuf.dbgLabelDepth--;
        ASSERT( cmdbuf.dbgLabelDepth >= 0 );

        if ( not logger.enableLog )
            return;

        auto&   log = cmdbuf.log;

        if ( not EndsWith( log, "\n\n" ))
            log << '\n';

        log << "  EndDebugLabel\n";
        log << "    depth: " << ToString( depth ) << "\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyQueryPoolResults
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyQueryPoolResults (VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint firstQuery, uint queryCount,
                                                                VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdCopyQueryPoolResults( commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto    buf_it = logger.bufferMap.find( dstBuffer );
        if ( buf_it == logger.bufferMap.end() )
            return;

        auto&   log = cmdbuf.log;
        log << "  CopyQueryPoolResults\n";
        log << "    dstBuffer: '" << buf_it->second.name << "'\n";
        log << "    dstRange:  [" << ToString(dstOffset) << ", "
            << ToString( dstOffset + queryCount * stride + (AllBits(flags, VK_QUERY_RESULT_64_BIT) ? sizeof(ulong) : sizeof(uint))) << ")\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdBuildAccelerationStructuresKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdBuildAccelerationStructuresKHR (VkCommandBuffer commandBuffer, uint infoCount, const VkAccelerationStructureBuildGeometryInfoKHR* pInfos,
                                                                         const VkAccelerationStructureBuildRangeInfoKHR* const* ppBuildRangeInfos)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdBuildAccelerationStructuresKHR( commandBuffer, infoCount, pInfos, ppBuildRangeInfos );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  BuildAccelerationStructures\n";

        const auto  LogGeometry = [&logger, &log] (const VkAccelerationStructureGeometryKHR &geom)
        {{
            BEGIN_ENUM_CHECKS();
            switch ( geom.geometryType )
            {
                case VK_GEOMETRY_TYPE_TRIANGLES_KHR :
                    log << "Triangles\n";
                    log << "      vertexData:    " << logger.GetBufferName( geom.geometry.triangles.vertexData ) << '\n';
                    log << "      indexData:     " << logger.GetBufferName( geom.geometry.triangles.indexData ) << '\n';
                    log << "      transformData: " << logger.GetBufferName( geom.geometry.triangles.transformData ) << '\n';
                    break;
                case VK_GEOMETRY_TYPE_AABBS_KHR :
                    log << "AABBs\n";
                    log << "      data: " << logger.GetBufferName( geom.geometry.aabbs.data ) << '\n';
                    break;
                case VK_GEOMETRY_TYPE_INSTANCES_KHR :
                    log << "Instances\n";
                    log << "      data: " << logger.GetBufferName( geom.geometry.instances.data ) << '\n';
                    break;
                case VK_GEOMETRY_TYPE_MAX_ENUM_KHR :
                default :
                    DBG_WARNING("unknown geometry type");
                    break;
            }
            END_ENUM_CHECKS();
        }};

        for (uint i = 0; i < infoCount; ++i)
        {
            const auto& info        = pInfos[i];
            auto        src_as_it   = logger.accelStructMap.find( info.srcAccelerationStructure );
            auto        dst_as_it   = logger.accelStructMap.find( info.dstAccelerationStructure );

            if ( src_as_it != logger.accelStructMap.end() )
                log << "    srcAS: '" << src_as_it->second.name << "'\n";

            if ( dst_as_it != logger.accelStructMap.end() )
                log << "    dstAS: '" << dst_as_it->second.name << "'\n";

            log << "    scratch: " << logger.GetBufferName( info.scratchData ) << '\n';

            log << "    type: " << (info.type == VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR       ? "TopLevel" :
                                    info.type == VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR    ? "BottomLevel" : "") << '\n';
            log << "    mode: " << (info.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR     ? "Build" :
                                    info.mode == VK_BUILD_ACCELERATION_STRUCTURE_MODE_UPDATE_KHR    ? "Update" : "") << '\n';

            if ( info.pGeometries != null )
            {
                for (uint j = 0; j < info.geometryCount; ++j)
                {
                    log << "    [" << ToString(i) << "] ";
                    LogGeometry( info.pGeometries[i] );
                }
            }
            if ( info.ppGeometries != null )
            {
                for (uint j = 0; j < info.geometryCount; ++j)
                {
                    log << "    [" << ToString(i) << "] ";
                    LogGeometry( *(info.ppGeometries[i]) );
                }
            }
            log << "    ----\n";
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdWriteAccelerationStructuresPropertiesKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdWriteAccelerationStructuresPropertiesKHR (VkCommandBuffer commandBuffer, uint accelerationStructureCount,
                                                                                    const VkAccelerationStructureKHR* pAccelerationStructures, VkQueryType queryType,
                                                                                    VkQueryPool queryPool, uint firstQuery)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdWriteAccelerationStructuresPropertiesKHR( commandBuffer, accelerationStructureCount, pAccelerationStructures, queryType, queryPool, firstQuery );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  WriteAccelerationStructuresProperties\n";
        log << "    AccelerationStructures: ";

        for (uint i = 0; i < accelerationStructureCount; ++i)
        {
            auto    as_it = logger.accelStructMap.find( pAccelerationStructures[i] );
            if ( as_it == logger.accelStructMap.end() )
                continue;

            log << (i > 0 ? ", " : "") << "'" << as_it->second.name << "'";
        }
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyAccelerationStructureKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyAccelerationStructureKHR (VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureInfoKHR* pInfo)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdCopyAccelerationStructureKHR( commandBuffer, pInfo );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto    src_as_it   = logger.accelStructMap.find( pInfo->src );
        auto    dst_as_it   = logger.accelStructMap.find( pInfo->dst );

        if ( src_as_it == logger.accelStructMap.end() or dst_as_it == logger.accelStructMap.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyAccelerationStructure\n";
        log << "    srcAS: '" << src_as_it->second.name << "'\n";
        log << "    dstAS: '" << dst_as_it->second.name << "'\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyAccelerationStructureToMemoryKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyAccelerationStructureToMemoryKHR (VkCommandBuffer commandBuffer, const VkCopyAccelerationStructureToMemoryInfoKHR* pInfo)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdCopyAccelerationStructureToMemoryKHR( commandBuffer, pInfo );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto    as_it = logger.accelStructMap.find( pInfo->src );
        if ( as_it == logger.accelStructMap.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyAccelerationStructureToMemory\n";
        log << "    srcAS:  '" << as_it->second.name << "'\n";
        log << "    dstBuf: " << logger.GetBufferName( pInfo->dst ) << '\n';
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkCmdCopyMemoryToAccelerationStructureKHR
=================================================
*/
    VKAPI_ATTR void VKAPI_CALL Wrap_vkCmdCopyMemoryToAccelerationStructureKHR (VkCommandBuffer commandBuffer, const VkCopyMemoryToAccelerationStructureInfoKHR* pInfo)
    {
        auto&       logger  = VulkanLogger::Get();
        logger.vkCmdCopyMemoryToAccelerationStructureKHR( commandBuffer, pInfo );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return;

        auto    iter = logger.commandBuffers.find( commandBuffer );
        if ( iter == logger.commandBuffers.end() )
            return;

        auto    as_it = logger.accelStructMap.find( pInfo->dst );
        if ( as_it == logger.accelStructMap.end() )
            return;

        auto&   cmdbuf = iter->second;
        ASSERT( cmdbuf.cmdBuffer == commandBuffer );

        auto&   log = cmdbuf.log;
        log << "  CopyMemoryToAccelerationStructure\n";
        log << "    srcBuf: " << logger.GetBufferName( pInfo->src ) << '\n';
        log << "    dstAS:  '" << as_it->second.name << "'\n";
        log << "  ----------\n\n";
    }

/*
=================================================
    Wrap_vkGetSemaphoreCounterValueKHR
=================================================
*/
    VKAPI_ATTR VkResult VKAPI_CALL Wrap_vkGetSemaphoreCounterValueKHR (VkDevice device, VkSemaphore semaphore, OUT ulong* pValue)
    {
        auto&       logger  = VulkanLogger::Get();
        VkResult    result  = logger.vkGetSemaphoreCounterValueKHR( device, semaphore, OUT pValue );

        EXLOCK( logger.guard );
        if ( not logger.enableLog )
            return result;

        logger.log
            << "GetSemaphoreCounterValue: " << logger.GetSyncName( semaphore, *pValue ) << "\n\n";

        return result;
    }

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
    CommandBufferData::Clear
=================================================
*/
    void  VulkanLogger::CommandBufferData::Clear ()
    {
        state           = EState::Initial;
        currentRP       = Default;
        currentFB       = Default;
        subpassIndex    = UMax;
        bindPoints      = Default;
        dbgLabelDepth   = 0;
        log.clear();
    }

/*
=================================================
    _PrintResourceUsage
=================================================
*/
    void  VulkanLogger::_PrintResourceUsage (CommandBufferData &cmdbuf, VkPipelineBindPoint pipelineBindPoint)
    {
      #if PRINT_ALL_DS == 0
        const uint          family_idx  = cmdbuf.queueFamilyIndex;
      #endif
        DescrSetArray_t *   ds_array    = null;

        BEGIN_ENUM_CHECKS();
        switch ( pipelineBindPoint )
        {
            case VK_PIPELINE_BIND_POINT_GRAPHICS :          ds_array = &cmdbuf.bindPoints[0];   break;
            case VK_PIPELINE_BIND_POINT_COMPUTE :           ds_array = &cmdbuf.bindPoints[1];   break;
            case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR :   ds_array = &cmdbuf.bindPoints[2];   break;

            case VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI :
            case VK_PIPELINE_BIND_POINT_MAX_ENUM :
            default :
                DBG_WARNING( "unknown pipeline bind point" );
                return;
        }
        END_ENUM_CHECKS();

        String  tmp_log;

        for (usize i = 0; i < ds_array->size(); ++i)
        {
            auto&   ds      = (*ds_array)[i];
            auto    ds_it   = descSetMap.find( ds );

            if ( ds_it == descSetMap.end() )
                continue;

            auto&   bindings = ds_it->second.bindings;
            for (usize j = 0; j < bindings.size(); ++j)
            {
                auto&   bind = bindings[j];

                BEGIN_ENUM_CHECKS();
                switch ( bind.descriptorType )
                {
                    case VK_DESCRIPTOR_TYPE_SAMPLER :
                        break;
                    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :
                    case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT :
                    {
                        for (usize a = 0; a < bind.images.size(); ++a)
                        {
                          #if PRINT_ALL_DS == 0
                            if ( bind.processed[a][family_idx] )
                                continue;

                            bind.processed[a][family_idx] = true;
                          #endif

                            auto    view_it = imageViewMap.find( bind.images[a].imageView );
                            if ( view_it == imageViewMap.end() )
                                continue;

                            auto    image_it = imageMap.find( view_it->second.info.image );
                            if ( image_it == imageMap.end() )
                                continue;

                            auto&   img = image_it->second;
                            tmp_log << "      image: '" << img.name << "', layout: " << VkImageLayoutToString( bind.images[a].imageLayout ) << '\n';
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER :
                    case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER :
                    {
                        for (usize a = 0; a < bind.texelBuffers.size(); ++a)
                        {
                          #if PRINT_ALL_DS == 0
                            if ( bind.processed[a][family_idx] )
                                continue;

                            bind.processed[a][family_idx] = true;
                          #endif

                            auto    view_it = bufferViewMap.find( bind.texelBuffers[a] );
                            if ( view_it == bufferViewMap.end() )
                                continue;

                            auto    buffer_it = bufferMap.find( view_it->second.info.buffer );
                            if ( buffer_it == bufferMap.end() )
                                continue;

                            auto&   buf = buffer_it->second;
                            tmp_log << "      buffer: '" << buf.name << "'\n"; 
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER :
                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER :
                    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC :
                    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC :
                    {
                        for (usize a = 0; a < bind.buffers.size(); ++a)
                        {
                          #if PRINT_ALL_DS == 0
                            if ( bind.processed[a][family_idx] )
                                continue;

                            bind.processed[a][family_idx] = true;
                          #endif

                            auto    buf_it = bufferMap.find( bind.buffers[a].buffer );
                            if ( buf_it == bufferMap.end() )
                                continue;

                            auto&   buf = buf_it->second;
                            tmp_log << "      buffer: '" << buf.name << "'\n";
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR :
                    {
                        for (usize a = 0; a < bind.accelStructs.size(); ++a)
                        {
                          #if PRINT_ALL_DS == 0
                            if ( bind.processed[a][family_idx] )
                                continue;

                            bind.processed[a][family_idx] = true;
                          #endif

                            auto    as_it = accelStructMap.find( bind.accelStructs[a] );
                            if ( as_it == accelStructMap.end() )
                                continue;

                            auto&   as = as_it->second;
                            tmp_log << "      accel struct: '" << as.name << "'\n";
                        }
                        break;
                    }
                    case VK_DESCRIPTOR_TYPE_MAX_ENUM :
                        break;

                    case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT :
                    case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV :
                    case VK_DESCRIPTOR_TYPE_MUTABLE_VALVE :
                    case VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM :
                    case VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM :
                    default :
                        DBG_WARNING( "unsupported descriptor type" );
                        break;
                }
                END_ENUM_CHECKS();
            }
        }

        if ( tmp_log.size() )
            cmdbuf.log << "    descriptors:\n" << tmp_log << "  ----------\n\n";
    }

/*
=================================================
    Initialize
=================================================
*/
    void  VulkanLogger::Initialize (INOUT VulkanDeviceFnTable& fnTable, FlatHashMap<VkQueue, String> queueNames)
    {
        EXLOCK( guard );

        queueMap = RVRef(queueNames);

        std::memcpy( &_originDeviceFnTable, &fnTable, sizeof(_originDeviceFnTable) );
        VulkanDeviceFn_Init( reinterpret_cast<VulkanDeviceFnTable*>(&_originDeviceFnTable) );

        auto&   table = *reinterpret_cast<DeviceFnTable *>(&fnTable);

        table._var_vkCreateBuffer                   = &Wrap_vkCreateBuffer;
        table._var_vkDestroyBuffer                  = &Wrap_vkDestroyBuffer;
        table._var_vkCreateBufferView               = &Wrap_vkCreateBufferView;
        table._var_vkDestroyBufferView              = &Wrap_vkDestroyBufferView;
        table._var_vkCreateImage                    = &Wrap_vkCreateImage;
        table._var_vkDestroyImage                   = &Wrap_vkDestroyImage;
        table._var_vkCreateImageView                = &Wrap_vkCreateImageView;
        table._var_vkDestroyImageView               = &Wrap_vkDestroyImageView;
        table._var_vkCreateFramebuffer              = &Wrap_vkCreateFramebuffer;
        table._var_vkDestroyFramebuffer             = &Wrap_vkDestroyFramebuffer;
        table._var_vkCreateRenderPass               = &Wrap_vkCreateRenderPass;
        table._var_vkDestroyRenderPass              = &Wrap_vkDestroyRenderPass;
        table._var_vkCreateGraphicsPipelines        = &Wrap_vkCreateGraphicsPipelines;
        table._var_vkCreateComputePipelines         = &Wrap_vkCreateComputePipelines;
        table._var_vkDestroyPipeline                = &Wrap_vkDestroyPipeline;
        table._var_vkCreateCommandPool              = &Wrap_vkCreateCommandPool;
        table._var_vkDestroyCommandPool             = &Wrap_vkDestroyCommandPool;
        table._var_vkAllocateCommandBuffers         = &Wrap_vkAllocateCommandBuffers;
        table._var_vkBeginCommandBuffer             = &Wrap_vkBeginCommandBuffer;
        table._var_vkEndCommandBuffer               = &Wrap_vkEndCommandBuffer;
        table._var_vkCreateRenderPass2              = &Wrap_vkCreateRenderPass2;
        table._var_vkCreateRenderPass2KHR           = &Wrap_vkCreateRenderPass2;
        table._var_vkCreateDescriptorSetLayout      = &Wrap_vkCreateDescriptorSetLayout;
        table._var_vkDestroyDescriptorSetLayout     = &Wrap_vkDestroyDescriptorSetLayout;
        table._var_vkAllocateDescriptorSets         = &Wrap_vkAllocateDescriptorSets;
        table._var_vkFreeDescriptorSets             = &Wrap_vkFreeDescriptorSets;
        table._var_vkUpdateDescriptorSets           = &Wrap_vkUpdateDescriptorSets;
        table._var_vkCreateFence                    = &Wrap_vkCreateFence;
        table._var_vkDestroyFence                   = &Wrap_vkDestroyFence;
        table._var_vkCreateSemaphore                = &Wrap_vkCreateSemaphore;
        table._var_vkDestroySemaphore               = &Wrap_vkDestroySemaphore;
        table._var_vkGetSwapchainImagesKHR          = &Wrap_vkGetSwapchainImagesKHR;
        table._var_vkCreateAccelerationStructureKHR = &Wrap_vkCreateAccelerationStructureKHR;
        table._var_vkDestroyAccelerationStructureKHR= &Wrap_vkDestroyAccelerationStructureKHR;
        table._var_vkGetBufferDeviceAddressKHR      = &Wrap_vkGetBufferDeviceAddressKHR;

        table._var_vkQueueSubmit                    = &Wrap_vkQueueSubmit;
        table._var_vkQueueSubmit2KHR                = &Wrap_vkQueueSubmit2KHR;
        table._var_vkSetDebugUtilsObjectNameEXT     = &Wrap_vkSetDebugUtilsObjectNameEXT;
        table._var_vkAcquireNextImageKHR            = &Wrap_vkAcquireNextImageKHR;
        table._var_vkQueuePresentKHR                = &Wrap_vkQueuePresentKHR;
        // TODO: bind sparse

        table._var_vkCmdInsertDebugUtilsLabelEXT    = &Wrap_vkCmdInsertDebugUtilsLabelEXT;
        table._var_vkCmdBeginDebugUtilsLabelEXT     = &Wrap_vkCmdBeginDebugUtilsLabelEXT;
        table._var_vkCmdEndDebugUtilsLabelEXT       = &Wrap_vkCmdEndDebugUtilsLabelEXT;
        table._var_vkCmdBindDescriptorSets          = &Wrap_vkCmdBindDescriptorSets;
        table._var_vkCmdPipelineBarrier             = &Wrap_vkCmdPipelineBarrier;
        table._var_vkCmdPipelineBarrier2KHR         = &Wrap_vkCmdPipelineBarrier2;
        table._var_vkCmdBeginRenderPass             = &Wrap_vkCmdBeginRenderPass;
        table._var_vkCmdNextSubpass                 = &Wrap_vkCmdNextSubpass;
        table._var_vkCmdEndRenderPass               = &Wrap_vkCmdEndRenderPass;
        table._var_vkCmdBeginRenderPass2            = &Wrap_vkCmdBeginRenderPass2;
        table._var_vkCmdNextSubpass2                = &Wrap_vkCmdNextSubpass2;
        table._var_vkCmdEndRenderPass2              = &Wrap_vkCmdEndRenderPass2;
        table._var_vkCmdBeginRenderPass2KHR         = &Wrap_vkCmdBeginRenderPass2;
        table._var_vkCmdNextSubpass2KHR             = &Wrap_vkCmdNextSubpass2;
        table._var_vkCmdEndRenderPass2KHR           = &Wrap_vkCmdEndRenderPass2;
        table._var_vkCmdCopyBuffer                  = &Wrap_vkCmdCopyBuffer;
        table._var_vkCmdCopyImage                   = &Wrap_vkCmdCopyImage;
        table._var_vkCmdBlitImage                   = &Wrap_vkCmdBlitImage;
        table._var_vkCmdCopyBufferToImage           = &Wrap_vkCmdCopyBufferToImage;
        table._var_vkCmdCopyImageToBuffer           = &Wrap_vkCmdCopyImageToBuffer;
        table._var_vkCmdUpdateBuffer                = &Wrap_vkCmdUpdateBuffer;
        table._var_vkCmdFillBuffer                  = &Wrap_vkCmdFillBuffer;
        table._var_vkCmdClearColorImage             = &Wrap_vkCmdClearColorImage;
        table._var_vkCmdClearDepthStencilImage      = &Wrap_vkCmdClearDepthStencilImage;
        table._var_vkCmdClearAttachments            = &Wrap_vkCmdClearAttachments;
        table._var_vkCmdResolveImage                = &Wrap_vkCmdResolveImage;
        table._var_vkCmdDispatch                    = &Wrap_vkCmdDispatch;
        table._var_vkCmdDispatchBase                = &Wrap_vkCmdDispatchBase;
        table._var_vkCmdDispatchIndirect            = &Wrap_vkCmdDispatchIndirect;
        table._var_vkCmdDispatchBaseKHR             = &Wrap_vkCmdDispatchBase;
        table._var_vkCmdDraw                        = &Wrap_vkCmdDraw;
        table._var_vkCmdDrawIndexed                 = &Wrap_vkCmdDrawIndexed;
        table._var_vkCmdDrawIndirect                = &Wrap_vkCmdDrawIndirect;
        table._var_vkCmdDrawIndexedIndirect         = &Wrap_vkCmdDrawIndexedIndirect;
        table._var_vkCmdDrawIndirectCount           = &Wrap_vkCmdDrawIndirectCount;
        table._var_vkCmdDrawIndexedIndirectCount    = &Wrap_vkCmdDrawIndexedIndirectCount;
        table._var_vkCmdDrawIndirectCountKHR        = &Wrap_vkCmdDrawIndirectCount;
        table._var_vkCmdDrawIndexedIndirectCountKHR = &Wrap_vkCmdDrawIndexedIndirectCount;
        table._var_vkCmdDrawMeshTasksEXT            = &Wrap_vkCmdDrawMeshTasks;
        table._var_vkCmdDrawMeshTasksIndirectEXT    = &Wrap_vkCmdDrawMeshTasksIndirect;
        table._var_vkCmdDrawMeshTasksIndirectCountEXT= &Wrap_vkCmdDrawMeshTasksIndirectCount;
        table._var_vkCmdTraceRaysKHR                = &Wrap_vkCmdTraceRaysKHR;
        table._var_vkCmdTraceRaysIndirectKHR        = &Wrap_vkCmdTraceRaysIndirectKHR;
        table._var_vkCmdCopyQueryPoolResults        = &Wrap_vkCmdCopyQueryPoolResults;

    //  table._var_vkGetSemaphoreCounterValueKHR    = &Wrap_vkGetSemaphoreCounterValueKHR;

        table._var_vkCmdBuildAccelerationStructuresKHR           = &Wrap_vkCmdBuildAccelerationStructuresKHR;
        table._var_vkCmdWriteAccelerationStructuresPropertiesKHR = &Wrap_vkCmdWriteAccelerationStructuresPropertiesKHR;
        table._var_vkCmdCopyAccelerationStructureKHR             = &Wrap_vkCmdCopyAccelerationStructureKHR;
        table._var_vkCmdCopyAccelerationStructureToMemoryKHR     = &Wrap_vkCmdCopyAccelerationStructureToMemoryKHR;
        table._var_vkCmdCopyMemoryToAccelerationStructureKHR     = &Wrap_vkCmdCopyMemoryToAccelerationStructureKHR;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  VulkanLogger::Deinitialize (OUT VulkanDeviceFnTable& fnTable)
    {
        EXLOCK( guard );

        std::memcpy( &fnTable, &_originDeviceFnTable, sizeof(_originDeviceFnTable) );
    }

/*
=================================================
    ResetSyncNames
=================================================
*/
    void  VulkanLogger::ResetSyncNames ()
    {
        EXLOCK( guard );

        _syncNameMap.clear();
        _syncNameCount  = 0;
    }

/*
=================================================
    GetSyncName
=================================================
*/
    String  VulkanLogger::GetSyncName (VkSemaphore sem, ulong val)
    {
        auto    it = _syncNameMap.find( MakePair( sem, val ));
        if ( it != _syncNameMap.end() )
            return it->second;

        String  name = "sync-";
        name << ToString( _syncNameCount++ );

        _syncNameMap.emplace( MakePair( sem, val ), name );
        return name;
    }

/*
=================================================
    GetBufferName
=================================================
*/
    String  VulkanLogger::GetBufferName (VkDeviceAddress addr)
    {
        if ( addr == 0 )
            return "null";

        usize   idx = LowerBound2( devAddrToBuffer, DeviceAddressKey{addr} );
        if ( idx != UMax )
        {
            auto    it = bufferMap.find( devAddrToBuffer[ idx ].buffer );
            if ( it != bufferMap.end() )
                return "'"s << it->second.name << '\'';
        }
        return "<unknown>";
    }

} // namespace


/*
=================================================
    Initialize
=================================================
*/
void  VulkanSyncLog::Initialize (INOUT VulkanDeviceFnTable& table, FlatHashMap<VkQueue, String> queueNames)
{
    auto&   logger = VulkanLogger::Get();
    PlacementNew<VulkanLogger>( OUT &logger );

    logger.Initialize( INOUT table, RVRef(queueNames) );
}

/*
=================================================
    Deinitialize
=================================================
*/
void  VulkanSyncLog::Deinitialize (INOUT VulkanDeviceFnTable& table)
{
    auto&   logger = VulkanLogger::Get();

    logger.Deinitialize( INOUT table );
    logger.~VulkanLogger();
}

/*
=================================================
    Enable
=================================================
*/
void  VulkanSyncLog::Enable ()
{
    auto&   logger = VulkanLogger::Get();
    EXLOCK( logger.guard );

    logger.enableLog = true;
    logger.ResetSyncNames();
}

/*
=================================================
    Disable
=================================================
*/
void  VulkanSyncLog::Disable ()
{
    auto&   logger = VulkanLogger::Get();
    EXLOCK( logger.guard );

    logger.enableLog = false;
}

/*
=================================================
    GetLog
=================================================
*/
void  VulkanSyncLog::GetLog (OUT String &log)
{
    auto&   logger = VulkanLogger::Get();
    EXLOCK( logger.guard );

    log.clear();
    std::swap( log, logger.log );
}

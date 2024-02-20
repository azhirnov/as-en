// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "../Impl/Common.h"
#include "graphics/Vulkan/VDevice.h"

namespace AE::PipelineCompiler
{
    using namespace AE::Graphics;

    enum class ETraceMode
    {
        None,
        DebugTrace,
        Performance,
        TimeMap,
    };


    class TestDevice final : public VulkanDeviceFn
    {
    // variables
    public:
        VkCommandPool           cmdPool             = Default;
        VkCommandBuffer         cmdBuffer           = Default;
        VkDescriptorPool        descPool            = Default;
        VkBuffer                debugOutputBuf      = Default;
        VkBuffer                readBackBuf         = Default;
        VkDeviceMemory          debugOutputMem      = Default;
        VkDeviceMemory          readBackMem         = Default;
        void *                  readBackPtr         = null;
        uint                    debugOutputSize     = 128 << 20;


    private:
        VDeviceInitializer      _vulkan;

        using Debuggable_t  = HashMap< VkShaderModule, ShaderTrace* >;

        Array<uint>             _tempBuf;
        Debuggable_t            _debuggableShaders;

    public:
        enum class EHandleType
        {
            Memory,
            Buffer,
            Image,
            ImageView,
            Pipeline,
            PipelineLayout,
            Shader,
            DescriptorSetLayout,
            RenderPass,
            Framebuffer,
            AccStruct,
        };
        using TempHandles_t = Array< Pair< EHandleType, ulong >>;
        TempHandles_t       tempHandles;


    // methods
    public:
        TestDevice ();

        ND_ bool  Create ();
            void  Destroy ();

        ND_ bool  GetMemoryTypeIndex (uint memoryTypeBits, VkMemoryPropertyFlags flags, OUT uint &memoryTypeIndex) const;

        ND_ bool  CreateDebugDescriptorSet (VkShaderStageFlags stages,
                                            OUT VkDescriptorSetLayout &dsLayout, OUT VkDescriptorSet &descSet);

        ND_ bool  CreateRenderTarget (VkFormat colorFormat, uint width, uint height, VkImageUsageFlags imageUsage,
                                      OUT VkRenderPass &outRenderPass, OUT VkImage &outImage,
                                      OUT VkFramebuffer &outFramebuffer);

        ND_ bool  CreateStorageImage (VkFormat format, uint width, uint height, VkImageUsageFlags imageUsage,
                                      OUT VkImage &outImage, OUT VkImageView &outView);

        ND_ bool  CreateGraphicsPipelineVar1 (VkShaderModule vertShader, VkShaderModule fragShader,
                                              VkDescriptorSetLayout dsLayout, VkRenderPass renderPass,
                                              OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline);

        ND_ bool  CreateGraphicsPipelineVar2 (VkShaderModule vertShader, VkShaderModule tessContShader, VkShaderModule tessEvalShader,
                                              VkShaderModule fragShader, VkDescriptorSetLayout dsLayout, VkRenderPass renderPass, uint patchSize,
                                              OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline);

        ND_ bool  CreateMeshPipelineVar1 (VkShaderModule meshShader, VkShaderModule fragShader,
                                          VkDescriptorSetLayout dsLayout, VkRenderPass renderPass,
                                          OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline);

        struct RTData
        {
            VkDeviceSize                shaderGroupSize     = 0;
            VkDeviceSize                shaderGroupAlign    = 0;
            VkBuffer                    shaderBindingTable  = Default;
            VkDeviceAddress             sbtAddress          = Default;
            VkAccelerationStructureKHR  topLevelAS          = Default;
            VkAccelerationStructureKHR  bottomLevelAS       = Default;
        };
        ND_ bool  CreateRayTracingScene (VkPipeline rtPipeline, uint numGroups, OUT RTData &outRTData);

        ND_ bool  Compile (OUT VkShaderModule&      shaderModule,
                           Array<const char *>      source,
                           EShLanguage              shaderType,
                           ETraceMode               mode                = ETraceMode::None,
                           uint                     dbgBufferSetIndex   = UMax);

        ND_ bool  TestDebugTraceOutput (Array<VkShaderModule> modules, String referenceFile);

        ND_ bool  TestPerformanceOutput (Array<VkShaderModule> modules, Array<String> fnNames);

        ND_ bool  CheckTimeMap (Array<VkShaderModule> modules, float emptyPxFactor = 1.0f);

        ND_ bool  SaveImage (VkImage image, VkImageLayout layout, uint width, uint height, const Path &filename);

            void  FreeTempHandles ();

        ND_ VkDevice    GetVkDevice ()      const   { return _vulkan.GetVkDevice(); }
        ND_ VkQueue     GetVkQueue ()       const   { return _vulkan.GetQueues()[0].handle; }
        ND_ uint        GetQueueFamily ()   const   { return uint(_vulkan.GetQueues()[0].familyIndex); }

        ND_ VkPhysicalDeviceProperties const&                       GetDeviceProps ()       const   { return _vulkan.GetVProperties().properties; }
        ND_ VkPhysicalDeviceFeatures const&                         GetDeviceFeats ()       const   { return _vulkan.GetVProperties().features; }
        ND_ VkPhysicalDeviceMeshShaderFeaturesEXT const&            GetMeshShaderFeats ()   const   { return _vulkan.GetVProperties().meshShaderFeats; }
        ND_ VkPhysicalDeviceMeshShaderPropertiesEXT const&          GetMeshShaderProps ()   const   { return _vulkan.GetVProperties().meshShaderProps; }
        ND_ VkPhysicalDeviceShaderClockFeaturesKHR const&           GetShaderClockFeats ()  const   { return _vulkan.GetVProperties().shaderClockFeats; }
        ND_ VkPhysicalDeviceRayTracingPipelineFeaturesKHR const&    GetRayTracingFeats ()   const   { return _vulkan.GetVProperties().rayTracingPipelineFeats; }
        ND_ VkPhysicalDeviceRayTracingPipelinePropertiesKHR const&  GetRayTracingProps ()   const   { return _vulkan.GetVProperties().rayTracingPipelineProps; }

    private:
        ND_ bool  _CreateResources ();
            void  _DestroyResources ();

        ND_ bool  _GetDebugOutput (VkShaderModule shaderModule, const void* ptr, VkDeviceSize maxSize, OUT Array<String> &result) const;

        ND_ bool  _Compile (OUT Array<uint> &       spirvData,
                            OUT ShaderTrace *       dbgInfo,
                            uint                    dbgBufferSetIndex,
                            Array<const char *>     source,
                            EShLanguage             shaderType,
                            ETraceMode              mode,
                            glslang::EShTargetLanguageVersion   spvVersion);
    };


#define TEST_PASSED()   AE_LOGI( Base::String{AE_FUNCTION_NAME} + " - passed" );


} // AE::PipelineCompiler

using namespace AE::PipelineCompiler;

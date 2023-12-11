// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

    //
    // Vulkan Descriptor Updater
    //

    class VDescriptorUpdater final : public IDescriptorUpdater
    {
    // types
    private:
        using Uniform_t     = PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
        using DT            = EDescriptorType;
        using Allocator_t   = LinearAllocator< UntypedAllocator, 4, false >;

        struct UpdateDescriptorSetsData
        {
            VkWriteDescriptorSet*   descriptors;
            uint                    index;
            uint                    count;
        };

        struct UpdateWithTemplateData
        {
            void *      tmplData;
            Bytes       tmplDataSize;
        };

        static constexpr uint   _MaxDescSetCount = 1024;

        static constexpr usize  _UpdTmplAlign = Max( Max( alignof(VkDescriptorImageInfo), alignof(VkDescriptorBufferInfo) ),
                                                     Max( alignof(VkBufferView), alignof(VkAccelerationStructureKHR) ));


    // variables
    private:
        VResourceManager &              _resMngr;
        Ptr<const VDescriptorSetLayout> _dsLayout;      // strong ref in '_descSetId'
        VkDescriptorSet                 _dsHandle       = Default;

        EDescUpdateMode                 _mode           = Default;
        union {
            UpdateDescriptorSetsData    _updDesc        {};
            UpdateWithTemplateData      _updDescTempl;
        };
        Allocator_t                     _allocator;
        Strong< DescriptorSetID >       _descSetId;

        DRC_ONLY( RWDataRaceCheck       _drCheck; )


    // methods
    public:
        VDescriptorUpdater ()                                                                                                                           __NE___;
        ~VDescriptorUpdater ()                                                                                                                          __NE_OV;

        bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode)                                                                                    __NE_OV;
        bool  Flush ()                                                                                                                                  __NE_OV;


        bool  BindImage  (const UniformName &name, VkImageView image, uint elementIndex = 0)                                                            __NE___;
        bool  BindImages (const UniformName &name, ArrayView<VkImageView> images, uint firstIndex = 0)                                                  __NE___;

        bool  BindImage  (const UniformName &name, ImageViewID image, uint elementIndex = 0)                                                            __NE_OV;
        bool  BindImages (const UniformName &name, ArrayView<ImageViewID> images, uint firstIndex = 0)                                                  __NE_OV;

        bool  BindVideoImage (const UniformName &name, VideoImageID image, uint elementIndex = 0)                                                       __NE_OV;

        uint  ImageCount (const UniformName &name)                                                                                                      C_NE_OV;


        bool  BindTexture  (const UniformName &name, VkImageView image, VkSampler sampler, uint elementIndex = 0)                                       __NE___;
        bool  BindTextures (const UniformName &name, ArrayView<VkImageView> images, VkSampler sampler, uint firstIndex = 0)                             __NE___;

        bool  BindTexture  (const UniformName &name, ImageViewID image, const SamplerName &sampler, uint elementIndex = 0)                              __NE_OV;
        bool  BindTextures (const UniformName &name, ArrayView<ImageViewID> images, const SamplerName &sampler, uint firstIndex = 0)                    __NE_OV;

        uint  TextureCount (const UniformName &name)                                                                                                    C_NE_OV;


        bool  BindSampler  (const UniformName &name, VkSampler sampler, uint elementIndex = 0)                                                          __NE___;
        bool  BindSamplers (const UniformName &name, ArrayView<VkSampler> samplers, uint firstIndex = 0)                                                __NE___;

        bool  BindSampler  (const UniformName &name, const SamplerName &sampler, uint elementIndex = 0)                                                 __NE_OV;
        bool  BindSamplers (const UniformName &name, ArrayView<SamplerName> samplers, uint firstIndex = 0)                                              __NE_OV;

        uint  SamplerCount (const UniformName &name)                                                                                                    C_NE_OV;


        using IDescriptorUpdater::BindBuffer;
        using IDescriptorUpdater::BindBuffers;

        bool  BindBuffer  (const UniformName &name, const ShaderStructName &typeName, VkBuffer buffer, Bytes offset, Bytes size, uint elementIndex = 0) __NE___;
        bool  BindBuffer  (const UniformName &name, const ShaderStructName &typeName, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0) __NE_OV;

        bool  BindBuffer  (const UniformName &name, const ShaderStructName &typeName, VkBuffer buffer, uint elementIndex = 0)                           __NE___;
        bool  BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<VkBuffer> buffers, uint firstIndex = 0)                 __NE___;

        bool  BindBuffer  (const UniformName &name, const ShaderStructName &typeName, BufferID buffer, uint elementIndex = 0)                           __NE_OV;
        bool  BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<BufferID> buffers, uint firstIndex = 0)                 __NE_OV;

        uint  BufferCount (const UniformName &name)                                                                                                     C_NE_OV;

        ShaderStructName  GetBufferStructName (const UniformName &name)                                                                                 C_NE_OV;


        bool  BindTexelBuffer  (const UniformName &name, VkBufferView view, uint elementIndex = 0)                                                      __NE___;
        bool  BindTexelBuffers (const UniformName &name, ArrayView<VkBufferView> views, uint firstIndex = 0)                                            __NE___;

        bool  BindTexelBuffer  (const UniformName &name, BufferViewID view, uint elementIndex = 0)                                                      __NE_OV;
        bool  BindTexelBuffers (const UniformName &name, ArrayView<BufferViewID> views, uint firstIndex = 0)                                            __NE_OV;

        uint  TexelBufferCount (const UniformName &name)                                                                                                C_NE_OV;


        bool  BindRayTracingScene (const UniformName &name, VkAccelerationStructureKHR scene, uint elementIndex = 0)                                    __NE___;
        bool  BindRayTracingScenes (const UniformName &name, ArrayView<VkAccelerationStructureKHR> scenes, uint firstIndex = 0)                         __NE___;

        bool  BindRayTracingScene (const UniformName &name, RTSceneID scene, uint elementIndex = 0)                                                     __NE_OV;
        bool  BindRayTracingScenes (const UniformName &name, ArrayView<RTSceneID> scenes, uint firstIndex = 0)                                          __NE_OV;

        uint  RayTracingSceneCount (const UniformName &name)                                                                                            C_NE_OV;


    private:
        bool  _Set (DescriptorSetID descrSetId, EDescUpdateMode mode);
        void  _Reset ();
        bool  _Flush ();

        ND_ bool  _UseUpdateTemplate () const   { return _mode == EDescUpdateMode::UpdateTemplate; }

        template <EDescriptorType DescType>
        ND_ Tuple< const Uniform_t*, const Byte16u* >  _FindUniform (const UniformName &name) const;

        template <EDescriptorType DescType>
        ND_ uint  _GetArraySize (const UniformName &name) const;

        template <typename T>
        bool  _BindImages (const UniformName &name, ArrayView<T> images, uint firstIndex);

        template <typename T1, typename T2>
        bool  _BindTextures (const UniformName &name, ArrayView<T1> images, const T2 &sampler, uint firstIndex);

        template <typename T>
        bool  _BindSamplers (const UniformName &name, ArrayView<T> samplers, uint firstIndex);

        template <typename T>
        bool  _BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<T> buffers, uint firstIndex);

        template <typename T>
        bool  _BindBuffer (const UniformName &name, const ShaderStructName &typeName, T buffer, Bytes offset, Bytes size, uint elementIndex);

        template <typename T>
        bool  _BindTexelBuffers (const UniformName &name, ArrayView<T> views, uint firstIndex);

        template <typename T>
        bool  _BindRayTracingScenes (const UniformName &name, ArrayView<T> scenes, uint firstIndex);
    };


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

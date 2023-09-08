// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Descriptors/VDescriptorUpdater.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Private/EnumToString.h"

namespace AE::Base
{
/*
=================================================
    ToString
=================================================
*/
    ND_ inline String  ToString (PipelineCompiler::EImageType type)
    {
        return PipelineCompiler::EImageType_ToString( type );
    }
}

namespace AE::Graphics
{
namespace
{
/*
=================================================
    GetImageType
=================================================
*/
    ND_ inline PipelineCompiler::EImageType  GetImageType (const ImageDesc &desc, const ImageViewDesc &view)
    {
        return  PipelineCompiler::EImageType_FromImage( view.viewType, desc.samples.IsEnabled() ) |
                PipelineCompiler::EImageType_FromPixelFormat( view.format );
    }

    ND_ inline PipelineCompiler::EImageType  GetImageType (const BufferViewDesc &view)
    {
        return  PipelineCompiler::EImageType::Buffer |
                PipelineCompiler::EImageType_FromPixelFormat( view.format );
    }
}

/*
=================================================
    constructor
=================================================
*/
    VDescriptorUpdater::VDescriptorUpdater () __NE___ :
        _resMngr{RenderTaskScheduler().GetResourceManager()}
    {}

/*
=================================================
    destructor
=================================================
*/
    VDescriptorUpdater::~VDescriptorUpdater () __NE___
    {
        _resMngr.ImmediatelyRelease( INOUT _descSetId );
    }

/*
=================================================
    _Reset
=================================================
*/
    void  VDescriptorUpdater::_Reset ()
    {
        _resMngr.ImmediatelyRelease( INOUT _descSetId );

        _dsHandle   = Default;
        _descSetId  = Default;
        _dsLayout   = null;
        _mode       = Default;

        _allocator.Discard();

        ZeroMem( OUT &_updDesc, Bytes{Max( sizeof(_updDesc), sizeof(_updDescTempl) )} );
    }

/*
=================================================
    Set
=================================================
*/
    bool  VDescriptorUpdater::Set (DescriptorSetID descrSetId, EDescUpdateMode mode) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if ( _Set( descrSetId, mode ))
            return true;

        // release DS
        _Reset();

        return false;
    }

    bool  VDescriptorUpdater::_Set (DescriptorSetID descrSetId, EDescUpdateMode mode)
    {
        // flush if:
        //  - previous mode is 'UpdateTemplate'
        //  - new mode is 'UpdateTemplate'
        if ( _mode == EDescUpdateMode::UpdateTemplate or mode == EDescUpdateMode::UpdateTemplate )
            _Flush();

        // release previous DS
        _resMngr.ImmediatelyRelease( INOUT _descSetId );
        _dsHandle   = Default;
        _dsLayout   = null;

        // acquire new DS
        auto*   desc_set = _resMngr.GetResource( descrSetId, True{"incRef"}, True{"quiet"} );
        CHECK_ERR( desc_set != null );

        _descSetId.Attach( descrSetId );
        _dsHandle = desc_set->Handle();

        auto*   ds_layout = _resMngr.GetResource( desc_set->LayoutID(), False{"don't inc ref"}, True{"quiet"} );
        CHECK_ERR( ds_layout != null );
        _dsLayout = ds_layout;

        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case EDescUpdateMode::Partialy :
            {
                if ( _mode != mode )
                {
                    _updDesc.count       = Max( _dsLayout->GetBindingCount(), _MaxDescSetCount );
                    _updDesc.index       = 0;
                    _updDesc.descriptors = _allocator.Allocate< VkWriteDescriptorSet >( _updDesc.count );
                }
                break;
            }
            case EDescUpdateMode::UpdateTemplate :
            {
                CHECK_ERR( _dsLayout->GetUpdateTemplate() != Default );

                _updDescTempl.tmplDataSize  = _dsLayout->GetUpdateTemplateSize();
                _updDescTempl.tmplData      = _allocator.Allocate( SizeAndAlign{ _updDescTempl.tmplDataSize, Bytes{_UpdTmplAlign} });
                break;
            }
            case EDescUpdateMode::Unknown :
            default_unlikely :
                RETURN_ERR( "unknown update mode" );
        }
        END_ENUM_CHECKS();

        _mode = mode;
        return true;
    }

/*
=================================================
    Flush
=================================================
*/
    bool  VDescriptorUpdater::Flush () __NE___
    {
        DRC_EXLOCK( _drCheck );

        return _Flush();
    }

    bool  VDescriptorUpdater::_Flush ()
    {
        if ( _dsLayout == null )
            return true;

        _resMngr.ImmediatelyRelease( INOUT _descSetId );

        auto&   dev = _resMngr.GetDevice();

        BEGIN_ENUM_CHECKS();
        switch ( _mode )
        {
            case EDescUpdateMode::Partialy :
                dev.vkUpdateDescriptorSets( dev.GetVkDevice(), _updDesc.index, _updDesc.descriptors, 0, null );
                break;

            case EDescUpdateMode::UpdateTemplate :
                dev.vkUpdateDescriptorSetWithTemplateKHR( dev.GetVkDevice(), _dsHandle, _dsLayout->GetUpdateTemplate(), _updDescTempl.tmplData );
                break;

            case EDescUpdateMode::Unknown :
            default_unlikely :
                RETURN_ERR( "unknown update mode" );
        }

        _Reset();
        return true;
    }

/*
=================================================
    _FindUniform
=================================================
*/
    template <EDescriptorType DescType>
    forceinline Tuple< const VDescriptorUpdater::Uniform_t*, const Bytes16u* >
        VDescriptorUpdater::_FindUniform (const UniformName &name) const
    {
        const auto  uniforms    = _dsLayout->GetUniformRange<DescType>();
        const usize count       = uniforms.template Get<0>();
        auto        un_names    = ArrayView<UniformName::Optimized_t>{ uniforms.template Get<1>(), count };
        usize       index       = BinarySearch( un_names, UniformName::Optimized_t{name} );

        if_likely( index < count )
        {
            return Tuple{ uniforms.template Get<2>() + index,
                          uniforms.template Get<3>() ? uniforms.template Get<3>() + index : null };
        }

        #if not AE_OPTIMIZE_IDS
        {
            String  str;
            str << "Can't find uniform '" << name.GetName() << "'\nAvailable uniforms for type " << ToString( DescType, 0 ) << ": ";

            for (auto un : un_names) {
                str << "'" << _resMngr.HashToName( un ) << "', ";
            }
            str.pop_back();
            str.pop_back();
            AE_LOGE( str );
        }
        #endif
        return Default;
    }

/*
=================================================
    _GetArraySize
=================================================
*/
    template <EDescriptorType DescType>
    forceinline uint  VDescriptorUpdater::_GetArraySize (const UniformName &name) const
    {
        auto [un, off] = _FindUniform< DescType >( name );
        return un->arraySize;
    }

    uint  VDescriptorUpdater::ImageCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::StorageImage>( name );
    }

    uint  VDescriptorUpdater::TextureCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::StorageImage>( name );
    }

    uint  VDescriptorUpdater::SamplerCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::Sampler>( name );
    }

    uint  VDescriptorUpdater::BufferCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::UniformBuffer>( name );
    }

    uint  VDescriptorUpdater::TexelBufferCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::UniformTexelBuffer>( name );
    }

    uint  VDescriptorUpdater::RayTracingSceneCount (const UniformName &name) C_NE___
    {
        return _GetArraySize<EDescriptorType::RayTracingScene>( name );
    }

/*
=================================================
    BindImage
=================================================
*/
    bool  VDescriptorUpdater::BindImage (const UniformName &name, ImageViewID image, uint elementIndex) __NE___
    {
        return _BindImages<ImageViewID>( name, {image}, elementIndex );
    }

    bool  VDescriptorUpdater::BindImage  (const UniformName &name, VkImageView image, uint elementIndex) __NE___
    {
        return _BindImages<VkImageView>( name, {image}, elementIndex );
    }

    bool  VDescriptorUpdater::BindImages (const UniformName &name, ArrayView<ImageViewID> images, uint firstIndex) __NE___
    {
        return _BindImages( name, images, firstIndex );
    }

    bool  VDescriptorUpdater::BindImages (const UniformName &name, ArrayView<VkImageView> images, uint firstIndex) __NE___
    {
        return _BindImages( name, images, firstIndex );
    }

    template <typename T>
    bool  VDescriptorUpdater::_BindImages (const UniformName &name, ArrayView<T> images, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::StorageImage>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( AnyEqual( un->type, DT::StorageImage, DT::SampledImage, DT::CombinedImage_ImmutableSampler, DT::SubpassInput ));
        CHECK_ERR( firstIndex + uint(images.size()) <= un->arraySize );

        const VkImageLayout     layout = EResourceState_ToDstImageLayout( un->image.state );
        VkDescriptorImageInfo*  dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkDescriptorImageInfo>( _updDescTempl.tmplData + *off + SizeOf<VkDescriptorImageInfo> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkDescriptorImageInfo >( images.size() );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = un->type == EDescriptorType::StorageImage ?   VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                                  un->type == EDescriptorType::SampledImage ?   VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE :
                                  un->type == EDescriptorType::SubpassInput ?   VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT :
                                  un->type == EDescriptorType::CombinedImage_ImmutableSampler ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :
                                                                                VK_DESCRIPTOR_TYPE_MAX_ENUM;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(images.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pImageInfo      = dst;
        }

        if constexpr( IsSameTypes< T, ImageViewID >)
        {
            for (usize i = 0; i < images.size(); ++i)
            {
                auto*   view = _resMngr.GetResource( images[i] );
                CHECK_ERR( view != null );

                DEBUG_ONLY(
                    auto*   img = _resMngr.GetResource( view->Image() );
                    CHECK_ERR( img != null );

                    const auto& desc        = img->Description();
                    const bool  is_sampled  = un->type == EDescriptorType::SampledImage and un->type == EDescriptorType::CombinedImage_ImmutableSampler;
                    const bool  is_storage  = un->type == EDescriptorType::StorageImage;
                    const bool  is_sp_input = un->type == EDescriptorType::SubpassInput;
                    const auto  img_type    = GetImageType( desc, view->Description() );

                    ASSERT( not is_sampled  or is_sampled  == AllBits( desc.usage, EImageUsage::Sampled ));
                    ASSERT( not is_storage  or is_storage  == AllBits( desc.usage, EImageUsage::Storage ));
                    ASSERT( not is_sp_input or is_sp_input == AllBits( desc.usage, EImageUsage::InputAttachment ));
                    DEV_CHECK_MSG( PipelineCompiler::EImageType_IsCompatible( img_type, un->image.type ),
                        "image view '"s << view->GetDebugName() << "' with type (" << ToString( img_type ) <<
                        ") is not compatible with sampler '" << name.GetName() << "' type (" << ToString( un->image.type ) << ")" );
                )

                dst[i].imageLayout  = layout;
                dst[i].imageView    = view->Handle();
                dst[i].sampler      = Default;
            }
            return true;
        }

        if constexpr( IsSameTypes< T, VkImageView >)
        {
            for (usize i = 0; i < images.size(); ++i)
            {
                CHECK_ERR( images[i] != Default );

                dst[i].imageLayout  = layout;
                dst[i].imageView    = images[i];
                dst[i].sampler      = Default;
            }
            return true;
        }
    }

/*
=================================================
    BindVideoImage
=================================================
*/
    bool  VDescriptorUpdater::BindVideoImage (const UniformName &name, VideoImageID image, uint elementIndex) __NE___
    {
        auto*   res = _resMngr.GetResource( image );
        CHECK_ERR( res != null );

        return BindImage( name, res->GetView(), elementIndex );
    }

/*
=================================================
    BindTexture
=================================================
*/
    bool  VDescriptorUpdater::BindTexture (const UniformName &name, ImageViewID image, const SamplerName &sampler, uint elementIndex) __NE___
    {
        return _BindTextures<ImageViewID>( name, {image}, sampler, elementIndex );
    }

    bool  VDescriptorUpdater::BindTexture  (const UniformName &name, VkImageView image, VkSampler sampler, uint elementIndex) __NE___
    {
        return _BindTextures<VkImageView>( name, {image}, sampler, elementIndex );
    }

    bool  VDescriptorUpdater::BindTextures (const UniformName &name, ArrayView<ImageViewID> images, const SamplerName &sampler, uint firstIndex) __NE___
    {
        return _BindTextures( name, images, sampler, firstIndex );
    }

    bool  VDescriptorUpdater::BindTextures (const UniformName &name, ArrayView<VkImageView> images, VkSampler sampler, uint firstIndex) __NE___
    {
        return _BindTextures( name, images, sampler, firstIndex );
    }

    template <typename T1, typename T2>
    bool  VDescriptorUpdater::_BindTextures (const UniformName &name, ArrayView<T1> images, const T2 &sampler, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::StorageImage>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( un->type == DT::CombinedImage );
        CHECK_ERR( firstIndex + uint(images.size()) <= un->arraySize );

        const VkImageLayout     layout = EResourceState_ToDstImageLayout( un->image.state );
        VkDescriptorImageInfo*  dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkDescriptorImageInfo>( _updDescTempl.tmplData + *off + SizeOf<VkDescriptorImageInfo> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkDescriptorImageInfo >( images.size() );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(images.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pImageInfo      = dst;
        }

        if constexpr( IsSameTypes< T1, ImageViewID > and IsSameTypes< T2, SamplerName >)
        {
            VkSampler   vk_sampler = _resMngr.GetVkSampler( sampler );
            CHECK_ERR( vk_sampler != Default );

            for (usize i = 0; i < images.size(); ++i)
            {
                auto*   view = _resMngr.GetResource( images[i] );
                CHECK_ERR( view != null );

                DEBUG_ONLY(
                    auto*   img = _resMngr.GetResource( view->Image() );
                    CHECK_ERR( img != null );

                    const auto& desc        = img->Description();
                    const auto  img_type    = GetImageType( desc, view->Description() );

                    ASSERT( AllBits( desc.usage, EImageUsage::Sampled ));
                    DEV_CHECK_MSG( PipelineCompiler::EImageType_IsCompatible( img_type, un->image.type ),
                        "image view '"s << view->GetDebugName() << "' with type " << ToString( img_type ) <<
                        " is not compatible with sampler '" << name.GetName() << "' type " << ToString( un->image.type ));
                )

                dst[i].imageLayout  = layout;
                dst[i].imageView    = view->Handle();
                dst[i].sampler      = vk_sampler;
            }
            return true;
        }

        if constexpr( IsSameTypes< T1, VkImageView > and IsSameTypes< T2, VkSampler >)
        {
            CHECK_ERR( sampler != Default );

            for (usize i = 0; i < images.size(); ++i)
            {
                CHECK_ERR( images[i] != Default );

                dst[i].imageLayout  = layout;
                dst[i].imageView    = images[i];
                dst[i].sampler      = sampler;
            }
            return true;
        }
    }

/*
=================================================
    BindSampler
=================================================
*/
    bool  VDescriptorUpdater::BindSampler (const UniformName &name, const SamplerName &sampler, uint elementIndex) __NE___
    {
        return _BindSamplers<SamplerName>( name, {sampler}, elementIndex );
    }

    bool  VDescriptorUpdater::BindSampler  (const UniformName &name, VkSampler sampler, uint elementIndex) __NE___
    {
        return _BindSamplers<VkSampler>( name, {sampler}, elementIndex );
    }

    bool  VDescriptorUpdater::BindSamplers (const UniformName &name, ArrayView<SamplerName> samplers, uint firstIndex) __NE___
    {
        return _BindSamplers( name, samplers, firstIndex );
    }

    bool  VDescriptorUpdater::BindSamplers (const UniformName &name, ArrayView<VkSampler> samplers, uint firstIndex) __NE___
    {
        return _BindSamplers( name, samplers, firstIndex );
    }

    template <typename T>
    bool  VDescriptorUpdater::_BindSamplers (const UniformName &name, ArrayView<T> samplers, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::Sampler>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( un->type == DT::Sampler );
        CHECK_ERR( firstIndex + uint(samplers.size()) <= un->arraySize );

        VkDescriptorImageInfo*  dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkDescriptorImageInfo>( _updDescTempl.tmplData + *off + SizeOf<VkDescriptorImageInfo> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkDescriptorImageInfo >( samplers.size() );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = VK_DESCRIPTOR_TYPE_SAMPLER;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(samplers.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pImageInfo      = dst;
        }

        if constexpr( IsSameTypes< T, SamplerName >)
        {
            for (usize i = 0; i < samplers.size(); ++i)
            {
                VkSampler   vk_samp = _resMngr.GetVkSampler( samplers[i] );
                CHECK_ERR( vk_samp != Default );

                dst[i].imageLayout  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                dst[i].imageView    = Default;
                dst[i].sampler      = vk_samp;
            }
            return true;
        }

        if constexpr( IsSameTypes< T, VkSampler >)
        {
            for (usize i = 0; i < samplers.size(); ++i)
            {
                CHECK_ERR( samplers[i] != Default );

                dst[i].imageLayout  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                dst[i].imageView    = Default;
                dst[i].sampler      = samplers[i];
            }
            return true;
        }
    }

/*
=================================================
    BindBuffer
=================================================
*/
    bool  VDescriptorUpdater::BindBuffer (const UniformName &name, const ShaderStructName &typeName, BufferID buffer, uint elementIndex) __NE___
    {
        return _BindBuffers<BufferID>( name, typeName, {buffer}, elementIndex );
    }

    bool  VDescriptorUpdater::BindBuffer  (const UniformName &name, const ShaderStructName &typeName, VkBuffer buffer, uint elementIndex) __NE___
    {
        return _BindBuffers<VkBuffer>( name, typeName, {buffer}, elementIndex );
    }

    bool  VDescriptorUpdater::BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<BufferID> buffers, uint firstIndex) __NE___
    {
        return _BindBuffers( name, typeName, buffers, firstIndex );
    }

    bool  VDescriptorUpdater::BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<VkBuffer> buffers, uint firstIndex) __NE___
    {
        return _BindBuffers( name, typeName, buffers, firstIndex );
    }

    template <typename T>
    bool  VDescriptorUpdater::_BindBuffers (const UniformName &name, const ShaderStructName &typeName, ArrayView<T> buffers, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::UniformBuffer>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( AnyEqual( un->type, DT::UniformBuffer, DT::StorageBuffer ));
        CHECK_ERR( firstIndex + uint(buffers.size()) <= un->arraySize );
        CHECK_ERR( not un->buffer.HasDynamicOffset() );     // set explicit size
        CHECK_ERR( typeName == Default or typeName == un->buffer.typeName );

        DEBUG_ONLY(
            const bool  is_uniform  = un->type == EDescriptorType::UniformBuffer;
            const bool  is_storage  = un->type == EDescriptorType::StorageBuffer;
            const auto& props       = _resMngr.GetDevice().GetDeviceProperties().res;
            const auto  max_size    = is_uniform ? Bytes{props.maxUniformBufferRange} : UMax;
        )

        const bool              is_dynamic  = (un->buffer.dynamicOffsetIndex != UMax);
        VkDescriptorBufferInfo* dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkDescriptorBufferInfo>( _updDescTempl.tmplData + *off + SizeOf<VkDescriptorBufferInfo> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkDescriptorBufferInfo >( buffers.size() );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = un->type == EDescriptorType::UniformBuffer ? (is_dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) :
                                  un->type == EDescriptorType::StorageBuffer ? (is_dynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) :
                                                                                VK_DESCRIPTOR_TYPE_MAX_ENUM;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(buffers.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pBufferInfo     = dst;
        }

        if constexpr( IsSameTypes< T, BufferID >)
        {
            for (usize i = 0; i < buffers.size(); ++i)
            {
                auto*   buf = _resMngr.GetResource( buffers[i] );
                CHECK_ERR( buf != null );

                DEBUG_ONLY(
                    const auto& desc = buf->Description();
                    ASSERT( desc.size <= max_size );
                    ASSERT( not is_uniform or is_uniform == AllBits( desc.usage, EBufferUsage::Uniform ));
                    ASSERT( not is_storage or is_storage == AllBits( desc.usage, EBufferUsage::Storage ));
                )

                dst[i].buffer   = buf->Handle();
                dst[i].offset   = 0;
                dst[i].range    = VK_WHOLE_SIZE;
            }
            return true;
        }

        if constexpr( IsSameTypes< T, VkBuffer >)
        {
            for (usize i = 0; i < buffers.size(); ++i)
            {
                CHECK_ERR( buffers[i] != Default );

                dst[i].buffer   = buffers[i];
                dst[i].offset   = 0;
                dst[i].range    = VK_WHOLE_SIZE;
            }
            return true;
        }
    }

/*
=================================================
    BindBuffer
=================================================
*/
    template <typename T>
    bool  VDescriptorUpdater::_BindBuffer (const UniformName &name, const ShaderStructName &typeName, T buffer, Bytes bufferOffset, Bytes bufferSize, uint elementIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::UniformBuffer>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( AnyEqual( un->type, DT::UniformBuffer, DT::StorageBuffer ));
        CHECK_ERR( elementIndex + 1 <= un->arraySize );
        CHECK_ERR( typeName == Default or typeName == un->buffer.typeName );

        DEBUG_ONLY(
            const bool  is_uniform  = un->type == EDescriptorType::UniformBuffer;
            const bool  is_storage  = un->type == EDescriptorType::StorageBuffer;
            const auto& props       = _resMngr.GetDevice().GetDeviceProperties().res;
            const auto  min_align   = Bytes{ is_uniform ? props.minUniformBufferOffsetAlign : props.minStorageBufferOffsetAlign };
            const auto  max_size    = is_uniform ? Bytes{props.maxUniformBufferRange} : UMax;
        )

        const bool              is_dynamic  = (un->buffer.dynamicOffsetIndex != UMax);
        VkDescriptorBufferInfo* dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkDescriptorBufferInfo>( _updDescTempl.tmplData + *off + SizeOf<VkDescriptorBufferInfo> * elementIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkDescriptorBufferInfo >( 1 );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = un->type == EDescriptorType::UniformBuffer ? (is_dynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) :
                                  un->type == EDescriptorType::StorageBuffer ? (is_dynamic ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER) :
                                                                                VK_DESCRIPTOR_TYPE_MAX_ENUM;
            wds.dstArrayElement = elementIndex;
            wds.descriptorCount = 1;
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pBufferInfo     = dst;
        }

        ASSERT( IsAligned( bufferOffset, min_align ));
        ASSERT( bufferSize <= max_size );

        if constexpr( IsSameTypes< T, BufferID >)
        {
            auto*   buf = _resMngr.GetResource( buffer );
            CHECK_ERR( buf != null );

            DEBUG_ONLY(
                const auto& desc = buf->Description();
                ASSERT( bufferOffset + bufferSize <= desc.size );
                ASSERT( not is_uniform or is_uniform == AllBits( desc.usage, EBufferUsage::Uniform ));
                ASSERT( not is_storage or is_storage == AllBits( desc.usage, EBufferUsage::Storage ));
            )

            dst->buffer = buf->Handle();
            dst->offset = VkDeviceSize(bufferOffset);
            dst->range  = VkDeviceSize(bufferSize);

            return true;
        }

        if constexpr( IsSameTypes< T, VkBuffer >)
        {
            CHECK_ERR( buffer != Default );

            dst->buffer = buffer;
            dst->offset = VkDeviceSize(bufferOffset);
            dst->range  = VkDeviceSize(bufferSize);

            return true;
        }
    }

    bool  VDescriptorUpdater::BindBuffer (const UniformName &name, const ShaderStructName &typeName, BufferID buffer, Bytes bufferOffset, Bytes bufferSize, uint elementIndex) __NE___
    {
        return _BindBuffer( name, typeName, buffer, bufferOffset, bufferSize, elementIndex );
    }

    bool  VDescriptorUpdater::BindBuffer  (const UniformName &name, const ShaderStructName &typeName, VkBuffer buffer, Bytes bufferOffset, Bytes bufferSize, uint elementIndex) __NE___
    {
        return _BindBuffer( name, typeName, buffer, bufferOffset, bufferSize, elementIndex );
    }

/*
=================================================
    GetBufferStructName
=================================================
*/
    ShaderStructName  VDescriptorUpdater::GetBufferStructName (const UniformName &name) C_NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::UniformBuffer>( name );
        CHECK_ERR( un != null );

        return un->buffer.typeName;
    }

/*
=================================================
    BindTexelBuffer
=================================================
*/
    bool  VDescriptorUpdater::BindTexelBuffer (const UniformName &name, BufferViewID view, uint elementIndex) __NE___
    {
        return _BindTexelBuffers<BufferViewID>( name, {view}, elementIndex );
    }

    bool  VDescriptorUpdater::BindTexelBuffer (const UniformName &name, VkBufferView view, uint elementIndex) __NE___
    {
        return _BindTexelBuffers<VkBufferView>( name, {view}, elementIndex );
    }

    bool  VDescriptorUpdater::BindTexelBuffers (const UniformName &name, ArrayView<BufferViewID> views, uint firstIndex) __NE___
    {
        return _BindTexelBuffers( name, views, firstIndex );
    }

    bool  VDescriptorUpdater::BindTexelBuffers (const UniformName &name, ArrayView<VkBufferView> views, uint firstIndex) __NE___
    {
        return _BindTexelBuffers( name, views, firstIndex );
    }

    template <typename T>
    bool  VDescriptorUpdater::_BindTexelBuffers (const UniformName &name, ArrayView<T> views, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::UniformTexelBuffer>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( AnyEqual( un->type, DT::UniformTexelBuffer, DT::StorageTexelBuffer ));
        CHECK_ERR( firstIndex + uint(views.size()) <= un->arraySize );

        VkBufferView*   dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkBufferView>( _updDescTempl.tmplData + *off + SizeOf<VkBufferView> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkBufferView >( views.size() );

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.descriptorType  = un->type == EDescriptorType::UniformTexelBuffer ? VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER :
                                  un->type == EDescriptorType::StorageTexelBuffer ? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER :
                                                                                    VK_DESCRIPTOR_TYPE_MAX_ENUM;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(views.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
            wds.pTexelBufferView= dst;
        }

        if constexpr( IsSameTypes< T, BufferViewID >)
        {
            for (usize i = 0; i < views.size(); ++i)
            {
                auto*   view = _resMngr.GetResource( views[i] );
                CHECK_ERR( view != null );

                DEBUG_ONLY(
                    auto*   buf = _resMngr.GetResource( view->Buffer() );
                    CHECK_ERR( buf != null );

                    const auto& desc        = buf->Description();
                    const bool  is_uniform  = un->type == EDescriptorType::UniformTexelBuffer;
                    const bool  is_storage  = un->type == EDescriptorType::StorageTexelBuffer;
                    const auto  img_type    = GetImageType( view->Description() );

                    ASSERT( not is_uniform or is_uniform == AllBits( desc.usage, EBufferUsage::UniformTexel ));
                    ASSERT( not is_storage or is_storage == AllBits( desc.usage, EBufferUsage::StorageTexel ));

                    DEV_CHECK_MSG( PipelineCompiler::EImageType_IsCompatible( img_type, un->texelBuffer.type ),
                        "buffer view '"s << view->GetDebugName() << "' with type " << ToString( img_type ) <<
                        " is not compatible with sampler '" << name.GetName() << "' type " << ToString( un->texelBuffer.type ));
                )

                dst[i] = view->Handle();
            }
            return true;
        }

        if constexpr( IsSameTypes< T, VkBufferView >)
        {
            for (usize i = 0; i < views.size(); ++i)
            {
                CHECK_ERR( views[i] != Default );

                dst[i] = views[i];
            }
            return true;
        }
    }

/*
=================================================
    BindRayTracingScene
=================================================
*/
    bool  VDescriptorUpdater::BindRayTracingScene (const UniformName &name, RTSceneID scene, uint elementIndex) __NE___
    {
        return _BindRayTracingScenes<RTSceneID>( name, {scene}, elementIndex );
    }

    bool  VDescriptorUpdater::BindRayTracingScene (const UniformName &name, VkAccelerationStructureKHR scene, uint elementIndex) __NE___
    {
        return _BindRayTracingScenes<VkAccelerationStructureKHR>( name, {scene}, elementIndex );
    }

    bool  VDescriptorUpdater::BindRayTracingScenes (const UniformName &name, ArrayView<RTSceneID> scenes, uint firstIndex) __NE___
    {
        return _BindRayTracingScenes( name, scenes, firstIndex );
    }

    bool  VDescriptorUpdater::BindRayTracingScenes (const UniformName &name, ArrayView<VkAccelerationStructureKHR> scenes, uint firstIndex) __NE___
    {
        return _BindRayTracingScenes( name, scenes, firstIndex );
    }

    template <typename T>
    bool  VDescriptorUpdater::_BindRayTracingScenes (const UniformName &name, ArrayView<T> scenes, uint firstIndex)
    {
        DRC_SHAREDLOCK( _drCheck );

        auto [un, off] = _FindUniform<EDescriptorType::RayTracingScene>( name );

        CHECK_ERR( un != null );
        CHECK_ERR( un->type == DT::RayTracingScene );
        CHECK_ERR( firstIndex + uint(scenes.size()) <= un->arraySize );

        VkAccelerationStructureKHR*     dst;

        if ( _UseUpdateTemplate() )
        {
            dst = Cast<VkAccelerationStructureKHR>( _updDescTempl.tmplData + *off + SizeOf<VkAccelerationStructureKHR> * firstIndex );
        }
        else
        {
            if_unlikely( _updDesc.index+1 >= _updDesc.count )
                _Flush();

            dst = _allocator.Allocate< VkAccelerationStructureKHR >( scenes.size() );

            auto&   as_wds = *_allocator.Allocate< VkWriteDescriptorSetAccelerationStructureKHR >(1);
            as_wds.sType                        = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
            as_wds.pNext                        = null;
            as_wds.accelerationStructureCount   = uint(scenes.size());
            as_wds.pAccelerationStructures      = dst;

            VkWriteDescriptorSet&   wds = _updDesc.descriptors[ _updDesc.index++ ];
            wds = {};
            wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            wds.pNext           = &as_wds;
            wds.descriptorType  = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
            wds.dstArrayElement = firstIndex;
            wds.descriptorCount = uint(scenes.size());
            wds.dstBinding      = un->binding.vkIndex;
            wds.dstSet          = _dsHandle;
        }

        if constexpr( IsSameTypes< T, RTSceneID >)
        {
            for (usize i = 0; i < scenes.size(); ++i)
            {
                auto*   as = _resMngr.GetResource( scenes[i] );
                CHECK_ERR( as != null );

                dst[i] = as->Handle();
            }
            return true;
        }

        if constexpr( IsSameTypes< T, VkAccelerationStructureKHR >)
        {
            for (usize i = 0; i < scenes.size(); ++i)
            {
                CHECK_ERR( scenes[i] != Default );

                dst[i] = scenes[i];
            }
            return true;
        }
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

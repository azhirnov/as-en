// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VSampler.h"
# include "graphics/Vulkan/VResourceManager.h"
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics
{

/*
=================================================
    destructor
=================================================
*/
    VSampler::~VSampler () __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK( _sampler == Default );
        CHECK( _ycbcrConversion == Default );
    }

/*
=================================================
    Create
=================================================
*/
    bool  VSampler::Create (const VResourceManager &resMngr, const VkSamplerCreateInfo &ci, const VkSamplerYcbcrConversionCreateInfo* convCI, StringView dbgName) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( _sampler == Default );
        CHECK_ERR( _ycbcrConversion == Default );

        VkSamplerCreateInfo             sampler_ci  = ci;
        VkSamplerYcbcrConversionInfo    conv_info   = {};

        auto&   dev = resMngr.GetDevice();
        GRES_CHECK( IsSupported( dev, sampler_ci ));

        if ( convCI != null )
        {
            CHECK_ERR( resMngr.GetFeatureSet().samplerYcbcrConversion == EFeature::RequireTrue );
            GRES_CHECK( IsSupported( dev, sampler_ci, *convCI ));

            VK_CHECK_ERR( dev.vkCreateSamplerYcbcrConversionKHR( dev.GetVkDevice(), convCI, null, OUT &_ycbcrConversion ));
            dev.SetObjectName( _ycbcrConversion, dbgName, VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION );

            sampler_ci.pNext        = &conv_info;

            conv_info.sType         = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO;
            conv_info.conversion    = _ycbcrConversion;

            _ycbcrFormat            = convCI->format;
        }

        VK_CHECK_ERR( dev.vkCreateSampler( dev.GetVkDevice(), &sampler_ci, null, OUT &_sampler ));
        dev.SetObjectName( _sampler, dbgName, VK_OBJECT_TYPE_SAMPLER );

        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VSampler::Destroy (VResourceManager &resMngr) __NE___
    {
        DRC_EXLOCK( _drCheck );

        auto&   dev = resMngr.GetDevice();

        if ( _ycbcrConversion != Default )
            dev.vkDestroySamplerYcbcrConversionKHR( dev.GetVkDevice(), _ycbcrConversion, null );

        if ( _sampler != Default )
            dev.vkDestroySampler( dev.GetVkDevice(), _sampler, null );

        _sampler            = Default;
        _ycbcrConversion    = Default;
        _ycbcrFormat        = VK_FORMAT_UNDEFINED;
    }

/*
=================================================
    ConvertSampler (SamplerDesc)
=================================================
*/
    void  VSampler::ConvertSampler (const SamplerDesc &desc, OUT VkSamplerCreateInfo &info) __NE___
    {
        info.sType              = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.pNext              = null;
        info.flags              = VEnumCast( desc.usage );
        info.magFilter          = VEnumCast( desc.magFilter );
        info.minFilter          = VEnumCast( desc.minFilter );
        info.mipmapMode         = VEnumCast( desc.mipmapMode );
        info.addressModeU       = VEnumCast( desc.addressMode.x );
        info.addressModeV       = VEnumCast( desc.addressMode.y );
        info.addressModeW       = VEnumCast( desc.addressMode.z );
        info.mipLodBias         = desc.mipLodBias;
        info.anisotropyEnable   = desc.maxAnisotropy.has_value() ? VK_TRUE : VK_FALSE;
        info.maxAnisotropy      = desc.maxAnisotropy.value_or( 0.0f );
        info.compareEnable      = desc.compareOp.has_value() ? VK_TRUE : VK_FALSE;
        info.compareOp          = VEnumCast( desc.compareOp.value_or( ECompareOp::Always ));
        info.minLod             = desc.minLod;
        info.maxLod             = desc.maxLod;
        info.borderColor        = VEnumCast( desc.borderColor );
        info.unnormalizedCoordinates= desc.unnormalizedCoordinates ? VK_TRUE : VK_FALSE;
    }

/*
=================================================
    ConvertSampler (SamplerYcbcrConversionDesc)
=================================================
*/
    void  VSampler::ConvertSampler (const SamplerYcbcrConversionDesc &desc, OUT VkSamplerYcbcrConversionCreateInfo &info) __NE___
    {
        info.sType          = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO;
        info.pNext          = null;
        info.format         = VEnumCast( desc.format );
        info.ycbcrModel     = VEnumCast( desc.ycbcrModel );
        info.ycbcrRange     = VEnumCast( desc.ycbcrRange );
        info.components     = VEnumCast( desc.components );
        info.xChromaOffset  = VEnumCast( desc.xChromaOffset );
        info.yChromaOffset  = VEnumCast( desc.yChromaOffset );
        info.chromaFilter   = VEnumCast( desc.chromaFilter );
        info.forceExplicitReconstruction = desc.forceExplicitReconstruction ? VK_TRUE : VK_FALSE;
    }

/*
=================================================
    IsSupported (SamplerDesc)
=================================================
*/
    bool  VSampler::IsSupported (const VDevice &, const VkSamplerCreateInfo &) __NE___
    {
        // TODO
        return true;
    }

/*
=================================================
    IsSupported (SamplerYcbcrConversionDesc)
=================================================
*/
    bool  VSampler::IsSupported (const VDevice &dev, const VkSamplerCreateInfo &desc, const VkSamplerYcbcrConversionCreateInfo &ycbcrDesc) __NE___
    {
        VkFormatProperties  props = {};
        vkGetPhysicalDeviceFormatProperties( dev.GetVkPhysicalDevice(), ycbcrDesc.format, OUT &props );

        VkFormatFeatureFlags        required    = 0;
        const VkFormatFeatureFlags  available   = props.optimalTilingFeatures;

        if ( ycbcrDesc.chromaFilter != VK_FILTER_NEAREST )
            required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_LINEAR_FILTER_BIT;

        if ( ycbcrDesc.xChromaOffset == VK_CHROMA_LOCATION_MIDPOINT or
             ycbcrDesc.yChromaOffset == VK_CHROMA_LOCATION_MIDPOINT )
            required |= VK_FORMAT_FEATURE_MIDPOINT_CHROMA_SAMPLES_BIT;

        if ( ycbcrDesc.xChromaOffset == VK_CHROMA_LOCATION_COSITED_EVEN or
             ycbcrDesc.yChromaOffset == VK_CHROMA_LOCATION_COSITED_EVEN )
            required |= VK_FORMAT_FEATURE_COSITED_CHROMA_SAMPLES_BIT;

        if ( ycbcrDesc.chromaFilter != desc.minFilter or
             ycbcrDesc.chromaFilter != desc.magFilter )
            required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_SEPARATE_RECONSTRUCTION_FILTER_BIT;

        if ( ycbcrDesc.forceExplicitReconstruction )
            required |= VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_YCBCR_CONVERSION_CHROMA_RECONSTRUCTION_EXPLICIT_FORCEABLE_BIT;

        return AllBits( available, required );
    }


} // AE::Graphics

#endif // AE_ENABLE_VULKAN

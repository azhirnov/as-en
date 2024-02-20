// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/ImageUtils.h"
#include "graphics/Private/EnumUtils.h"

namespace AE::Graphics
{

/*
=================================================
    ImageDesc::SetDimension
=================================================
*/
    ImageDesc&  ImageDesc::SetDimension (const uint value) __NE___
    {
        dimension   = uint3{ value, 1, 1 };
        imageDim    = (imageDim == Default ? EImageDim_1D : imageDim);
        return *this;
    }

    ImageDesc&  ImageDesc::SetDimension (const uint2 &value) __NE___
    {
        dimension   = uint3{ value, 1 };
        imageDim    = (imageDim == Default ? EImageDim_2D : imageDim);
        return *this;
    }

    ImageDesc&  ImageDesc::SetDimension (const uint3 &value) __NE___
    {
        dimension   = value;
        imageDim    = (imageDim == Default ? EImageDim_3D : imageDim);
        return *this;
    }

/*
=================================================
    ImageDesc::SetType
=================================================
*/
    ImageDesc&  ImageDesc::SetType (EImage value) __NE___
    {
        switch_enum( value )
        {
            case EImage_1D :
            case EImage_1DArray :       imageDim = EImageDim_1D;    break;
            case EImage_2D :
            case EImage_2DArray :
            case EImage_Cube :
            case EImage_CubeArray :     imageDim = EImageDim_2D;    break;
            case EImage_3D :            imageDim = EImageDim_3D;    break;

            case EImage::Unknown :
            case EImage::_Count :
            default_unlikely :          DBG_WARNING( "unknown image type" );    break;
        }
        switch_end
        return *this;
    }

/*
=================================================
    ImageDesc::ViewFormatListSize
=================================================
*/
    usize  ImageDesc::ViewFormatListSize () C_NE___
    {
        usize   size = 0;
        for (auto& dst : viewFormats)
            size += (dst != Default);
        return size;
    }

/*
=================================================
    ImageDesc::AddViewFormat
=================================================
*/
    ImageDesc&  ImageDesc::AddViewFormat (EPixelFormat value) __NE___
    {
        for (auto& dst : viewFormats)
        {
            if ( dst == Default )
            {
                dst = value;
                return *this;
            }
        }

        DBG_WARNING( "'viewFormats' overflow" );
        return *this;
    }

/*
=================================================
    Validate
=================================================
*/
    void  ImageDesc::Validate () __NE___
    {
        ASSERT( format != Default );
        ASSERT( imageDim != Default );

        dimension   = Max( dimension, uint3{1} );
        arrayLayers = Max( arrayLayers, 1_layer );

        switch_enum( imageDim )
        {
            case EImageDim_1D :
                ASSERT( not samples.IsEnabled() );
                ASSERT( dimension.y == 1 and dimension.z == 1 );
                ASSERT( not AnyBits( options, EImageOpt::Array2DCompatible | EImageOpt::CubeCompatible ));  // this options are not supported for 1D

                options     &= ~(EImageOpt::Array2DCompatible | EImageOpt::CubeCompatible);
                samples     = 1_samples;
                dimension   = uint3{ dimension.x, 1, 1 };
                break;

            case EImageDim_2D :
                ASSERT( dimension.z == 1 );
                dimension.z = 1;

                if ( AllBits( options, EImageOpt::CubeCompatible ) and not IsMultipleOf( arrayLayers.Get(), 6 ))
                    options &= ~EImageOpt::CubeCompatible;

                if ( AllBits( options, EImageOpt::CubeCompatible ))
                {
                    ASSERT( not samples.IsEnabled() );
                    ASSERT( dimension.x == dimension.y );

                    samples     = 1_samples;
                    dimension.x = dimension.y = Min( dimension.x, dimension.y );
                }
                break;

            case EImageDim_3D :
                ASSERT( not samples.IsEnabled() );
                ASSERT( arrayLayers == 1_layer );
                ASSERT( not AnyBits( options, EImageOpt::CubeCompatible )); // options are not supported for 1D

                options     &= ~EImageOpt::CubeCompatible;
                samples     = 1_samples;
                arrayLayers = 1_layer;
                break;

            case EImageDim::Unknown :
            default_unlikely:
                DBG_WARNING( "unknown image dimension type" );
                break;
        }
        switch_end

        if ( memType == Default )
            memType = EMemoryType::DeviceLocal;

        if ( usage == Default )
            usage = EImageUsage::Transfer | EImageUsage::Sampled;

        if ( not AllBits( memType, EMemoryType::DeviceLocal ))
        {
            options &= ~(EImageOpt::SparseResidencyAliased);
            usage   &= ~(EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment | EImageUsage::Sampled |
                         EImageUsage::ShadingRate | EImageUsage::InputAttachment | EImageUsage::Storage);
        }

        if ( AllBits( memType, EMemoryType::Transient ))
        {
            memType &= ~(EMemoryType::HostCachedCoherent | EMemoryType::DeviceLocal);
            usage   &= (EImageUsage::ColorAttachment | EImageUsage::DepthStencilAttachment | EImageUsage::InputAttachment);
        }

        if ( not AllBits( usage, EImageUsage::ColorAttachment ))
            options &= ~EImageOpt::ColorAttachmentBlend;

        if ( not AllBits( usage, EImageUsage::Storage ))
            options &= ~(EImageOpt::StorageAtomic | EImageOpt::VertexPplnStore | EImageOpt::FragmentPplnStore);

        if ( not AllBits( usage, EImageUsage::Sampled ))
            options &= ~(EImageOpt::SampledLinear | EImageOpt::SampledMinMax);

        if ( not AllBits( usage, EImageUsage::DepthStencilAttachment ))
            options &= ~EImageOpt::SampleLocationsCompatible;

        // validate samples and mipmaps
        if ( samples.IsEnabled() )
        {
            ASSERT( maxLevel <= 1_mipmap );
            maxLevel = 1_mipmap;
        }
        else
        {
            samples  = 1_samples;
            maxLevel = MipmapLevel( Clamp( maxLevel.Get(), 1u, ImageUtils::NumberOfMipmaps( dimension )));
        }

        /*if ( ViewFormatListSize() > 1 )
        {
            if ( not AllBits( options, EImageOpt::MutableFormat ))
            {
                DBG_WARNING( "only 1 format can be set in 'viewFormats'" );
                viewFormats.resize( 1 );
            }
        }*/

        ASSERT( usage != Default );
    }

/*
=================================================
    Validate
=================================================
*/
    bool  ImageDesc::operator == (const ImageDesc &rhs) C_NE___
    {
        return  (All( dimension == rhs.dimension )) and
                (arrayLayers    == rhs.arrayLayers) and
                (maxLevel       == rhs.maxLevel)    and
                (imageDim       == rhs.imageDim)    and
                (options        == rhs.options)     and
                (usage          == rhs.usage)       and
                (format         == rhs.format)      and
                (samples        == rhs.samples)     and
                (memType        == rhs.memType)     and
                (queues         == rhs.queues);
    }

/*
=================================================
    Create*
=================================================
*/
    ImageDesc  ImageDesc::CreateColorAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers) __NE___
    {
        return ImageDesc{}
            .SetDimension( dim )
            .SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Transfer )
            .SetFormat( fmt )
            .SetArrayLayers( layers.Get() );
    }

    ImageDesc  ImageDesc::CreateDepthAttachment (const uint2 &dim, EPixelFormat fmt, ImageLayer layers) __NE___
    {
        return ImageDesc{}
            .SetDimension( dim )
            .SetUsage( EImageUsage::DepthStencilAttachment )
            .SetFormat( fmt )
            .SetArrayLayers( layers.Get() );
    }

    ImageDesc  ImageDesc::CreateShadingRate (const uint2 &dim) __NE___
    {
        return ImageDesc{}
            .SetDimension( dim )
            .SetUsage( EImageUsage::ShadingRate | EImageUsage::Transfer | EImageUsage::Storage )
            .SetFormat( EPixelFormat::R8U );
    }

    ImageDesc  ImageDesc::CreateStaging (const uint2 &dim, EPixelFormat fmt) __NE___
    {
        return ImageDesc{}
            .SetDimension( dim )
            .SetUsage( EImageUsage::Transfer )
            .SetFormat( fmt )
            .SetMemory( EMemoryType::HostCoherent );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    ImageViewDesc
=================================================
*/
    ImageViewDesc::ImageViewDesc (EImage            viewType,
                                  EPixelFormat      format,
                                  MipmapLevel       baseMipmap,
                                  uint              mipmapCount,
                                  ImageLayer        baseLayer,
                                  uint              layerCount,
                                  ImageSwizzle      swizzle,
                                  EImageAspect      aspectMask) __NE___ :
        viewType{ viewType },       format{ format },
        aspectMask{ aspectMask },
        baseMipmap{ baseMipmap },   mipmapCount{ ushort(mipmapCount) },
        baseLayer{ baseLayer },     layerCount{ ushort(layerCount) },
        swizzle{ swizzle }
    {
        ASSERT( this->mipmapCount == mipmapCount or mipmapCount == UMax );
        ASSERT( this->layerCount == layerCount or layerCount == UMax );
    }

/*
=================================================
    ImageViewDesc
=================================================
*/
    ImageViewDesc::ImageViewDesc (const ImageDesc &desc) __NE___ :
        format{ desc.format },      aspectMask{ EPixelFormat_ToImageAspect( format )},
        baseMipmap{},               mipmapCount{ ushort(desc.maxLevel.Get()) },
        baseLayer{},                layerCount{ ushort(desc.arrayLayers.Get()) },
        swizzle{ "RGBA"_swizzle }
    {}

/*
=================================================
    ImageViewDesc::Validate
=================================================
*/
    void ImageViewDesc::Validate (const ImageDesc &desc) __NE___
    {
        baseMipmap  = MipmapLevel{Clamp( baseMipmap.Get(), 0u, desc.maxLevel.Get()-1 )};
        mipmapCount = CheckCast<ushort>( Clamp( mipmapCount, 1u, desc.maxLevel.Get() - baseMipmap.Get() ));

        // validate format
        if ( format == Default )
        {
            format = desc.format;
        }
        else
        if ( format != desc.format )
        {
            if ( not ArrayContains( ArrayView<EPixelFormat>{desc.viewFormats}, format ) and
                 not AllBits( desc.options, EImageOpt::MutableFormat ))
            {
                DBG_WARNING( "can't change format if 'MutableFormat' is not set and 'viewFormats' does not contains this format" );
                format = desc.format;
            }
        }

        // validate aspect mask
        EImageAspect    mask = EPixelFormat_ToImageAspect( format );
        aspectMask           = (aspectMask == Default ? mask : (aspectMask & mask));
        ASSERT( aspectMask != Default );


        // choose view type
        if ( viewType == Default )
        {
            const uint  max_layers  = desc.arrayLayers.Get();

            baseLayer   = ImageLayer{Clamp( baseLayer.Get(), 0u, max_layers-1 )};
            layerCount  = CheckCast<ushort>( Clamp( layerCount, 1u, max_layers - baseLayer.Get() ));

            switch_enum( desc.imageDim )
            {
                case EImageDim_1D :
                    if ( layerCount > 1 )
                        viewType = EImage_1DArray;
                    else
                        viewType = EImage_1D;
                    break;

                case EImageDim_2D :
                    if ( layerCount > 6 and AllBits( desc.options, EImageOpt::CubeCompatible ))
                        viewType = EImage_CubeArray;
                    else
                    if ( layerCount == 6 and AllBits( desc.options, EImageOpt::CubeCompatible ))
                        viewType = EImage_Cube;
                    else
                    if ( layerCount > 1 )
                        viewType = EImage_2DArray;
                    else
                        viewType = EImage_2D;
                    break;

                case EImageDim_3D :
                    viewType = EImage_3D;
                    break;

                case EImageDim::Unknown :
                    break;
            }
            switch_end
        }
        else
        // validate view type
        {
            const uint  max_layers  = (desc.imageDim == EImageDim_3D and viewType != EImage_3D ? desc.dimension.z : desc.arrayLayers.Get());

            baseLayer = ImageLayer{Clamp( baseLayer.Get(), 0u, max_layers-1 )};

            switch_enum( viewType )
            {
                case EImage_1D :
                    ASSERT( desc.imageDim == EImageDim_1D );
                    ASSERT( layerCount == UMax or layerCount == 1 );
                    layerCount = 1;
                    break;

                case EImage_1DArray :
                    ASSERT( desc.imageDim == EImageDim_1D );
                    layerCount = CheckCast<ushort>( Clamp( layerCount, 1u, max_layers - baseLayer.Get() ));
                    break;

                case EImage_2D :
                    ASSERT( desc.imageDim == EImageDim_2D or
                            (desc.imageDim == EImageDim_3D and AllBits( desc.options, EImageOpt::Array2DCompatible )));
                    ASSERT( layerCount == UMax or layerCount == 1 );
                    layerCount = 1;
                    break;

                case EImage_2DArray :
                    ASSERT( desc.imageDim == EImageDim_2D or
                            (desc.imageDim == EImageDim_3D and AllBits( desc.options, EImageOpt::Array2DCompatible )));
                    layerCount = CheckCast<ushort>( Clamp( layerCount, 1u, max_layers - baseLayer.Get() ));
                    break;

                case EImage_Cube :
                    ASSERT( desc.imageDim == EImageDim_2D or
                            (desc.imageDim == EImageDim_3D and AllBits( desc.options, EImageOpt::Array2DCompatible )));
                    ASSERT( AllBits( desc.options, EImageOpt::CubeCompatible ));
                    ASSERT( layerCount == UMax or layerCount == 6 );
                    layerCount = 6;
                    break;

                case EImage_CubeArray :
                    ASSERT( desc.imageDim == EImageDim_2D or
                            (desc.imageDim == EImageDim_3D and AllBits( desc.options, EImageOpt::Array2DCompatible )));
                    ASSERT( AllBits( desc.options, EImageOpt::CubeCompatible ));
                    ASSERT( layerCount == UMax or IsMultipleOf( layerCount, 6 ));
                    layerCount = CheckCast<ushort>( Max( 1u, ((max_layers - baseLayer.Get()) / 6) ) * 6 );
                    break;

                case EImage_3D :
                    ASSERT( desc.imageDim == EImageDim_3D );
                    ASSERT( layerCount == UMax or layerCount == 1 );
                    layerCount = 1;
                    break;

                case EImage::Unknown :
                case EImage::_Count :
                default_unlikely :
                    DBG_WARNING( "unknown image view type" );
                    break;
            }
            switch_end
        }
    }

/*
=================================================
    ImageViewDesc::operator ==
=================================================
*/
    bool ImageViewDesc::operator == (const ImageViewDesc &rhs) C_NE___
    {
        return  (this->viewType     == rhs.viewType)    and
                (this->format       == rhs.format)      and
                (this->baseMipmap   == rhs.baseMipmap)  and
                (this->mipmapCount  == rhs.mipmapCount) and
                (this->baseLayer    == rhs.baseLayer)   and
                (this->layerCount   == rhs.layerCount)  and
                (this->aspectMask   == rhs.aspectMask)  and
                (this->swizzle      == rhs.swizzle);
    }

} // AE::Graphics

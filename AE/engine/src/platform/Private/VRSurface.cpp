// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/VRSurface.h"

namespace AE::App
{
    using namespace AE::Graphics;

/*
=================================================
    constructor
=================================================
*/
    VRSurface::VRSurface () __NE___
    {
    }

/*
=================================================
    destructor
=================================================
*/
    VRSurface::~VRSurface () __NE___
    {}

/*
=================================================
    Create
=================================================
*/
    bool  VRSurface::Create (const VRImageDesc &desc) __NE___
    {
        CHECK_ERR( desc.format != Default );

        EXLOCK( _guard );

        Destroy();

        auto&           res_mngr = RenderTaskScheduler().GetResourceManager();
        ImageDesc       img_desc;
        ImageViewDesc   view_desc;

        img_desc.dimension      = uint3{ desc.dimension, 1u };
        img_desc.imageDim       = EImageDim_2D;
        img_desc.usage          = desc.usage;
        img_desc.options        = desc.options;
        img_desc.samples        = desc.samples;
        img_desc.format         = desc.format;

        view_desc.viewType      = EImage_2D;
        view_desc.aspectMask    = EImageAspect::Color;

        for (usize i = 0; i < _images.size(); ++i)
        {
            _images[i] = res_mngr.CreateImage( img_desc, "VR image-" + ToString(i) );
            CHECK_ERR( _images[i] );

            _views[i] = res_mngr.CreateImageView( view_desc, _images[i], "VR image-"  + ToString(i) + " view" );
            CHECK_ERR( _views[i] );
        }

        _desc = desc;

        return true;
    }

/*
=================================================
    Destroy
=================================================
*/
    void  VRSurface::Destroy () __NE___
    {
        EXLOCK( _guard );

        CHECK( RenderTaskScheduler().WaitAll() );

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        for (usize i = 0; i < _images.size(); ++i)
        {
            res_mngr.ReleaseResource( _images[i] );
            res_mngr.ReleaseResource( _views[i] );

            _images[i]  = Default;
            _views[i]   = Default;
        }
    }

/*
=================================================
    IsInitialized
=================================================
*/
    bool  VRSurface::IsInitialized () C_NE___
    {
        EXLOCK( _guard );
        return _images[0].IsValid();
    }

/*
=================================================
    GetRenderPassInfo
=================================================
*/
    IOutputSurface::RenderPassInfo  VRSurface::GetRenderPassInfo () C_NE___
    {
        RenderPassInfo::Attachment  att;
        att.format      = _desc.format;
        att.samples     = _desc.samples;

        RenderPassInfo  result;
        result.attachments.push_back( att );
        return result;
    }

/*
=================================================
    GetTargets
=================================================
*/
    bool  VRSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
    {
        targets.clear();

        EXLOCK( _guard );

        targets.resize( _images.size() );

        for (usize i = 0; i < _images.size(); ++i)
        {
            auto&   dst = targets[i];

            dst.imageId         = _images[i];
            dst.viewId          = _views[i];
            dst.region          = RectI{ int2{0}, int2(_desc.dimension) };
            dst.layer           = 0_layer;  // TODO: supports 2D array
            dst.pixToMm         = float2{1.0f};
            dst.initialState    = EResourceState::ColorAttachment;
            dst.finalState      = EResourceState::ColorAttachment;
            dst.format          = _desc.format;
            dst.colorSpace      = _colorSpace;
            dst.projection      = null;
        }
        return true;
    }

/*
=================================================
    GetTargetSizes
=================================================
*/
    IOutputSurface::TargetSizes_t  VRSurface::GetTargetSizes () C_NE___
    {
        TargetSizes_t   result;
        for (usize i = 0; i < _images.size(); ++i) {
            result.push_back( _desc.dimension );
        }
        return result;
    }


} // AE::App


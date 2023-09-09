// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/GraphicsHL.pch.h"

namespace AE::Graphics
{

    //
    // Surface Dimensions
    //

    class SurfaceDimensions
    {
    // variables
    private:
        // TODO: minimize
        float2      _mmToViewport;
        float2      _mmToPixels;
        float2      _pixelsToMm;
        float2      _unormPxToMm;
        float2      _invSurfaceSize;


    // methods
    public:
        SurfaceDimensions ()                                                            __NE___;

        void  SetDimensions (const uint2 &surfaceSizeInPix, const float2 &mmPerPixel)   __NE___;
        void  SetDimensions (const App::IOutputSurface::RenderTarget &rt)               __NE___;
        void  CopyDimensions (const SurfaceDimensions &)                                __NE___;

        // mm -> viewport
        ND_ float2  MmToViewport (const float2 &mm)                                     C_NE___;
        ND_ RectF   MmToViewport (const RectF &mm)                                      C_NE___;
        ND_ float2  MmSizeToViewport (const float2 &mm)                                 C_NE___;
        ND_ RectF   MmSizeToViewport (const RectF &mm)                                  C_NE___;

        ND_ float2  AlignMmToPixel (const float2 &mm)                                   C_NE___;
        ND_ RectF   AlignMmToPixel (const RectF &mm)                                    C_NE___;

        // norm/viewport -> mm
    //  ND_ float2  UNormToMm (const float2 &unorm)                                     C_NE___;
    //  ND_ float2  UNormToMm (const float2 &unorm)                                     C_NE___;

        // mm -> pixels
        ND_ float2  MmToPixels (const float2 &mm)                                       C_NE___;
        ND_ RectF   MmToPixels (const RectF &mm)                                        C_NE___;

        // pixels -> viewport
        ND_ float2  PixelsToViewport (const float2 &pix)                                C_NE___;
        ND_ RectF   PixelsToViewport (const RectF &pix)                                 C_NE___;

        // viewport -> pixels
        ND_ RectF   ViewportToPixels (const RectF &vp)                                  C_NE___;        // slow

        ND_ float2  PixelsToMm (const float2 &pix)                                      C_NE___;
        ND_ float2  PixelsToUNorm (const float2 &pix)                                   C_NE___;
        ND_ float2  UNormPixelsToMm (const float2 &unorm)                               C_NE___;

        ND_ float2  GetMmToPixels ()                                                    C_NE___ { return _mmToPixels; }
        ND_ float2  GetPixelsToMm ()                                                    C_NE___ { return 1.f / _mmToPixels; }           // TODO: optimize
        ND_ float2  GetMmToViewport ()                                                  C_NE___ { return _mmToViewport; }
        ND_ float2  GetPixelsToViewport ()                                              C_NE___ { return _pixelsToMm * _mmToViewport; }
        ND_ float2  GetInvSurfaceSize ()                                                C_NE___ { return _invSurfaceSize; }
        ND_ float2  GetSurfaceSize ()                                                   C_NE___ { return 1.f / _invSurfaceSize; }       // TODO: optimize
    };


/*
=================================================
    constructor
=================================================
*/
    inline SurfaceDimensions::SurfaceDimensions () __NE___ :
        _mmToPixels{1.0f},      _pixelsToMm{1.0f}
    {}

/*
=================================================
    SetDimensions
=================================================
*/
    inline void  SurfaceDimensions::SetDimensions (const uint2 &surfaceSizeInPix, const float2 &mmPerPixel) __NE___
    {
        _invSurfaceSize     = 1.0f / float2(surfaceSizeInPix);
        _mmToPixels         = mmPerPixel;
        _pixelsToMm         = 1.0f / mmPerPixel;
        _mmToViewport       = 2.0f * _invSurfaceSize * mmPerPixel;
        _unormPxToMm        = _invSurfaceSize * _pixelsToMm;
    }

    inline void  SurfaceDimensions::SetDimensions (const App::IOutputSurface::RenderTarget &rt) __NE___
    {
        return SetDimensions( rt.RegionSize(), rt.pixToMm );
    }

/*
=================================================
    CopyDimensions
=================================================
*/
    inline void  SurfaceDimensions::CopyDimensions (const SurfaceDimensions &other) __NE___
    {
        this->_mmToViewport     = other._mmToViewport;
        this->_mmToPixels       = other._mmToPixels;
        this->_pixelsToMm       = other._pixelsToMm;
        this->_unormPxToMm      = other._unormPxToMm;
        this->_invSurfaceSize   = other._invSurfaceSize;
    }

/*
=================================================
    MmToViewport
=================================================
*/
    inline float2  SurfaceDimensions::MmToViewport (const float2 &mm) C_NE___
    {
        return float2{ mm.x * _mmToViewport.x - 1.0f,
                       mm.y * _mmToViewport.y - 1.0f };
    }

    inline RectF  SurfaceDimensions::MmToViewport (const RectF &mm) C_NE___
    {
        return RectF{ mm.left   * _mmToViewport.x - 1.0f,
                      mm.top    * _mmToViewport.y - 1.0f,
                      mm.right  * _mmToViewport.x - 1.0f,
                      mm.bottom * _mmToViewport.y - 1.0f };
    }

/*
=================================================
    MmSizeToViewport
=================================================
*/
    inline float2  SurfaceDimensions::MmSizeToViewport (const float2 &mm) C_NE___
    {
        return float2{ mm.x * _mmToViewport.x,
                       mm.y * _mmToViewport.y };
    }

    inline RectF  SurfaceDimensions::MmSizeToViewport (const RectF &mm) C_NE___
    {
        return RectF{ mm.left   * _mmToViewport.x,
                      mm.top    * _mmToViewport.y,
                      mm.right  * _mmToViewport.x,
                      mm.bottom * _mmToViewport.y };
    }

/*
=================================================
    AlignMmToPixel
=================================================
*/
    inline float2  SurfaceDimensions::AlignMmToPixel (const float2 &mm) C_NE___
    {
        return float2{ Round( mm.x * _mmToPixels.x ) * _mmToPixels.x,
                       Round( mm.y * _mmToPixels.y ) * _mmToPixels.y };
    }

    inline RectF  SurfaceDimensions::AlignMmToPixel (const RectF &mm) C_NE___
    {
        return RectF{ Round( mm.left   * _mmToPixels.x ) * _mmToPixels.x,
                      Round( mm.top    * _mmToPixels.y ) * _mmToPixels.y,
                      Round( mm.right  * _mmToPixels.x ) * _mmToPixels.x,
                      Round( mm.bottom * _mmToPixels.y ) * _mmToPixels.y };
    }

/*
=================================================
    PixelsToUNorm
=================================================
*/
    inline float2  SurfaceDimensions::PixelsToUNorm (const float2 &value) C_NE___
    {
        return value * _invSurfaceSize;
    }

/*
=================================================
    PixelsToMm
=================================================
*/
    inline float2  SurfaceDimensions::PixelsToMm (const float2 &value) C_NE___
    {
        return value * _pixelsToMm;
    }

/*
=================================================
    UNormPixelsToMm
=================================================
*/
    inline float2  SurfaceDimensions::UNormPixelsToMm (const float2 &value) C_NE___
    {
        return value * _unormPxToMm;
    }

/*
=================================================
    MmToPixels
=================================================
*/
    inline float2  SurfaceDimensions::MmToPixels (const float2 &value) C_NE___
    {
        return value * _mmToPixels;
    }

    inline RectF  SurfaceDimensions::MmToPixels (const RectF &value) C_NE___
    {
        return value * _mmToPixels;
    }

/*
=================================================
    PixelsToViewport
=================================================
*/
    inline float2  SurfaceDimensions::PixelsToViewport (const float2 &pix) C_NE___
    {
        return GetPixelsToViewport() * pix;
    }

    inline RectF  SurfaceDimensions::PixelsToViewport (const RectF &pix) C_NE___
    {
        return pix * GetPixelsToViewport();
    }

/*
=================================================
    ViewportToPixels
=================================================
*/
    inline RectF  SurfaceDimensions::ViewportToPixels (const RectF &vp) C_NE___
    {
        const float2    size = 0.5f / _invSurfaceSize;
        return RectF{ (vp.left   + 1.0f) * size.x,
                      (vp.top    + 1.0f) * size.y,
                      (vp.right  + 1.0f) * size.x,
                      (vp.bottom + 1.0f) * size.y };
    }


} // AE::Graphics

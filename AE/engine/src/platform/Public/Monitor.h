// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/ScreenCoords.h"

namespace AE::App
{

    //
    // Monitor
    //
    struct Monitor final
    {
    // types
    public:
        enum class ID : uint
        {
            Unknown = ~0u,
        };

        enum class EOrientation : short
        {
            Default             = -1,
            Sensor              = -2,
            Portrait            = 0,
            PortraitReverse     = 180,
            Landscape           = 90,
            LandscapeReverse    = 270,

            Orient_0_deg        = Portrait,
            Orient_90_deg       = Landscape,
            Orient_180_deg      = PortraitReverse,
            Orient_270_deg      = LandscapeReverse,

            Unknown             = Default
        };

        using Name_t            = FixedString< 32 >;
        using NativeMonitor_t   = void *;


    // variables
    public:
        ID                  id                  = Default;
        PixelsRectI         workArea;                       // area available for window
        PixelsRectI         region;                         // area available for fullscreen window only

        Meters2f            physicalSize;                   // in meters
        float2              ppi;                            // pixels per inch
        Meters2f            curvatureRadius;                // radius of curvature on X and Y axis, only for curved monitors
        uint                freq                = 0;        // update frequency in Hz
        EOrientation        orient              = Default;
        bool                isExternal          = false;

        Name_t              name;
        NativeMonitor_t     native              = null;


    // methods
    public:
        Monitor ()                                              __NE___ {}

        ND_ bool        IsDefined ()                            C_NE___ { return id != Default; }
        ND_ bool        IsCurvedScreen ()                       C_NE___ { return Any( curvatureRadius.meters > 0.f ); }

        ND_ float       AspectRatio ()                          C_NE___ { return region.pixels.Width() / float(region.pixels.Height()); }
        ND_ bool        IsHorizontal ()                         C_NE___ { return region.pixels.Width() > region.pixels.Height(); }
        ND_ bool        IsVertical ()                           C_NE___ { return not IsHorizontal(); }

        ND_ float2      DipsPerPixel ()                         C_NE___ { return ppi / _DipToPixel(); }

    #if 0
        ND_ float2      MillimetersPerPixel ()                  C_NE___ { return (physicalSize.meters * 1.0e+3f) / RegionSize(); }
        ND_ float2      PixelPerMillimeters ()                  C_NE___ { return RegionSize() / (physicalSize.meters * 1.0e+3f); }
    #else
        ND_ float2      MillimetersPerPixel ()                  C_NE___ { return 1.f / (ppi * _InchsInMillimeter()); }
    //  ND_ float2      PixelPerMillimeters ()                  C_NE___ { return ppi * _InchsInMillimeter(); }
    #endif

        ND_ float2      RegionSize ()                           C_NE___ { return float2(region.pixels.Size()); }


        // converter
        ND_ Dips2f      PixelsToDips (const Pixels2f &c)        C_NE___ { return Dips2f{ c.pixels * (_DipToPixel() / ppi) }; }
        ND_ Dips2f      PixelsToDips (const Pixels2i &c)        C_NE___ { return PixelsToDips( Pixels2f{float2(c.pixels)} ); }
        ND_ Dips2f      PixelsToDips (const Pixels2u &c)        C_NE___ { return PixelsToDips( Pixels2f{float2(c.pixels)} ); }
        ND_ Dips2f      MetersToDips (const Meters2f &c)        C_NE___ { return PixelsToDips( MetersToPixels( c )); }
        ND_ Dips2f      SNormToDips (const SNorm2f &c)          C_NE___ { return PixelsToDips( SNormToPixels( c )); }
        ND_ Dips2f      UNormToDips (const UNorm2f &c)          C_NE___ { return PixelsToDips( UNormToPixels( c )); }

        ND_ Meters2f    DipsToMeters (const Dips2f &c)          C_NE___ { return UNormToMeters( DipsToUNorm( c )); }
        ND_ Meters2f    PixelsToMeters (const Pixels2f &c)      C_NE___ { return UNormToMeters( PixelsToUNorm( c )); }
        ND_ Meters2f    PixelsToMeters (const Pixels2i &c)      C_NE___ { return UNormToMeters( PixelsToUNorm( c )); }
        ND_ Meters2f    PixelsToMeters (const Pixels2u &c)      C_NE___ { return UNormToMeters( PixelsToUNorm( c )); }
        ND_ Meters2f    SNormToMeters (const SNorm2f &c)        C_NE___ { return UNormToMeters( UNorm2f{c} ); }
        ND_ Meters2f    UNormToMeters (const UNorm2f &c)        C_NE___ { return Meters2f{ c.unorm * physicalSize.meters }; }

        ND_ Pixels2f    DipsToPixels (const Dips2f &c)          C_NE___ { return Pixels2f{ c.dips * (ppi / _DipToPixel()) }; }
        ND_ Pixels2f    MetersToPixels (const Meters2f &c)      C_NE___ { return UNormToPixels( MetersToUNorm( c )); }
        ND_ Pixels2f    SNormToPixels (const SNorm2f &c)        C_NE___ { return UNormToPixels( UNorm2f{c} ); }
        ND_ Pixels2f    UNormToPixels (const UNorm2f &c)        C_NE___ { return Pixels2f{ c.unorm * RegionSize() }; }

        ND_ UNorm2f     DipsToUNorm (const Dips2f &c)           C_NE___ { return PixelsToUNorm( DipsToPixels( c )); }
        ND_ UNorm2f     MetersToUNorm (const Meters2f &c)       C_NE___ { return UNorm2f{ c.meters / physicalSize.meters }; }
        ND_ UNorm2f     PixelsToUNorm (const Pixels2f &c)       C_NE___ { return UNorm2f{ c.pixels / RegionSize() }; }
        ND_ UNorm2f     PixelsToUNorm (const Pixels2i &c)       C_NE___ { return UNorm2f{ float2{c.pixels} / RegionSize() }; }
        ND_ UNorm2f     PixelsToUNorm (const Pixels2u &c)       C_NE___ { return UNorm2f{ float2{c.pixels} / RegionSize() }; }

        ND_ SNorm2f     DipsToSNorm (const Dips2f &c)           C_NE___ { return SNorm2f{ DipsToUNorm( c )}; }
        ND_ SNorm2f     MetersToSNorm (const Meters2f &c)       C_NE___ { return SNorm2f{ MetersToUNorm( c )}; }
        ND_ SNorm2f     PixelsToSNorm (const Pixels2f &c)       C_NE___ { return SNorm2f{ PixelsToUNorm( c )}; }
        ND_ SNorm2f     PixelsToSNorm (const Pixels2i &c)       C_NE___ { return SNorm2f{ PixelsToUNorm( c )}; }
        ND_ SNorm2f     PixelsToSNorm (const Pixels2u &c)       C_NE___ { return SNorm2f{ PixelsToUNorm( c )}; }

        ND_ Pixels2i    ClampPixels (const Pixels2i &c)         C_NE___ { return Pixels2i{ Math::Clamp( c.pixels,   int2{0},    region.pixels.Size()    )}; }
        ND_ Pixels2f    ClampPixels (const Pixels2f &c)         C_NE___ { return Pixels2f{ Math::Clamp( c.pixels,   float2{0},  RegionSize()            )}; }
        ND_ Meters2f    ClampMeters (const Meters2f &c)         C_NE___ { return Meters2f{ Math::Clamp( c.meters,   float2{0},  physicalSize.meters     )}; }
        ND_ Dips2f      ClampDips (const Dips2f &c)             C_NE___ { return Dips2f{   Math::Clamp( c.dips,     float2{0},  PixelsToDips( Pixels2f{RegionSize()} ).dips )}; }

        ND_ static constexpr float  _MetersInInch ()            __NE___ { return 0.0254f; }
        ND_ static constexpr float  _InchsInMillimeter ()       __NE___ { return 0.0393700787f; }
        ND_ static constexpr float  _DipToPixel ()              __NE___ { return 160.0f; }


            void  Print ()                                      C_NE___;


        // utils
        ND_ float2      _CalculatePPI ()                        C_NE___ { return RegionSize() / physicalSize.meters * _MetersInInch(); }
        ND_ Meters2f    _CalculatePhysicalSize ()               C_NE___ { return Meters2f{ RegionSize() / ppi * _MetersInInch() }; }
    };


} // AE::App

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
/*
=================================================
    constructors
=================================================
*/
    template <typename T>
    static void  RGBAColor_Ctor1 (void* mem, T val)
    {
        PlacementNew< RGBAColor<T> >( OUT mem, val );
    }

    template <typename T>
    static void  RGBAColor_Ctor4 (void* mem, T r, T g, T b, T a)
    {
        PlacementNew< RGBAColor<T> >( OUT mem, r, g, b, a );
    }

    static void  RGBA32f_Ctor_HSV_Alpha (void* mem, const HSVColor &c, float alpha)
    {
        PlacementNew< RGBA32f >( OUT mem, c, alpha );
    }

    template <typename T>
    static void  RGBA32f_Ctor_RGBA (void* mem, const RGBAColor<T> &c)
    {
        PlacementNew< RGBA32f >( OUT mem, c );
    }

    template <typename T>
    static void  RGBA32i_Ctor_RGBA (void* mem, const RGBAColor<T> &c)
    {
        PlacementNew< RGBA32i >( OUT mem, c );
    }

    template <typename T>
    static void  RGBA32u_Ctor_RGBA (void* mem, const RGBAColor<T> &c)
    {
        PlacementNew< RGBA32u >( OUT mem, c );
    }

    template <typename T>
    static void  RGBA8u_Ctor_RGBA (void* mem, const RGBAColor<T> &c)
    {
        PlacementNew< RGBA8u >( OUT mem, c );
    }

    static void  float4_Ctor_RGBA32f (void* mem, const RGBA32f &c)
    {
        PlacementNew< packed_float4 >( OUT mem, c );
    }

    static RGBA8u  Ctor_ARGB (uint u)
    {
        return RGBA8u{ ubyte((u >> 16) & 0xFF), ubyte((u >> 8) & 0xFF), ubyte((u >> 0) & 0xFF), ubyte((u >> 24) & 0xFF) };
    }

    static RGBA8u  Ctor_RGBA (uint u)
    {
        return RGBA8u{ ubyte((u >> 24) & 0xFF), ubyte((u >> 16) & 0xFF), ubyte((u >> 8) & 0xFF), ubyte((u >> 0) & 0xFF) };
    }

    static RGBA32f  RGBA32f_mul_RGBA32f (const RGBA32f &lhs, const RGBA32f &rhs)
    {
        return lhs * rhs;
    }

    static RGBA8u  RGBA8u_Lerp (const RGBA8u &x, const RGBA8u &y, float factor)
    {
        return Lerp( x, y, factor );
    }

    static RGBA32f  RGBA32f_Lerp (const RGBA32f &x, const RGBA32f &y, float factor)
    {
        return Lerp( x, y, factor );
    }

    static RGBA32f  RGBA32f_AdjustContrast (const RGBA32f &col, float factor)
    {
        return AdjustContrast( col, factor );
    }

    static RGBA8u  RGBA8u_AdjustContrast (const RGBA8u &col, float factor)
    {
        return AdjustContrast( col, factor );
    }

    static float  RGBA32f_Luminance (const RGBA32f &col)
    {
        return Luminance( col );
    }

    static float  RGBA8u_Luminance (const RGBA8u &col)
    {
        return Luminance( col );
    }

    static RGBA8u  RGBA8u_AdjustSaturation (const RGBA8u &col, float factor)
    {
        return AdjustSaturation( col, factor );
    }

    static RGBA32f  RGBA32f_Rainbow (float factor)
    {
        return Rainbow( factor );
    }

    static RGBA32f  RGBA32f_RainbowWrap (float factor)
    {
        return RainbowWrap( factor );
    }

/*
=================================================
    BindRGBAColor
=================================================
*/
    template <typename T>
    static void  BindRGBAColor (const ScriptEnginePtr &se)
    {
        ClassBinder<T>  binder{ se };

        binder.CreateClassValue();
        binder.AddConstructor( &RGBAColor_Ctor1< typename T::value_type >, {"value"} );
        binder.AddConstructor( &RGBAColor_Ctor4< typename T::value_type >, {"r", "g", "b", "a"} );

        binder.AddProperty( &T::r, "r" );
        binder.AddProperty( &T::g, "g" );
        binder.AddProperty( &T::b, "b" );
        binder.AddProperty( &T::a, "a" );

        binder.Operators()
            .Equal( &T::operator== );

        if constexpr( IsSameTypes< typename T::value_type, float >)
        {
            se->AddFunction( &RGBA32f_Lerp,             "Lerp",             {"x", "y", "factor"} );
            se->AddFunction( &RGBA32f_AdjustContrast,   "AdjustContrast",   {"col", "factor"} );
            se->AddFunction( &RGBA32f_Luminance,        "Luminance",        {"col"} );
            se->AddFunction( &RGBA32f_Rainbow,          "Rainbow",          {"factor"} );
            se->AddFunction( &RGBA32f_RainbowWrap,      "RainbowWrap",      {"factor"} );

            binder.Operators()
                .Binary( EBinaryOperator::Mul, &RGBA32f_mul_RGBA32f );

            ClassBinder<packed_float4>  binder2{ se };
            if ( binder2.IsRegistred() )
            {
                binder2.AddConstructor( &float4_Ctor_RGBA32f, {"x"} );
            }
        }

        if constexpr( IsSameTypes< typename T::value_type, ubyte >)
        {
            se->AddFunction( &RGBA8u_AdjustContrast,    "AdjustContrast",   {"col", "factor"} );
            se->AddFunction( &RGBA8u_Luminance,         "Luminance",        {"col"} );
            se->AddFunction( &RGBA8u_AdjustSaturation,  "AdjustSaturation", {"col", "factor"} );
            se->AddFunction( &RGBA8u_Lerp,              "Lerp",             {"x", "y", "factor"} );
        }
    }

/*
=================================================
    BindRGBAColorCast
=================================================
*/
    template <typename T>
    static void  BindRGBAColorCast (const ScriptEnginePtr &se)
    {
        ClassBinder<T>  binder{ se };

        if constexpr( IsSameTypes< T, RGBA32f > )
        {
            binder.AddConstructor( &RGBA32f_Ctor_HSV_Alpha,     {"hsv", "alpha"} );
            binder.AddConstructor( &RGBA32f_Ctor_RGBA<ubyte>,   {"c"} );
            binder.AddConstructor( &RGBA32f_Ctor_RGBA<uint>,    {"c"} );
        }
        if constexpr( IsSameTypes< T, RGBA32i > )
        {
            binder.AddConstructor( &RGBA32i_Ctor_RGBA<ubyte>,   {"c"} );
            binder.AddConstructor( &RGBA32i_Ctor_RGBA<uint>,    {"c"} );
        }
        if constexpr( IsSameTypes< T, RGBA32u > )
        {
            binder.AddConstructor( &RGBA32u_Ctor_RGBA<ubyte>,   {"c"} );
            binder.AddConstructor( &RGBA32u_Ctor_RGBA<int>,     {"c"} );
        }
        if constexpr( IsSameTypes< T, RGBA8u > )
        {
            binder.AddConstructor( &RGBA8u_Ctor_RGBA<int>,      {"c"} );
            binder.AddConstructor( &RGBA8u_Ctor_RGBA<uint>,     {"c"} );
            binder.AddConstructor( &RGBA8u_Ctor_RGBA<float>,    {"c"} );

            se->AddFunction( &Ctor_ARGB,    "asARGB",   {} );
            se->AddFunction( &Ctor_RGBA,    "asRGBA",   {} );
        }
    }

/*
=================================================
    constructors
=================================================
*/
    static void  HSVColor_Ctor1 (void* mem, float h)
    {
        PlacementNew< HSVColor >( OUT mem, h );
    }

    static void  HSVColor_Ctor2 (void* mem, float h, float s)
    {
        PlacementNew< HSVColor >( OUT mem, h, s );
    }

    static void  HSVColor_Ctor3 (void* mem, float h, float s, float v)
    {
        PlacementNew< HSVColor >( OUT mem, h, s, v );
    }

    static void  HSVColor_Ctor_RGBA32f (void* mem, const RGBA32f &c)
    {
        PlacementNew< HSVColor >( OUT mem, c );
    }

/*
=================================================
    BindHSVColor
=================================================
*/
    static void  BindHSVColor (const ScriptEnginePtr &se)
    {
        ClassBinder<HSVColor>   binder{ se };

        binder.CreateClassValue();

        binder.AddProperty( &HSVColor::h, "h" );
        binder.AddProperty( &HSVColor::s, "s" );
        binder.AddProperty( &HSVColor::v, "v" );

        binder.AddConstructor( &HSVColor_Ctor1,         {"h"} );
        binder.AddConstructor( &HSVColor_Ctor2,         {"h", "s"} );
        binder.AddConstructor( &HSVColor_Ctor3,         {"h", "s", "v"} );
        binder.AddConstructor( &HSVColor_Ctor_RGBA32f,  {"col"} );

        binder.Operators()
            .Equal( &HSVColor::operator== );
    }

/*
=================================================
    constructors
=================================================
*/
    static void  DepthStencil_Ctor (void* mem, float d, int s)
    {
        PlacementNew< DepthStencil >( OUT mem, d, uint(s) );
    }

/*
=================================================
    BindDepthStencil
=================================================
*/
    static void  BindDepthStencil (const ScriptEnginePtr &se)
    {
        ClassBinder<DepthStencil>   binder{ se };

        binder.CreateClassValue();

        binder.AddProperty( &DepthStencil::depth,   "depth" );
        binder.AddProperty( &DepthStencil::stencil, "stencil" );

        binder.AddConstructor( &DepthStencil_Ctor,  {"depth", "stencil"} );

        binder.Operators()
            .Equal( &DepthStencil::operator== );
    }

} // namespace


/*
=================================================
    BindColor
=================================================
*/
    void  CoreBindings::BindColor (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        BindRGBAColor<RGBA32f>( se );
        BindRGBAColor<RGBA32i>( se );
        BindRGBAColor<RGBA32u>( se );
        BindRGBAColor<RGBA8u>( se );
        BindHSVColor( se );
        BindDepthStencil( se );

        BindRGBAColorCast<RGBA32f>( se );
        BindRGBAColorCast<RGBA32i>( se );
        BindRGBAColorCast<RGBA32u>( se );
        BindRGBAColorCast<RGBA8u>( se );
    }

} // AE::Scripting

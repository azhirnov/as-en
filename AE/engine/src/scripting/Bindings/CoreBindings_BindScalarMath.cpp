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
    ScalarFunc
=================================================
*/
    struct ScalarFunc
    {
        template <typename T>   static T  Abs (T value)                     { return Math::Abs( value ); }
        template <typename T>   static T  Floor (T value)                   { return Math::Floor( value ); }
        template <typename T>   static T  Ceil (T value)                    { return Math::Ceil( value ); }
        template <typename T>   static T  Trunc (T value)                   { return Math::Trunc( value ); }
        template <typename T>   static T  Fract (T value)                   { return Math::Fract( value ); }

        template <typename T>   static T  Round (T value)                   { return Math::Round( value ); }
        template <typename T>   static auto  RoundToInt (T value)           { return Math::RoundToInt( value ); }
        template <typename T>   static auto  RoundToUint (T value)          { return Math::RoundToUint( value ); }

        template <typename T>   static T  AlignDown (T value, T align)      { return Math::AlignDown( value, align ); }
        template <typename T>   static T  AlignUp (T value, T align)        { return Math::AlignUp( value, align ); }
        template <typename T>   static bool  IsMultipleOf (T value, T align)    { return Math::IsMultipleOf( value, align ); }

        template <typename T>   static T  SafeDiv (T lhs, T rhs, T def)     { return Math::SafeDiv( lhs, rhs, def ); }

        template <typename T>   static bool  All (T value)                  { return Math::All( value ); }
        template <typename T>   static bool  Any (T value)                  { return Math::Any( value ); }

        template <typename T>   static T  Pow (T x, T y)                    { return Math::Pow( x, y ); }
        template <typename T>   static T  Ln (T x)                          { return Math::Ln( x ); }
        template <typename T>   static T  Log (T x, T base)                 { return Math::Log( x, base ); }
        template <typename T>   static T  Log2 (T x)                        { return Math::Log2( x ); }
        template <typename T>   static T  Log10 (T x)                       { return Math::Log10( x ); }
        template <typename T>   static T  Exp (T x)                         { return Math::Exp( x ); }
        template <typename T>   static T  Exp2 (T x)                        { return Math::Exp2( x ); }
        template <typename T>   static T  Exp10 (T x)                       { return Math::Exp10( x ); }
        template <typename T>   static T  ExpMinus1 (T x)                   { return Math::ExpMinus1( x ); }

        template <typename T>   static int  IntLog2 (T x)                   { return Math::IntLog2( x ); }
        template <typename T>   static int  CeilIntLog2 (T x)               { return Math::CeilIntLog2( x ); }
        template <typename T>   static int  BitScanReverse (T x)            { return Math::BitScanReverse( x ); }
        template <typename T>   static int  BitScanForward (T x)            { return Math::BitScanForward( x ); }
        template <typename T>   static uint  BitCount (T x)                 { using U = ToUnsignedInteger<T>; return uint(Math::BitCount( U(x) )); }
        template <typename T>   static bool  IsPowerOfTwo (T x)             { return Math::IsPowerOfTwo( x ); }
        template <typename T>   static bool  IsSingleBitSet (T x)           { return Math::IsSingleBitSet( x ); }

        template <typename T>   static bool  AllBits (T x, T y)             { return Math::AllBits( x, y ); }
        template <typename T>   static bool  AnyBits (T x, T y)             { return Math::AnyBits( x, y ); }

        template <typename T>   static T  ExtractBit (T& x)                 { return Math::ExtractBit( x ); }
        template <typename T>   static T  ExtractBitLog2 (T& x)             { return Math::ExtractBitLog2( x ); }

        template <typename T>   static T  BitRotateLeft (T x, uint shift)   { return Math::BitRotateLeft( x, shift ); }
        template <typename T>   static T  BitRotateRight (T x, uint shift)  { return Math::BitRotateRight( x, shift ); }

        template <typename T>   static T  Sin (T value)                     { return Math::Sin( TRadians<T>{value} ); }
        template <typename T>   static T  SinH (T value)                    { return Math::SinH( TRadians<T>{value} ); }
        template <typename T>   static T  ASin (T value)                    { return T(Math::ASin( value )); }
        template <typename T>   static T  Cos (T value)                     { return Math::Cos( TRadians<T>{value} ); }
        template <typename T>   static T  CosH (T value)                    { return Math::CosH( TRadians<T>{value} ); }
        template <typename T>   static T  ACos (T value)                    { return T(Math::ACos( value )); }
        template <typename T>   static T  Tan (T value)                     { return Math::Tan( TRadians<T>{value} ); }
        template <typename T>   static T  TanH (T value)                    { return Math::TanH( TRadians<T>{value} ); }
        template <typename T>   static T  ATan (T y, T x)                   { return T(Math::ATan( y, x )); }

        template <typename T>   static T  Square (T value)                  { return Math::Square( value ); }
        template <typename T>   static T  Sqrt (T value)                    { return Math::Sqrt( value ); }
        template <typename T>   static T  Mod (T x, T y)                    { return std::fmod( x, y ); }

        template <typename T>   static T  FloorPOT (T x)                    { return Math::FloorPOT( x ); }
        template <typename T>   static T  CeilPOT (T x)                     { return Math::CeilPOT( x ); }

        template <typename T>   static T  IsInfinity (T x)                  { return Math::IsInfinity( x ); }
        template <typename T>   static T  IsNaN (T x)                       { return Math::IsNaN( x ); }
        template <typename T>   static T  IsFinite (T x)                    { return Math::IsFinite( x ); }

        template <typename T>   static T  Lerp (T x, T y, T factor)         { return Math::Lerp( x, y, factor ); }

        template <typename T>   static T  Min (T x, T y)                    { return Math::Min( x, y ); }
        template <typename T>   static T  Max (T x, T y)                    { return Math::Max( x, y ); }
        template <typename T>   static T  Clamp (T x, T min, T max)         { return Math::Clamp( x, min, max ); }
        template <typename T>   static T  Wrap (T x, T min, T max)          { return Math::Wrap( x, min, max ); }
        template <typename T>   static T  MirroredWrap (T x, T min, T max)  { return Math::MirroredWrap( x, min, max ); }

        template <typename T>   static T  ToRad (T x)                       { return TRadians<T>::DegToRad() * x; }
        template <typename T>   static T  ToDeg (T x)                       { return TRadians<T>::RadToDeg() * x; }

        template <typename T>   static T  ToSNorm (T x)                     { return Math::ToSNorm( x ); }
        template <typename T>   static T  ToUNorm (T x)                     { return Math::ToUNorm( x ); }

        template <typename T>   static T  Average (T x, T y)                { return Math::Average( x, y ); }

        template <typename T>   static T  Remap (T srcMin, T srcMax, T dstMin, T dstMax, T x)           { return Math::Remap( Range{srcMin, srcMax}, Range{dstMin, dstMax}, x ); }
        template <typename T>   static T  RemapClamped (T srcMin, T srcMax, T dstMin, T dstMax, T x)    { return Math::RemapClamped( Range{srcMin, srcMax}, Range{dstMin, dstMax}, x ); }

        template <typename T>   static T  Pi ()                             { return T{Math::TRadians<T>::Pi()}; }
    };

/*
=================================================
    BindBoolScalar
=================================================
*/
    template <typename T>
    static void  BindBoolScalar (const ScriptEnginePtr &)
    {
    }

/*
=================================================
    BindIntScalar
=================================================
*/
    template <typename T>
    static void  BindIntScalar (const ScriptEnginePtr &se)
    {
        se->AddFunction( &ScalarFunc::IntLog2<T>,           "IntLog2",          {"x"} );
        se->AddFunction( &ScalarFunc::CeilIntLog2<T>,       "CeilIntLog2",      {"x"} );
        se->AddFunction( &ScalarFunc::BitScanReverse<T>,    "BitScanReverse",   {"x"} );
        se->AddFunction( &ScalarFunc::BitScanForward<T>,    "BitScanForward",   {"x"} );
        se->AddFunction( &ScalarFunc::BitCount<T>,          "BitCount",         {"x"} );
        se->AddFunction( &ScalarFunc::IsPowerOfTwo<T>,      "IsPowerOfTwo",     {"x"} );
        se->AddFunction( &ScalarFunc::IsSingleBitSet<T>,    "IsSingleBitSet",   {"x"} );
        se->AddFunction( &ScalarFunc::AllBits<T>,           "AllBits",          {"x", "y"} );
        se->AddFunction( &ScalarFunc::AnyBits<T>,           "AnyBits",          {"x", "y"} );
        se->AddFunction( &ScalarFunc::ExtractBit<T>,        "ExtractBit",       {"x"} );
        se->AddFunction( &ScalarFunc::ExtractBitLog2<T>,    "ExtractBitLog2",   {"x"} );
        se->AddFunction( &ScalarFunc::BitRotateLeft<T>,     "BitRotateLeft",    {"x", "shift"} );
        se->AddFunction( &ScalarFunc::BitRotateRight<T>,    "BitRotateRight",   {"x", "shift"} );
        se->AddFunction( &ScalarFunc::FloorPOT<T>,          "FloorPOT",         {"x"} );
        se->AddFunction( &ScalarFunc::CeilPOT<T>,           "CeilPOT",          {"x"} );

        se->AddFunction( &ScalarFunc::AlignDown<T>,         "AlignDown",        {"x", "align"} );
        se->AddFunction( &ScalarFunc::AlignUp<T>,           "AlignUp",          {"x", "align"} );
        se->AddFunction( &ScalarFunc::IsMultipleOf<T>,      "IsMultipleOf",     {"x", "align"} );
    }

/*
=================================================
    BindFloatScalar
=================================================
*/
    template <typename T>
    static void  BindFloatScalar (const ScriptEnginePtr &se)
    {
        const String    suffix = IsSameTypes<T,float> ? "" : "D";

        //
        se->AddFunction( &ScalarFunc::Floor<T>,         "Floor",        {"x"} );
        se->AddFunction( &ScalarFunc::Ceil<T>,          "Ceil",         {"x"} );
        se->AddFunction( &ScalarFunc::Trunc<T>,         "Trunc",        {"x"} );
        se->AddFunction( &ScalarFunc::Fract<T>,         "Fract",        {"x"} );
        se->AddFunction( &ScalarFunc::Round<T>,         "Round",        {"x"} );
        se->AddFunction( &ScalarFunc::Mod<T>,           "Mod",          {"x", "y"} );

        //
        se->AddFunction( &ScalarFunc::Sqrt<T>,          "Sqrt",         {"x"} );
        se->AddFunction( &ScalarFunc::Pow<T>,           "Pow",          {"x", "pow"} );
        se->AddFunction( &ScalarFunc::Ln<T>,            "Ln",           {"x"} );
        se->AddFunction( &ScalarFunc::Log<T>,           "Log",          {"x", "base"} );
        se->AddFunction( &ScalarFunc::Log2<T>,          "Log2",         {"x"} );
        se->AddFunction( &ScalarFunc::Log10<T>,         "Log10",        {"x"} );
        se->AddFunction( &ScalarFunc::Exp<T>,           "Exp",          {"x"} );
        se->AddFunction( &ScalarFunc::Exp2<T>,          "Exp2",         {"x"} );
        se->AddFunction( &ScalarFunc::Exp10<T>,         "Exp10",        {"x"} );
        se->AddFunction( &ScalarFunc::ExpMinus1<T>,     "ExpMinus1",    {"x"} );

        // trigonometry
        se->AddFunction( &ScalarFunc::Sin<T>,           "Sin",          {"x"} );
        se->AddFunction( &ScalarFunc::SinH<T>,          "SinH",         {"x"} );
        se->AddFunction( &ScalarFunc::ASin<T>,          "ASin",         {"x"} );
        se->AddFunction( &ScalarFunc::Cos<T>,           "Cos",          {"x"} );
        se->AddFunction( &ScalarFunc::CosH<T>,          "CosH",         {"x"} );
        se->AddFunction( &ScalarFunc::ACos<T>,          "ACos",         {"x"} );
        se->AddFunction( &ScalarFunc::Tan<T>,           "Tan",          {"x"} );
        se->AddFunction( &ScalarFunc::TanH<T>,          "TanH",         {"x"} );
        se->AddFunction( &ScalarFunc::ATan<T>,          "ATan",         {"y", "x"} );
        se->AddFunction( &ScalarFunc::ToRad<T>,         "ToRad",        {"x"} );
        se->AddFunction( &ScalarFunc::ToDeg<T>,         "ToDeg",        {"x"} );

        // interpolation
        se->AddFunction( &ScalarFunc::Lerp<T>,          "Lerp",         {"x", "y", "factor"} );

        se->AddFunction( &ScalarFunc::ToSNorm<T>,       "ToSNorm",      {"x"} );
        se->AddFunction( &ScalarFunc::ToUNorm<T>,       "ToUNorm",      {"x"} );

        se->AddFunction( &ScalarFunc::Remap<T>,         "Remap",        {"srcMin", "srcMax", "dstMin", "dstMax", "x"} );
        se->AddFunction( &ScalarFunc::RemapClamped<T>,  "RemapClamped", {"srcMin", "srcMax", "dstMin", "dstMax", "x"} );

        se->AddFunction( &ScalarFunc::RoundToInt<T>,    "RoundToInt",   {"x"} );
        se->AddFunction( &ScalarFunc::RoundToUint<T>,   "RoundToUint",  {"x"} );

        se->AddFunction( &ScalarFunc::IsInfinity<T>,    "IsInfinity",   {"x"} );
        se->AddFunction( &ScalarFunc::IsNaN<T>,         "IsNaN",        {"x"} );
        se->AddFunction( &ScalarFunc::IsFinite<T>,      "IsFinite",     {"x"} );

        if constexpr( IsSameTypes< T, float >)
            se->AddFunction( &ScalarFunc::Pi<T>,        "Pi",           {} );

        if constexpr( IsSameTypes< T, double >)
            se->AddFunction( &ScalarFunc::Pi<T>,        "dPi",          {} );
    }

/*
=================================================
    BindIntFloatScalar
=================================================
*/
    template <typename T>
    static void  BindIntFloatScalar (const ScriptEnginePtr &se)
    {
        if constexpr( IsSignedInteger<T> or IsFloatPoint<T> )
        {
            se->AddFunction( &ScalarFunc::Abs<T>,           "Abs",          {"x"} );
            se->AddFunction( &ScalarFunc::MirroredWrap<T>,  "MirroredWrap", {"x", "min", "max"} );
        }

        se->AddFunction( &ScalarFunc::Square<T>,        "Square",       {"x"} );

        se->AddFunction( &ScalarFunc::Min<T>,           "Min",          {"x", "y"} );
        se->AddFunction( &ScalarFunc::Max<T>,           "Max",          {"x", "y"} );
        se->AddFunction( &ScalarFunc::Clamp<T>,         "Clamp",        {"x", "min", "max"} );
        se->AddFunction( &ScalarFunc::Wrap<T>,          "Wrap",         {"x", "min", "max"} );

        se->AddFunction( &ScalarFunc::Average<T>,       "Average",      {"x", "y"} );
    }

} // namespace


/*
=================================================
    BindScalarMath
=================================================
*/
    void  CoreBindings::BindScalarMath (const ScriptEnginePtr &se) __Th___
    {
        CHECK_THROW( se and se->IsInitialized() );

        BindBoolScalar<bool>( se );

        BindIntFloatScalar<int>( se );
        BindIntScalar<int>( se );

        BindIntFloatScalar<uint>( se );
        BindIntScalar<uint>( se );

        BindIntFloatScalar<float>( se );
        BindFloatScalar<float>( se );
    }


} // AE::Scripting

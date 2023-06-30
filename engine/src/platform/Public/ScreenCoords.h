// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{
namespace _hidden_
{

    template <typename V>
    struct Pixels
    {
        V   pixels;

        Pixels ()                               __NE___ {}
        explicit Pixels (const V &val)          __NE___ : pixels{val} {}

        ND_ explicit operator V ()              C_NE___ { return pixels; }
    };


    template <typename V>
    struct Dips
    {
        V   dips;

        Dips ()                                 __NE___ {}
        explicit Dips (const V &val)            __NE___ : dips{val} {}

        ND_ explicit operator V ()              C_NE___ { return dips; }
    };


    template <typename V>
    struct Meters
    {
        V   meters;

        Meters ()                               __NE___ {}
        explicit Meters (const V &val)          __NE___ : meters{val} {}

        ND_ explicit operator V ()              C_NE___ { return meters; }
    };


    template <typename V>
    struct SNorm;


    template <typename V>
    struct UNorm
    {
        V   unorm;

        UNorm ()                                __NE___ {}
        explicit UNorm (const V &val)           __NE___ : unorm{val} {}
        explicit UNorm (const SNorm<V> &val)    __NE___;

        ND_ explicit operator V ()              C_NE___ { return unorm; }
    };


    template <typename V>
    struct SNorm
    {
        V   snorm;

        SNorm ()                                __NE___ {}
        explicit SNorm (const V &val)           __NE___ : snorm{val} {}
        explicit SNorm (const UNorm<V> &val)    __NE___;

        ND_ explicit operator V ()              C_NE___ { return snorm; }
    };


    template <> inline UNorm<float2>::UNorm (const SNorm<float2> &val) __NE___ : unorm{ (val.snorm - 1.0f) * 0.5f } {}
    template <> inline SNorm<float2>::SNorm (const UNorm<float2> &val) __NE___ : snorm{ (val.unorm * 2.0f) - 1.0f } {}

} // _hidden_


    using Pixels2f      = App::_hidden_::Pixels< float2 >;
    using Pixels2u      = App::_hidden_::Pixels< uint2 >;
    using Pixels2i      = App::_hidden_::Pixels< int2 >;
    using PixelsRectI   = App::_hidden_::Pixels< RectI >;
    using PixelsRectF   = App::_hidden_::Pixels< RectF >;

    using Dips2f        = App::_hidden_::Dips< float2 >;
    using Dips2u        = App::_hidden_::Dips< uint2 >;
    using Dips2i        = App::_hidden_::Dips< int2 >;
    using DipsRectF     = App::_hidden_::Dips< RectF >;

    using Meters2f      = App::_hidden_::Meters< float2 >;
    using Meters2u      = App::_hidden_::Meters< uint2 >;
    using Meters2i      = App::_hidden_::Meters< int2 >;

    using UNorm2f       = App::_hidden_::UNorm< float2 >;
    using SNorm2f       = App::_hidden_::SNorm< float2 >;


} // AE::App

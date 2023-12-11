// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    512 bit SIMD
*/

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{
    /*
#if AE_SIMD_AVX >= 3
    //
    // 128 bit half (AVX512_FP16 + AVX512VL)
    //
    struct SimdHalf8
    {
    // types
    public:
        using Value_t   = glm::detail::hdata;
        using Self      = SimdHalf8;


    // variables
    private:
        __m128      _value;     // half[8]


    // methods
    public:
        SimdHalf8 ()    __NE___ : _value{_mm_setzero_ps()}  {}
    };



    //
    // 256 bit half (AVX512_FP16 + AVX512VL)
    //
    struct SimdHalf16
    {
    // types
    public:
        using Value_t   = glm::detail::hdata;
        using Self      = SimdHalf16;


    // variables
    private:
        __m256      _value;     // half[16]


    // methods
    public:
        SimdHalf16 ()   __NE___ : _value{_mm256_setzero_ps()}   {}
    };
#endif // AE_SIMD_AVX >= 3
    */

//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 3

    //
    // 512 bit float (AVX512)
    //
    struct SimdFloat16
    {
    // types
    public:
        using Value_t   = float;
        using Self      = SimdFloat16;


    // variables
    private:
        __m512      _value;     // float[16]


    // methods
    public:
        SimdFloat16 ()  __NE___ : _value{_mm512_setzero_ps()}   {}
    };



    //
    // 512 bit double (AVX512)
    //
    struct SimdDouble8
    {
    // types
    public:
        using Value_t   = double;
        using Self      = SimdDouble8;


    // variables
    private:
        __m512d     _value;     // doubel[8]


    // methods
    public:
        SimdDouble8 ()  __NE___ : _value{_mm512_setzero_pd()}   {}
    };



    //
    // 512 bit integer (AVX512)
    //
    struct Int512b
    {
    // types
    public:
        using Self  = Int512b;


    // variables
    private:
        __m512i     _value;


    // methods
    public:
        Int512b ()      __NE___ : _value{ _mm512_setzero_si512() } {}
    };



    //
    // 512 bit integer (AVX512)
    //
    template <typename IntType>
    struct SimdTInt512
    {
        StaticAssert(( IsSameTypes<IntType, Int128b> or IsSameTypes<IntType, Int256b> or IsInteger<IntType> ));

    // types
    public:
        using Value_t   = IntType;
        using Self      = SimdTInt512< IntType >;


    // variables
    private:
        __m512i     _value;     // long[8], int[16], short[32], byte[64]


    // methods
    public:
        SimdTInt512 ()  __NE___ : _value{_mm512_setzero_si512() } {}

        template <typename IT>
        ND_ SimdTInt512<IT> &       Cast ()     __NE___ { return reinterpret_cast< SimdTInt512<IT> &>(*this); }

        template <typename IT>
        ND_ SimdTInt512<IT> const&  Cast ()     C_NE___ { return reinterpret_cast< SimdTInt512<IT> const &>(*this); }

        ND_ Int512b const&          Cast ()     C_NE___ { return reinterpret_cast< Int512b const &>(*this); }
    };


#endif // AE_SIMD_AVX >= 3

} // AE::Math

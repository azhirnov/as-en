// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    256 bit SIMD
*/

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{
#if AE_SIMD_AVX >= 1


    //
    // 256 bits float (AVX)
    //
    struct SimdFloat8
    {
    // types
    public:
        using Value_t   = float;
        using Self      = SimdFloat8;
        using Array_t   = StaticArray< Value_t, 8 >;

        STATIC_ASSERT( sizeof(Array_t) == sizeof(__m256) );


        struct Bool8
        {
        private:
            __m256i     _value;     // uint[8]

        public:
            explicit Bool8 (bool val)                       __NE___ : _value{_mm256_set1_epi32( val ? -1 : 0 )} {}
            explicit Bool8 (const __m256i &val)             __NE___ : _value{val} {}
            explicit Bool8 (const __m256 &val)              __NE___ : _value{_mm256_castps_si256(val)} {}

            ND_ Bool8  operator | (const Bool8 &rhs)        C_NE___ { return Bool8{ _mm256_or_si256(  _value, rhs._value )}; }
            ND_ Bool8  operator & (const Bool8 &rhs)        C_NE___ { return Bool8{ _mm256_and_si256( _value, rhs._value )}; }
            ND_ Bool8  operator ^ (const Bool8 &rhs)        C_NE___ { return Bool8{ _mm256_xor_si256( _value, rhs._value )}; }
            ND_ Bool8  operator ~ ()                        C_NE___ { return Bool8{ _mm256_andnot_si256( _value, _mm256_set1_epi32(-1) )}; }

            ND_ bool  All ()                                C_NE___ { return _mm256_movemask_epi8( _value ) == 0xFFFF'FFFF; }
            ND_ bool  Any ()                                C_NE___ { return _mm256_movemask_epi8( _value ) != 0; }
            ND_ bool  None ()                               C_NE___ { return _mm256_movemask_epi8( _value ) == 0; }

        //  ND_ bool  All ()                                C_NE___ { alignas(__m256i) uint u[8];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] & u[1] & u[2] & u[3] & u[4] & u[5] & u[6] & u[7]) == UMax; }
        //  ND_ bool  Any ()                                C_NE___ { alignas(__m256i) uint u[8];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] | u[1] | u[2] | u[3] | u[4] | u[5] | u[6] | u[7]) == UMax; }
        //  ND_ bool  None ()                               C_NE___ { alignas(__m256i) uint u[8];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] | u[1] | u[2] | u[3] | u[4] | u[5] | u[6] | u[7]) == 0; }
        };


    // variables
    private:
        __m256      _value;     // float[8]


    // methods
    public:
        SimdFloat8 ()                                       __NE___ : _value{ _mm256_setzero_ps() } {}
        explicit SimdFloat8 (float v)                       __NE___ : _value{ _mm256_set1_ps( v )} {}
        explicit SimdFloat8 (const float* ptr)              __NE___ : _value{ _mm256_loadu_ps( ptr )} { ASSERT( ptr != null ); }
        explicit SimdFloat8 (const __m256 &v)               __NE___ : _value{v} {}

        SimdFloat8 (float v0, float v1, float v2, float v3,
                    float v4, float v5, float v6, float v7) __NE___ :
            _value{ _mm256_set_ps( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

        ND_ Self  operator +  (const Self &rhs)             C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)             C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)             C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)             C_NE___ { return Div( rhs ); }

        ND_ Self  operator &  (const Self &rhs)             C_NE___ { return And( rhs ); }
        ND_ Self  operator |  (const Self &rhs)             C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^  (const Self &rhs)             C_NE___ { return Xor( rhs ); }

        ND_ Bool8  operator == (const Self &rhs)            C_NE___ { return Equal( rhs ); }
        ND_ Bool8  operator != (const Self &rhs)            C_NE___ { return NotEqual( rhs ); }
        ND_ Bool8  operator >  (const Self &rhs)            C_NE___ { return Greater( rhs ); }
        ND_ Bool8  operator <  (const Self &rhs)            C_NE___ { return Less( rhs ); }
        ND_ Bool8  operator >= (const Self &rhs)            C_NE___ { return GEqual( rhs ); }
        ND_ Bool8  operator <= (const Self &rhs)            C_NE___ { return LEqual( rhs ); }

        ND_ Value_t  GetX ()                                C_NE___ { return _mm256_cvtss_f32( _value ); }

        ND_ __m256 const&  Get ()                           C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                     C_NE___ { return Self{ _mm256_add_ps( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                     C_NE___ { return Self{ _mm256_sub_ps( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                     C_NE___ { return Self{ _mm256_mul_ps( _value, rhs._value )}; }
        ND_ Self  Div (const Self &rhs)                     C_NE___ { return Self{ _mm256_div_ps( _value, rhs._value )}; }
        ND_ Self  Min (const Self &rhs)                     C_NE___ { return Self{ _mm256_min_ps( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                     C_NE___ { return Self{ _mm256_max_ps( _value, rhs._value )}; }

        ND_ Self  And (const Self &rhs)                     C_NE___ { return Self{ _mm256_and_ps( _value, rhs._value )}; }
        ND_ Self  Or  (const Self &rhs)                     C_NE___ { return Self{ _mm256_or_ps( _value, rhs._value )}; }
        ND_ Self  Xor (const Self &rhs)                     C_NE___ { return Self{ _mm256_xor_ps( _value, rhs._value )}; }
        ND_ Self  AndNot (const Self &rhs)                  C_NE___ { return Self{ _mm256_andnot_ps( _value, rhs._value )}; } // !a & b

        ND_ Self  HAdd (const Self &rhs)                    C_NE___ { return Self{ _mm256_hadd_ps( _value, rhs._value )}; }     // { a0 + a1, a2 + a3, b0 + b1, b2 + b3 }
        ND_ Self  HSub (const Self &rhs)                    C_NE___ { return Self{ _mm256_hsub_ps( _value, rhs._value )}; }     // { a0 - a1, a2 - a3, b0 - b1, b2 - b3 }

        ND_ Self  AddSub (const Self &rhs)                  C_NE___ { return Self{ _mm256_addsub_ps( _value, rhs._value )}; }   // { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }

        ND_ Self  Abs ()                                    C_NE___
        {
            __m256  signmask = _mm256_set1_ps( -0.0f );
            return Self{ _mm256_andnot_ps( signmask, _value )};
        }

        ND_ Self  Reciprocal ()                             C_NE___ { return Self{ _mm256_rcp_ps( _value )}; }                              // 1 / x
        ND_ Self  Sqrt ()                                   C_NE___ { return Self{ _mm256_sqrt_ps( _value )}; }
        ND_ Self  RSqrt ()                                  C_NE___ { return Self{ _mm256_rsqrt_ps( _value )}; }                            // 1 / sqrt(x)
        ND_ Self  FastSqrt ()                               C_NE___ { return Self{ _mm256_mul_ps( _value, _mm256_rsqrt_ps( _value ))}; }    // x / sqrt(x)

        // ordered - comparison with NaN returns false
        ND_ Bool8  Equal    (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
        ND_ Bool8  NotEqual (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
        ND_ Bool8  Greater  (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
        ND_ Bool8  Less     (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
        ND_ Bool8  GEqual   (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
        ND_ Bool8  LEqual   (const Self &rhs)               C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }

        // unordered - comparison with NaN returns true
        ND_ Bool8  EqualU    (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
        ND_ Bool8  NotEqualU (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
        ND_ Bool8  GreaterU  (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
        ND_ Bool8  LessU     (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
        ND_ Bool8  GEqualU   (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
        ND_ Bool8  LEqualU   (const Self &rhs)              C_NE___ { return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }

        ND_ Self  Dot (const Self &rhs)                     C_NE___ { return Self{ _mm256_dp_ps( _value, rhs._value, 0xFF )}; }

        ND_ Self  Floor ()                                  C_NE___ { return Self{ _mm256_floor_ps( _value )}; }
        ND_ Self  Ceil ()                                   C_NE___ { return Self{ _mm256_ceil_ps( _value )}; }

        template <int Mode>
        ND_ Self  Round ()                                  C_NE___ { return Self{ _mm256_round_ps( _value, Mode )}; }

        template <uint Idx>
        ND_ SimdFloat4  ToFloat4 ()                         C_NE___ { STATIC_ASSERT( Idx < 2 );  return SimdFloat4{ _mm256_extractf128_ps( _value, Idx )}; }

        ND_ Array_t     ToArray ()                          C_NE___ { Array_t arr;  _mm256_storeu_ps( OUT arr.data(), _value );  return arr; }
            void        ToArray (OUT Value_t* dst)          C_NE___ { _mm256_storeu_ps( OUT dst, _value ); }
            void        ToAlignedArray (OUT Value_t* dst)   C_NE___ { CheckPointerCast<__m256>( dst );  _mm256_store_ps( OUT dst, _value ); }

        template <typename DstType>
        ND_ auto        Cast ()                             C_NE___;
    };



    //
    // 256 bits double (AVX)
    //
    struct SimdDouble4
    {
    // types
    public:
        using Value_t   = double;
        using Self      = SimdDouble4;
        using Array_t   = StaticArray< Value_t, 4 >;

        STATIC_ASSERT( sizeof(Array_t) == sizeof(__m256d) );


        struct Bool4
        {
        private:
            __m256i     _value;     // ulong[8]

        public:
            explicit Bool4 (const __m256i &val)             __NE___ : _value{val} {}
            explicit Bool4 (const __m256d &val)             __NE___ : _value{_mm256_castpd_si256(val)} {}

            ND_ Bool4  operator | (const Bool4 &rhs)        C_NE___ { return Bool4{ _mm256_or_si256(  _value, rhs._value )}; }
            ND_ Bool4  operator & (const Bool4 &rhs)        C_NE___ { return Bool4{ _mm256_and_si256( _value, rhs._value )}; }
            ND_ Bool4  operator ^ (const Bool4 &rhs)        C_NE___ { return Bool4{ _mm256_xor_si256( _value, rhs._value )}; }
            ND_ Bool4  operator ~ ()                        C_NE___ { return Bool4{ _mm256_andnot_si256( _value, _mm256_set1_epi64x(-1) )}; }
            ND_ bool  operator [] (usize i)                 C_NE___ { ASSERT( i < 4 );  return _value.m256i_u64[i] == UMax; }

            ND_ bool  All ()                                C_NE___ { return _mm256_movemask_epi8( _value ) == 0xFFFF'FFFF; }
            ND_ bool  Any ()                                C_NE___ { return _mm256_movemask_epi8( _value ) != 0; }
            ND_ bool  None ()                               C_NE___ { return _mm256_movemask_epi8( _value ) == 0; }

        //  ND_ bool  All ()                                C_NE___ { alignas(__m256i) ulong u[4];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] & u[1] & u[2] & u[3]) == UMax; }
        //  ND_ bool  Any ()                                C_NE___ { alignas(__m256i) ulong u[4];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] | u[1] | u[2] | u[3]) == UMax; }
        //  ND_ bool  None ()                               C_NE___ { alignas(__m256i) ulong u[4];  _mm256_store_si256( OUT reinterpret_cast<__m256i *>(u), _value );  return (u[0] | u[1] | u[2] | u[3]) == 0; }
        };


    // variables
    private:
        __m256d     _value;     // double[4]


    // methods
    public:
        SimdDouble4 ()                                      __NE___ : _value{ _mm256_setzero_pd() } {}
        explicit SimdDouble4 (double v)                     __NE___ : _value{ _mm256_set1_pd( v )} {}
        explicit SimdDouble4 (const double *v)              __NE___ : _value{ _mm256_loadu_pd( v )} {}
        explicit SimdDouble4 (const __m256d &v)             __NE___ : _value{ v } {}
        SimdDouble4 (double x, double y, double z, double w)__NE___ : _value{ _mm256_set_pd( x, y, z, w )} {}

        ND_ Self  operator + (const Self &rhs)              C_NE___ { return Add( rhs ); }
        ND_ Self  operator - (const Self &rhs)              C_NE___ { return Sub( rhs ); }
        ND_ Self  operator * (const Self &rhs)              C_NE___ { return Mul( rhs ); }
        ND_ Self  operator / (const Self &rhs)              C_NE___ { return Div( rhs ); }

        ND_ Self  operator &  (const Self &rhs)             C_NE___ { return And( rhs ); }
        ND_ Self  operator |  (const Self &rhs)             C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^  (const Self &rhs)             C_NE___ { return Xor( rhs ); }

        ND_ Bool4  operator == (const Self &rhs)            C_NE___ { return Equal( rhs ); }
        ND_ Bool4  operator != (const Self &rhs)            C_NE___ { return NotEqual( rhs ); }
        ND_ Bool4  operator >  (const Self &rhs)            C_NE___ { return Greater( rhs ); }
        ND_ Bool4  operator <  (const Self &rhs)            C_NE___ { return Less( rhs ); }
        ND_ Bool4  operator >= (const Self &rhs)            C_NE___ { return GEqual( rhs ); }
        ND_ Bool4  operator <= (const Self &rhs)            C_NE___ { return LEqual( rhs ); }

        ND_ Value_t         GetX ()                         C_NE___ { return _mm256_cvtsd_f64( _value ); }

        ND_ __m256d const&  Get ()                          C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                     C_NE___ { return Self{ _mm256_add_pd( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                     C_NE___ { return Self{ _mm256_sub_pd( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                     C_NE___ { return Self{ _mm256_mul_pd( _value, rhs._value )}; }
        ND_ Self  Div (const Self &rhs)                     C_NE___ { return Self{ _mm256_div_pd( _value, rhs._value )}; }
        ND_ Self  Min (const Self &rhs)                     C_NE___ { return Self{ _mm256_min_pd( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                     C_NE___ { return Self{ _mm256_max_pd( _value, rhs._value )}; }

        ND_ Self  And (const Self &rhs)                     C_NE___ { return Self{ _mm256_and_pd( _value, rhs._value )}; }
        ND_ Self  Or  (const Self &rhs)                     C_NE___ { return Self{ _mm256_or_pd( _value, rhs._value )}; }
        ND_ Self  Xor (const Self &rhs)                     C_NE___ { return Self{ _mm256_xor_pd( _value, rhs._value )}; }
        ND_ Self  AndNot (const Self &rhs)                  C_NE___ { return Self{ _mm256_andnot_pd( _value, rhs._value )}; } // !a & b

        // returns { a[0] + a[1],  a[2] + a[3],  b[0] + b[1],  b[2] + b[3] }
        ND_ Self  HAdd (const Self &rhs)                    C_NE___ { return Self{ _mm256_hadd_pd( _value, rhs._value )}; }

        // returns { a[0] - b[0], a[1] + b[1], a[2] - b[2], a[3] + b[3] }
        ND_ Self  AddSub (const Self &rhs)                  C_NE___ { return Self{ _mm256_addsub_pd( _value, rhs._value )}; }

        ND_ Self  Sqrt ()                                   C_NE___ { return Self{ _mm256_sqrt_pd( _value )}; }

        ND_ Self  Abs ()                                    C_NE___
        {
            __m256d signmask = _mm256_set1_pd( -0.0 );
            return Self{ _mm256_andnot_pd( signmask, _value )};
        }

        // ordered - comparison with NaN returns false
        ND_ Bool4  Equal    (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
        ND_ Bool4  NotEqual (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
        ND_ Bool4  Greater  (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
        ND_ Bool4  Less     (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
        ND_ Bool4  GEqual   (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
        ND_ Bool4  LEqual   (const Self &rhs)               C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }

        // unordered - comparison with NaN returns true
        ND_ Bool4  EqualU    (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
        ND_ Bool4  NotEqualU (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
        ND_ Bool4  GreaterU  (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
        ND_ Bool4  LessU     (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
        ND_ Bool4  GEqualU   (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
        ND_ Bool4  LEqualU   (const Self &rhs)              C_NE___ { return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }


        ND_ Self  Floor ()                                  C_NE___ { return Self{ _mm256_floor_pd( _value )}; }
        ND_ Self  Ceil ()                                   C_NE___ { return Self{ _mm256_ceil_pd( _value )}; }

        template <int Mode>
        ND_ Self  Round ()                                  C_NE___ { return Self{ _mm256_round_pd( _value, Mode )}; }

        ND_ SimdDouble2  Dot (const Self &rhs)              C_NE___
        {
            __m256d xy      = _mm256_mul_pd( _value, rhs._value );
            __m256d temp    = _mm256_hadd_pd( xy, xy );
            __m128d hi128   = _mm256_extractf128_pd( temp, 1 );
            return SimdDouble2{ _mm_add_pd( _mm256_extractf128_pd( temp, 0 ), hi128 )};
        }

        ND_ static Self  DotV4 (const Self x[4], const Self y[4]) __NE___
        {
            __m256d xy0 = _mm256_mul_pd( x[0]._value, y[0]._value );
            __m256d xy1 = _mm256_mul_pd( x[1]._value, y[1]._value );
            __m256d xy2 = _mm256_mul_pd( x[2]._value, y[2]._value );
            __m256d xy3 = _mm256_mul_pd( x[3]._value, y[3]._value );

            // low to high: xy00+xy01 xy10+xy11 xy02+xy03 xy12+xy13
            __m256d temp01  = _mm256_hadd_pd( xy0, xy1 );   

            // low to high: xy20+xy21 xy30+xy31 xy22+xy23 xy32+xy33
            __m256d temp23  = _mm256_hadd_pd( xy2, xy3 );

            // low to high: xy02+xy03 xy12+xy13 xy20+xy21 xy30+xy31
            __m256d swapped = _mm256_permute2f128_pd( temp01, temp23, 0x21 );

            // low to high: xy00+xy01 xy10+xy11 xy22+xy23 xy32+xy33
            __m256d blended = _mm256_blend_pd( temp01, temp23, 0b1100 );

            return Self{ _mm256_add_pd( swapped, blended )};
        }

        template <uint Idx>
        ND_ SimdDouble2 ToDouble2 ()                        C_NE___ { STATIC_ASSERT( Idx < 2 );  return SimdDouble2{ _mm256_extractf128_pd( _value, Idx )}; }

        ND_ Array_t     ToArray ()                          C_NE___ { Array_t arr;  _mm256_storeu_pd( OUT arr.data(), _value );  return arr; }
            void        ToArray (OUT Value_t* dst)          C_NE___ { _mm256_storeu_pd( OUT dst, _value ); }
            void        ToAlignedArray (OUT Value_t* dst)   C_NE___ { CheckPointerCast<__m256d>( dst );  _mm256_store_pd( OUT dst, _value ); }

        template <typename DstType>
        ND_ auto        Cast ()                             C_NE___;
    };

#endif // AE_SIMD_AVX >= 1
//-----------------------------------------------------------------------------

#if AE_SIMD_AVX >= 2


    //
    // 256 bit int (AVX 2)
    //
    struct Int256b
    {
    // types
    public:
        using Self  = Int256b;


    // variables
    private:
        __m256i     _value;


    // methods
    public:
        Int256b ()                                      __NE___ : _value{ _mm256_setzero_si256() } {}
        explicit Int256b (__m256i v)                    __NE___ : _value{ v } {}

        ND_ Self  operator & (const Self &rhs)          C_NE___ { return And( rhs ); }
        ND_ Self  operator | (const Self &rhs)          C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^ (const Self &rhs)          C_NE___ { return Xor( rhs ); }

        ND_ Self  And (const Self &rhs)                 C_NE___ { return Self{ _mm256_and_si256( _value, rhs._value )}; }
        ND_ Self  Or  (const Self &rhs)                 C_NE___ { return Self{ _mm256_or_si256( _value, rhs._value )}; }
        ND_ Self  Xor (const Self &rhs)                 C_NE___ { return Self{ _mm256_xor_si256( _value, rhs._value )}; }
        ND_ Self  AndNot (const Self &rhs)              C_NE___ { return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // !a & b

        ND_ __m256i const&  Get ()                      C_NE___ { return _value; }

        // shift in bytes
        template <int ShiftBytes> ND_ Self  LShiftB ()  C_NE___ { return Self{ _mm256_slli_si256( _value, ShiftBytes )}; }
        template <int ShiftBytes> ND_ Self  RShiftB ()  C_NE___ { return Self{ _mm256_srli_si256( _value, ShiftBytes )}; }

        template <typename T>
        ND_ auto    ToArray ()                          C_NE___
        {
            STATIC_ASSERT( IsInteger<T> );
            StaticArray< T, sizeof(__m256i) / sizeof(T) >   arr;
            if constexpr( sizeof(T) == 1 )  _mm256_storeu_epi8(   OUT static_cast<void*>(arr.data()), _value );
            if constexpr( sizeof(T) == 2 )  _mm256_storeu_epi16(  OUT static_cast<void*>(arr.data()), _value );
            if constexpr( sizeof(T) == 4 )  _mm256_storeu_epi32(  OUT static_cast<void*>(arr.data()), _value );
            if constexpr( sizeof(T) == 8 )  _mm256_storeu_epi64(  OUT static_cast<void*>(arr.data()), _value );
            return arr;
        }
    };



    //
    // 256 bit integer (AVX 2)
    //
    template <typename IntType>
    struct SimdTInt256
    {
        STATIC_ASSERT(( IsSameTypes<IntType, Int128b> or IsInteger<IntType> ));

    // types
    public:
        using Value_t   = IntType;
        using Self      = SimdTInt256< IntType >;
        using Bool256b  = SimdTInt256< IntType >;
        using Native_t  = __m256i;

        static constexpr uint   count   = sizeof(Native_t) / sizeof(IntType);
        using Array_t                   = StaticArray< Value_t, count >;
        STATIC_ASSERT( sizeof(Array_t) == sizeof(Native_t) );


    // variables
    private:
        Native_t    _value;     // int128[2], int64[4], int32[8], int16[16], int8[32]

        static constexpr bool   is8   = sizeof( IntType ) == 1;
        static constexpr bool   is16  = sizeof( IntType ) == 2;
        static constexpr bool   is32  = sizeof( IntType ) == 4;
        static constexpr bool   is64  = sizeof( IntType ) == 8;
        static constexpr bool   is128 = sizeof( IntType ) == 16;

        static constexpr bool   isU8  = IsSameTypes< IntType, ubyte >;
        static constexpr bool   isU16 = IsSameTypes< IntType, ushort >;
        static constexpr bool   isU32 = IsSameTypes< IntType, uint >;
        static constexpr bool   isU64 = IsSameTypes< IntType, ulong >;

        static constexpr bool   isI8  = IsSameTypes< IntType, sbyte >;
        static constexpr bool   isI16 = IsSameTypes< IntType, sshort >;
        static constexpr bool   isI32 = IsSameTypes< IntType, sint >;
        static constexpr bool   isI64 = IsSameTypes< IntType, slong >;


    // methods
    public:
        SimdTInt256 ()                              __NE___ : _value{ _mm256_setzero_si256()} {}
        explicit SimdTInt256 (const Native_t &v)    __NE___ : _value{ v } {}

        explicit SimdTInt256 (IntType v)            __NE___
        {
            if constexpr( isI8 )    _value = _mm256_set1_epi8(   v );   else
            if constexpr( isU8 )    _value = _mm256_set1_epu8(   v );   else
            if constexpr( is16 )    _value = _mm256_set1_epi16(  v );   else
            if constexpr( is32 )    _value = _mm256_set1_epi32(  v );   else
            if constexpr( is64 )    _value = _mm256_set1_epi64x( v );   else
            if constexpr( is128 )   _value = _mm256_set_m128( v, v );   else
                                    _value = v; // compilation error
        }

        explicit SimdTInt256 (Base::_hidden_::_UMax) __NE___ :
            SimdTInt256{ ~IntType{0} }
        {}

        explicit SimdTInt256 (const IntType* v)     __NE___
        {
            if constexpr( isI8 )    _value = _mm256_loadu_epi8(   v );  else
            if constexpr( isU8 )    _value = _mm256_loadu_epu8(   v );  else
            if constexpr( is16 )    _value = _mm256_loadu_epi16(  v );  else
            if constexpr( is32 )    _value = _mm256_loadu_epi32(  v );  else
            if constexpr( is64 )    _value = _mm256_loadu_epi64x( v );  else
                                    _value = v; // compilation error
        }

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, ubyte > or IsSameTypes< T, sbyte >>
                 >
        SimdTInt256 (T v00, T v01, T v02, T v03,
                     T v04, T v05, T v06, T v07,
                     T v08, T v09, T v10, T v11,
                     T v12, T v13, T v14, T v15,
                     T v16, T v17, T v18, T v19,
                     T v20, T v21, T v22, T v23,
                     T v24, T v25, T v26, T v27,
                     T v28, T v29, T v30, T v31)        __NE___ :
            _value{ _mm256_set_epi8( v00, v01, v02, v03, v04, v05, v06, v07,
                                     v08, v09, v10, v11, v12, v13, v14, v15,
                                     v16, v17, v18, v19, v20, v21, v22, v23,
                                     v24, v25, v26, v27, v28, v29, v30, v31)} {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, ushort > or IsSameTypes< T, sshort >>
                 >
        SimdTInt256 (T v00, T v01, T v02, T v03,
                     T v04, T v05, T v06, T v07,
                     T v08, T v09, T v10, T v11,
                     T v12, T v13, T v14, T v15)        __NE___ :
            _value{ _mm256_set_epi16( v00, v01, v02, v03, v04, v05, v06, v07,
                                      v08, v09, v10, v11, v12, v13, v14, v15 )} {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, uint > or IsSameTypes< T, sint >>
                 >
        SimdTInt256 (T v0, T v1, T v2, T v3,
                     T v4, T v5, T v6, T v7)            __NE___ :
            _value{ _mm256_set_epi32( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, ulong > or IsSameTypes< T, slong >>
                 >
        SimdTInt256 (T v0, T v1, T v2, T v3)            __NE___ :
            _value{ _mm256_set_epi64x( v0, v1, v2, v3 )} {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, __m128i >>
                 >
        SimdTInt256 (T v0, T v1)                        __NE___ :
            _value{ _mm256_set_m128( v0, v1 )} {}

        ND_ Self  operator - ()                         C_NE___ { return Negative(); }
        ND_ Self  operator ~ ()                         C_NE___ { return Not(); }

        ND_ Self  operator + (const Self &rhs)          C_NE___ { return Add( rhs ); }
        ND_ Self  operator - (const Self &rhs)          C_NE___ { return Sub( rhs ); }
        ND_ Self  operator * (const Self &rhs)          C_NE___ { return Mul( rhs ); }
        ND_ Self  operator / (const Self &rhs)          C_NE___ { return Div( rhs ); }

        ND_ Self  operator & (const Self &rhs)          C_NE___ { return And( rhs ); }
        ND_ Self  operator | (const Self &rhs)          C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^ (const Self &rhs)          C_NE___ { return Xor( rhs ); }

        ND_ Self  operator << (int shift)               C_NE___ { return LShift( shift ); }
        ND_ Self  operator << (const Self &rhs)         C_NE___ { return LShift( rhs ); }
        ND_ Self  operator >> (int shift)               C_NE___ { return RShift( shift ); }
        ND_ Self  operator >> (const Self &rhs)         C_NE___ { return RShift( rhs ); }

        ND_ Bool256b  operator == (const Self &rhs)     C_NE___ { return Equal( rhs ); }
        ND_ Bool256b  operator != (const Self &rhs)     C_NE___ { return NotEqual( rhs ); }
        ND_ Bool256b  operator >  (const Self &rhs)     C_NE___ { return Greater( rhs ); }
        ND_ Bool256b  operator <  (const Self &rhs)     C_NE___ { return Less( rhs ); }
        ND_ Bool256b  operator >= (const Self &rhs)     C_NE___ { return GEqual( rhs ); }
        ND_ Bool256b  operator <= (const Self &rhs)     C_NE___ { return LEqual( rhs ); }

        ND_ Native_t const&  Get ()                     C_NE___ { return _value; }

        ND_ Self    Negative ()                         C_NE___ { return Self{0}.Sub( *this ); }        // TODO: optimize ?

        ND_ Self  Add (const Self &rhs)                 C_NE___
        {
            if constexpr( is8 )     return Self{ _mm256_add_epi8(  _value, rhs._value )};
            if constexpr( is16 )    return Self{ _mm256_add_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_add_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_add_epi64( _value, rhs._value )};
        }

        ND_ Self  AddSaturate (const Self &rhs)         C_NE___
        {
            if constexpr( isI8 )    return Self{ _mm256_adds_epi8(  _value, rhs._value )};
            if constexpr( isU8 )    return Self{ _mm256_adds_epu8(  _value, rhs._value )};
            if constexpr( isI16 )   return Self{ _mm256_adds_epi16( _value, rhs._value )};
            if constexpr( isU16 )   return Self{ _mm256_adds_epu16( _value, rhs._value )};
        }

        ND_ Self  Sub (const Self &rhs)                 C_NE___
        {
            if constexpr( is8 )     return Self{ _mm256_sub_epi8(  _value, rhs._value )};
            if constexpr( is16 )    return Self{ _mm256_sub_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_sub_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_sub_epi64( _value, rhs._value )};
        }

        ND_ Self  SubSaturate (const Self &rhs)         C_NE___
        {
            if constexpr( isI8 )    return Self{ _mm256_subs_epi8(  _value, rhs._value )};
            if constexpr( isU8 )    return Self{ _mm256_subs_epu8(  _value, rhs._value )};
            if constexpr( isI16 )   return Self{ _mm256_subs_epi16( _value, rhs._value )};
            if constexpr( isU16 )   return Self{ _mm256_subs_epu16( _value, rhs._value )};
        }

        ND_ Self  Mul (const Self &rhs)                 C_NE___
        {
            if constexpr( isI32 )   return Self{ _mm256_mul_epi32( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ _mm256_mul_epu32( _value, rhs._value )};
        }


        ND_ Self    Not ()                              C_NE___ { return AndNot( Self{UMax} ); }
        ND_ Self    And (const Self &rhs)               C_NE___ { return Self{ _mm256_and_si256( _value, rhs._value )}; }
        ND_ Self    Or  (const Self &rhs)               C_NE___ { return Self{ _mm256_or_si256( _value, rhs._value )}; }
        ND_ Self    Xor (const Self &rhs)               C_NE___ { return Self{ _mm256_xor_si256( _value, rhs._value )}; }
        ND_ Self    AndNot (const Self &rhs)            C_NE___ { return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // ~a & b


        ND_ Self  Min (const Self &rhs)                 C_NE___
        {
            if constexpr( isI8 )    return Self{ _mm256_min_epi8(  _value, rhs._value )};
            if constexpr( isU8 )    return Self{ _mm256_min_epu8(  _value, rhs._value )};
            if constexpr( isI16 )   return Self{ _mm256_min_epi16( _value, rhs._value )};
            if constexpr( isU16 )   return Self{ _mm256_min_epu16( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ _mm256_min_epi32( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ _mm256_min_epu32( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ _mm256_min_epi64( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ _mm256_min_epu64( _value, rhs._value )};
        }

        ND_ Self  Max (const Self &rhs)                 C_NE___
        {
            if constexpr( isI8 )    return Self{ _mm256_max_epi8(  _value, rhs._value )};
            if constexpr( isU8 )    return Self{ _mm256_max_epu8(  _value, rhs._value )};
            if constexpr( isI16 )   return Self{ _mm256_max_epi16( _value, rhs._value )};
            if constexpr( isU16 )   return Self{ _mm256_max_epu16( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ _mm256_max_epi32( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ _mm256_max_epu32( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ _mm256_max_epi64( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ _mm256_max_epu64( _value, rhs._value )};
        }

        ND_ Bool256b  Equal (const Self &rhs)           C_NE___
        {
            if constexpr( is8 )     return Self{ _mm256_cmpeq_epi8(  _value, rhs._value )};
            if constexpr( is16 )    return Self{ _mm256_cmpeq_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_cmpeq_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_cmpeq_epi64( _value, rhs._value )};
        }

        ND_ Bool256b  Greater (const Self &rhs)         C_NE___
        {
            if constexpr( is8 )     return Self{ _mm256_cmpgt_epi8(  _value, rhs._value )};
            if constexpr( is16 )    return Self{ _mm256_cmpgt_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_cmpgt_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_cmpgt_epi64( _value, rhs._value )};
        }

        ND_ Bool256b  Less (const Self &rhs)            C_NE___
        {
            if constexpr( is8 )     return Self{ _mm256_cmplt_epi8(  _value, rhs._value )};
            if constexpr( is16 )    return Self{ _mm256_cmplt_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_cmplt_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_cmplt_epi64( _value, rhs._value )};
        }


        // zero on overflow
        ND_ Self  LShift (int shift)                    C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_slli_epi16( _value, shift )};
            if constexpr( is32 )    return Self{ _mm256_slli_epi32( _value, shift )};
            if constexpr( is64 )    return Self{ _mm256_slli_epi64( _value, shift )};
        }

        ND_ Self  LShift (const Self &rhs)              C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_sll_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_sll_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_sll_epi64( _value, rhs._value )};
        }


        // zero on overflow
        ND_ Self  RShift (int shift)                    C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_srli_epi16( _value, shift )};
            if constexpr( is32 )    return Self{ _mm256_srli_epi32( _value, shift )};
            if constexpr( is64 )    return Self{ _mm256_srli_epi64( _value, shift )};
        }

        ND_ Self  RShift (const Self &rhs)              C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_srl_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_srl_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_srl_epi64( _value, rhs._value )};
        }


        // max on overflow
        ND_ Self  RShiftA (int shift)                   C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_srai_epi16( _value, shift )};
            if constexpr( is32 )    return Self{ _mm256_srai_epi32( _value, shift )};
            if constexpr( is64 )    return Self{ _mm256_srai_epi64( _value, shift )};
        }

        ND_ Self  RShiftA (const Self &rhs)             C_NE___
        {
            if constexpr( is16 )    return Self{ _mm256_sra_epi16( _value, rhs._value )};
            if constexpr( is32 )    return Self{ _mm256_sra_epi32( _value, rhs._value )};
            if constexpr( is64 )    return Self{ _mm256_sra_epi64( _value, rhs._value )};
        }


        ND_ Array_t ToArray ()                          C_NE___
        {
            Array_t arr;
            ToArray( OUT arr.data() );
            return arr;
        }

        void        ToArray (OUT IntType* dst)          C_NE___
        {
            if constexpr( is8 )     _mm256_storeu_epi8(  OUT static_cast<void*>(dst), _value );
            if constexpr( is16 )    _mm256_storeu_epi16( OUT static_cast<void*>(dst), _value );
            if constexpr( is32 )    _mm256_storeu_epi32( OUT static_cast<void*>(dst), _value );
            if constexpr( is64 )    _mm256_storeu_epi64( OUT static_cast<void*>(dst), _value );
        }

        void        ToAlignedArray (OUT Value_t* dst)   C_NE___
        {
            CheckPointerCast<__m256i>( dst );
            _mm256_store_si256( OUT reinterpret_cast<__m256i *>(dst), _value );
        }

        ND_ bool  All ()                                C_NE___ { return _mm256_movemask_epi8( _value ) == 0xFFFF'FFFF; }
        ND_ bool  Any ()                                C_NE___ { return _mm256_movemask_epi8( _value ) != 0; }
        ND_ bool  None ()                               C_NE___ { return _mm256_movemask_epi8( _value ) == 0; }

        /*// non SIMD
        ND_ bool  All ()                                C_NE___
        {
            alignas(__m256i) Array_t    arr;
            _mm256_store_si256( OUT reinterpret_cast<__m256i *>(arr.data()), _value );

            IntType     accum = UMax;
            for (uint i = 0; i < count; ++i) {
                accum &= arr[i];
            }
            return accum == UMax;
        }

        ND_ bool  Any ()                                C_NE___
        {
            alignas(__m256i) Array_t    arr;
            _mm256_store_si256( OUT reinterpret_cast<__m256i *>(arr.data()), _value );

            IntType     accum = UMax;
            for (uint i = 0; i < count; ++i) {
                accum |= arr[i];
            }
            return accum == UMax;
        }

        ND_ bool  None ()                               C_NE___
        {
            alignas(__m256i) Array_t    arr;
            _mm256_store_si256( OUT reinterpret_cast<__m256i *>(arr.data()), _value );

            IntType     accum = UMax;
            for (uint i = 0; i < count; ++i) {
                accum |= arr[i];
            }
            return accum == 0;
        }*/

        // TODO: AES

        template <typename IT>
        ND_ SimdTInt256<IT> &       Cast ()             __NE___ { return reinterpret_cast< SimdTInt256<IT> &>(*this); }

        template <typename IT>
        ND_ SimdTInt256<IT> const&  Cast ()             C_NE___ { return reinterpret_cast< SimdTInt256<IT> const &>(*this); }

        ND_ Int256b const&          Cast ()             C_NE___ { return reinterpret_cast< Int256b const &>(*this); }

        template <typename DstType>
        ND_ auto                    Cast ()             C_NE___;
    };

#endif // AE_SIMD_AVX >= 2
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 1

    template <typename DstType>
    auto  SimdFloat8::Cast ()                           C_NE___
    {
        #if AE_SIMD_AVX >= 2
        if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
            return DstType{ _mm256_castps_si256( _value )};
        #endif

        if constexpr( IsSameTypes< DstType, SimdDouble4 >)
            return SimdDouble4{ _mm256_castps_pd( _value )};
    }

    template <typename DstType>
    auto  SimdDouble4::Cast ()                          C_NE___
    {
        #if AE_SIMD_AVX >= 2
        if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
            return DstType{ _mm256_castpd_si256( _value )};
        #endif

        if constexpr( IsSameTypes< DstType, SimdFloat8 >)
            return SimdFloat8{ _mm256_castpd_ps( _value )};
    }

  #if AE_SIMD_AVX >= 2
    template <typename IntType>
    template <typename DstType>
    auto  SimdTInt256<IntType>::Cast ()                 C_NE___
    {
        if constexpr( IsSameTypes< DstType, SimdDouble4 >)
            return SimdDouble4{ _mm256_castsi256_pd( _value )};

        if constexpr( IsSameTypes< DstType, SimdFloat8 >)
            return SimdFloat8{ _mm256_castsi256_ps( _value )};
    }
  #endif

#endif // AE_SIMD_AVX >= 1

} // AE::Math

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
 Neon:
    https://developer.arm.com/architectures/instruction-sets/intrinsics/

 Features:
    __ARM_FEATURE_BF16
    __ARM_FEATURE_SVE
    __ARM_FEATURE_SVE_BF16
    __ARM_FEATURE_BF16_SCALAR_ARITHMETIC

TODO:
    intrinsics: asimd, aes, pmull sha1 sha2 crc32, fphp, asimdhp, asimdrdm, asimddp
*/

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/Float16.h"

#if AE_SIMD_NEON

namespace AE::Math
{
    struct SimdHalf4;
    template <typename IntType> struct SimdTInt64;
    using SimdByte8     = SimdTInt64< sbyte >;
    using SimdUByte8    = SimdTInt64< ubyte >;
    using SimdShort4    = SimdTInt64< sshort >;
    using SimdUShort4   = SimdTInt64< ushort >;
    using SimdInt2      = SimdTInt64< sint >;
    using SimdUInt2     = SimdTInt64< uint >;

    struct SimdFloat4;
    struct SimdHalf8;
    template <typename IntType> struct SimdTInt128;
    using SimdByte16    = SimdTInt128< sbyte >;
    using SimdUByte16   = SimdTInt128< ubyte >;
    using SimdShort8    = SimdTInt128< sshort >;
    using SimdUShort8   = SimdTInt128< ushort >;
    using SimdInt4      = SimdTInt128< sint >;
    using SimdUInt4     = SimdTInt128< uint >;
    using SimdLong2     = SimdTInt128< slong >;
    using SimdULong2    = SimdTInt128< ulong >;

# ifdef __aarch64__
    struct SimdDouble2;
# endif


# if AE_SIMD_NEON_HALF

    //
    // 64 bit half float (Neon)
    //
#   define AE_SIMD_SimdHalf4
    struct SimdHalf4
    {
    // types
    public:
        using Value_t   = float16_t;
        using Self      = SimdHalf4;
        using Native_t  = float16x4_t;

        struct Bool4
        {
        private:
            uint16x4_t  _value;

        public:
            explicit Bool4 (const uint16x4_t &v)        __NE___ : _value{v} {}
            Bool4 (const SimdUShort4 &v)                __NE___;

            ND_ Bool4  operator | (const Bool4 &rhs)    C_NE___ { return Bool4{ vorr_u16( _value, rhs._value )}; }
            ND_ Bool4  operator & (const Bool4 &rhs)    C_NE___ { return Bool4{ vand_u16( _value, rhs._value )}; }
            ND_ Bool4  operator ^ (const Bool4 &rhs)    C_NE___ { return Bool4{ veor_u16( _value, rhs._value )}; }
            ND_ Bool4  operator ~ ()                    C_NE___ { return Bool4{ vmvn_u16( _value )}; }
            ND_ bool   operator [] (usize i)            C_NE___ { ASSERT( i < 4 );  return _value[i] == UMax; }

            ND_ bool  All ()                            C_NE___ { return uint16_t(_value[0] & _value[1] & _value[2] & _value[3]) == UMax; }
            ND_ bool  Any ()                            C_NE___ { return uint16_t(_value[0] | _value[1] | _value[2] | _value[3]) == UMax; }
            ND_ bool  None ()                           C_NE___ { return uint16_t(_value[0] | _value[1] | _value[2] | _value[3]) == 0; }
        };


    // variables
    private:
        Native_t    _value;     // half[4]

        static constexpr uint   count = 4;


    // methods
    public:
        SimdHalf4 ()                                        __NE___ : _value{}                      {}
        explicit SimdHalf4 (Value_t val)                    __NE___ : _value{ vdup_n_f16( val )}    {}
        explicit SimdHalf4 (const Value_t* ptr)             __NE___ : _value{ vld1_f16( ptr )}      { ASSERT( ptr != null ); }
        explicit SimdHalf4 (const Native_t &val)            __NE___ : _value{ val }                 {}

        ND_ Self  operator - ()                             C_NE___ { return Negative(); }
        ND_ Self  operator +  (const Self &rhs)             C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)             C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)             C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)             C_NE___ { return Div( rhs ); }

        ND_ Value_t         operator [] (usize i)           C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                          C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                     C_NE___ { return Self{ vadd_f16( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                     C_NE___ { return Self{ vsub_f16( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                     C_NE___ { return Self{ vmul_f16( _value, rhs._value )}; }
        ND_ Self  Div (const Self &rhs)                     C_NE___ { return Self{ vdiv_f16( _value, rhs._value )}; }

        ND_ Self  Mul (Value_t rhs)                         C_NE___ { return Self{ vmul_n_f16( _value, rhs )}; }

        ND_ Self  Min (const Self &rhs)                     C_NE___ { return Self{ vmin_f16( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                     C_NE___ { return Self{ vmax_f16( _value, rhs._value )}; }

        ND_ Self  Abs ()                                    C_NE___ { return Self{ vabs_f16( _value )}; }                           // abs(x)
        ND_ Self  Negative ()                               C_NE___ { return Self{ vneg_f16( _value )}; }                           // -x
        ND_ Self  Reciprocal ()                             C_NE___ { return Self{ vrecpe_f16( _value )}; }                         // 1 / x
        ND_ Self  RSqrt ()                                  C_NE___ { return Self{ vrsqrte_f16( _value )}; }                        // 1 / sqrt(x)
        ND_ Self  FastSqrt ()                               C_NE___ { return Self{ vmul_f16( _value, vrsqrte_f16( _value ))}; }     // x / sqrt(x)

        ND_ Self  FMAdd  (const Self &b, const Self &c)     C_NE___ { return Self{ vfma_f16(   _value, b._value, c._value )}; }     // (a * b) + c
        ND_ Self  FMAdd  (const Self &b, Value_t c)         C_NE___ { return Self{ vfma_n_f16( _value, b._value, c )}; }            //
        ND_ Self  FMSub  (const Self &b, const Self &c)     C_NE___ { return Self{ vfms_f16(   _value, b._value, c._value )}; }     // (a * b) - c
        ND_ Self  FMSub  (const Self &b, Value_t c)         C_NE___ { return Self{ vfms_n_f16( _value, b._value, c )}; }            //

        template <uint Lane> ND_ Self  Mul (const Self &rhs)C_NE___ { STATIC_ASSERT( Lane < 4 );  return Self{ vmul_lane_f16( _value, rhs, Lane )}; }   // a * b[lane]

        ND_ Bool4  operator == (const Self &rhs)            C_NE___ { return Equal( rhs ); }
        ND_ Bool4  operator != (const Self &rhs)            C_NE___ { return NotEqual( rhs ); }

        ND_ Bool4  Equal (const Self &rhs)                  C_NE___ { return Bool4{ vceq_f16( _value, rhs._value )}; }
        ND_ Bool4  NotEqual (const Self &rhs)               C_NE___ { return ~Equal( rhs ); }

        template <typename T> ND_ EnableIf< IsSameTypes<T,float>,  SimdFloat4>   Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,int>,    SimdInt4>     Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,uint>,   SimdUInt4>    Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort4>   Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort4>  Cast ()    C_NE___;

        template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort4>   BitCast () C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort4>  BitCast () C_NE___;
    };

# endif // AE_SIMD_NEON_HALF



    //
    // 64 bit integer (Neon)
    //
#   define AE_SIMD_SimdTInt64
    template <typename IntType>
    struct SimdTInt64
    {
        STATIC_ASSERT( IsInteger<IntType> );

    // types
    public:
        using Value_t   = IntType;
        using Self      = SimdTInt64< IntType >;
        using Bool64b   = SimdTInt64< IntType >;
        using Native_t  = Conditional< IsSameTypes< IntType, sbyte >, int8x8_t,
                            Conditional< IsSameTypes< IntType, ubyte >, uint8x8_t,
                                Conditional< IsSameTypes< IntType, sshort >, int16x4_t,
                                    Conditional< IsSameTypes< IntType, ushort >, uint16x4_t,
                                        Conditional< IsSameTypes< IntType, sint >, int32x2_t,
                                            Conditional< IsSameTypes< IntType, uint >, uint32x2_t, void >>>>>>;
        STATIC_ASSERT( sizeof(Native_t)*8 == 64 );


    // variables
    private:
        Native_t    _value;

        static constexpr bool   isU8    = IsSameTypes< IntType, ubyte >;
        static constexpr bool   isU16   = IsSameTypes< IntType, ushort >;
        static constexpr bool   isU32   = IsSameTypes< IntType, uint >;

        static constexpr bool   isI8    = IsSameTypes< IntType, sbyte >;
        static constexpr bool   isI16   = IsSameTypes< IntType, sshort >;
        static constexpr bool   isI32   = IsSameTypes< IntType, sint >;

        static constexpr uint   count   = sizeof(Native_t) / sizeof(IntType);


    // methods
    public:
        SimdTInt64 ()                                   __NE___ : _value{} {}

        explicit SimdTInt64 (Base::_hidden_::_UMax)     __NE___ : SimdTInt64{ ~IntType{0} } {}
        explicit SimdTInt64 (const Native_t &val)       __NE___ : _value{ val } {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, IntType >>
                 >
        explicit SimdTInt64 (T val)                     __NE___
        {
            if constexpr( isU8 )    _value = vdup_n_u8(  val );
            if constexpr( isI8 )    _value = vdup_n_s8(  val );
            if constexpr( isU16 )   _value = vdup_n_u16( val );
            if constexpr( isI16 )   _value = vdup_n_s16( val );
            if constexpr( isU32 )   _value = vdup_n_u32( val );
            if constexpr( isI32 )   _value = vdup_n_s32( val );
        }

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, IntType >>
                 >
        explicit SimdTInt64 (const T* ptr)              __NE___
        {
            ASSERT( ptr != null );
            if constexpr( isU8 )    _value = vld1_u8(  ptr );
            if constexpr( isI8 )    _value = vld1_s8(  ptr );
            if constexpr( isU16 )   _value = vld1_u16( ptr );
            if constexpr( isI16 )   _value = vld1_s16( ptr );
            if constexpr( isU32 )   _value = vld1_u32( ptr );
            if constexpr( isI32 )   _value = vld1_s32( ptr );
        }

        ND_ Self  operator - ()                         C_NE___ { return Negative(); }
        ND_ Self  operator +  (const Self &rhs)         C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)         C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)         C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)         C_NE___ { return Div( rhs ); }

        ND_ Self  operator ~ ()                         C_NE___ { return Not(); }
        ND_ Self  operator & (const Self &rhs)          C_NE___ { return And( rhs ); }
        ND_ Self  operator | (const Self &rhs)          C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^ (const Self &rhs)          C_NE___ { return Xor( rhs ); }

        ND_ Value_t         operator [] (usize i)       C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                      C_NE___ { return _value; }

        ND_ Self  Negative ()                           C_NE___ // -x
        {
            if constexpr( isI8 )    return Self{ vneg_s8(  _value )};
            if constexpr( isI16 )   return Self{ vneg_s16( _value )};
            if constexpr( isI32 )   return Self{ vneg_s32( _value )};
        }

        ND_ Self  Add (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vadd_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vadd_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vadd_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vadd_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vadd_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vadd_s32( _value, rhs._value )};
        }

        ND_ Self  Sub (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vsub_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vsub_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vsub_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vsub_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vsub_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vsub_s32( _value, rhs._value )};
        }

        ND_ Self  Mul (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vmul_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vmul_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vmul_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vmul_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vmul_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vmul_s32( _value, rhs._value )};
        }

        ND_ Self  Mul (Value_t rhs)                     C_NE___
        {
            if constexpr( isU16 )   return Self{ vmul_n_u16( _value, rhs )};
            if constexpr( isI16 )   return Self{ vmul_n_s16( _value, rhs )};
            if constexpr( isU32 )   return Self{ vmul_n_u32( _value, rhs )};
            if constexpr( isI32 )   return Self{ vmul_n_s32( _value, rhs )};
        }

        ND_ Self  Not ()                                C_NE___ // ~a
        {
            if constexpr( isU8 )    return Self{ vmvn_u8(  _value )};
            if constexpr( isI8 )    return Self{ vmvn_s8(  _value )};
            if constexpr( isU16 )   return Self{ vmvn_u16( _value )};
            if constexpr( isI16 )   return Self{ vmvn_s16( _value )};
            if constexpr( isU32 )   return Self{ vmvn_u32( _value )};
            if constexpr( isI32 )   return Self{ vmvn_s32( _value )};
        }

        ND_ Self  And (const Self &rhs)                 C_NE___ // a & b
        {
            if constexpr( isU8 )    return Self{ vand_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vand_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vand_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vand_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vand_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vand_s32( _value, rhs._value )};
        }

        ND_ Self  Or (const Self &rhs)                  C_NE___ // a | b
        {
            if constexpr( isU8 )    return Self{ vorr_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vorr_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vorr_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vorr_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vorr_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vorr_s32( _value, rhs._value )};
        }

        ND_ Self  Xor (const Self &rhs)                 C_NE___ // a ^ b
        {
            if constexpr( isU8 )    return Self{ veor_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ veor_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ veor_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ veor_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ veor_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ veor_s32( _value, rhs._value )};
        }

        ND_ Self  OrNot (const Self &rhs)               C_NE___ // a | ~b
        {
            if constexpr( isU8 )    return Self{ vorn_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vorn_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vorn_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vorn_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vorn_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vorn_s32( _value, rhs._value )};
        }

        ND_ Self  Min (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vmin_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vmin_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vmin_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vmin_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vmin_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vmin_s32( _value, rhs._value )};
        }

        ND_ Self  Max (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vmax_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vmax_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vmax_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vmax_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vmax_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vmax_s32( _value, rhs._value )};
        }

        ND_ Bool64b  operator == (const Self &rhs)      C_NE___ { return Equal( rhs ); }
        ND_ Bool64b  operator != (const Self &rhs)      C_NE___ { return NotEqual( rhs ); }

        ND_ Bool64b  Equal (const Self &rhs)            C_NE___
        {
            if constexpr( isU8 )    return Bool64b{ vceq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Bool64b{ vceq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Bool64b{ vceq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Bool64b{ vceq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Bool64b{ vceq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Bool64b{ vceq_s32( _value, rhs._value )};
        }

        ND_ Bool64b  NotEqual (const Self &rhs)         C_NE___ { return ~Equal( rhs ); }


        // non SIMD
        ND_ bool  All ()                                C_NE___
        {
            if constexpr( isU8  )   { IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value[i];  return accum == UMax; }
            if constexpr( isU16 )   { IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value[i];  return accum == UMax; }
            if constexpr( isU32 )   return IntType(_value[0] & _value[1]) == UMax;
        }

        ND_ bool  Any ()                                C_NE___
        {
            if constexpr( isU8  )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == UMax; }
            if constexpr( isU16 )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == UMax; }
            if constexpr( isU32 )   return IntType(_value[0] | _value[1]) == UMax;
        }

        ND_ bool  None ()                               C_NE___
        {
            if constexpr( isU8  )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == 0; }
            if constexpr( isU16 )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == 0; }
            if constexpr( isU32 )   return IntType(_value[0] | _value[1]) == 0;
        }
    };



    //
    // 128 bit integer (Neon)
    //
#   define AE_SIMD_SimdTInt128
    template <typename IntType>
    struct SimdTInt128
    {
        STATIC_ASSERT( IsInteger<IntType> );

    // types
    public:
        using Value_t   = IntType;
        using Self      = SimdTInt128< IntType >;
        using Bool128b  = SimdTInt128< IntType >;
        using Native_t  = Conditional< IsSameTypes< IntType, sbyte >, int8x16_t,
                            Conditional< IsSameTypes< IntType, ubyte >, uint8x16_t,
                                Conditional< IsSameTypes< IntType, sshort >, int16x8_t,
                                    Conditional< IsSameTypes< IntType, ushort >, uint16x8_t,
                                        Conditional< IsSameTypes< IntType, sint >, int32x4_t,
                                            Conditional< IsSameTypes< IntType, uint >, uint32x4_t,
                                                Conditional< IsSameTypes< IntType, slong >, int64x2_t,
                                                    Conditional< IsSameTypes< IntType, ulong >, uint64x2_t, void >>>>>>>>;
        STATIC_ASSERT( sizeof(Native_t)*8 == 128 );


    // variables
    private:
        Native_t    _value;

        static constexpr bool   isU8    = IsSameTypes< IntType, ubyte >;
        static constexpr bool   isU16   = IsSameTypes< IntType, ushort >;
        static constexpr bool   isU32   = IsSameTypes< IntType, uint >;
        static constexpr bool   isU64   = IsSameTypes< IntType, ulong >;

        static constexpr bool   isI8    = IsSameTypes< IntType, sbyte >;
        static constexpr bool   isI16   = IsSameTypes< IntType, sshort >;
        static constexpr bool   isI32   = IsSameTypes< IntType, sint >;
        static constexpr bool   isI64   = IsSameTypes< IntType, slong >;

        static constexpr uint   count   = sizeof(Native_t) / sizeof(IntType);


    // methods
    public:
        SimdTInt128 () : _value{} {}

        explicit SimdTInt128 (Base::_hidden_::_UMax)    __NE___ : SimdTInt128{ ~IntType{0} } {}
        explicit SimdTInt128 (const Native_t &val)      __NE___ : _value{ val } {}

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, IntType >>
                 >
        explicit SimdTInt128 (T val)                    __NE___
        {
            if constexpr( isU8 )    _value = vdupq_n_u8(  val );
            if constexpr( isI8 )    _value = vdupq_n_s8(  val );
            if constexpr( isU16 )   _value = vdupq_n_u16( val );
            if constexpr( isI16 )   _value = vdupq_n_s16( val );
            if constexpr( isU32 )   _value = vdupq_n_u32( val );
            if constexpr( isI32 )   _value = vdupq_n_s32( val );
            if constexpr( isU64 )   _value = vdupq_n_u64( val );
            if constexpr( isI64 )   _value = vdupq_n_s64( val );
        }

        template <typename T,
                  typename = EnableIf< IsSameTypes< T, IntType >>
                 >
        explicit SimdTInt128 (const T* ptr)             __NE___
        {
            ASSERT( ptr != null );
            if constexpr( isU8 )    _value = vld1q_u8(  ptr );
            if constexpr( isI8 )    _value = vld1q_s8(  ptr );
            if constexpr( isU16 )   _value = vld1q_u16( ptr );
            if constexpr( isI16 )   _value = vld1q_s16( ptr );
            if constexpr( isU32 )   _value = vld1q_u32( ptr );
            if constexpr( isI32 )   _value = vld1q_s32( ptr );
            if constexpr( isU64 )   _value = vld1q_u64( ptr );
            if constexpr( isI64 )   _value = vld1q_s64( ptr );
        }

        ND_ Self  operator - ()                         C_NE___ { return Negative(); }
        ND_ Self  operator +  (const Self &rhs)         C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)         C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)         C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)         C_NE___ { return Div( rhs ); }

        ND_ Self  operator ~ ()                         C_NE___ { return Not(); }
        ND_ Self  operator & (const Self &rhs)          C_NE___ { return And( rhs ); }
        ND_ Self  operator | (const Self &rhs)          C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^ (const Self &rhs)          C_NE___ { return Xor( rhs ); }

        ND_ Value_t         operator [] (usize i)       C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                      C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vaddq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vaddq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vaddq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vaddq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vaddq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vaddq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vaddq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vaddq_s64( _value, rhs._value )};
        }

        ND_ Self  Sub (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vsubq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vsubq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vsubq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vsubq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vsubq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vsubq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vsubq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vsubq_s64( _value, rhs._value )};
        }

        ND_ Self  Mul (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vmulq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vmulq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vmulq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vmulq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vmulq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vmulq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vmulq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vmulq_s64( _value, rhs._value )};
        }

        ND_ Self  Mul (Value_t rhs)                     C_NE___
        {
            if constexpr( isU16 )   return Self{ vmulq_n_u16( _value, rhs )};
            if constexpr( isI16 )   return Self{ vmulq_n_s16( _value, rhs )};
            if constexpr( isU32 )   return Self{ vmulq_n_u32( _value, rhs )};
            if constexpr( isI32 )   return Self{ vmulq_n_s32( _value, rhs )};
            if constexpr( isU64 )   return Self{ vmulq_n_u64( _value, rhs )};
            if constexpr( isI64 )   return Self{ vmulq_n_s64( _value, rhs )};
        }

        ND_ Self  Not ()                                C_NE___ // ~a
        {
            if constexpr( isU8 )    return Self{ vmvnq_u8(  _value )};
            if constexpr( isI8 )    return Self{ vmvnq_s8(  _value )};
            if constexpr( isU16 )   return Self{ vmvnq_u16( _value )};
            if constexpr( isI16 )   return Self{ vmvnq_s16( _value )};
            if constexpr( isU32 )   return Self{ vmvnq_u32( _value )};
            if constexpr( isI32 )   return Self{ vmvnq_s32( _value )};
            if constexpr( isU64 )   return Self{ vmvnq_u64( _value )};
            if constexpr( isI64 )   return Self{ vmvnq_s64( _value )};
        }

        ND_ Self  And (const Self &rhs)                 C_NE___ // a & b
        {
            if constexpr( isU8 )    return Self{ vandq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vandq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vandq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vandq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vandq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vandq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vandq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vandq_s64( _value, rhs._value )};
        }

        ND_ Self  Or (const Self &rhs)                  C_NE___ // a | b
        {
            if constexpr( isU8 )    return Self{ vorrq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vorrq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vorrq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vorrq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vorrq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vorrq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vorrq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vorrq_s64( _value, rhs._value )};
        }

        ND_ Self  Xor (const Self &rhs)                 C_NE___ // a ^ b
        {
            if constexpr( isU8 )    return Self{ veorq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ veorq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ veorq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ veorq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ veorq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ veorq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ veorq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ veorq_s64( _value, rhs._value )};
        }

        ND_ Self  OrNot (const Self &rhs)               C_NE___ // a | ~b
        {
            if constexpr( isU8 )    return Self{ vornq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vornq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vornq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vornq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vornq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vornq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vornq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vornq_s64( _value, rhs._value )};
        }

        ND_ Self  Min (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vminq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vminq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vminq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vminq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vminq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vminq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vminq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vminq_s64( _value, rhs._value )};
        }

        ND_ Self  Max (const Self &rhs)                 C_NE___
        {
            if constexpr( isU8 )    return Self{ vmaxq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Self{ vmaxq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Self{ vmaxq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Self{ vmaxq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Self{ vmaxq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Self{ vmaxq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Self{ vmaxq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Self{ vmaxq_s64( _value, rhs._value )};
        }

        ND_ Bool128b  operator == (const Self &rhs)     C_NE___ { return Equal( rhs ); }
        ND_ Bool128b  operator != (const Self &rhs)     C_NE___ { return NotEqual( rhs ); }

        ND_ Bool128b  Equal (const Self &rhs)           C_NE___
        {
            if constexpr( isU8 )    return Bool128b{ vceqq_u8(  _value, rhs._value )};
            if constexpr( isI8 )    return Bool128b{ vceqq_s8(  _value, rhs._value )};
            if constexpr( isU16 )   return Bool128b{ vceqq_u16( _value, rhs._value )};
            if constexpr( isI16 )   return Bool128b{ vceqq_s16( _value, rhs._value )};
            if constexpr( isU32 )   return Bool128b{ vceqq_u32( _value, rhs._value )};
            if constexpr( isI32 )   return Bool128b{ vceqq_s32( _value, rhs._value )};
            if constexpr( isU64 )   return Bool128b{ vceqq_u64( _value, rhs._value )};
            if constexpr( isI64 )   return Bool128b{ vceqq_s64( _value, rhs._value )};
        }

        ND_ Bool128b  NotEqual (const Self &rhs)        C_NE___ { return ~Equal( rhs ); }


        ND_ Self  Abs ()                                C_NE___ // abs(x)
        {
            if constexpr( isI8 )    return Self{ vabsq_s8( _value )};
            if constexpr( isU8 )    return Self{ _value };
            if constexpr( isI16 )   return Self{ vabsq_s16( _value )};
            if constexpr( isU16 )   return Self{ _value };
            if constexpr( isI32 )   return Self{ vabsq_s32( _value )};
            if constexpr( isU32 )   return Self{ _value };
            if constexpr( isI64 )   return Self{ vabsq_s64( _value )};
            if constexpr( isU64 )   return Self{ _value };
        }

        ND_ Self  Negative ()                           C_NE___ // -x
        {
            if constexpr( isI8 )    return Self{ vnegq_s8(  _value )};
            if constexpr( isI16 )   return Self{ vnegq_s16( _value )};
            if constexpr( isI32 )   return Self{ vnegq_s32( _value )};
            if constexpr( isI64 )   return Self{ vnegq_s64( _value )};
        }

        ND_ Self  MulAdd (const Self &b, const Self &c) C_NE___ // a + (b * c)
        {
            if constexpr( isI32 )   return Self{ vmlaq_s16( _value, b._value, c._value )};
            if constexpr( isU32 )   return Self{ vmlaq_u16( _value, b._value, c._value )};
            if constexpr( isI32 )   return Self{ vmlaq_s32( _value, b._value, c._value )};
            if constexpr( isU32 )   return Self{ vmlaq_u32( _value, b._value, c._value )};
        }

        ND_ Self  MulAdd (const Self &b, Value_t c)     C_NE___ // a + (b * c)
        {
            if constexpr( isI16 )   return Self{ vmlaq_n_s16( _value, b._value, c )};
            if constexpr( isU16 )   return Self{ vmlaq_n_u16( _value, b._value, c )};
            if constexpr( isI32 )   return Self{ vmlaq_n_s32( _value, b._value, c )};
            if constexpr( isU32 )   return Self{ vmlaq_n_u32( _value, b._value, c )};
        }

        ND_ Self  MulSub (const Self &b, const Self &c) C_NE___ // a - (b * c)
        {
            if constexpr( isI8 )    return Self{ vmlsq_s8(  _value, b._value, c._value )};
            if constexpr( isU8 )    return Self{ vmlsq_u8(  _value, b._value, c._value )};
            if constexpr( isI16 )   return Self{ vmlsq_s16( _value, b._value, c._value )};
            if constexpr( isU16 )   return Self{ vmlsq_u16( _value, b._value, c._value )};
            if constexpr( isI32 )   return Self{ vmlsq_s32( _value, b._value, c._value )};
            if constexpr( isU32 )   return Self{ vmlsq_u32( _value, b._value, c._value )};
        }

        ND_ Self  MulSub (const Self &b, Value_t c)     C_NE___ // a - (b * c)
        {
            if constexpr( isI16 )   return Self{ vmlsq_n_s16( _value, b._value, c )};
            if constexpr( isU16 )   return Self{ vmlsq_n_u16( _value, b._value, c )};
            if constexpr( isI32 )   return Self{ vmlsq_n_s32( _value, b._value, c )};
            if constexpr( isU32 )   return Self{ vmlsq_n_u32( _value, b._value, c )};
        }


        // non SIMD
        ND_ bool  All ()                                C_NE___
        {
            if constexpr( isU8  )   { IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value[i];  return accum == UMax; }
            if constexpr( isU16 )   { IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value[i];  return accum == UMax; }
            if constexpr( isU32 )   return IntType(_value[0] & _value[1] & _value[2] & _value[3]) == UMax;
            if constexpr( isU64 )   return IntType(_value[0] & _value[1]) == UMax;
        }

        ND_ bool  Any ()                                C_NE___
        {
            if constexpr( isU8  )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == UMax; }
            if constexpr( isU16 )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == UMax; }
            if constexpr( isU32 )   return IntType(_value[0] | _value[1] | _value[2] | _value[3]) == UMax;
            if constexpr( isU64 )   return IntType(_value[0] | _value[1]) == UMax;
        }

        ND_ bool  None ()                               C_NE___
        {
            if constexpr( isU8  )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == 0; }
            if constexpr( isU16 )   { IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value[i];  return accum == 0; }
            if constexpr( isU32 )   return IntType(_value[0] | _value[1] | _value[2] | _value[3]) == 0;
            if constexpr( isU64 )   return IntType(_value[0] | _value[1]) == 0;
        }
    };



# if AE_SIMD_NEON_HALF

    //
    // 128 bit half float (Neon)
    //
#   define AE_SIMD_SimdHalf8
    struct SimdHalf8
    {
    // types
    public:
        using Value_t   = float16_t;
        using Self      = SimdHalf8;
        using Native_t  = float16x8_t;

        struct Bool8
        {
        private:
            uint16x8_t  _value;

        public:
            explicit Bool8 (const uint16x8_t &v)        __NE___ : _value{v} {}

            ND_ Bool8  operator | (const Bool8 &rhs)    C_NE___ { return Bool8{ vorrq_u16( _value, rhs._value )}; }
            ND_ Bool8  operator & (const Bool8 &rhs)    C_NE___ { return Bool8{ vandq_u16( _value, rhs._value )}; }
            ND_ Bool8  operator ^ (const Bool8 &rhs)    C_NE___ { return Bool8{ veorq_u16( _value, rhs._value )}; }
            ND_ Bool8  operator ~ ()                    C_NE___ { return Bool8{ vmvnq_u16( _value )}; }
            ND_ bool   operator [] (usize i)            C_NE___ { ASSERT( i < 8 );  return _value[i] == UMax; }

            ND_ bool  All ()                            C_NE___ { return uint16_t(_value[0] & _value[1] & _value[2] & _value[3] & _value[4] & _value[5] & _value[6] & _value[7]) == UMax; }
            ND_ bool  Any ()                            C_NE___ { return uint16_t(_value[0] | _value[1] | _value[2] | _value[3] | _value[4] | _value[5] | _value[6] | _value[7]) == UMax; }
            ND_ bool  None ()                           C_NE___ { return uint16_t(_value[0] | _value[1] | _value[2] | _value[3] | _value[4] | _value[5] | _value[6] | _value[7]) == 0; }
        };


    // variables
    private:
        Native_t    _value;     // half[8]

        static constexpr uint   count = 8;


    // methods
    public:
        SimdHalf8 ()                                        __NE___ : _value{}                          {}
        SimdHalf8 (float16x4_t a, float16x4_t b)            __NE___ : _value{ vcombine_f16( a, b )}     {}
        SimdHalf8 (const SimdHalf4 &a, const SimdHalf4 &b)  __NE___ : SimdHalf8{ a._value, b._value }   {}
        explicit SimdHalf8 (Value_t val)                    __NE___ : _value{ vdupq_n_f16( val )}       {}
        explicit SimdHalf8 (const Value_t* ptr)             __NE___ : _value{ vld1q_f16( ptr )}         { ASSERT( ptr != null ); }
        explicit SimdHalf8 (const Native_t &val)            __NE___ : _value{ val }                     {}

        ND_ Self  operator - ()                             C_NE___ { return Negative(); }
        ND_ Self  operator +  (const Self &rhs)             C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)             C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)             C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)             C_NE___ { return Div( rhs ); }

        ND_ Value_t         operator [] (usize i)           C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                          C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                     C_NE___ { return Self{ vaddq_f16( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                     C_NE___ { return Self{ vsubq_f16( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                     C_NE___ { return Self{ vmulq_f16( _value, rhs._value )}; }
        ND_ Self  Div (const Self &rhs)                     C_NE___ { return Self{ vdivq_f16( _value, rhs._value )}; }

        ND_ Self  Mul (Value_t rhs)                         C_NE___ { return Self{ vmulq_n_f16( _value, rhs )}; }

        ND_ Self  Min (const Self &rhs)                     C_NE___ { return Self{ vminq_f16( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                     C_NE___ { return Self{ vmaxq_f16( _value, rhs._value )}; }

        ND_ Self  Abs ()                                    C_NE___ { return Self{ vabsq_f16( _value )}; }                          // abs(x)
        ND_ Self  Negative ()                               C_NE___ { return Self{ vnegq_f16( _value )}; }                          // -x
        ND_ Self  Reciprocal ()                             C_NE___ { return Self{ vrecpeq_f16( _value )}; }                        // 1 / x
        ND_ Self  RSqrt ()                                  C_NE___ { return Self{ vrsqrteq_f16( _value )}; }                       // 1 / sqrt(x)
        ND_ Self  FastSqrt ()                               C_NE___ { return Self{ vmulq_f16( _value, vrsqrteq_f16( _value ))}; }   // x / sqrt(x)

        ND_ Self  FMAdd  (const Self &b, const Self &c)     C_NE___ { return Self{ vfmaq_f16(   _value, b._value, c._value )}; }    // (a * b) + c
        ND_ Self  FMAdd  (const Self &b, Value_t c)         C_NE___ { return Self{ vfmaq_n_f16( _value, b._value, c )}; }           //
        ND_ Self  FMSub  (const Self &b, const Self &c)     C_NE___ { return Self{ vfmsq_f16(   _value, b._value, c._value )}; }    // (a * b) - c
        ND_ Self  FMSub  (const Self &b, Value_t c)         C_NE___ { return Self{ vfmsq_n_f16( _value, b._value, c )}; }           //

        template <uint Lane> ND_ Self  Mul (const Self &rhs)C_NE___ { STATIC_ASSERT( Lane < 4 );  return Self{ vmulq_lane_f16( _value, rhs, Lane )}; }  // a * b[lane]

        ND_ Bool8  operator == (const Self &rhs)            C_NE___ { return Equal( rhs ); }
        ND_ Bool8  operator != (const Self &rhs)            C_NE___ { return NotEqual( rhs ); }

        ND_ Bool8  Equal (const Self &rhs)                  C_NE___ { return Bool8{ vceqq_f16( _value, rhs._value )}; }
        ND_ Bool8  NotEqual (const Self &rhs)               C_NE___ { return ~Equal( rhs ); }

        template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort8>   BitCast () C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort8>  BitCast () C_NE___;
    };

# endif // AE_SIMD_NEON_HALF



    //
    // 128 bit float (Neon)
    //
#   define AE_SIMD_SimdFloat4
    struct SimdFloat4
    {
    // types
    public:
        using Value_t   = float;
        using Self      = SimdFloat4;
        using Native_t  = float32x4_t;

        struct Bool4
        {
        private:
            uint32x4_t  _value;

        public:
            explicit Bool4 (const uint32x4_t &v)        __NE___ : _value{v} {}

            ND_ Bool4  operator | (const Bool4 &rhs)    C_NE___ { return Bool4{ vorrq_u32( _value, rhs._value )}; }
            ND_ Bool4  operator & (const Bool4 &rhs)    C_NE___ { return Bool4{ vandq_u32( _value, rhs._value )}; }
            ND_ Bool4  operator ^ (const Bool4 &rhs)    C_NE___ { return Bool4{ veorq_u32( _value, rhs._value )}; }
            ND_ Bool4  operator ~ ()                    C_NE___ { return Bool4{ vmvnq_u32( _value )}; }
            ND_ bool   operator [] (usize i)            C_NE___ { ASSERT( i < 4 );  return _value[i] == UMax; }

            ND_ bool  All ()                            C_NE___ { return uint(_value[0] & _value[1] & _value[2] & _value[3]) == UMax; }
            ND_ bool  Any ()                            C_NE___ { return uint(_value[0] | _value[1] | _value[2] | _value[3]) == UMax; }
            ND_ bool  None ()                           C_NE___ { return uint(_value[0] | _value[1] | _value[2] | _value[3]) == 0; }
        };


    // variables
    private:
        Native_t    _value;     // float[4]

        static constexpr uint   count = 4;


    // methods
    public:
        SimdFloat4 ()                                   __NE___ : _value{}                      {}
        explicit SimdFloat4 (Value_t val)               __NE___ : _value{ vdupq_n_f32( val )}   {}
        explicit SimdFloat4 (const Value_t* ptr)        __NE___ : _value{ vld1q_f32( ptr )}     { ASSERT( ptr != null ); }
        explicit SimdFloat4 (const Native_t &val)       __NE___ : _value{ val }                 {}

        ND_ Self  operator - ()                         C_NE___ { return Negative(); }
        ND_ Self  operator +  (const Self &rhs)         C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)         C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)         C_NE___ { return Mul( rhs ); }

        ND_ Value_t         operator [] (usize i)       C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                      C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                 C_NE___ { return Self{ vaddq_f32( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                 C_NE___ { return Self{ vsubq_f32( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                 C_NE___ { return Self{ vmulq_f32( _value, rhs._value )}; }

        ND_ Self  Mul (Value_t rhs)                     C_NE___ { return Self{ vmulq_n_f32( _value, rhs )}; }

        ND_ Self  Min (const Self &rhs)                 C_NE___ { return Self{ vminq_f32( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                 C_NE___ { return Self{ vmaxq_f32( _value, rhs._value )}; }

        ND_ Bool4  operator == (const Self &rhs)        C_NE___ { return Equal( rhs ); }
        ND_ Bool4  operator != (const Self &rhs)        C_NE___ { return NotEqual( rhs ); }

        ND_ Bool4  Equal (const Self &rhs)              C_NE___ { return Bool4{ vceqq_f32( _value, rhs._value )}; }
        ND_ Bool4  NotEqual (const Self &rhs)           C_NE___ { return ~Equal( rhs ); }

        ND_ Self  Abs ()                                C_NE___ { return Self{ vabsq_f32( _value )}; }                          // abs(x)
        ND_ Self  Negative ()                           C_NE___ { return Self{ vnegq_f32( _value )}; }                          // -x
        ND_ Self  Reciprocal ()                         C_NE___ { return Self{ vrecpeq_f32( _value )}; }                        // 1 / x
        ND_ Self  RSqrt ()                              C_NE___ { return Self{ vrsqrteq_f32( _value )}; }                       // 1 / sqrt(x)
        ND_ Self  FastSqrt ()                           C_NE___ { return Self{ vmulq_f32( _value, vrsqrteq_f32( _value ))}; }   // x / sqrt(x)

        ND_ Self  MulAdd (const Self &b, const Self &c) C_NE___ { return Self{ vmlaq_f32(   _value, b._value, c._value )}; }    // a + (b * c)
        ND_ Self  MulAdd (const Self &b, Value_t c)     C_NE___ { return Self{ vmlaq_n_f32( _value, b._value, c )}; }           //
        ND_ Self  MulSub (const Self &b, const Self &c) C_NE___ { return Self{ vmlsq_f32(   _value, b._value, c._value )}; }    // a - (b * c)
        ND_ Self  MulSub (const Self &b, Value_t c)     C_NE___ { return Self{ vmlsq_n_f32( _value, b._value, c )}; }           //

      #ifdef __aarch64__
        ND_ Self  operator /  (const Self &rhs)         C_NE___ { return Div( rhs ); }
        ND_ Self  Div (const Self &rhs)                 C_NE___ { return Self{ vdivq_f32( _value, rhs._value )}; }

        ND_ Self  FMAdd  (const Self &b, const Self &c) C_NE___ { return Self{ vfmaq_f32(   _value, b._value, c._value )}; }    // (a * b) + c
        ND_ Self  FMAdd  (const Self &b, Value_t c)     C_NE___ { return Self{ vfmaq_n_f32( _value, b._value, c )}; }           //
        ND_ Self  FMSub  (const Self &b, const Self &c) C_NE___ { return Self{ vfmsq_f32(   _value, b._value, c._value )}; }    // (a * b) - c
        ND_ Self  FMSub  (const Self &b, Value_t c)     C_NE___ { return Self{ vfmsq_n_f32( _value, b._value, c )}; }           //
      #endif

    //  template <uint Lane> ND_ Self  Mul (const Self &rhs)                            C_NE___ { STATIC_ASSERT( Lane < count );  return Self{ vmulq_lane_f32( _value, rhs, Lane )}; }                  // a * b[lane]
    //  template <uint Lane> ND_ Self  MulAdd (const Self &b, const Self &c)            C_NE___ { STATIC_ASSERT( Lane < count );  return Self{ vmlaq_lane_f32( _value, b._value, c._value, Lane )}; }   // a + (b * c[Lane])
    //  template <uint Lane> ND_ Self  MulSub (const Self &b, const Self &c)            C_NE___ { STATIC_ASSERT( Lane < count );  return Self{ vmlsq_lane_f32( _value, b._value, c._value, Lane )}; }   // a - (b * c[Lane])

    //  template <typename T> ND_ EnableIf< IsSameTypes<T,half>, SimdHalf4>  Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,int>,  SimdInt4>   Cast ()    C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,uint>, SimdUInt4>  Cast ()    C_NE___;

        template <typename T> ND_ EnableIf< IsSameTypes<T,int>,  SimdInt4>   BitCast () C_NE___;
        template <typename T> ND_ EnableIf< IsSameTypes<T,uint>, SimdUInt4>  BitCast () C_NE___;
    };



#ifdef __aarch64__

    //
    // 128 bit double (Neon64)
    //
#   define AE_SIMD_SimdDouble2
    struct SimdDouble2
    {
    // types
    public:
        using Value_t   = double;
        using Self      = SimdDouble2;
        using Native_t  = float64x2_t;

        struct Bool2
        {
        private:
            uint64x2_t  _value;

        public:
            explicit Bool2 (const uint64x2_t &v)        __NE___ : _value{v} {}

            ND_ Bool2  operator | (const Bool2 &rhs)    C_NE___ { return Bool2{ vorrq_u64( _value, rhs._value )}; }
            ND_ Bool2  operator & (const Bool2 &rhs)    C_NE___ { return Bool2{ vandq_u64( _value, rhs._value )}; }
            ND_ Bool2  operator ^ (const Bool2 &rhs)    C_NE___ { return Bool2{ veorq_u64( _value, rhs._value )}; }
        //  ND_ Bool2  operator ~ ()                    C_NE___ { return Bool2{ vmvnq_u64( _value )}; }
            ND_ bool   operator [] (usize i)            C_NE___ { ASSERT( i < 2 );  return _value[i] == UMax; }

            ND_ bool  All ()                            C_NE___ { return ulong(_value[0] & _value[1]) == UMax; }
            ND_ bool  Any ()                            C_NE___ { return ulong(_value[0] | _value[1]) == UMax; }
            ND_ bool  None ()                           C_NE___ { return ulong(_value[0] | _value[1]) == 0; }
        };


    // variables
    private:
        Native_t    _value;     // double[2]

        static constexpr uint   count = 2;


    // methods
    public:
        SimdDouble2 ()                                  __NE___ : _value{}                      {}
        explicit SimdDouble2 (Value_t val)              __NE___ : _value{ vdupq_n_f64( val )}   {}
        explicit SimdDouble2 (const Value_t* ptr)       __NE___ : _value{ vld1q_f64( ptr )}     { ASSERT( ptr != null ); }
        explicit SimdDouble2 (const Native_t &val)      __NE___ : _value{ val }                 {}

        ND_ Self  operator +  (const Self &rhs)         C_NE___ { return Add( rhs ); }
        ND_ Self  operator -  (const Self &rhs)         C_NE___ { return Sub( rhs ); }
        ND_ Self  operator *  (const Self &rhs)         C_NE___ { return Mul( rhs ); }
        ND_ Self  operator /  (const Self &rhs)         C_NE___ { return Div( rhs ); }

        ND_ Value_t         operator [] (usize i)       C_NE___ { ASSERT( i < count );  return _value[i]; }
        ND_ Native_t const& Get ()                      C_NE___ { return _value; }

        ND_ Self  Add (const Self &rhs)                 C_NE___ { return Self{ vaddq_f64( _value, rhs._value )}; }
        ND_ Self  Sub (const Self &rhs)                 C_NE___ { return Self{ vsubq_f64( _value, rhs._value )}; }
        ND_ Self  Mul (const Self &rhs)                 C_NE___ { return Self{ vmulq_f64( _value, rhs._value )}; }
        ND_ Self  Div (const Self &rhs)                 C_NE___ { return Self{ vdivq_f64( _value, rhs._value )}; }

        ND_ Self  Mul (Value_t rhs)                     C_NE___ { return Self{ vmulq_n_f64( _value, rhs )}; }

        ND_ Self  Min (const Self &rhs)                 C_NE___ { return Self{ vminq_f64( _value, rhs._value )}; }
        ND_ Self  Max (const Self &rhs)                 C_NE___ { return Self{ vmaxq_f64( _value, rhs._value )}; }

        ND_ Bool2  operator == (const Self &rhs)        C_NE___ { return Equal( rhs ); }
    //  ND_ Bool2  operator != (const Self &rhs)        C_NE___ { return NotEqual( rhs ); }

        ND_ Bool2  Equal (const Self &rhs)              C_NE___ { return Bool2{ vceqq_f64( _value, rhs._value )}; }
    //  ND_ Bool2  NotEqual (const Self &rhs)           C_NE___ { return ~Equal( rhs ); }

        ND_ Self  Abs ()                                C_NE___ { return Self{ vabsq_f64( _value )}; }      // abs(x)
        ND_ Self  Negative ()                           C_NE___ { return Self{ vnegq_f64( _value )}; }      // -x
    };

#endif // __aarch64__



    //
    // 128 bit int
    //
#   define AE_SIMD_Int128b
    struct Int128b
    {
    // types
    public:
        using Self  = Int128b;


    // variables
    private:
        uint8x16_t      _value;


    // methods
    public:
        Int128b ()                              __NE___ : _value{ vdupq_n_u8( 0 )}  {}
        explicit Int128b (const ubyte* ptr)     __NE___ : _value{ vld1q_u8( ptr )}  { ASSERT( ptr != null ); }
        explicit Int128b (const uint8x16_t &val)__NE___ : _value{ val }     {}

        ND_ Self  operator ~ ()                 C_NE___ { return Not(); }
        ND_ Self  operator & (const Self &rhs)  C_NE___ { return And( rhs ); }
        ND_ Self  operator | (const Self &rhs)  C_NE___ { return Or( rhs ); }
        ND_ Self  operator ^ (const Self &rhs)  C_NE___ { return Xor( rhs ); }

        ND_ Self  Not   ()                      C_NE___ { return Self{ vmvnq_u8( _value )}; }                   // ~a
        ND_ Self  And   (const Self &rhs)       C_NE___ { return Self{ vandq_u8( _value, rhs._value )}; }       // a & b
        ND_ Self  Or    (const Self &rhs)       C_NE___ { return Self{ vorrq_u8( _value, rhs._value )}; }       // a | b
        ND_ Self  Xor   (const Self &rhs)       C_NE___ { return Self{ veorq_u8( _value, rhs._value )}; }       // a ^ b
        ND_ Self  OrNot (const Self &rhs)       C_NE___ { return Self{ vornq_u8( _value, rhs._value )}; }       // a | ~b
    };



# if AE_SIMD_NEON_HALF
    inline SimdHalf4::Bool4::Bool4 (const SimdUShort4 &v) __NE___ : _value{v.Get()} {}

    template <typename T> ND_ EnableIf< IsSameTypes<T,float>,  SimdFloat4>      SimdHalf4::Cast ()      C_NE___ { return SimdFloat4{ vcvt_f32_f16( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,int>,    SimdInt4>        SimdHalf4::Cast ()      C_NE___ { return SimdInt4{ vcvtq_s32_f32( vcvt_f32_f16( _value ))}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,uint>,   SimdUInt4>       SimdHalf4::Cast ()      C_NE___ { return SimdUInt4{ vcvtq_u32_f32( vcvt_f32_f16( _value ))}; }
    //template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort4>    SimdHalf4::Cast ()      C_NE___ { return SimdShort4{}; }
    //template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort4>   SimdHalf4::Cast ()      C_NE___ { return SimdUShort4{}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort4>      SimdHalf4::BitCast ()   C_NE___ { return SimdShort4{ vreinterpret_s16_f16( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort4>     SimdHalf4::BitCast ()   C_NE___ { return SimdUShort4{ vreinterpret_u16_f16( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,short>,  SimdShort8>      SimdHalf8::BitCast ()   C_NE___ { return SimdShort8{ vreinterpretq_s16_f16( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort8>     SimdHalf8::BitCast ()   C_NE___ { return SimdUShort8{ vreinterpretq_u16_f16( _value )}; }
# endif

//  template <typename T> ND_ EnableIf< IsSameTypes<T,half>, SimdHalf4>         SimdFloat4::Cast ()     C_NE___ { return SimdHalf4{ vcvt_f16_f32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,int>,  SimdInt4>          SimdFloat4::Cast ()     C_NE___ { return SimdInt4{ vcvtq_s32_f32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,uint>, SimdUInt4>         SimdFloat4::Cast ()     C_NE___ { return SimdUInt4{ vcvtq_u32_f32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,int>,  SimdInt4>          SimdFloat4::BitCast ()  C_NE___ { return SimdInt4{ vreinterpretq_s32_f32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,uint>, SimdUInt4>         SimdFloat4::BitCast ()  C_NE___ { return SimdUInt4{ vreinterpretq_u32_f32( _value )}; }

    /*
    template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort4>     SimdShort4::BitCast ()  C_NE___ { return SimdUShort4{ vreinterpret_u16_s16( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,short>, SimdShort4>       SimdUShort4::BitCast () C_NE___ { return SimdShort4{ vreinterpret_s16_u16( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,ushort>, SimdUShort8>     SimdShort8::BitCast ()  C_NE___ { return SimdUShort8{ vreinterpretq_u16_s16( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,short>, SimdShort8>       SimdUShort8::BitCast () C_NE___ { return SimdShort8{ vreinterpretq_s16_u16( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,float>, SimdFloat4>       SimdInt4::Cast ()       C_NE___ { return SimdFloat4{ vcvtq_f32_s32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,half>,  SimdHalf4>        SimdInt4::Cast ()       C_NE___ { return SimdHalf4{ vcvt_f16_f32( vcvtq_f32_s32( _value ))}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,uint>,  SimdUInt4>        SimdInt4::BitCast ()    C_NE___ { return SimdUInt4{ vreinterpretq_u32_s32( _value )}; }

    template <typename T> ND_ EnableIf< IsSameTypes<T,float>, SimdFloat4>       SimdUInt4::Cast ()      C_NE___ { return SimdFloat4{ vcvtq_f32_u32( _value )}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,half>,  SimdHalf4>        SimdUInt4::Cast ()      C_NE___ { return SimdHalf4{ vcvt_f16_f32( vcvtq_f32_u32( _value ))}; }
    template <typename T> ND_ EnableIf< IsSameTypes<T,int>,   SimdInt4>         SimdUInt4::BitCast ()   C_NE___ { return SimdInt4{ vreinterpretq_s32_u32( _value )}; }
    */

} // AE::Math

#endif // AE_SIMD_NEON

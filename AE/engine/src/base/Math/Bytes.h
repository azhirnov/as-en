// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Math/BitMath.h"

namespace AE::Math
{

    //
    // Byte type
    //

    template <typename T>
    struct TBytes
    {
        STATIC_ASSERT( IsInteger<T> and IsScalar<T>, "must be integer scalar" );

    // types
    public:
        using Value_t   = T;
        using Self      = TBytes<T>;

        struct SizeAndAlign
        {
            Self    size;
            Self    align;

            constexpr SizeAndAlign ()                               __NE___ {}
            constexpr SizeAndAlign (const SizeAndAlign &)           __NE___ = default;

            template <typename B1, typename B2>
            constexpr explicit SizeAndAlign (B1 inSize, B2 inAlign) __NE___ : size{inSize}, align{inAlign} {}

            template <typename B>
            constexpr explicit SizeAndAlign (const B &other)        __NE___ : size{other.size}, align{other.align} {}

            constexpr SizeAndAlign  operator * (usize count)        C_NE___ { SizeAndAlign tmp{*this};  tmp.size *= count;  return tmp; }
        };


    // variables
    private:
        T   _value;


    // methods
    public:
        constexpr TBytes ()                                             __NE___ : _value{0} {}
        constexpr TBytes (Base::_hidden_::_UMax)                        __NE___ : _value{UMax} {}

        explicit constexpr TBytes (T value)                             __NE___ : _value{value} {}
        explicit constexpr TBytes (std::align_val_t val)                __NE___ : _value{usize(val)} {}
        explicit constexpr TBytes (const void* ptr)                     __NE___ : _value{BitCast<usize>(ptr)} {}

        template <typename B>
        constexpr TBytes (const TBytes<B> &other)                       __NE___ : _value{CheckCast<T>(other)} {}

        ND_ explicit constexpr operator sbyte ()                        C_NE___ { return static_cast<sbyte >(_value); }
        ND_ explicit constexpr operator sshort ()                       C_NE___ { return static_cast<sshort>(_value); }
        ND_ explicit constexpr operator int ()                          C_NE___ { return static_cast<int   >(_value); }
        ND_ explicit constexpr operator slong ()                        C_NE___ { return static_cast<slong >(_value); }

        ND_ explicit constexpr operator ubyte ()                        C_NE___ { return static_cast<ubyte >(_value); }
        ND_ explicit constexpr operator ushort ()                       C_NE___ { return static_cast<ushort>(_value); }
        ND_ explicit constexpr operator uint ()                         C_NE___ { return static_cast<uint  >(_value); }
        ND_ explicit constexpr operator ulong ()                        C_NE___ { return static_cast<ulong >(_value); }

      #if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_APPLE) or defined(AE_PLATFORM_EMSCRIPTEN)
        ND_ explicit constexpr operator signed long ()                  C_NE___ { return static_cast< signed long >(_value); }
        ND_ explicit constexpr operator unsigned long ()                C_NE___ { return static_cast< unsigned long >(_value); }
      #endif

        template <typename R>
        ND_ constexpr R *       AsPtr ()                                C_NE___ { return BitCast<R *>( CheckCast<usize>( _value )); }
        ND_ constexpr void*     AsPtr ()                                C_NE___ { return BitCast<void *>( CheckCast<usize>( _value )); }

        template <typename R>
        ND_ explicit constexpr operator R * ()                          C_NE___ { return BitCast<R *>( CheckCast<usize>( _value )); }

        ND_ constexpr T     get ()                                      C_NE___ { return _value; }
        ND_ constexpr T     Kb ()                                       C_NE___ { return _value >> 10; }
        ND_ constexpr T     Mb ()                                       C_NE___ { return _value >> 20; }
        ND_ constexpr T     Gb ()                                       C_NE___ { return _value >> 30; }

        ND_ static constexpr Self   FromBits (T value)                  __NE___ { return Self( value >> 3 ); }
        ND_ static constexpr Self   FromKb (T value)                    __NE___ { return Self( value << 10 ); }
        ND_ static constexpr Self   FromMb (T value)                    __NE___ { return Self( value << 20 ); }
        ND_ static constexpr Self   FromGb (T value)                    __NE___ { return Self( value << 30 ); }


        template <typename B>   ND_ static constexpr Self           SizeOf ()                   __NE___ { STATIC_ASSERT( not IsVoid<B> );  return Self( sizeof(B) ); }
        template <typename B>   ND_ static constexpr Self           SizeOf (const B &)          __NE___ { STATIC_ASSERT( not IsVoid<B> );  return Self( sizeof(B) ); }

        template <typename B>   ND_ static constexpr Self           AlignOf ()                  __NE___ { STATIC_ASSERT( not IsVoid<B> );  return Self( alignof(B) ); }
        template <typename B>   ND_ static constexpr Self           AlignOf (const B &)         __NE___ { STATIC_ASSERT( not IsVoid<B> );  return Self( alignof(B) ); }

        template <typename B>   ND_ static constexpr SizeAndAlign   SizeAndAlignOf ()           __NE___ { return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }
        template <typename B>   ND_ static constexpr SizeAndAlign   SizeAndAlignOf (const B &)  __NE___ { return SizeAndAlign{ SizeOf<B>(), AlignOf<B>() }; }


        // move any pointer
        template <typename B>   ND_ friend B*  operator +  (B *lhs, const Self &rhs)    __NE___ { return BitCast<B *>( usize(lhs) + usize(rhs._value) ); }
        template <typename B>   ND_ friend B*  operator -  (B *lhs, const Self &rhs)    __NE___ { return BitCast<B *>( usize(lhs) - usize(rhs._value) ); }
        template <typename B>       friend B*& operator += (B* &lhs, const Self &rhs)   __NE___ { return (lhs = lhs + rhs); }
        template <typename B>       friend B*& operator -= (B* &lhs, const Self &rhs)   __NE___ { return (lhs = lhs + rhs); }


            constexpr Self& operator = (Base::_hidden_::_UMax)          __NE___ { _value = UMax;    return *this; }
            constexpr Self& operator = (Base::_hidden_::DefaultType)    __NE___ { _value = 0;       return *this; }
            constexpr Self& operator = (const Self &)                   __NE___ = default;

        ND_ constexpr Self  operator ~ ()                               C_NE___ { return Self( ~_value ); }

            Self&           operator ++ ()                              __NE___ { ++_value; return *this; }
            Self            operator ++ (int)                           __NE___ { auto res = _value++; return Self{res}; }

            Self&           operator += (const Self &rhs)               __NE___ { _value += rhs._value;  return *this; }
        ND_ constexpr Self  operator +  (const Self &rhs)               C_NE___ { return Self( _value + rhs._value ); }

            Self&           operator -= (const Self &rhs)               __NE___ { _value -= rhs._value;  return *this; }
        ND_ constexpr Self  operator -  (const Self &rhs)               C_NE___ { return Self( _value - rhs._value ); }

            Self&           operator *= (const Self &rhs)               __NE___ { _value *= rhs._value;  return *this; }
        ND_ constexpr Self  operator *  (const Self &rhs)               C_NE___ { return Self( _value * rhs._value ); }

            Self&           operator /= (const Self &rhs)               __NE___ { _value /= rhs._value;  return *this; }
        ND_ constexpr Self  operator /  (const Self &rhs)               C_NE___ { return Self( _value / rhs._value ); }

            Self&           operator %= (const Self &rhs)               __NE___ { _value %= rhs._value;  return *this; }
        ND_ constexpr Self  operator %  (const Self &rhs)               C_NE___ { return Self( _value % rhs._value ); }


            Self&           operator += (const T rhs)                   __NE___ { _value += rhs;  return *this; }
        ND_ constexpr Self  operator +  (const T rhs)                   C_NE___ { return Self( _value + rhs ); }

            Self&           operator -= (const T rhs)                   __NE___ { _value -= rhs;  return *this; }
        ND_ constexpr Self  operator -  (const T rhs)                   C_NE___ { return Self( _value - rhs ); }

            Self&           operator *= (const T rhs)                   __NE___ { _value *= rhs;  return *this; }
        ND_ constexpr Self  operator *  (const T rhs)                   C_NE___ { return Self( _value * rhs ); }

            Self&           operator /= (const T rhs)                   __NE___ { _value /= rhs;  return *this; }
        ND_ constexpr Self  operator /  (const T rhs)                   C_NE___ { return Self( _value / rhs ); }

            Self&           operator %= (const T rhs)                   __NE___ { _value %= rhs;  return *this; }
        ND_ constexpr Self  operator %  (const T rhs)                   C_NE___ { return Self( _value % rhs ); }

        ND_ Self&           operator >>= (const T rhs)                  __NE___ { _value >>= rhs;  return *this; }
        ND_ constexpr Self  operator >>  (const T rhs)                  C_NE___ { return Self{ _value >> rhs }; }

        ND_ Self&           operator <<= (const T rhs)                  __NE___ { _value <<= rhs;  return *this; }
        ND_ constexpr Self  operator <<  (const T rhs)                  C_NE___ { return Self{ _value << rhs }; }

        ND_ constexpr bool  operator == (const Self &rhs)               C_NE___ { return _value == rhs._value; }
        ND_ constexpr bool  operator != (const Self &rhs)               C_NE___ { return _value != rhs._value; }
        ND_ constexpr bool  operator >  (const Self &rhs)               C_NE___ { return _value >  rhs._value; }
        ND_ constexpr bool  operator <  (const Self &rhs)               C_NE___ { return _value <  rhs._value; }
        ND_ constexpr bool  operator >= (const Self &rhs)               C_NE___ { return _value >= rhs._value; }
        ND_ constexpr bool  operator <= (const Self &rhs)               C_NE___ { return _value <= rhs._value; }

        ND_ constexpr bool  operator == (const T rhs)                   C_NE___ { return _value == rhs; }
        ND_ constexpr bool  operator != (const T rhs)                   C_NE___ { return _value != rhs; }
        ND_ constexpr bool  operator >  (const T rhs)                   C_NE___ { return _value >  rhs; }
        ND_ constexpr bool  operator <  (const T rhs)                   C_NE___ { return _value <  rhs; }
        ND_ constexpr bool  operator >= (const T rhs)                   C_NE___ { return _value >= rhs; }
        ND_ constexpr bool  operator <= (const T rhs)                   C_NE___ { return _value <= rhs; }

        ND_ friend constexpr Self  operator +  (T lhs, const Self &rhs) __NE___ { return Self( lhs + rhs._value ); }
        ND_ friend constexpr Self  operator -  (T lhs, const Self &rhs) __NE___ { return Self( lhs - rhs._value ); }
        ND_ friend constexpr Self  operator *  (T lhs, const Self &rhs) __NE___ { return Self( lhs * rhs._value ); }
        ND_ friend constexpr Self  operator /  (T lhs, const Self &rhs) __NE___ { return Self( lhs / rhs._value ); }
        ND_ friend constexpr Self  operator %  (T lhs, const Self &rhs) __NE___ { return Self( lhs % rhs._value ); }

        ND_ friend constexpr bool  operator == (T lhs, Self rhs)        __NE___ { return lhs == rhs._value; }
        ND_ friend constexpr bool  operator != (T lhs, Self rhs)        __NE___ { return lhs != rhs._value; }
        ND_ friend constexpr bool  operator >  (T lhs, Self rhs)        __NE___ { return lhs >  rhs._value; }
        ND_ friend constexpr bool  operator <  (T lhs, Self rhs)        __NE___ { return lhs <  rhs._value; }
        ND_ friend constexpr bool  operator >= (T lhs, Self rhs)        __NE___ { return lhs >= rhs._value; }
        ND_ friend constexpr bool  operator <= (T lhs, Self rhs)        __NE___ { return lhs <= rhs._value; }

        ND_ static constexpr Self  Max ()                               __NE___ { return Self{ MaxValue<T>() }; }
        ND_ static constexpr Self  Min ()                               __NE___ { return Self{ MinValue<T>() }; }
    };


    using Bytes64u      = TBytes< ulong >;
    using Bytes32u      = TBytes< uint >;
    using Bytes16u      = TBytes< ushort >;
    using Bytes8u       = TBytes< ubyte >;
    using BytesUSize    = TBytes< usize >;

    using Bytes64s      = TBytes< slong >;
    using Bytes32s      = TBytes< int >;
    using Bytes16s      = TBytes< short >;
    using BytesSSize    = TBytes< ssize >;

    using Bytes         = Bytes64u;
    using SizeAndAlign  = Bytes::SizeAndAlign;

    template <typename T>
    inline static constexpr Bytes  SizeOf = Bytes::SizeOf<T>();

    template <typename T>
    ND_ constexpr Bytes  Sizeof (const T &x) __NE___    { return Bytes::SizeOf(x); }

    template <typename T>
    inline static constexpr Bytes  AlignOf = Bytes::AlignOf<T>();

    template <typename T>
    ND_ constexpr Bytes  Alignof (const T &x) __NE___   { return Bytes::AlignOf(x); }

    template <typename T>
    inline static constexpr SizeAndAlign  SizeAndAlignOf = Bytes::SizeAndAlignOf<T>();

    template <typename T>
    ND_ constexpr SizeAndAlign  SizeAndAlignof (const T &x) __NE___ { return Bytes::SizeAndAlignOf(x); }


    ND_ constexpr Bytes  operator "" _b  (unsigned long long value) __NE___ { return Bytes( CheckCast<Bytes::Value_t>(value) ); }
    ND_ constexpr Bytes  operator "" _Kb (unsigned long long value) __NE___ { return Bytes::FromKb( CheckCast<Bytes::Value_t>(value) ); }
    ND_ constexpr Bytes  operator "" _Mb (unsigned long long value) __NE___ { return Bytes::FromMb( CheckCast<Bytes::Value_t>(value) ); }
    ND_ constexpr Bytes  operator "" _Gb (unsigned long long value) __NE___ { return Bytes::FromGb( CheckCast<Bytes::Value_t>(value) ); }


    namespace _hidden_
    {
        template <typename T>
        struct _IsBytes {
            static constexpr bool   value = false;
        };

        template <typename T>
        struct _IsBytes< TBytes<T> > {
            static constexpr bool   value = true;
        };
    }

    template <typename T>
    static constexpr bool  IsBytes = Math::_hidden_::_IsBytes<T>::value;

/*
=================================================
    OffsetOf
=================================================
*/
    template <typename A, typename B>
    ND_ constexpr Bytes  OffsetOf (A B::*member) __NE___
    {
        const union U {
            B       b;
            int     tmp;
            U () : tmp{0} {}
            ~U () {}
        } u;
        return Bytes( usize(std::addressof(u.b.*member)) - usize(std::addressof(u.b)) );
    }

/*
=================================================
    FloorPOT / CeilPOT
=================================================
*/
    template <typename T>
    ND_ TBytes<T>  FloorPOT (const TBytes<T> x) __NE___
    {
        return TBytes<T>{ FloorPOT( T{x} )};
    }

    template <typename T>
    ND_ TBytes<T>  CeilPOT (const TBytes<T> x) __NE___
    {
        return TBytes<T>{ CeilPOT( T{x} )};
    }

/*
=================================================
    IsPowerOfTwo
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsPowerOfTwo (const TBytes<T> x) __NE___
    {
        return IsPowerOfTwo( T{x} );
    }

} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< TBytes<T> >       { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< TBytes<T> >       { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< TBytes<T> >  { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base


template <typename T>
struct std::hash< AE::Math::TBytes<T> >
{
    ND_ size_t  operator () (const AE::Math::TBytes<T> &value) C_NE___
    {
        return size_t(AE::Base::HashOf( T(value) ));
    }
};

template <typename T>
class std::numeric_limits< AE::Math::TBytes<T> >
{
private:
    using Bytes = AE::Math::TBytes<T>;
    using Base  = std::numeric_limits<T>;

public:
    ND_ static constexpr Bytes  min () __NE___ {
        return Bytes{Base::min()};
    }

    ND_ static constexpr Bytes  max () __NE___ {
        return Bytes{Base::max()};
    }
};

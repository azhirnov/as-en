// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_XXHASH
# include "xxhash.h"
#endif

namespace AE::Base
{

    //
    // Hash Value
    //

    template <typename T>
    struct THashVal final
    {
    // variables
    private:
        T   _value  = 0;

    // methods
    public:
        constexpr THashVal ()                                   __NE___ {}
        explicit constexpr THashVal (T val)                     __NE___ : _value{val} {}

        template <typename B>
        explicit constexpr THashVal (THashVal<B> h)             __NE___ : _value{static_cast<T>(B{h})} {}

        ND_ constexpr bool  operator == (const THashVal &rhs)   C_NE___ { return _value == rhs._value; }
        ND_ constexpr bool  operator != (const THashVal &rhs)   C_NE___ { return not (*this == rhs); }
        ND_ constexpr bool  operator >  (const THashVal &rhs)   C_NE___ { return _value > rhs._value; }
        ND_ constexpr bool  operator <  (const THashVal &rhs)   C_NE___ { return _value < rhs._value; }

        constexpr THashVal&  operator << (const THashVal &rhs)  __NE___ { Append( rhs );  return *this; }
        constexpr THashVal&  operator += (const THashVal &rhs)  __NE___ { Append( rhs );  return *this; }

        constexpr void  Append (const THashVal &rhs)            __NE___
        {
            const T mask    = T(CT_SizeofInBits(_value) - 1);
            T       val     = rhs._value;
            T       shift   = 8;

            shift  &= mask;
            _value ^= (val << shift) | (val >> ( ~(shift-1) & mask ));
        }

        ND_ constexpr const THashVal<T>  operator + (const THashVal<T> &rhs) C_NE___
        {
            return THashVal<T>(*this) << rhs;
        }

        ND_ explicit constexpr operator T ()                    C_NE___ { return _value; }

        template <typename R>
        ND_ constexpr R  Cast ()                                C_NE___
        {
            if constexpr( sizeof(R) >= sizeof(T) )
                return R(_value);
            else
            if constexpr( sizeof(R)*2 >= sizeof(T) )
            {
                constexpr usize bits    = CT_SizeOfInBits<T> / 2;
                constexpr T     mask    = (T{1} << bits) - 1;

                return R( ((_value >> bits) ^ _value) & mask );
            }
            else
            if constexpr( sizeof(R)*4 >= sizeof(T) )
            {
                constexpr usize bits    = CT_SizeOfInBits<T> / 4;
                constexpr T     mask    = (T{1} << bits) - 1;

                return R( ((_value >> bits*3) ^ (_value >> bits*2) ^ (_value >> bits) ^ _value) & mask );
            }
        }
    };

    using HashVal   = THashVal<usize>;
    using HashVal32 = THashVal<uint>;
    using HashVal64 = THashVal<ulong>;


    //
    // Hasher
    //

    template <typename T>
    struct DefaultHasher_CalcHash
    {
        ND_ usize  operator () (const T &key) C_NE___ {
            return usize(key.CalcHash());
        }
    };

    template <typename T>
    struct DefaultHasher_GetHash
    {
        ND_ usize  operator () (const T &key) C_NE___ {
            return usize(key.GetHash());
        }
    };
//-----------------------------------------------------------------------------



/*
=================================================
    HashOf
=================================================
*/
    template <typename T>
    ND_ EnableIf<not IsFloatPoint<T>, HashVal>  HashOf (const T &value) __NE___
    {
        return HashVal( std::hash<T>()( value ));
    }

/*
=================================================
    HashOf (float)
=================================================
*/
    ND_ inline HashVal  HashOf (const float &value, uint ignoreMantissaBits = (23-10)) __NE___
    {
        ASSERT( ignoreMantissaBits < 23 );
        uint    dst;
        std::memcpy( OUT &dst, &value, sizeof(dst) );
        dst &= ~((1 << ignoreMantissaBits)-1);
        return HashVal( std::hash<uint>()( dst ));
    }

/*
=================================================
    HashOf (double)
=================================================
*/
    ND_ inline HashVal  HashOf (const double &value, uint ignoreMantissaBits = (52-10)) __NE___
    {
        ASSERT( ignoreMantissaBits < 52 );
        ulong   dst;
        std::memcpy( OUT &dst, &value, sizeof(dst) );
        dst &= ~((1 << ignoreMantissaBits)-1);
        return HashVal( std::hash<ulong>()( dst ));
    }
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_XXHASH
/*
=================================================
    XXHash32 (buffer)
=================================================
*/
    ND_ inline HashVal32  XXHash32 (const void* ptr, const usize sizeInBytes) __NE___
    {
        return HashVal32{ XXH32( ptr, sizeInBytes, 0 )};
    }

/*
=================================================
    XXHash64 (buffer)
=================================================
*/
    ND_ inline HashVal64  XXHash64 (const void* ptr, const usize sizeInBytes) __NE___
    {
      #if AE_HAS_SIMD
        return HashVal64{ XXH3_64bits( ptr, sizeInBytes )};
      #else
        return HashVal64{ XXH64( ptr, sizeInBytes, 0 )};
      #endif
    }

#endif // AE_ENABLE_XXHASH
//-----------------------------------------------------------------------------


/*
=================================================
    HashOf (buffer)
----
    use private api to calculate hash of buffer
=================================================
*/
    ND_ inline HashVal  HashOf (const void* ptr, const usize sizeInBytes) __NE___
    {
        ASSERT( ptr != null and sizeInBytes > 0 );

        #if defined(AE_ENABLE_XXHASH)
            if constexpr( sizeof(HashVal) == sizeof(HashVal64) )
                return HashVal{XXHash64( ptr, sizeInBytes )};
            else
            if constexpr( sizeof(HashVal) == sizeof(HashVal32) )
                return HashVal{XXHash32( ptr, sizeInBytes )};

        #elif defined(AE_HAS_HASHFN_HashArrayRepresentation)
            return HashVal{std::_Hash_array_representation( static_cast<const unsigned char*>(ptr), sizeInBytes )};

        #elif defined(AE_HAS_HASHFN_Murmur2OrCityhash)
            return HashVal{std::__murmur2_or_cityhash<usize>()( ptr, sizeInBytes )};

        #elif defined(AE_HAS_HASHFN_HashBytes)
            return HashVal{std::_Hash_bytes( ptr, sizeInBytes, 0 )};

        #else
            const ubyte*    buf     = static_cast<const ubyte*>(ptr);
            HashVal         result;
            for (usize i = 0; i < sizeInBytes; ++i) {
                result << HashVal{buf[i]};
            }
            return result;
        #endif
    }

} // AE::Base


template <>
struct std::hash< AE::Base::HashVal >
{
    ND_ size_t  operator () (const AE::Base::HashVal &value) C_NE___
    {
        return size_t(value);
    }
};

template <typename First, typename Second>
struct std::hash< std::pair<First, Second> >
{
    ND_ size_t  operator () (const std::pair<First, Second> &value) C_NE___
    {
        return size_t(AE::Base::HashOf( value.first ) + AE::Base::HashOf( value.second ));
    }
};

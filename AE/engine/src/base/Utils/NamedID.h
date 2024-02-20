// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/FixedString.h"
#include "base/CompileTime/Hash.h"

namespace AE::Base
{

    //
    // ID With String
    //

    template <usize Size, uint UID, bool Optimize, uint Seed = UMax>
    struct NamedID
    {
    // types
    public:
        using Self          = NamedID< Size, UID, Optimize, Seed >;
        using WithString_t  = NamedID< Size, UID, false, Seed >;
        using Optimized_t   = NamedID< Size, UID, true, Seed >;
        using Ref           = const Self;

        using Hasher_t      = std::hash< Self >;

        struct EqualTo_t {
            ND_ bool  operator () (const Self &lhs, const Self &rhs) C_NE___ {
                return lhs._hash == rhs._hash;
            }
        };


    // variables
    private:
        HashVal32   _hash;

        static constexpr THashVal<uint> _emptyHash  = CT_Hash( "", 0, Seed );


    // methods
    public:
        constexpr NamedID ()                                __NE___ : _hash{_emptyHash}                                     {}
        explicit constexpr NamedID (HashVal32 hash)         __NE___ : _hash{hash}                                           {}
        explicit constexpr NamedID (StringView name)        __NE___ : _hash{CT_Hash( name.data(), name.length(), Seed )}    {}
        explicit constexpr NamedID (const char* name)       __NE___ : _hash{CT_Hash( name, UMax, Seed )}                    {}

        constexpr NamedID (Self &&)                         __NE___ = default;
        constexpr NamedID (const Self &)                    __NE___ = default;

        constexpr Self&     operator = (Self &&)            __NE___ = default;
        constexpr Self&     operator = (const Self &)       __NE___ = default;

        ND_ constexpr bool  operator == (const Self &rhs)   C_NE___ { return _hash == rhs._hash; }
        ND_ constexpr bool  operator != (const Self &rhs)   C_NE___ { return not (*this == rhs); }
        ND_ constexpr bool  operator >  (const Self &rhs)   C_NE___ { return _hash > rhs._hash; }
        ND_ constexpr bool  operator <  (const Self &rhs)   C_NE___ { return rhs > *this; }
        ND_ constexpr bool  operator >= (const Self &rhs)   C_NE___ { return not (*this <  rhs); }
        ND_ constexpr bool  operator <= (const Self &rhs)   C_NE___ { return not (*this >  rhs); }

        ND_ constexpr explicit operator uint ()             C_NE___ { return uint{_hash}; }

        ND_ constexpr HashVal       GetHash ()              C_NE___ { return HashVal{uint{_hash}}; }
        ND_ constexpr HashVal32     GetHash32 ()            C_NE___ { return _hash; }
        ND_ constexpr bool          IsDefined ()            C_NE___ { return _hash != _emptyHash; }
        ND_ constexpr static bool   IsOptimized ()          __NE___ { return true; }
        ND_ constexpr static uint   GetSeed ()              __NE___ { return Seed; }
        ND_ constexpr static uint   GetUID ()               __NE___ { return UID; }
        ND_ constexpr static usize  MaxStringLength ()      __NE___ { return Size; }
    };



    //
    // ID With String
    //

    template <usize Size, uint UID, uint Seed>
    struct NamedID< Size, UID, false, Seed >
    {
    // types
    public:
        using Self          = NamedID< Size, UID, false, Seed >;
        using Optimized_t   = NamedID< Size, UID, true, Seed >;
        using WithString_t  = NamedID< Size, UID, false, Seed >;
        using Ref           = Self const &;

        using Hasher_t      = std::hash< Self >;

        struct EqualTo_t {
            ND_ bool  operator () (const Self &lhs, const Self &rhs) C_NE___ {
                return lhs._hash == rhs._hash and lhs._name == rhs._name;
            }
        };


    // variables
    private:
        HashVal32           _hash;
        FixedString<Size>   _name;

        static constexpr THashVal<uint> _emptyHash  = CT_Hash( "", 0, Seed );


    // methods
    public:
        constexpr NamedID ()                                    __NE___ : _hash{_emptyHash}                                                     {}
        explicit constexpr NamedID (HashVal32 hash)             __NE___ :  _hash{hash}                                                          {}
        explicit constexpr NamedID (StringView name)            __NE___ : _hash{ CT_Hash( name.data(), name.length(), Seed )}, _name{name}      {}
        explicit constexpr NamedID (const char* name)           __NE___ : _hash{ CT_Hash( name, UMax, Seed )}, _name{name}                      {}

        template <usize StrSize>
        explicit constexpr NamedID (const FixedString<StrSize> &name) __NE___ : _hash{CT_Hash( name.data(), name.length(), Seed )}, _name{name} {}

        explicit constexpr NamedID (const Optimized_t &other)   __NE___ : _hash{other.GetHash32()}                                              {}

        constexpr NamedID (Self &&)                             __NE___ = default;
        constexpr NamedID (const Self &)                        __NE___ = default;

        constexpr Self&     operator = (Self &&)                __NE___ = default;
        constexpr Self&     operator = (const Self &)           __NE___ = default;

        ND_ constexpr bool  operator == (const Self &rhs)       C_NE___ { return _hash == rhs._hash; }
        ND_ constexpr bool  operator != (const Self &rhs)       C_NE___ { return not (*this == rhs); }
        ND_ constexpr bool  operator >  (const Self &rhs)       C_NE___ { return _hash > rhs._hash; }
        ND_ constexpr bool  operator <  (const Self &rhs)       C_NE___ { return rhs > *this; }
        ND_ constexpr bool  operator >= (const Self &rhs)       C_NE___ { return not (*this <  rhs); }
        ND_ constexpr bool  operator <= (const Self &rhs)       C_NE___ { return not (*this >  rhs); }

        ND_ constexpr operator Optimized_t ()                   C_NE___ { return Optimized_t{ GetHash32() }; }
        ND_ constexpr explicit operator uint ()                 C_NE___ { return uint{_hash}; }

        ND_ constexpr StringView    GetName ()                  C_NE___ { return _name; }
        ND_ constexpr HashVal       GetHash ()                  C_NE___ { return HashVal{uint{_hash}}; }
        ND_ constexpr HashVal32     GetHash32 ()                C_NE___ { return _hash; }
        ND_ constexpr bool          IsDefined ()                C_NE___ { return _hash != _emptyHash; }
        ND_ constexpr static bool   IsOptimized ()              __NE___ { return false; }
        ND_ constexpr static uint   GetSeed ()                  __NE___ { return Seed; }
        ND_ constexpr static uint   GetUID ()                   __NE___ { return UID; }
        ND_ constexpr static usize  MaxStringLength ()          __NE___ { return Size; }
    };


    template <usize Size, uint UID, uint Seed>
    ND_ inline bool  operator == (const NamedID< Size, UID, false, Seed > &lhs, const NamedID< Size, UID, true, Seed > &rhs) __NE___ {
        return lhs.GetHash32() == rhs.GetHash32();
    }

    template <usize Size, uint UID, uint Seed>
    ND_ inline bool  operator == (const NamedID< Size, UID, true, Seed > &lhs, const NamedID< Size, UID, false, Seed > &rhs) __NE___ {
        return lhs.GetHash32() == rhs.GetHash32();
    }


    template <usize Size, uint UID, uint Seed>  struct TMemCopyAvailable<       NamedID< Size, UID, true, Seed >>   { static constexpr bool  value = true;  };
    template <usize Size, uint UID, uint Seed>  struct TZeroMemAvailable<       NamedID< Size, UID, true, Seed >>   { static constexpr bool  value = false; };
    template <usize Size, uint UID, uint Seed>  struct TTriviallySerializable<  NamedID< Size, UID, true, Seed >>   { static constexpr bool  value = true;  };
    template <usize Size, uint UID, uint Seed>  struct TTriviallyDestructible<  NamedID< Size, UID, true, Seed >>   { static constexpr bool  value = true;  };

    template <usize Size, uint UID, uint Seed>  struct TMemCopyAvailable<       NamedID< Size, UID, false, Seed >>  { static constexpr bool  value = true;  };
    template <usize Size, uint UID, uint Seed>  struct TZeroMemAvailable<       NamedID< Size, UID, false, Seed >>  { static constexpr bool  value = false; };
    template <usize Size, uint UID, uint Seed>  struct TTriviallySerializable<  NamedID< Size, UID, false, Seed >>  { static constexpr bool  value = false; };
    template <usize Size, uint UID, uint Seed>  struct TTriviallyDestructible<  NamedID< Size, UID, false, Seed >>  { static constexpr bool  value = true;  };

} // AE::Base


template <size_t Size, uint32_t UID, bool Optimize, uint32_t Seed>
struct std::hash< AE::Base::NamedID<Size, UID, Optimize, Seed> >
{
    ND_ size_t  operator () (const AE::Base::NamedID<Size, UID, Optimize, Seed> &value) C_NE___ {
        return size_t(value.GetHash());
    }
};

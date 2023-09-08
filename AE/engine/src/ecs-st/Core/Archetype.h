// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ecs-st/Core/ComponentID.h"

namespace AE::ECS
{

    //
    // Archetype description
    //

    class ArchetypeDesc
    {
    // types
    private:
        using                   Chunk_t         = ulong;
        static constexpr uint   BitsPerChunk    = CT_SizeOfInBits<Chunk_t>;
        static constexpr uint   ChunkCount      = ECS_Config::MaxComponents / BitsPerChunk;
        using                   CompBits_t      = StaticArray< Chunk_t, ChunkCount >;

        using ComponentIDs_t = FixedArray< ComponentID, ECS_Config::MaxComponentsPerArchetype >;


    // variables
    private:
        CompBits_t      _bits;      // TODO: use SSE


    // methods
    public:
        ArchetypeDesc ()                                        __NE___ { ZeroMem( OUT _bits.data(), ArraySizeOf(_bits) ); }

        template <typename Comp>    ArchetypeDesc&  Add ()      __NE___ { return Add( ComponentTypeInfo<Comp>::id ); }
        template <typename Comp>    ArchetypeDesc&  Remove ()   __NE___ { return Remove( ComponentTypeInfo<Comp>::id ); }
        template <typename Comp>    ND_ bool        Exists ()   C_NE___ { return Exists( ComponentTypeInfo<Comp>::id ); }

        ArchetypeDesc&  Add (ComponentID id)                    __NE___;
        ArchetypeDesc&  Remove (ComponentID id)                 __NE___;

        ArchetypeDesc&  Add (const ArchetypeDesc &other)        __NE___;
        ArchetypeDesc&  Remove (const ArchetypeDesc &other)     __NE___;

        ND_ CompBits_t const&   Raw ()                          C_NE___ { return _bits; }

        ND_ ComponentIDs_t      GetIDs ()                       C_NE___;

        ND_ bool        Exists (ComponentID id)                 C_NE___;
        ND_ bool        All (const ArchetypeDesc &)             C_NE___;
        ND_ bool        Any (const ArchetypeDesc &)             C_NE___;
        ND_ bool        AnyOrEmpty (const ArchetypeDesc &)      C_NE___;
        ND_ bool        Equals (const ArchetypeDesc &)          C_NE___;
        ND_ bool        Empty ()                                C_NE___;
        ND_ usize       Count ()                                C_NE___;

        ND_ HashVal     GetHash ()                              C_NE___;
    };



    //
    // Archetype
    //

    class Archetype final
    {
    // variables
    private:
        HashVal             _hash;
        ArchetypeDesc       _desc;


    // methods
    public:
        explicit Archetype (const ArchetypeDesc &desc)  __NE___ : _hash{desc.GetHash()}, _desc{desc} {}

        ND_ HashVal                 Hash ()             C_NE___ { return _hash; }
        ND_ ArchetypeDesc const&    Desc ()             C_NE___ { return _desc; }

        ND_ bool operator == (const Archetype &rhs)     C_NE___ { return Equals( rhs ); }

        ND_ bool    Equals (const Archetype &rhs)       C_NE___ { return _desc.Equals( rhs._desc ); }
        ND_ bool    Contains (const Archetype &rhs)     C_NE___ { return _desc.All( rhs._desc ); }
        ND_ bool    Exists (ComponentID id)             C_NE___ { return _desc.Exists( id ); }

        template <typename T>
        ND_ bool    Exists ()                           C_NE___ { return Exists( ComponentTypeInfo<T>::id ); }
    };



    //
    // Archetype Query description
    //

    struct ArchetypeQueryDesc
    {
        ArchetypeDesc       required;
        ArchetypeDesc       subtractive;
        ArchetypeDesc       requireAny;

        ArchetypeQueryDesc ()                                   __NE___ {}

        ND_ bool  Compatible (const ArchetypeDesc &)            C_NE___;

        ND_ bool  operator == (const ArchetypeQueryDesc &rhs)   C_NE___;

        ND_ bool  IsValid ()                                    C_NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    Add
=================================================
*/
    inline ArchetypeDesc&  ArchetypeDesc::Add (ComponentID id) __NE___
    {
        ASSERT( id.value < ECS_Config::MaxComponents );
        _bits[id.value / BitsPerChunk] |= (Chunk_t{1} << (id.value % BitsPerChunk));
        return *this;
    }

/*
=================================================
    Remove
=================================================
*/
    inline ArchetypeDesc&  ArchetypeDesc::Remove (ComponentID id) __NE___
    {
        ASSERT( id.value < ECS_Config::MaxComponents );
        _bits[id.value / BitsPerChunk] &= ~(Chunk_t{1} << (id.value % BitsPerChunk));
        return *this;
    }

/*
=================================================
    Add
=================================================
*/
    inline ArchetypeDesc&  ArchetypeDesc::Add (const ArchetypeDesc &other) __NE___
    {
        for (usize i = 0; i < _bits.size(); ++i) {
            _bits[i] |= other._bits[i];
        }
        return *this;
    }

/*
=================================================
    Remove
=================================================
*/
    inline ArchetypeDesc&  ArchetypeDesc::Remove (const ArchetypeDesc &other) __NE___
    {
        for (usize i = 0; i < _bits.size(); ++i) {
            _bits[i] &= ~other._bits[i];
        }
        return *this;
    }

/*
=================================================
    Exists
=================================================
*/
    inline bool  ArchetypeDesc::Exists (ComponentID id) C_NE___
    {
        ASSERT( id.value < ECS_Config::MaxComponents );
        return _bits[id.value / BitsPerChunk] & (Chunk_t{1} << (id.value % BitsPerChunk));
    }

/*
=================================================
    All
=================================================
*/
    inline bool  ArchetypeDesc::All (const ArchetypeDesc &rhs) C_NE___
    {
        bool    result = true;
        for (usize i = 0; i < _bits.size(); ++i) {
            result &= ((_bits[i] & rhs._bits[i]) == rhs._bits[i]);
        }
        return result;
    }

/*
=================================================
    Any
=================================================
*/
    inline bool  ArchetypeDesc::Any (const ArchetypeDesc &rhs) C_NE___
    {
        bool    result  = false;
        for (usize i = 0; i < _bits.size(); ++i) {
            result |= !!(_bits[i] & rhs._bits[i]);
        }
        return result;
    }

/*
=================================================
    AnyOrEmpty
=================================================
*/
    inline bool  ArchetypeDesc::AnyOrEmpty (const ArchetypeDesc &rhs) C_NE___
    {
        bool    result  = false;
        bool    empty   = true;

        for (usize i = 0; i < _bits.size(); ++i)
        {
            result |= !!(_bits[i] & rhs._bits[i]);
            empty  &= !_bits[i];
        }
        return result | empty;
    }

/*
=================================================
    Equals
=================================================
*/
    inline bool  ArchetypeDesc::Equals (const ArchetypeDesc &rhs) C_NE___
    {
        bool    result = true;
        for (usize i = 0; i < _bits.size(); ++i) {
            result &= (_bits[i] == rhs._bits[i]);
        }
        return result;
    }

/*
=================================================
    Empty
=================================================
*/
    inline bool  ArchetypeDesc::Empty () C_NE___
    {
        bool    result = true;
        for (usize i = 0; i < _bits.size(); ++i) {
            result &= !_bits[i];
        }
        return result;
    }

/*
=================================================
    Count
=================================================
*/
    inline usize  ArchetypeDesc::Count () C_NE___
    {
        usize   result = 0;
        for (usize i = 0; i < _bits.size(); ++i) {
            result += BitCount( _bits[i] );
        }
        return result;
    }

/*
=================================================
    GetHash
=================================================
*/
    inline HashVal  ArchetypeDesc::GetHash () C_NE___
    {
        Chunk_t h = _bits[0];
        for (usize i = 1; i < _bits.size(); ++i) {
            h = BitRotateLeft( h, 4 + i*4 ) ^ _bits[i];
        }

        #if AE_PLATFORM_BITS == 64
            STATIC_ASSERT( sizeof(usize) == sizeof(h) );
            return HashVal{ h };
        #else
            STATIC_ASSERT( sizeof(usize) != sizeof(h) );
            return HashVal{usize( h ^ (h >> 32) )};
        #endif
    }

/*
=================================================
    GetIDs
=================================================
*/
    inline ArchetypeDesc::ComponentIDs_t  ArchetypeDesc::GetIDs () C_NE___
    {
        ComponentIDs_t  result;

        for (usize i = 0; i < _bits.size(); ++i)
        {
            Chunk_t u = _bits[i];
            int     j = BitScanForward( u );

            for (; (j >= 0) & (result.size() <= result.capacity());
                 j = BitScanForward( u ))
            {
                u &= ~(1ull << j);

                ComponentID id{ CheckCast<ushort>( j + i*BitsPerChunk )};

                result.push_back( id );
            }
        }
        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Compatible
=================================================
*/
    inline bool  ArchetypeQueryDesc::Compatible (const ArchetypeDesc &desc) C_NE___
    {
        return  desc.All( required )            &
                (not subtractive.Any( desc ))   &
                requireAny.AnyOrEmpty( desc );
    }

/*
=================================================
    operator ==
=================================================
*/
    inline bool  ArchetypeQueryDesc::operator == (const ArchetypeQueryDesc &rhs) C_NE___
    {
        return  required.Equals( rhs.required )         &
                subtractive.Equals( rhs.subtractive )   &
                requireAny.Equals( rhs.requireAny );
    }

/*
=================================================
    IsValid
=================================================
*/
    inline bool  ArchetypeQueryDesc::IsValid () C_NE___
    {
        return not subtractive.Any( required );
    }

} // AE::ECS


template <>
struct std::hash< AE::ECS::Archetype >
{
    ND_ size_t  operator () (const AE::ECS::Archetype &value) C_NE___
    {
        return size_t(value.Hash());
    }
};

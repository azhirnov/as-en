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
		using					Chunk_t			= Bitfield< ulong >;
		static constexpr uint	BitsPerChunk	= CT_SizeOfInBits<Chunk_t>;
		static constexpr uint	ChunkCount		= ECS_Config::MaxComponents / BitsPerChunk;
		using					CompBits_t		= StaticArray< Chunk_t, ChunkCount >;

		using ComponentIDs_t = FixedArray< ComponentID, ECS_Config::MaxComponentsPerArchetype >;


	// variables
	private:
		CompBits_t		_bits	{};		// TODO: use SSE


	// methods
	public:
		constexpr ArchetypeDesc ()												__NE___	{}

		template <typename Comp>	constexpr ArchetypeDesc&	Add ()			__NE___	{ return Add( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	constexpr ArchetypeDesc&	Remove ()		__NE___	{ return Remove( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	ND_ constexpr bool			Exists ()		C_NE___	{ return Exists( ComponentTypeInfo<Comp>::id ); }

			constexpr ArchetypeDesc&  Add (ComponentID id)						__NE___;
			constexpr ArchetypeDesc&  Remove (ComponentID id)					__NE___;

			constexpr ArchetypeDesc&  Add (const ArchetypeDesc &other)			__NE___;
			constexpr ArchetypeDesc&  Remove (const ArchetypeDesc &other)		__NE___;

		ND_ constexpr CompBits_t const&		Raw ()								C_NE___	{ return _bits; }

		ND_ ComponentIDs_t		GetIDs ()										C_NE___;

		ND_ constexpr bool		Exists (ComponentID id)							C_NE___;
		ND_ constexpr bool		All (const ArchetypeDesc &)						C_NE___;
		ND_ constexpr bool		Any (const ArchetypeDesc &)						C_NE___;
		ND_ constexpr bool		AnyOrEmpty (const ArchetypeDesc &)				C_NE___;
		ND_ constexpr bool		Equal (const ArchetypeDesc &)					C_NE___;
		ND_ constexpr bool		Empty ()										C_NE___;
		ND_ constexpr usize		Count ()										C_NE___;

		ND_ constexpr HashVal	GetHash ()										C_NE___;
	};



	//
	// Archetype
	//

	class Archetype final
	{
	// variables
	private:
		HashVal				_hash;
		ArchetypeDesc		_desc;


	// methods
	public:
		constexpr explicit Archetype (const ArchetypeDesc &desc)	__NE___	: _hash{desc.GetHash()}, _desc{desc} {}

		ND_ constexpr HashVal				Hash ()					C_NE___	{ return _hash; }
		ND_ constexpr ArchetypeDesc const&	Desc ()					C_NE___	{ return _desc; }

		ND_ constexpr bool  operator == (const Archetype &rhs)		C_NE___	{ return Equal( rhs ); }

		ND_ constexpr bool	Equal (const Archetype &rhs)			C_NE___	{ return _desc.Equal( rhs._desc ); }
		ND_ constexpr bool	Contains (const Archetype &rhs)			C_NE___	{ return _desc.All( rhs._desc ); }
		ND_ constexpr bool	Exists (ComponentID id)					C_NE___	{ return _desc.Exists( id ); }

		template <typename T>
		ND_ constexpr bool	Exists ()								C_NE___	{ return Exists( ComponentTypeInfo<T>::id ); }
	};



	//
	// Archetype Query description
	//

	struct ArchetypeQueryDesc
	{
		ArchetypeDesc		required;
		ArchetypeDesc		subtractive;
		ArchetypeDesc		requireAny;

		constexpr ArchetypeQueryDesc ()									__NE___	{}

		ND_ constexpr bool  Compatible (const ArchetypeDesc &)			C_NE___;

		ND_ constexpr bool  operator == (const ArchetypeQueryDesc &rhs)	C_NE___;

		ND_ constexpr bool  IsValid ()									C_NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Add
=================================================
*/
	inline constexpr ArchetypeDesc&  ArchetypeDesc::Add (ComponentID id) __NE___
	{
		ASSERT( id.value < ECS_Config::MaxComponents );
		_bits[ id.value / BitsPerChunk ].Set( id.value % BitsPerChunk );
		return *this;
	}

/*
=================================================
	Remove
=================================================
*/
	inline constexpr ArchetypeDesc&  ArchetypeDesc::Remove (ComponentID id) __NE___
	{
		ASSERT( id.value < ECS_Config::MaxComponents );
		_bits[ id.value / BitsPerChunk ].Erase( id.value % BitsPerChunk );
		return *this;
	}

/*
=================================================
	Add
=================================================
*/
	inline constexpr ArchetypeDesc&  ArchetypeDesc::Add (const ArchetypeDesc &other) __NE___
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
	inline constexpr ArchetypeDesc&  ArchetypeDesc::Remove (const ArchetypeDesc &other) __NE___
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
	inline constexpr bool  ArchetypeDesc::Exists (ComponentID id) C_NE___
	{
		ASSERT( id.value < ECS_Config::MaxComponents );
		return _bits[ id.value / BitsPerChunk ].Has( id.value % BitsPerChunk );
	}

/*
=================================================
	All
=================================================
*/
	inline constexpr bool  ArchetypeDesc::All (const ArchetypeDesc &rhs) C_NE___
	{
		bool	result = true;
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
	inline constexpr bool  ArchetypeDesc::Any (const ArchetypeDesc &rhs) C_NE___
	{
		bool	result	= false;
		for (usize i = 0; i < _bits.size(); ++i) {
			result |= (_bits[i] & rhs._bits[i]).Any();
		}
		return result;
	}

/*
=================================================
	AnyOrEmpty
=================================================
*/
	inline constexpr bool  ArchetypeDesc::AnyOrEmpty (const ArchetypeDesc &rhs) C_NE___
	{
		bool	result	= false;
		bool	empty	= true;

		for (usize i = 0; i < _bits.size(); ++i)
		{
			result |= (_bits[i] & rhs._bits[i]).Any();
			empty  &= _bits[i].None();
		}
		return result | empty;
	}

/*
=================================================
	Equal
=================================================
*/
	inline constexpr bool  ArchetypeDesc::Equal (const ArchetypeDesc &rhs) C_NE___
	{
		bool	result = true;
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
	inline constexpr bool  ArchetypeDesc::Empty () C_NE___
	{
		bool	result = true;
		for (usize i = 0; i < _bits.size(); ++i) {
			result &= _bits[i].None();
		}
		return result;
	}

/*
=================================================
	Count
=================================================
*/
	inline constexpr usize  ArchetypeDesc::Count () C_NE___
	{
		usize	result = 0;
		for (usize i = 0; i < _bits.size(); ++i) {
			result += _bits[i].BitCount();
		}
		return result;
	}

/*
=================================================
	GetHash
=================================================
*/
	inline constexpr HashVal  ArchetypeDesc::GetHash () C_NE___
	{
		auto	h = _bits[0].Get();
		for (usize i = 1; i < _bits.size(); ++i) {
			h = BitRotateLeft( h, 4 + i*4 ) ^ _bits[i].Get();
		}

		#if AE_PLATFORM_BITS == 64
			StaticAssert( sizeof(usize) == sizeof(h) );
			return HashVal{ h };
		#else
			StaticAssert( sizeof(usize) != sizeof(h) );
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
		ComponentIDs_t	result;

		for (usize i = 0; i < _bits.size(); ++i)
		{
			Chunk_t	u = _bits[i];
			int		j = u.ExtractBitIndex();

			for (; (j >= 0) and (not result.IsFull()); j = u.ExtractBitIndex())
			{
				ComponentID	id{ CheckCast<ushort>( j + i*BitsPerChunk )};

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
	inline constexpr bool  ArchetypeQueryDesc::Compatible (const ArchetypeDesc &desc) C_NE___
	{
		return	desc.All( required )			and
				(not subtractive.Any( desc ))	and
				requireAny.AnyOrEmpty( desc );
	}

/*
=================================================
	operator ==
=================================================
*/
	inline constexpr bool  ArchetypeQueryDesc::operator == (const ArchetypeQueryDesc &rhs) C_NE___
	{
		return	required.Equal( rhs.required )			and
				subtractive.Equal( rhs.subtractive )	and
				requireAny.Equal( rhs.requireAny );
	}

/*
=================================================
	IsValid
=================================================
*/
	inline constexpr bool  ArchetypeQueryDesc::IsValid () C_NE___
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

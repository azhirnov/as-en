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
		__Cx__ ArchetypeDesc ()											__NE___	{}

		template <typename Comp>	__Cz__ ArchetypeDesc&	Add ()		__NE___	{ return Add( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	__Cz__ ArchetypeDesc&	Remove ()	__NE___	{ return Remove( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	NdCz__ bool				Exists ()	C_NE___	{ return Exists( ComponentTypeInfo<Comp>::id ); }

		__Cz__ ArchetypeDesc&		Add (ComponentID id)				__NE___;
		__Cz__ ArchetypeDesc&		Remove (ComponentID id)				__NE___;

		__Cx__ ArchetypeDesc&		Add (const ArchetypeDesc &other)	__NE___;
		__Cx__ ArchetypeDesc&		Remove (const ArchetypeDesc &other)	__NE___;

		NdCx__ CompBits_t const&	Raw ()								C_NE___	{ return _bits; }

		Nd____ ComponentIDs_t		GetIDs ()							C_NE___;

		NdCz__ bool		Exists (ComponentID id)							C_NE___;
		NdCx__ bool		All (const ArchetypeDesc &)						C_NE___;
		NdCx__ bool		Any (const ArchetypeDesc &)						C_NE___;
		NdCx__ bool		AnyOrEmpty (const ArchetypeDesc &)				C_NE___;
		NdCx__ bool		Equal (const ArchetypeDesc &)					C_NE___;
		NdCx__ bool		Empty ()										C_NE___;
		NdCx__ usize	Count ()										C_NE___;

		NdCx__ HashVal	GetHash ()										C_NE___;
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
		__Cx__ explicit Archetype (const ArchetypeDesc &desc)	__NE___	: _hash{desc.GetHash()}, _desc{desc} {}

		NdCx__ HashVal				Hash ()						C_NE___	{ return _hash; }
		NdCx__ ArchetypeDesc const&	Desc ()						C_NE___	{ return _desc; }

		NdCx__ bool  operator == (const Archetype &rhs)			C_NE___	{ return Equal( rhs ); }

		NdCx__ bool  Equal (const Archetype &rhs)				C_NE___	{ return _desc.Equal( rhs._desc ); }
		NdCx__ bool  Contains (const Archetype &rhs)			C_NE___	{ return _desc.All( rhs._desc ); }
		NdCz__ bool	 Exists (ComponentID id)					C_NE___	{ return _desc.Exists( id ); }

		template <typename T>
		NdCz__ bool  Exists ()									C_NE___	{ return Exists( ComponentTypeInfo<T>::id ); }
	};



	//
	// Archetype Query description
	//

	struct ArchetypeQueryDesc
	{
		ArchetypeDesc		required;
		ArchetypeDesc		subtractive;
		ArchetypeDesc		requireAny;

		__Cx__ ArchetypeQueryDesc ()								__NE___	{}

		NdCx__ bool  Compatible (const ArchetypeDesc &)				C_NE___;

		NdCx__ bool  operator == (const ArchetypeQueryDesc &rhs)	C_NE___;

		NdCx__ bool  IsValid ()										C_NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	Add
=================================================
*/
	__CzIn ArchetypeDesc&  ArchetypeDesc::Add (ComponentID id) __NE___
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
	__CzIn ArchetypeDesc&  ArchetypeDesc::Remove (ComponentID id) __NE___
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
	__CxIn ArchetypeDesc&  ArchetypeDesc::Add (const ArchetypeDesc &other) __NE___
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
	__CxIn ArchetypeDesc&  ArchetypeDesc::Remove (const ArchetypeDesc &other) __NE___
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
	__CzIn bool  ArchetypeDesc::Exists (ComponentID id) C_NE___
	{
		ASSERT( id.value < ECS_Config::MaxComponents );
		return _bits[ id.value / BitsPerChunk ].Has( id.value % BitsPerChunk );
	}

/*
=================================================
	All
=================================================
*/
	__CxIn bool  ArchetypeDesc::All (const ArchetypeDesc &rhs) C_NE___
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
	__CxIn bool  ArchetypeDesc::Any (const ArchetypeDesc &rhs) C_NE___
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
	__CxIn bool  ArchetypeDesc::AnyOrEmpty (const ArchetypeDesc &rhs) C_NE___
	{
		bool	result	= false;
		bool	empty	= true;

		for (usize i = 0; i < _bits.size(); ++i)
		{
			result |= (_bits[i] & rhs._bits[i]).Any();
			empty  &= _bits[i].None();
		}
		return result or empty;
	}

/*
=================================================
	Equal
=================================================
*/
	__CxIn bool  ArchetypeDesc::Equal (const ArchetypeDesc &rhs) C_NE___
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
	__CxIn bool  ArchetypeDesc::Empty () C_NE___
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
	__CxIn usize  ArchetypeDesc::Count () C_NE___
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
	__CxIn HashVal  ArchetypeDesc::GetHash () C_NE___
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
	__CxIn bool  ArchetypeQueryDesc::Compatible (const ArchetypeDesc &desc) C_NE___
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
	__CxIn bool  ArchetypeQueryDesc::operator == (const ArchetypeQueryDesc &rhs) C_NE___
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
	__CxIn bool  ArchetypeQueryDesc::IsValid () C_NE___
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

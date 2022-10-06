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
		using					Chunk_t			= ulong;
		static constexpr uint	BitsPerChunk	= CT_SizeOfInBits<Chunk_t>;
		static constexpr uint	ChunkCount		= ECS_Config::MaxComponents / BitsPerChunk;
		using					CompBits_t		= StaticArray< Chunk_t, ChunkCount >;

		using ComponentIDs_t = FixedArray< ComponentID, ECS_Config::MaxComponentsPerArchetype >;


	// variables
	private:
		CompBits_t		_bits;		// TODO: use SSE


	// methods
	public:
		ArchetypeDesc ()												{ ZeroMem( _bits.data(), ArraySizeOf(_bits) ); }

		template <typename Comp>	ArchetypeDesc&  Add ()				{ return Add( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	ArchetypeDesc&  Remove ()			{ return Remove( ComponentTypeInfo<Comp>::id ); }
		template <typename Comp>	ND_ bool		Exists ()	const	{ return Exists( ComponentTypeInfo<Comp>::id ); }

		ArchetypeDesc&  Add (ComponentID id);
		ArchetypeDesc&  Remove (ComponentID id);
		
		ArchetypeDesc&  Add (const ArchetypeDesc &other);
		ArchetypeDesc&  Remove (const ArchetypeDesc &other);

		ND_ CompBits_t const&	Raw () const							{ return _bits; }

		ND_ ComponentIDs_t		GetIDs () const;

		ND_ bool		Exists (ComponentID id) const;
		ND_ bool		All (const ArchetypeDesc &) const;
		ND_ bool		Any (const ArchetypeDesc &) const;
		ND_ bool		AnyOrEmpty (const ArchetypeDesc &) const;
		ND_ bool		Equals (const ArchetypeDesc &) const;
		ND_ bool		Empty () const;
		ND_ usize		Count () const;

		ND_ HashVal		GetHash () const;
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
		explicit Archetype (const ArchetypeDesc &desc) : _hash{desc.GetHash()}, _desc{desc} {}

		ND_ HashVal					Hash ()			const	{ return _hash; }
		ND_ ArchetypeDesc const&	Desc ()			const	{ return _desc; }

		ND_ bool operator == (const Archetype &rhs)	const	{ return Equals( rhs ); }

		ND_ bool	Equals (const Archetype &rhs)	const	{ return _desc.Equals( rhs._desc ); }
		ND_ bool	Contains (const Archetype &rhs)	const	{ return _desc.All( rhs._desc ); }
		ND_ bool	Exists (ComponentID id)			const	{ return _desc.Exists( id ); }

		template <typename T>
		ND_ bool	Exists () const							{ return Exists( ComponentTypeInfo<T>::id ); }
	};
	


	//
	// Archetype Query description
	//

	struct ArchetypeQueryDesc
	{
		ArchetypeDesc		required;
		ArchetypeDesc		subtractive;
		ArchetypeDesc		requireAny;

		ArchetypeQueryDesc () {}

		ND_ bool  Compatible (const ArchetypeDesc &) const;

		ND_ bool  operator == (const ArchetypeQueryDesc &rhs) const;

		ND_ bool  IsValid () const;
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	Add
=================================================
*/
	inline ArchetypeDesc&  ArchetypeDesc::Add (ComponentID id)
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
	inline ArchetypeDesc&  ArchetypeDesc::Remove (ComponentID id)
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
	inline ArchetypeDesc&  ArchetypeDesc::Add (const ArchetypeDesc &other)
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
	inline ArchetypeDesc&  ArchetypeDesc::Remove (const ArchetypeDesc &other)
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
	inline bool  ArchetypeDesc::Exists (ComponentID id) const
	{
		ASSERT( id.value < ECS_Config::MaxComponents );
		return _bits[id.value / BitsPerChunk] & (Chunk_t{1} << (id.value % BitsPerChunk));
	}
	
/*
=================================================
	All
=================================================
*/
	inline bool  ArchetypeDesc::All (const ArchetypeDesc &rhs) const
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
	inline bool  ArchetypeDesc::Any (const ArchetypeDesc &rhs) const
	{
		bool	result	= false;
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
	inline bool  ArchetypeDesc::AnyOrEmpty (const ArchetypeDesc &rhs) const
	{
		bool	result	= false;
		bool	empty	= true;

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
	inline bool  ArchetypeDesc::Equals (const ArchetypeDesc &rhs) const
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
	inline bool  ArchetypeDesc::Empty () const
	{
		bool	result = true;
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
	inline usize  ArchetypeDesc::Count () const
	{
		usize	result = 0;
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
	inline HashVal  ArchetypeDesc::GetHash () const
	{
		Chunk_t	h = _bits[0];
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
	inline ArchetypeDesc::ComponentIDs_t  ArchetypeDesc::GetIDs () const
	{
		ComponentIDs_t	result;

		for (usize i = 0; i < _bits.size(); ++i)
		{
			Chunk_t	u = _bits[i];
			int		j = BitScanForward( u );

			for (; (j >= 0) & (result.size() <= result.capacity());
				 j = BitScanForward( u ))
			{
				u &= ~(1ull << j);
				
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
	inline bool  ArchetypeQueryDesc::Compatible (const ArchetypeDesc &desc) const
	{
		return	desc.All( required )			&
				(not subtractive.Any( desc ))	&
				requireAny.AnyOrEmpty( desc );
	}
	
/*
=================================================
	operator ==
=================================================
*/
	inline bool  ArchetypeQueryDesc::operator == (const ArchetypeQueryDesc &rhs) const
	{
		return	required.Equals( rhs.required )			&
				subtractive.Equals( rhs.subtractive )	&
				requireAny.Equals( rhs.requireAny );
	}
	
/*
=================================================
	IsValid
=================================================
*/
	inline bool  ArchetypeQueryDesc::IsValid () const
	{
		return not subtractive.Any( required );
	}

}	// AE::ECS


namespace std
{
	template <>
	struct hash< AE::ECS::Archetype >
	{
		ND_ size_t  operator () (const AE::ECS::Archetype &value) const
		{
			return size_t(value.Hash());
		}
	};

}	// std

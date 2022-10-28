// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/NamedID.h"

namespace AE::Base
{

	//
	// Hash Collision Check
	//

	class NamedID_HashCollisionCheck
	{
	// types
	private:
		using StString_t	= FixedString<64>;
		using UniqueIDs_t	= HashMultiMap< usize, StString_t >;
		
		struct Info
		{
			UniqueIDs_t	data;
			uint		seed			= 0;
			bool		hasCollisions	= false;

			Info () {}
			explicit Info (uint seed) : seed{seed} {}
		};
		using IdMap_t	= HashMap< /*UID*/uint, Info >;


	// variables
	private:
		IdMap_t		_idMap;


	// methods
	public:
		~NamedID_HashCollisionCheck ();
		
		// require write lock
		template <usize Size, uint UID, bool Optimize, uint Seed>
		void  Add (const NamedID<Size, UID, Optimize, Seed> &id);
		
		// require read lock
		template <usize Size, uint UID, bool Optimize, uint Seed>
		ND_ uint  RecalculateSeed (const NamedID<Size, UID, Optimize, Seed> &);

		ND_ bool  HasCollisions () const;
		
	private:
		ND_ uint  _RecalculateSeed (Info &) const;
	};

	
/*
=================================================
	Add
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	inline void  NamedID_HashCollisionCheck::Add (const NamedID<Size, UID, Optimize, Seed> &id)
	{
		if constexpr( not Optimize )
		{
			STATIC_ASSERT( Size <= StString_t::capacity() );

			auto&	info	 = _idMap.emplace( UID, Info{Seed} ).first->second;
			usize	key		 = usize(id.GetHash());
			auto	iter	 = info.data.find( key );
			bool	inserted = false;

			for (; iter != info.data.end() and iter->first == key; ++iter)
			{
				if ( iter->second != id.GetName() )
				{
					DBG_WARNING( "hash collision detected" );
					info.hasCollisions = true;
				}
				inserted = true;
			}

			if ( not inserted )
				info.data.emplace( key, id.GetName() );
		}
	}

/*
=================================================
	RecalculateSeed
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	inline uint  NamedID_HashCollisionCheck::RecalculateSeed (const NamedID<Size, UID, Optimize, Seed> &)
	{
		auto	iter = _idMap.find( UID );
		if ( iter == _idMap.end() )
			return Seed;
		
		if ( iter->second.seed != Seed )
			return iter->second.seed;

		return _RecalculateSeed( iter->second );
	}


} // AE::Base

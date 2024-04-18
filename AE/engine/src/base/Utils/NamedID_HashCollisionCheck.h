// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

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
		using StString_t	= FixedString<128>;
		using UniqueIDs_t	= HashMultiMap< /*hash*/usize, StString_t >;

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
		IdMap_t		_uidMap;


	// methods
	public:
		~NamedID_HashCollisionCheck ()												__NE___;

		// require write lock
		template <usize Size, uint UID, uint Seed>
		void  Add (const NamedID<Size, UID, false, Seed> &id)						__Th___;

		template <usize Size, uint UID, bool Optimize, uint Seed>
		void  Add (const NamedID<Size, UID, Optimize, Seed> &id, StringView name)	__Th___;

		void  Merge (const NamedID_HashCollisionCheck &src)							__Th___;


		// require read lock
		template <usize Size, uint UID, bool Optimize, uint Seed>
		ND_ StringView  GetString (const NamedID<Size, UID, Optimize, Seed> &id)	C_NE___;

		template <usize Size, uint UID, bool Optimize, uint Seed>
		ND_ uint  RecalculateSeed (const NamedID<Size, UID, Optimize, Seed> &)		__Th___;

		ND_ bool  HasCollisions ()													C_NE___;

			void  Clear ()															__NE___;
			void  CheckAndClear ()													__NE___;

	private:
		ND_ uint  _RecalculateSeed (Info &) const									__Th___;
	};


/*
=================================================
	Add
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	void  NamedID_HashCollisionCheck::Add (const NamedID<Size, UID, Optimize, Seed> &id, StringView name) __Th___
	{
		StaticAssert( Size <= StString_t::capacity() );

		auto&		info	 = _uidMap.emplace( UID, Info{Seed} ).first->second;	// throw
		const usize	key		 = usize(id.GetHash());
		auto		it		 = info.data.find( key );
		bool		inserted = false;

		for (; it != info.data.end() and it->first == key; ++it)
		{
			if ( it->second != name )
			{
				DBG_WARNING( "hash collision detected" );
				info.hasCollisions = true;
			}
			inserted = true;
		}

		if ( not inserted )
			info.data.emplace( key, StString_t{name} );	// throw
	}

/*
=================================================
	Add
=================================================
*/
	template <usize Size, uint UID, uint Seed>
	void  NamedID_HashCollisionCheck::Add (const NamedID<Size, UID, false, Seed> &id) __Th___
	{
		return Add( id, id.GetName() );
	}

/*
=================================================
	GetString
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	StringView  NamedID_HashCollisionCheck::GetString (const NamedID<Size, UID, Optimize, Seed> &id) C_NE___
	{
		auto	uid_it = _uidMap.find( UID );
		if ( uid_it != _uidMap.end() )
		{
			const usize	key		 = usize(id.GetHash());
			auto&		name_map = uid_it->second.data;
			auto		it		 = name_map.find( key );

			ASSERT( not uid_it->second.hasCollisions );

			if ( it != name_map.end() )
				return StringView{it->second};
		}
		return Default;
	}

/*
=================================================
	RecalculateSeed
=================================================
*/
	template <usize Size, uint UID, bool Optimize, uint Seed>
	uint  NamedID_HashCollisionCheck::RecalculateSeed (const NamedID<Size, UID, Optimize, Seed> &) __Th___
	{
		auto	uid_it = _uidMap.find( UID );
		if ( uid_it == _uidMap.end() )
			return Seed;

		if ( uid_it->second.seed != Seed )
			return uid_it->second.seed;

		return _RecalculateSeed( uid_it->second );	// throw
	}


} // AE::Base

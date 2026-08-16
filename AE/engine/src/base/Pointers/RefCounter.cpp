// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Pointers/RefCounter.h"
#include "base/FileSystem/FileSystem.h"

#ifdef AE_RC_TRACK_ALL_REFS

# ifndef AE_DEBUG
#	error Can not defined AE_RC_TRACK_ALL_REFS for non-debug build!
# endif

namespace AE::Base
{
namespace
{
	ND_ static String  GetCallStack ()
	{
		return StackTrace::ToString{};
	}

	struct RCLocation
	{
		uint	uid			= 0;;
		int		refCount	= 0;
	};

	struct RCObjectRefs
	{
		using Locations_t	= HashMap< String, RCLocation >;
		using UidToLoc_t	= HashMap< uint, Locations_t::iterator >;

		Locations_t		locations;
		UidToLoc_t		uidToLoc;
		uint			uidCounter			= 0;
		uint			decRefWithoutUID	= 0;
	};

	static SharedMutex								s_allRCObjectsGuard;
	static HashMap< EnableRCBase*, RCObjectRefs >	s_allRCObjects;

} // namespace


/*
=================================================
	IncRef
=================================================
*/
	int  RefCounterUtils::IncRef (EnableRCBase &obj) __NE___
	{
		// TODO
		return obj._counter.fetch_add( 1 );
	}

/*
=================================================
	DecRef
=================================================
*/
	int  RefCounterUtils::DecRef (EnableRCBase &obj) __NE___
	{
		// TODO
		return obj._counter.fetch_sub( 1 );
	}

/*
=================================================
	IncRef
=================================================
*/
	int  RefCounterUtils::IncRef (EnableRCBase &obj, OUT uint &rcUID) __NE___
	{
		{
			String	stack = GetCallStack();

			EXLOCK( s_allRCObjectsGuard );

			auto&	all_obj = s_allRCObjects;
			auto&	cur		= all_obj[ &obj ];

			auto	[it, inserted] = cur.locations.emplace( RVRef(stack), RCLocation{} );
			if ( inserted )
			{
				cur.uidCounter++;
				it->second.uid = cur.uidCounter;

				cur.uidToLoc.emplace( cur.uidCounter, it );
				ASSERT( cur.locations.size() == cur.uidToLoc.size() );
			}
			++it->second.refCount;
			rcUID = it->second.uid;
		}

		return obj._counter.fetch_add( 1 );
	}

/*
=================================================
	DecRef
=================================================
*/
	int  RefCounterUtils::DecRef (EnableRCBase &obj, uint rcUID) __NE___
	{
		int		cnt = obj._counter.fetch_sub( 1 );
		{
			EXLOCK( s_allRCObjectsGuard );

			auto&	all_obj = s_allRCObjects;

			auto	it = all_obj.find( &obj );
			ASSERT( it != all_obj.end() );

			if ( it != all_obj.end() )
			{
				if ( cnt == 1 )
				{
					if ( it->second.decRefWithoutUID == 0 and rcUID != 0 )
					{
						ASSERT( it->second.locations.size() == 1 );
						ASSERT( it->second.locations.begin()->second.uid == rcUID );
						ASSERT( it->second.locations.begin()->second.refCount == 1 );
					}
					all_obj.erase( it );
				}
				else
				if ( rcUID != 0 )
				{
					auto&	cur = it->second;

					auto	it2	= cur.uidToLoc.find( rcUID );
					ASSERT( it2 != cur.uidToLoc.end() );

					auto	it3	= it2->second;

					if ( --it3->second.refCount <= 0 )
					{
						ASSERT( it3->second.refCount == 0 );

						cur.locations.erase( it3 );
						cur.uidToLoc.erase( it2 );

						ASSERT( cur.locations.size() == cur.uidToLoc.size() );
					}
				}
				else
				{
					it->second.decRefWithoutUID++;
				}
			}
		}
		return cnt;
	}

/*
=================================================
	PrintRefs
=================================================
*/
	void  RefCounterUtils::PrintRefs () __NE___
	{
		EXLOCK( s_allRCObjectsGuard );

		if ( s_allRCObjects.empty() )
			return;

		String	str = "Found alive pointers";

		for (auto& [ptr, info] : s_allRCObjects)
		{
			str << "\n0x" << ToString<16>( BitCast<usize>( ptr ))
				<< " - " << typeid(*ptr).name();

			for (auto& [loc, uid_rc] : info.locations)
			{
				str << "\n  ref count: " << ToString(uid_rc.refCount)
					<< "\n" << loc
					<< "\n----\n";
			}
		}

		//decltype(s_allRCObjects)	temp;
		//std::swap( temp, s_allRCObjects );

		AE_LOGW( str );
	}

} // AE::Base
#endif // AE_RC_TRACK_ALL_REFS

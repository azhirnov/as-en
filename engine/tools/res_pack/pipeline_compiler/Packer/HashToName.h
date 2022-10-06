// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/ObjectFactory.h"
#include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

	//
	// Hash To Name
	//
	struct HashToName
	{
	// types
	public:
		struct NameHash
		{
			uint	hash	= 0;
			uint	uid		= 0;

			ND_ bool  operator == (const NameHash &rhs) const {
				return	hash == rhs.hash and uid == rhs.uid;
			}
		};

		struct NameHashHash {
			ND_ usize  operator () (const NameHash &key) const {
				return usize( HashOf( key.hash ) + HashOf( key.uid ));
			}
		};


	// variables
	private:
		HashMap< NameHash, String, NameHashHash >		_map;


	// methods
	public:
		HashToName () {}
		

		template <usize Size, uint UID, bool Opt, uint Seed>
		ND_ String  operator () (const NamedID< Size, UID, Opt, Seed > &name) const
		{
			NameHash	key{ uint(name.GetHash32()), UID };

			auto	iter = _map.find( key );
			if ( iter != _map.end() )
				return iter->second;
			else
			{
				if constexpr( not Opt )
				{
					if ( not name.GetName().empty() )
						return String{name.GetName()};
				}
				return ToString<16>( usize(name.GetHash()) );
			}
		}

		ND_ bool  Deserialize (Serializing::Deserializer &des)
		{
			uint	count = 0;
			CHECK_ERR( des( OUT count ));

			_map.reserve( count );

			for (uint i = 0; i < count; ++i)
			{
				NameHash	info;
				String		name;
				CHECK_ERR( des( OUT info.hash, OUT info.uid, OUT name ));
				CHECK_ERR( not name.empty() );

				CHECK( _map.emplace( info, name ).second );
			}
			return true;
		}


		void  Merge (const HashToName &value)
		{
			for (auto& [h, n] : value._map) {
				_map.emplace( h, n );
			}
		}
	};

} // AE::Base

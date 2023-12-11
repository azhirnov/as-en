// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Basic/ObjectFactory.h"
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
            uint    hash    = 0;
            uint    uid     = 0;

            ND_ bool  operator == (const NameHash &rhs) C_NE___ {
                return  hash == rhs.hash and uid == rhs.uid;
            }
        };

        struct NameHashHash {
            ND_ usize  operator () (const NameHash &key) C_NE___ {
                return usize( HashOf( key.hash ) + HashOf( key.uid ));
            }
        };


    // variables
    private:
        HashMap< NameHash, String, NameHashHash >       _map;


    // methods
    public:
        HashToName () __NE___ {}


        template <usize Size, uint UID, bool Opt, uint Seed>
        ND_ String  operator () (const NamedID< Size, UID, Opt, Seed > &name) C_Th___
        {
            NameHash    key{ uint(name.GetHash32()), UID };

            auto    iter = _map.find( key );
            if ( iter != _map.end() )
                return iter->second;    // throw
            else
            {
                if constexpr( not Opt )
                {
                    if ( not name.GetName().empty() )
                        return String{name.GetName()};  // throw
                }
                return ToString<16>( usize(name.GetHash()) );   // throw
            }
        }

        ND_ bool  Deserialize (Serializing::Deserializer &des) __NE___
        {
            try {
                uint    count = 0;
                CHECK_ERR( des( OUT count ));

                _map.reserve( count );  // throw

                for (uint i = 0; i < count; ++i)
                {
                    NameHash    info;
                    String      name;
                    CHECK_ERR( des( OUT info.hash, OUT info.uid, OUT name ));
                    CHECK_ERR( not name.empty() );

                    CHECK( _map.emplace( info, name ).second ); // throw
                }
                return true;
            }
            catch(...) {
                return false;
            }
        }


        void  Merge (const HashToName &value) __Th___
        {
            for (auto& [h, n] : value._map) {
                _map.emplace( h, n );
            }
        }
    };

} // AE::Base

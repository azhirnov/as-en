// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/StringUtils.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"

namespace AE::Base
{

/*
=================================================
    destructor
=================================================
*/
    NamedID_HashCollisionCheck::~NamedID_HashCollisionCheck () __NE___
    {
        CheckAndClear();
    }

/*
=================================================
    CheckAndClear
=================================================
*/
    void  NamedID_HashCollisionCheck::CheckAndClear () __NE___
    {
        TRY{
            for (auto& id : _uidMap)
            {
                if_unlikely( id.second.hasCollisions )
                {
                    uint    seed = _RecalculateSeed( id.second );
                    CHECK_MSG( false, "Use new seed: "s << ToString( seed ));
                }
            }
        }
        CATCH_ALL()

        _uidMap.clear();
    }

/*
=================================================
    _RecalculateSeed
=================================================
*/
    uint  NamedID_HashCollisionCheck::_RecalculateSeed (Info &info) C_Th___
    {
        std::mt19937    gen{ std::random_device{}() };
        for (;;)
        {
            // generate new seed
            uint    new_seed        = gen();
            bool    has_collision   = false;

            FlatHashMap< usize, StString_t >    map;
            for (auto& item : info.data)
            {
                uint    key = uint{CT_Hash( item.second.data(), item.second.length(), new_seed )};

                if ( not map.emplace( key, StringView{item.second} ).second )
                {
                    has_collision = true;
                    break;
                }
            }

            if ( not has_collision )
            {
                info.seed = new_seed;
                return new_seed;
            }
        }
    }

/*
=================================================
    HasCollisions
=================================================
*/
    bool  NamedID_HashCollisionCheck::HasCollisions () C_NE___
    {
        for (auto& item : _uidMap)
        {
            if ( item.second.hasCollisions )
                return true;
        }
        return false;
    }

/*
=================================================
    Clear
=================================================
*/
    void  NamedID_HashCollisionCheck::Clear () __NE___
    {
        _uidMap.clear();
    }

/*
=================================================
    Merge
=================================================
*/
    void  NamedID_HashCollisionCheck::Merge (const NamedID_HashCollisionCheck &src) __Th___
    {
        for (auto& [uid, src_info] : src._uidMap)
        {
            auto&   dst_info = this->_uidMap.emplace( uid, Info{src_info.seed} ).first->second;  // throw

            for (auto& [key, name] : src_info.data)
            {
                auto    it       = dst_info.data.find( key );
                bool    inserted = false;

                for (; it != dst_info.data.end() and it->first == key; ++it)
                {
                    if ( it->second != name )
                    {
                        DBG_WARNING( "hash collision detected" );
                        dst_info.hasCollisions = true;
                    }
                    inserted = true;
                }

                if ( not inserted )
                    dst_info.data.emplace( key, name );  // throw
            }
        }
    }


} // AE::Base

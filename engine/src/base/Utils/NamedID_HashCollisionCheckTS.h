// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:  yes
*/

#pragma once

#include "base/Utils/Threading.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"

namespace AE::Base
{

    //
    // Hash Collision Check (thread-safe)
    //

    class NamedID_HashCollisionCheckTS
    {
    // variables
    private:
        mutable SharedMutex             _guard;
        NamedID_HashCollisionCheck      _hcc;


    // methods
    public:
        ~NamedID_HashCollisionCheckTS ()                                            __Th___ { EXLOCK( _guard ); }


        template <usize Size, uint UID, uint Seed>
        void  Add (const NamedID<Size, UID, false, Seed> &id)                       __Th___ { EXLOCK( _guard );  _hcc.Add( id ); }

        template <usize Size, uint UID, bool Optimize, uint Seed>
        void  Add (const NamedID<Size, UID, Optimize, Seed> &id, StringView name)   __Th___ { EXLOCK( _guard );  _hcc.Add( id, name ); }

        void  Merge (const NamedID_HashCollisionCheck &src)                         __Th___ { EXLOCK( _guard );  _hcc.Merge( src ); }


        template <usize Size, uint UID, bool Optimize, uint Seed>
        ND_ StringView  GetString (const NamedID<Size, UID, Optimize, Seed> &id)    C_NE___ { SHAREDLOCK( _guard );  return _hcc.GetString( id ); }

        template <usize Size, uint UID, bool Optimize, uint Seed>
        ND_ uint  RecalculateSeed (const NamedID<Size, UID, Optimize, Seed> &id)    __Th___ { SHAREDLOCK( _guard );  return _hcc.RecalculateSeed( id ); }

        ND_ bool  HasCollisions ()                                                  C_NE___ { SHAREDLOCK( _guard );  return _hcc.HasCollisions(); }

            void  Clear ()                                                          __NE___ { SHAREDLOCK( _guard );  _hcc.Clear(); }
    };


} // AE::Base

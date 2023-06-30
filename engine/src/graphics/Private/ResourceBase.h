// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"

namespace AE::Graphics
{

    //
    // Resource Wrapper
    //

    template <typename ResType>
    class alignas(AE_CACHE_LINE) ResourceBase final
    {
    // types
    public:
        enum class EState : uint
        {
            Initial         = 0,
            Failed,
            Created,
        };

        using Self          = ResourceBase< ResType >;
        using Resource_t    = ResType;
        using Generation_t  = BufferID::Generation_t;


    // variables
    private:
        // instance counter used to detect deprecated handles
        Atomic<uint>            _generation {0};

        Atomic<EState>          _state      {EState::Initial};

        ResType                 _data;

        mutable Atomic<int>     _refCounter {0};


    // methods
    public:
        ResourceBase ()                         __NE___ {}

        ResourceBase (Self &&)                  = delete;
        ResourceBase (const Self &)             = delete;

        Self& operator = (Self &&)              = delete;
        Self& operator = (const Self &)         = delete;

        ~ResourceBase ()                        __NE___ { ASSERT( IsDestroyed() ); }

            void    AddRef ()                   C_NE___ { _refCounter.fetch_add( 1 ); }
        ND_ int     ReleaseRef (int refCount)   C_NE___ { return (_refCounter.fetch_sub( refCount ) - refCount); }


        ND_ bool            IsCreated ()        C_NE___ { return _GetState() == EState::Created; }
        ND_ bool            IsDestroyed ()      C_NE___ { return _GetState() <= EState::Failed; }

        ND_ Generation_t    GetGeneration ()    C_NE___ { return Generation_t(_generation.load()); }
        ND_ int             GetRefCount ()      C_NE___ { return _refCounter.load(); }

        ND_ ResType&        Data ()             __NE___ { return _data; }
        ND_ ResType const&  Data ()             C_NE___ { return _data; }


        template <typename ...Args>
        ND_ bool  Create (Args&& ...args)       __NE___
        {
            ASSERT( IsDestroyed() );
            ASSERT( GetRefCount() == 0 );

            bool    result = _data.Create( FwdArg<Args &&>( args )... );

            // set state and flush cache
            _state.store( result ? EState::Created : EState::Failed, EMemoryOrder::Release );

            return result;
        }

        template <typename ...Args>
        void  Destroy (Args&& ...args)          __NE___
        {
            ASSERT( not IsDestroyed() );
            //ASSERT( GetRefCount() == 0 );

            _data.Destroy( FwdArg<Args &&>( args )... );

            // flush cache
            MemoryBarrier( EMemoryOrder::Release );

            // update atomics
            _refCounter.store( 0 );
            _state.store( EState::Initial );

            constexpr uint  max_gen = BufferID::MaxGeneration();

            for (uint exp = _generation.load();
                 not _generation.CAS( INOUT exp, (exp < max_gen ? exp + 1 : 0) );)
            {
                ThreadUtils::Pause();
            }
        }

    private:
        ND_ EState  _GetState ()                C_NE___ { return _state.load(); }
    };


} // AE::Graphics

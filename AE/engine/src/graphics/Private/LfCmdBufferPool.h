// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Graphics
{

    //
    // Lock-free Command Buffer Pool
    //

    template <typename NativeCmdBufferType,
              typename BakedCommandsType
             >
    struct LfCmdBufferPool
    {
    // types
    public:
        using NativeCmdBuffer_t     = NativeCmdBufferType;
        using BakedCommands_t       = BakedCommandsType;

        STATIC_ASSERT( sizeof(NativeCmdBuffer_t) >= sizeof(BakedCommands_t) );

        union Cmdbuf
        {
            NativeCmdBuffer_t       native;
            BakedCommands_t         baked;

            Cmdbuf () __NE___ {}
        };
        using Pool_t = StaticArray< Cmdbuf, GraphicsConfig::MaxCmdBufPerBatch >;


    // variables
    protected:
        Atomic<uint>    _ready      {0};    // 1 - commands recording have been completed and added to pool
        Atomic<uint>    _cmdTypes   {0};    // 0 - vulkan cmd buffer, 1 - backed commands
        Atomic<uint>    _counter    {0};    // index in '_pool'
        Atomic<uint>    _count      {0};    // number of commands in '_pool'
        Pool_t          _pool;

        // don't use 'UMax' because after ++ it will be 0 - valid value again.
        static constexpr uint   _CounterLargeValue = (1u << 31);

        DRC_ONLY( RWDataRaceCheck   _drCheck; ) // for '_pool' and '_count'


    // methods
    public:
        LfCmdBufferPool ()                                          __NE___;

        // user api (thread safe)
        ND_ uint  Current ()                                        C_NE___ { return _counter.load(); }
        ND_ uint  Acquire ()                                        __NE___;
            void  Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf)   __NE___;
            void  Add (INOUT uint& idx, BakedCommands_t ctx)        __NE___;
            void  Complete (INOUT uint& idx)                        __NE___;
        ND_ uint  Count ()                                          __NE___ { ASSERT( IsLocked() );  return _count.load(); }

        // owner api
            void  Lock ()                                           __NE___;
            void  Reset ()                                          __NE___;
        ND_ bool  IsReady ()                                        __NE___;
        ND_ bool  IsLocked ()                                       __NE___;

    protected:
            void  _GetCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount) __NE___;
    };


/*
=================================================
    constructor
=================================================
*/
    template <typename A, typename B>
    LfCmdBufferPool<A,B>::LfCmdBufferPool () __NE___
    {
        Reset();
    }

/*
=================================================
    Acquire
----
    thread safe
=================================================
*/
    template <typename A, typename B>
    uint  LfCmdBufferPool<A,B>::Acquire () __NE___
    {
        ASSERT( not IsReady() );

        uint    idx = _counter.fetch_add( 1 );
        if_likely( idx < _pool.size() )
            return idx;

        DEV_WARNING( "counter overflow" );
        return UMax;
    }

/*
=================================================
    Add
----
    thread safe for unique id
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, NativeCmdBuffer_t cmdbuf) __NE___
    {
        ASSERT( idx < _pool.size() );

        if_likely( idx < _pool.size() )
        {
            DRC_SHAREDLOCK( _drCheck );

            _pool[idx].native = cmdbuf;

            uint    old_bits = _ready.fetch_or( 1u << idx, EMemoryOrder::Release );
            ASSERT( (old_bits & (1u << idx)) == 0 );
            Unused( old_bits );
        }
        idx = UMax;
    }

/*
=================================================
    Add
----
    thread safe for unique id
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::Add (INOUT uint& idx, BakedCommands_t ctx) __NE___
    {
        ASSERT( idx < _pool.size() );

        if_likely( idx < _pool.size() )
        {
            DRC_SHAREDLOCK( _drCheck );

            PlacementNew<BakedCommands_t>( OUT &_pool[idx].baked, RVRef(ctx) );     // nothrow
            _cmdTypes.fetch_or( 1u << idx );

            uint    old_bits = _ready.fetch_or( 1u << idx, EMemoryOrder::Release );
            ASSERT( (old_bits & (1u << idx)) == 0 );
            Unused( old_bits );
        }
        idx = UMax;
    }

/*
=================================================
    Complete
----
    thread safe for unique id
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::Complete (INOUT uint& idx) __NE___
    {
        ASSERT( idx < _pool.size() );

        if_likely( idx < _pool.size() )
        {
            uint    old_bits = _ready.fetch_or( 1u << idx );
            ASSERT( (old_bits & (1u << idx)) == 0 );
            Unused( old_bits );
        }
        idx = UMax;
    }

/*
=================================================
    Lock
----
    prevent for reserving new slots for command buffers, call for 'Acquire()' will fail.
    call once per frame.
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::Lock () __NE___
    {
        DRC_SHAREDLOCK( _drCheck );

        // on first call '_count' must be 0, on second call may not be 0 - error
        //ASSERT( _count == 0 );

        uint    count = _counter.exchange( _CounterLargeValue );

        if ( count >= _CounterLargeValue )
        {
            ASSERT( _count.load() > 0 );
            return; // already locked
        }

        count = Min( count, uint(_pool.size()) );
        _count.store( count );

        // set unused bits to 1
        uint    mask = ~ToBitMask<uint>( count );
        if ( mask )
            _ready.fetch_or( mask );
    }

/*
=================================================
    IsLocked
=================================================
*/
    template <typename A, typename B>
    bool  LfCmdBufferPool<A,B>::IsLocked () __NE___
    {
        return _counter.load() >= _CounterLargeValue;
    }

/*
=================================================
    Reset
----
    unlock and reset.
    not thread safe !!!
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::Reset () __NE___
    {
        DRC_EXLOCK( _drCheck );

        #ifdef AE_DEBUG
        {
            uint    types = _cmdTypes.load();
            while ( types != 0 )
            {
                int i = ExtractBitLog2( INOUT types );
                ASSERT( not _pool[i].baked.IsValid() );
            }
        }
        #endif

        _ready.store( 0 );
        _cmdTypes.store( 0 );
        _counter.store( 0 );
        _count.store( 0 );

        ZeroMem( OUT _pool.data(), Sizeof(_pool) );

        MemoryBarrier( EMemoryOrder::Release );
    }

/*
=================================================
    IsReady
=================================================
*/
    template <typename A, typename B>
    bool  LfCmdBufferPool<A,B>::IsReady () __NE___
    {
        return _ready.load() == UMax;
    }

/*
=================================================
    _GetCommands
----
    not thread safe !!!
=================================================
*/
    template <typename A, typename B>
    void  LfCmdBufferPool<A,B>::_GetCommands (OUT NativeCmdBuffer_t* cmdbufs, OUT uint &cmdbufCount, uint maxCount) __NE___
    {
        ASSERT( cmdbufs != null );
        ASSERT( maxCount >= GraphicsConfig::MaxCmdBufPerBatch );
        Unused( maxCount );

        cmdbufCount = 0;

        DRC_EXLOCK( _drCheck );
        MemoryBarrier( EMemoryOrder::Acquire );

        ASSERT( _cmdTypes.load() == 0 );    // software command buffers is not supported here
        ASSERT( IsReady() );

        for (uint i = 0, cnt = _count.load(); i < cnt; ++i)
        {
            // command buffer can be null
            if_likely( _pool[i].native != NativeCmdBuffer_t{} )
            {
                ASSERT( cmdbufCount < maxCount );
                cmdbufs[cmdbufCount++] = _pool[i].native;
            }
        }

        DEBUG_ONLY( Reset() );
    }


} // AE::Graphics

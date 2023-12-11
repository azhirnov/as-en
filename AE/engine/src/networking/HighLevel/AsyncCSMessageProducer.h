// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe:  yes
        - used split lock: CreateMsg() - lock_shared, AddMessage() - unlock_shared.
        - Produce() use exclusive lock.
*/

#pragma once

#include "networking/HighLevel/MessageFactory.h"

namespace AE::Networking
{

    //
    // Async Client/Server Message Producer
    //

    template <typename AllocatorType>
    class alignas(AE_CACHE_LINE) AsyncCSMessageProducer : public ICSMessageProducer
    {
        StaticAssert( IsThreadSafeAllocator< AllocatorType >);

    // types
    private:
        using _SpinLock_t       = Threading::TRWSpinLock< false, true >;
        using Allocator_t       = AllocatorType;
        using DoubleBufAlloc_t  = StaticArray< RC<Allocator_t>, 2 >;
        using LfMessageList_t   = Threading::LfChunkList< CSMessagePtr, NetConfig::MsgPerChunk >;

    public:
        // Message and chunk with message must use the same allocator,
        // so we need to use shared_lock to allow allocations from multiple thread,
        // but only one thread can reset allocator (see 'Produce()' method).

        template <typename T>
        struct MsgAndSync : MovableOnly
        {
            friend class AsyncCSMessageProducer;

        // variables
        private:
            T *             _msg    = null;
            _SpinLock_t *   _guard  = null;
            DEBUG_ONLY(
                Bytes       _extraSize;
            )

        // methods
        public:
            MsgAndSync ()                                               __NE___ {}
            MsgAndSync (T* msg, _SpinLock_t &guard, Bytes extraSize)    __NE___ : _msg{msg}, _guard{&guard} DEBUG_ONLY(, _extraSize{extraSize}) {}
            MsgAndSync (MsgAndSync &&other)                             __NE___ : _msg{other._msg}, _guard{other._guard} { other._guard = null; }
            ~MsgAndSync ()                                              __NE___ { if_unlikely( _guard != null ) _guard->unlock_shared(); }

            MsgAndSync&  operator = (MsgAndSync &&rhs)                  __NE___;

            ND_ T*  operator -> ()                                      __NE___ { ASSERT( _msg != null );  return _msg; }

            ND_ explicit operator bool ()                               C_NE___ { return _msg != null; }
            ND_ explicit operator CSMessagePtr ()                       C_NE___ { return CSMessagePtr{ _msg }; }


            template <typename E>
            ND_ E*  Extra ()                                            __NE___;

            template <typename E>
            E*      PutExtra (const E* data, usize count)               __NE___;
            void*   PutExtra (const void* data, Bytes dataSize)         __NE___;

            template <typename E>
            E*      PutExtra (BasicStringView<E> str)                   __NE___ { return PutExtra( str.data(), str.size() ); }

            template <typename E>
            E*      PutExtra (ArrayView<E> arr)                         __NE___ { return PutExtra( arr.data(), arr.size() ); }


            template <typename E>
            void    PutInPlace (E T::*, const void* src, Bytes size)    __NE___;

        //  template <typename E>
        //  void    PutInPlace (E (T::*m)[], BasicStringView<E> str)    __NE___ { PutInPlace( m, str.data(), StringSizeOf(str) ); }

        //  template <typename E>
        //  void    PutInPlace (E (T::*m)[], ArrayView<E> arr)          __NE___ { PutInPlace( m, arr.data(), ArraySizeOf(arr) ); }
        };


    // variables
    private:
        _SpinLock_t         _guard;
        Atomic<uint>        _index      {0};
        LfMessageList_t     _outputMsg;
        DoubleBufAlloc_t    _dbAlloc;


    // methods
    public:
        AsyncCSMessageProducer ()                                       __NE___;
        ~AsyncCSMessageProducer ()                                      __NE___ { Unused( _outputMsg.Release() ); }

        template <typename T>
        ND_ MsgAndSync<T>   CreateMsg (Bytes extraSize = 0_b)           __NE___;

        template <typename T>
        ND_ MsgAndSync<T>   CreateMsgOpt (Bytes extraSize = 0_b)        __NE___;

        template <typename T>
        ND_ bool            AddMessage (MsgAndSync<T> &)                __NE___;

        // safe to use between 'CreateMsg()' and 'AddMessage()'
        ND_ Allocator_t&    GetAllocator ()                             __NE___ { return *_dbAlloc[ _index.load() & 1 ]; }

    private:
        // ICSMessageProducer //
        ChunkList<CSMessagePtr>  Produce (FrameUID frameId)             __NE_OF;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    operator =
=================================================
*/
    template <typename A>
    template <typename T>
    typename AsyncCSMessageProducer<A>::template MsgAndSync<T>&
        AsyncCSMessageProducer<A>::MsgAndSync<T>::operator = (MsgAndSync &&rhs) __NE___
    {
        ASSERT( _guard == null );

        _msg    = rhs._msg;
        _guard  = rhs._guard;

        DEBUG_ONLY( _extraSize = rhs._extraSize;)

        rhs._guard  = null;
        return *this;
    }

/*
=================================================
    Extra
=================================================
*/
    template <typename A>
    template <typename T>
    template <typename E>
    E*  AsyncCSMessageProducer<A>::MsgAndSync<T>::Extra () __NE___
    {
        ASSERT( _msg != null );

        if constexpr( alignof(E) <= alignof(T) )
            return Cast<E>( _msg + SizeOf<T> );
        else
            return Cast<E>( AlignUp( _msg, AlignOf<E> ) + SizeOf<T> );
    }

/*
=================================================
    PutExtra
=================================================
*/
    template <typename A>
    template <typename T>
    template <typename E>
    E*  AsyncCSMessageProducer<A>::MsgAndSync<T>::PutExtra (const E* data, const usize count) __NE___
    {
        ASSERT( _msg != null );

        const Bytes  data_size = SizeOf<E> * count;
        ASSERT( data_size <= _extraSize );

        E*  dst = Extra<E>();
        ASSERT_MSG( dst + data_size <= (Cast<void>(_msg) + SizeOf<T> + _extraSize),
                    "'extraSize' must include alignment to dst type" );

        MemCopy( OUT dst, data, data_size );
        return dst;
    }

    template <typename A>
    template <typename T>
    void*  AsyncCSMessageProducer<A>::MsgAndSync<T>::PutExtra (const void* data, const Bytes dataSize) __NE___
    {
        ASSERT( _msg != null );
        ASSERT( dataSize <= _extraSize );

        void*   dst = Extra<char>();
        MemCopy( OUT dst, data, dataSize );

        return dst;
    }

/*
=================================================
    PutInPlace
=================================================
*/
    template <typename A>
    template <typename T>
    template <typename E>
    void  AsyncCSMessageProducer<A>::MsgAndSync<T>::PutInPlace (E T::*member, const void* srcData, const Bytes srcDataSize) __NE___
    {
        ASSERT( _msg != null );

        void*   dst = &(_msg->*member);
        ASSERT( dst + srcDataSize <= (Cast<void>(_msg) + SizeOf<T> + _extraSize) );

        MemCopy( OUT dst, srcData, srcDataSize );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    template <typename A>
    AsyncCSMessageProducer<A>::AsyncCSMessageProducer () __NE___ :
        _dbAlloc{ MakeRC<Allocator_t>(), MakeRC<Allocator_t>() }
    {
        CHECK( _outputMsg.Init( GetAllocator() ));
    }

/*
=================================================
    CreateMsg
=================================================
*/
    template <typename A>
    template <typename T>
    typename AsyncCSMessageProducer<A>::template MsgAndSync<T>  AsyncCSMessageProducer<A>::CreateMsg (Bytes extraSize) __NE___
    {
        _guard.lock_shared();

        CSMessagePtr    msg;
        Unused( CSMessageCtor<T>::CreateForEncode( OUT msg, GetAllocator(), extraSize ));
        return MsgAndSync<T>{ Cast<T>(msg.get()), _guard, extraSize };
    }

    template <typename A>
    template <typename T>
    typename AsyncCSMessageProducer<A>::template MsgAndSync<T>  AsyncCSMessageProducer<A>::CreateMsgOpt (Bytes extraSize) __NE___
    {
        if_unlikely( not _guard.try_lock_shared( 200 ))
            return Default;

        CSMessagePtr    msg;
        Unused( CSMessageCtor<T>::CreateForEncode( OUT msg, GetAllocator(), extraSize ));
        return MsgAndSync<T>{ Cast<T>(msg.get()), _guard, extraSize };
    }

/*
=================================================
    AddMessage
=================================================
*/
    template <typename A>
    template <typename T>
    bool  AsyncCSMessageProducer<A>::AddMessage (MsgAndSync<T> &msg) __NE___
    {
        ASSERT( msg );
        bool    res = _outputMsg.Emplace( GetAllocator(), CSMessagePtr{msg} );

        _guard.unlock_shared();

        msg._msg    = null;
        msg._guard  = null;

        return res;
    }

/*
=================================================
    Produce
=================================================
*/
    template <typename A>
    ChunkList<CSMessagePtr>  AsyncCSMessageProducer<A>::Produce (const FrameUID frameId) __NE___
    {
        const uint                  new_fid = frameId.Remap2();
        ChunkList<CSMessagePtr>     result;
        {
            EXLOCK( _guard );

            const uint  bits        = _index.load();
            const uint  prev_id     = bits & 1;
            const uint  prev_fid    = (bits >> 2) & 1;

            if ( new_fid != prev_fid )
            {
                // New frameId indicates that all previous messages are sent and memory can be reused.
                const uint  id = (new_fid == prev_fid ? prev_id : ((prev_id+1) & 1));

                _index.store( id | (new_fid << 2) );
                _dbAlloc[id]->Discard();
            }

            result = _outputMsg.Release();
            CHECK( _outputMsg.Init( GetAllocator() ));
        }
        return result;
    }


} // AE::Networking

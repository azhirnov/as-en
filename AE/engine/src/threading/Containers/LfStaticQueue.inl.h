// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{
/*
=================================================
    Init
----
    must be externally synchronized
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::Init (usize size) __NE___
    {
        DRC_EXLOCK( _drCheck );
        CHECK_ERR( size > 0 and size < _MaxSize );
        CHECK_ERR( _arr == null );

        _count  = uint(size);
        _arr    = _allocator.Allocate( SizeAndAlignOf<Value_t> * _count );
        CHECK_ERR( _arr != null );

        MemoryBarrier( EMemoryOrder::Release );
        return true;
    }

/*
=================================================
    Release
----
    must be externally synchronized
=================================================
*/
    template <typename V, typename A>
    void  LfStaticQueue<V,A>::Release () __NE___
    {
        DRC_EXLOCK( _drCheck );

        MemoryBarrier( EMemoryOrder::Acquire );

        if ( _arr == null )
            return;

        ASSERT( Empty() );
        ASSERT( not IsLockedForRead() );

        _allocator.Deallocate( _arr, SizeAndAlign{ SizeOf<Value_t> * _count, AlignOf<Value_t> });
        _arr    = null;
        _count  = 0;
        _packed.store( Bits{} );
    }

/*
=================================================
    Clear
=================================================
*/
    template <typename V, typename A>
    void  LfStaticQueue<V,A>::Clear () __NE___
    {
        DRC_EXLOCK( _drCheck );
        ASSERT( _arr != null );

        const Bits  pack = _packed.exchange( Bits{} );
        ASSERT( not pack.locked );

        for (uint i = pack.first; i < pack.last; ++i) {
            PlacementDelete( Cast<Value_t>(_arr)[i] );
        }
    }

/*
=================================================
    Push
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::Push (const Value_t &value) __NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _arr != null );

        Bits    pack = _packed.load();
        for (uint i = 0;; ++i)
        {
            if_unlikely( pack.locked | (i > _MaxSpin) )
                return false;

            Bits    next = pack;
            next.last++;
            if ( _packed.CAS( INOUT pack, next ))
                break;

            ThreadUtils::Pause();
        }

        if_likely( pack.last < _count )
        {
            PlacementNew<Value_t>( OUT (_arr + SizeOf<Value_t> * pack.last), value );
            MemoryBarrier( EMemoryOrder::Release );
            return true;
        }
        return false;
    }

/*
=================================================
    Push
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::Push (Value_t &&value) __NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _arr != null );

        Bits    pack = _packed.load();
        for (uint i = 0;; ++i)
        {
            if_unlikely( pack.locked | (i > _MaxSpin) )
                return false;

            Bits    next = pack;
            next.last++;
            if ( _packed.CAS( INOUT pack, next ))
                break;

            ThreadUtils::Pause();
        }

        if_likely( pack.last < _count )
        {
            PlacementNew<Value_t>( OUT (_arr + SizeOf<Value_t> * pack.last), RVRef(value) );
            MemoryBarrier( EMemoryOrder::Release );
            return true;
        }
        return false;
    }

/*
=================================================
    Empty
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::Empty () __NE___
    {
        auto    pack = _packed.load();
        return pack.Empty();
    }

/*
=================================================
    Count
=================================================
*/
    template <typename V, typename A>
    uint  LfStaticQueue<V,A>::Count () __NE___
    {
        auto    pack = _packed.load();
        return pack.Count();
    }

/*
=================================================
    IsLockedForRead
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::IsLockedForRead () __NE___
    {
        auto    pack = _packed.load();
        return pack.locked;
    }

/*
=================================================
    First
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::First (OUT Value_t &value) __NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _arr != null );

        Bits    pack = _packed.load();
        if_likely( (pack.first < pack.last) & not pack.locked );
        {
            MemoryBarrier( EMemoryOrder::Acquire );
            value = Cast<Value_t>(_arr) [pack.first];
            return true;
        }
        return false;
    }

/*
=================================================
    Pop
=================================================
*/
    template <typename V, typename A>
    bool  LfStaticQueue<V,A>::Pop (OUT Value_t &value) __NE___
    {
        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _arr != null );

        Bits    pack = _packed.load();
        for (uint i = 0;; ++i)
        {
            if_unlikely( pack.locked | (i > _MaxSpin) )
                return false;

            Bits    next = pack;
            next.first++;
            if ( _packed.CAS( INOUT pack, next ))
                break;

            ThreadUtils::Pause();
        }

        if_likely( pack.first < pack.last )
        {
            MemoryBarrier( EMemoryOrder::Acquire );
            value = RVRef( Cast<Value_t>(_arr) [pack.first] );
            PlacementDelete( Cast<Value_t>(_arr) [pack.first] );
            return true;
        }
        return false;
    }

/*
=================================================
    ExtractAll
=================================================
*/
    template <typename V, typename A>
    template <typename ArrayType>
    bool  LfStaticQueue<V,A>::ExtractAll (OUT ArrayType &dstArr) __NE___
    {
        // lock
        Bits    pack = _packed.load();
        for (uint i = 0;; ++i)
        {
            if_unlikely( pack.locked | (i > _MaxSpin) )
                return false;

            Bits    next = pack;
            next.locked = false;

            if_likely( _packed.CAS( INOUT pack, next ))
                break;

            ThreadUtils::Pause();
        }

        ASSERT( pack.Count() <= dstArr.capacity() );
        ASSERT( dstArr.empty() );

        DRC_SHAREDLOCK( _drCheck );
        ASSERT( _arr != null );

        MemoryBarrier( EMemoryOrder::Acquire );

        auto*   src = Cast<Value_t>(_arr);
        for (uint i = pack.first; i < pack.last; ++i)
        {
            dstArr.push_back( RVRef( src[i] ));

            if constexpr( _NonTrivialDtor )
                PlacementDelete( src[i] );
        }

        // unlock and clear
        _packed.store( Bits{}, EMemoryOrder::Release );
        return true;
    }


} // AE::Threading

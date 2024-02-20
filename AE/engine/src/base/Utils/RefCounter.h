// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/Ptr.h"
#include "base/Algorithms/Cast.h"
#include "base/Memory/MemUtils.h"
#include "base/Platforms/ThreadUtils.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/Atomic.h"

namespace AE::Base
{
    template <typename T>
    struct RC;

    struct RefCounterUtils;


    //
    // Enable Reference Counting
    //
    class EnableRCBase : public Noncopyable, public NothrowAllocatable
    {
        friend struct RefCounterUtils;

    // variables
    private:
        Atomic<int>     _counter {0};


    // methods
    public:
        EnableRCBase ()                 __NE___ {}
        virtual ~EnableRCBase ()        __NE___ { ASSERT( _counter.load() == 0 ); }

    protected:

        // This methods allows to catch object destruction and change behavior,
        // for example - add back to object pool.
        //
        virtual void  _ReleaseObject () __NE___
        {
            // update cache before calling destructor
            MemoryBarrier( EMemoryOrder::Acquire );
            delete this;

            // TODO: flush cache depends on allocator - default allocator flush cache because of internal sync, lock-free allocator may not flush cache
            //MemoryBarrier( EMemoryOrder::Release );
        }
    };



    //
    // Ref Counter Utils
    //
    struct RefCounterUtils final : Noninstanceable
    {
        // returns previous value of ref counter
            forceinline static int   IncRef (EnableRCBase &obj)         __NE___ { return obj._counter.fetch_add( 1 ); }

        // returns previous value of ref counter
            forceinline static int   AddRef (EnableRCBase &obj, int cnt)__NE___ { return obj._counter.fetch_add( cnt ); }

        // returns '1' if object must be destroyed
            forceinline static int   DecRef (EnableRCBase &obj)         __NE___ { return obj._counter.fetch_sub( 1 ); }

        // returns '1' if object have been destroyed.
        // 'ptr' can be null
        template <typename T>
            forceinline static int   DecRefAndRelease (INOUT T* &ptr)   __NE___;

        ND_ forceinline static int   UseCount (EnableRCBase &obj)       __NE___ { return obj._counter.load(); }
    };



    //
    // Reference Counter Pointer
    //

    template <typename T = EnableRCBase>
    struct RC
    {
    // types
    public:
        using Value_t   = T;
        using Self      = RC<T>;


    // variables
    private:
        T *     _ptr = null;


    // methods
    public:
        RC ()                                           __NE___ {}
        RC (std::nullptr_t)                             __NE___ {}

        enum class DontIncRef {};
        explicit RC (T* ptr, DontIncRef)                __NE___ : _ptr{ptr}             {}

        RC (T* ptr)                                     __NE___ : _ptr{ptr}             { _IncSelf(); }
        RC (Ptr<T> ptr)                                 __NE___ : _ptr{ptr}             { _IncSelf(); }
        RC (Self &&other)                               __NE___ : _ptr{other.release()} {}
        RC (const Self &other)                          __NE___ : _ptr{other._ptr}      { _IncSelf(); }

        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< T, B >)>
        RC (RC<B> &&other)                              __NE___ : _ptr{other.release()} {}

        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< T, B >)>
        RC (const RC<B> &other)                         __NE___ : _ptr{other.get()}     { _IncSelf(); }


        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< B, T >)>
        explicit RC (RC<B> &&other)                     __NE___ : _ptr{static_cast<T*>(other.release())}    {}

        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< B, T >)>
        explicit RC (const RC<B> &other)                __NE___ : _ptr{static_cast<T*>(other.get())}        { _IncSelf(); }


        ~RC ()                                          __NE___ { _Dec(); }

        Self&  operator = (std::nullptr_t)              __NE___ {                       _Dec();  _ptr = null;           return *this; }
        Self&  operator = (T* rhs)                      __NE___ { _Inc( rhs );          _Dec();  _ptr = rhs;            return *this; }
        Self&  operator = (Ptr<T> rhs)                  __NE___ { _Inc( rhs.get() );    _Dec();  _ptr = rhs.get();      return *this; }
        Self&  operator = (const Self &rhs)             __NE___ { _Inc( rhs._ptr );     _Dec();  _ptr = rhs._ptr;       return *this; }
        Self&  operator = (Self &&rhs)                  __NE___ {                       _Dec();  _ptr = rhs.release();  return *this; }

        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< T, B >)>
        Self&  operator = (RC<B> &&rhs)                 __NE___ { _Dec();  _ptr = static_cast<T*>(rhs.release());        return *this; }

        template <typename B,
                  ENABLEIF( IsBaseOfNotSame< T, B >)>
        Self&  operator = (const RC<B> &rhs)            __NE___ { _Inc( static_cast<T*>(rhs.get()) );  _Dec();  _ptr = static_cast<T*>(rhs.get());  return *this; }

        ND_ bool  operator == (const T* rhs)            C_NE___ { return _ptr == rhs; }
        ND_ bool  operator == (Ptr<T> rhs)              C_NE___ { return _ptr == rhs.get(); }
        ND_ bool  operator == (const Self &rhs)         C_NE___ { return _ptr == rhs._ptr; }
        ND_ bool  operator == (std::nullptr_t)          C_NE___ { return _ptr == null; }

        template <typename B>
        ND_ bool  operator != (const B& rhs)            C_NE___ { return not (*this == rhs); }

        ND_ bool  operator <  (const Self &rhs)         C_NE___ { return _ptr <  rhs._ptr; }
        ND_ bool  operator >  (const Self &rhs)         C_NE___ { return _ptr >  rhs._ptr; }
        ND_ bool  operator <= (const Self &rhs)         C_NE___ { return _ptr <= rhs._ptr; }
        ND_ bool  operator >= (const Self &rhs)         C_NE___ { return _ptr >= rhs._ptr; }

        ND_ T *     operator -> ()                      C_NE___ { ASSERT( _ptr != null );  return _ptr; }
        ND_ T &     operator *  ()                      C_NE___ { ASSERT( _ptr != null );  return *_ptr; }

        ND_ T *     get ()                              C_NE___ { return _ptr; }
        ND_ T *     release ()                          __NE___ { T* p = _ptr;  _ptr = null;  return p; }   // TODO: detach?
        ND_ int     use_count ()                        C_NE___ { return _ptr != null ? RefCounterUtils::UseCount( *_ptr ) : 0; }

        ND_ explicit operator bool ()                   C_NE___ { return _ptr != null; }

            void    attach (T* ptr)                     __NE___ {               _Dec();  _ptr = ptr; }
            void    reset (T* ptr)                      __NE___ { _Inc( ptr );  _Dec();  _ptr = ptr; }

            void    Swap (INOUT Self &rhs)              __NE___;

    private:
        static  void    _Inc (T* ptr)                   __NE___;
                void    _IncSelf ()                     __NE___;
                void    _Dec ()                         __NE___;
    };



    //
    // Enable Reference Counting
    //

    template <typename T>
    class EnableRC : public EnableRCBase
    {
    // methods
    public:
        ND_ RC<T>  GetRC ()     __NE___ { return RC<T>{ static_cast<T*>(this) }; }

        template <typename B>
        ND_ RC<B>  GetRC ()     __NE___ { StaticAssert( IsBaseOf< T, B >);  return RC<B>{ static_cast<B*>(this) }; }
    };



    //
    // Static Reference Counter
    //

    template <typename T>
    class StaticRC
    {
    // variables
    private:
        T   _value;


    // methods
    public:
        template <typename ...Args>
        explicit StaticRC (Args&& ...args)  __NE___ :
            _value{ FwdArg<Args>( args )... }
        {
            StaticAssert( IsBaseOf< EnableRCBase, T > );
            CheckNothrow( IsNothrowCtor< T, Args... >);

            const int   cnt = RefCounterUtils::IncRef( _value );
            Unused( cnt );
            ASSERT( cnt == 0 );
        }

        ~StaticRC ()                        __NE___
        {
            const int   cnt = RefCounterUtils::DecRef( _value );
            Unused( cnt );
            ASSERT( cnt == 1 );
        }

        ND_ T *         operator -> ()      __NE___ { return &_value; }
        ND_ T const*    operator -> ()      C_NE___ { return &_value; }

        ND_ T &         operator * ()       __NE___ { return _value; }
        ND_ T const&    operator * ()       C_NE___ { return _value; }

        ND_ RC<T>       GetRC ()            __NE___ { return _value.template GetRC<T>(); }
    };



    //
    // Static Reference Counter with dependent RC
    //

    template <typename T>
    class StaticRC2 : public StaticRC<T>
    {
    // variables
    private:
        EnableRCBase &      _base;


    // methods
    public:
        template <typename ...Args>
        explicit StaticRC2 (EnableRCBase &base, Args&& ...args) __NE___ :
            StaticRC<T>{ FwdArg<Args>( args )... },
            _base{ base }
        {
            RefCounterUtils::IncRef( _base );
        }

        ~StaticRC2 () __NE___
        {
            const int   cnt = RefCounterUtils::DecRef( _base );
            Unused( cnt );
            ASSERT( cnt >= 1 );
        }
    };



    //
    // Reference Counter Atomic Pointer
    //

    template <typename T>
    struct AtomicRC
    {
    // types
    public:
        using Value_t   = T;
        using RC_t      = RC<T>;
        using Self      = AtomicRC<T>;


    // variables
    private:
        Atomic< T *>    _ptr {null};


    // methods
    public:
        AtomicRC ()                                                 __NE___ {}
        ~AtomicRC ()                                                __NE___ { _ResetDec();  StaticAssert( alignof(T) > 1, "first bit is used for lock bit" ); }

        ND_ T *     unsafe_get ()                                   C_NE___ { return _RemoveLockBit( _ptr.load() ); }
        ND_ RC_t    release ()                                      __NE___;

        ND_ RC_t    load ()                                         __NE___;

            void    store (T* ptr)                                  __NE___ { _Inc( ptr );  _Dec( _Exchange( ptr )); }
            void    store (RC_t ptr)                                __NE___ { _Dec( _Exchange( ptr.release() )); }

            void    reset ()                                        __NE___ { _ResetDec(); }

        ND_ RC_t    exchange (T* desired)                           __NE___;
        ND_ RC_t    exchange (RC_t desired)                         __NE___;

        ND_ bool    CAS (INOUT RC_t& expected, RC_t desired)        __NE___ { return _CAS<false>( INOUT expected, RVRef(desired) ); }
        ND_ bool    CAS_Loop (INOUT RC_t& expected, RC_t desired)   __NE___ { return _CAS<true>( INOUT expected, RVRef(desired) ); }


    private:
            void    _IncSet (T* ptr)                                __NE___;
            void    _ResetDec ()                                    __NE___ { _Dec( _Exchange( null )); }

        ND_ T*      _Lock ()                                        __NE___;
            void    _Unlock ()                                      __NE___;
        ND_ T*      _Exchange (T* ptr)                              __NE___;

        template <bool IsStrong>
        ND_ bool    _CAS (INOUT RC_t& expected, RC_t desired)       __NE___;

            static void _Inc (T* ptr)                               __NE___;
            static void _Dec (T* ptr)                               __NE___;

        ND_ static bool _HasLockBit (T* ptr)                        __NE___ { return (usize(ptr) & usize{1}); }
        ND_ static T*   _SetLockBit (T* ptr)                        __NE___ { return reinterpret_cast< T *>((usize(ptr) | usize{1})); }
        ND_ static T*   _RemoveLockBit (T* ptr)                     __NE___ { return reinterpret_cast< T *>((usize(ptr) & ~usize{1})); }
    };


    namespace _hidden_
    {
        template <typename T>
        struct _RemoveRC {
            using type = T;
        };

        template <typename T>
        struct _RemoveRC< RC<T> > {
            using type = T;
        };

        template <typename T>
        struct _RemoveRC< AtomicRC<T> > {
            using type = T;
        };

    } // _hidden_

    template <typename T>
    using RemoveRC  = typename Base::_hidden_::_RemoveRC<T>::type;

    template <typename T>
    static constexpr bool   IsRC = IsSpecializationOf< T, RC >;

//-----------------------------------------------------------------------------



/*
=================================================
    MakeRC
=================================================
*/
    template <typename T, typename ...Args>
    ND_ RC<T>  MakeRC (Args&& ...args) __NE___
    {
        StaticAssert( not IsBaseOf< NonAllocatable, T >);
        StaticAssert( IsBaseOf< EnableRCBase, T >);

        CheckNothrow( IsNothrowCtor< T, Args... >);
        //CheckNothrow( IsNoExcept( new T{ FwdArg<Args>(args)... }));

        return RC<T>{ new T{ FwdArg<Args>(args)... }};
    }

    template <typename T, typename ...Args>
    ND_ RC<T>  MakeRCTh (Args&& ...args) __Th___
    {
        StaticAssert( not IsBaseOf< NonAllocatable, T >);
        StaticAssert( IsBaseOf< EnableRCBase, T >);

        return RC<T>{ new T{ FwdArg<Args>(args)... }};
    }

    template <typename T, typename ...Args>
    ND_ RC<T>  MakeRCNe (Args&& ...args) __NE___
    {
        StaticAssert( not IsBaseOf< NonAllocatable, T >);
        StaticAssert( IsBaseOf< EnableRCBase, T >);

        if constexpr( IsNoExcept( new T{ FwdArg<Args>(args)... }))
        {
            return RC<T>{ new T{ FwdArg<Args>(args)... }};
        }else{
            TRY{
                return RC<T>{ new T{ FwdArg<Args>(args)... }};
            }
            CATCH_ALL( return null; )
        }
    }

/*
=================================================
    DecRefAndRelease
=================================================
*/
    template <typename T>
    int  RefCounterUtils::DecRefAndRelease (INOUT T* &ptr) __NE___
    {
        StaticAssert( sizeof(T) > 0 );

        if_likely( ptr != null )
        {
            const auto  res = DecRef( *ptr );
            ASSERT( res > 0 );

            if_unlikely( res == 1 )
            {
                static_cast< EnableRCBase *>( ptr )->_ReleaseObject();
                ptr = null;
            }
            return res;
        }
        return 0;
    }

/*
=================================================
    _Inc / _Dec
=================================================
*/
    template <typename T>
    void  RC<T>::_Inc (T* ptr) __NE___
    {
        StaticAssert( IsBaseOf< EnableRCBase, T >);

        if_likely( ptr != null )
            RefCounterUtils::IncRef( *ptr );
    }

    template <typename T>
    void  RC<T>::_IncSelf () __NE___
    {
        StaticAssert( IsBaseOf< EnableRCBase, T >);

        if_likely( _ptr != null )
            RefCounterUtils::IncRef( *_ptr );
    }

    template <typename T>
    void  RC<T>::_Dec () __NE___
    {
        StaticAssert( IsBaseOf< EnableRCBase, T >);
        RefCounterUtils::DecRefAndRelease( INOUT _ptr );
    }

/*
=================================================
    Swap
=================================================
*/
    template <typename T>
    void  RC<T>::Swap (INOUT RC<T> &rhs) __NE___
    {
        std::swap( _ptr, rhs._ptr );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _Inc
=================================================
*/
    template <typename T>
    void  AtomicRC<T>::_Inc (T* ptr) __NE___
    {
        ASSERT( not _HasLockBit( ptr ));

        if_likely( ptr != null )
            RefCounterUtils::IncRef( *ptr );
    }

/*
=================================================
    _Dec
=================================================
*/
    template <typename T>
    void  AtomicRC<T>::_Dec (T* ptr) __NE___
    {
        ASSERT( not _HasLockBit( ptr ));

        RefCounterUtils::DecRefAndRelease( INOUT ptr );
    }

/*
=================================================
    _IncSet
=================================================
*/
    template <typename T>
    void  AtomicRC<T>::_IncSet (T* ptr) __NE___
    {
        _Inc( ptr );

        T*  old = _Exchange( ptr );

        // pointer may be changed in another thread
        _Dec( old );
    }

/*
=================================================
    release
=================================================
*/
    template <typename T>
    RC<T>  AtomicRC<T>::release () __NE___
    {
        T*  old = _Exchange( null );
        ASSERT( not _HasLockBit( old ));

        RC_t    ptr;
        ptr.attach( old );

        return ptr;
    }

/*
=================================================
    load
=================================================
*/
    template <typename T>
    RC<T>  AtomicRC<T>::load () __NE___
    {
        RC_t    res{ _Lock() };
        _Unlock();
        return res;
    }

/*
=================================================
    exchange
=================================================
*/
    template <typename T>
    RC<T>  AtomicRC<T>::exchange (T* desired) __NE___
    {
        _Inc( desired );

        T*  old = _Exchange( desired );

        return RC_t{ old, RC_t::DontIncRef(0) };
    }

    template <typename T>
    RC<T>  AtomicRC<T>::exchange (RC_t desired) __NE___
    {
        T*  old = _Exchange( desired.release() );

        return RC_t{ old, RC_t::DontIncRef(0) };
    }

/*
=================================================
    CAS
=================================================
*/
    template <typename T>
    template <bool IsStrong>
    ND_ bool  AtomicRC<T>::_CAS (INOUT RC_t& expected, RC_t desired) __NE___
    {
        T*  exp = expected.get();
        T*  des = desired.get();

        ASSERT( not _HasLockBit( exp ));
        ASSERT( not _HasLockBit( des ));

        bool    res;

        if constexpr( IsStrong )
            res = _ptr.CAS_Loop( INOUT exp, des );
        else
            res = _ptr.CAS( INOUT exp, des );

        if ( res ) {
            RefCounterUtils::DecRefAndRelease( exp );
            Unused( desired.release() );    // 'desired' copied to '_ptr' so don't decrease ref counter
        }else
            expected = exp;

        return res;
    }

/*
=================================================
    _Exchange
=================================================
*/
    template <typename T>
    T*  AtomicRC<T>::_Exchange (T* desired) __NE___
    {
        T*  exp = _ptr.load();
        for (;;)
        {
            for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
            {
                exp = _RemoveLockBit( exp );

                // wait until it unlocks, then set new value
                if_likely( _ptr.CAS( INOUT exp, desired ))
                {
                    ASSERT( not _HasLockBit( exp ));
                    return exp;
                }

                ThreadUtils::Pause();
            }

            ThreadUtils::Sleep_1us();
        }
    }

/*
=================================================
    _Lock
=================================================
*/
    template <typename T>
    T*  AtomicRC<T>::_Lock () __NE___
    {
        T*  exp = _ptr.load();
        for (;;)
        {
            // spin until we can set the lock bit
            for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
            {
                exp = _RemoveLockBit( exp );

                if_likely( _ptr.CAS( INOUT exp, _SetLockBit( exp )) )
                {
                    ASSERT( not _HasLockBit( exp ));
                    return exp;
                }

                ThreadUtils::Pause();
            }

            ThreadUtils::Sleep_1us();
        }
    }

/*
=================================================
    _Unlock
=================================================
*/
    template <typename T>
    void  AtomicRC<T>::_Unlock () __NE___
    {
        T*  exp     = _RemoveLockBit( _ptr.load() );
        T*  prev    = _ptr.exchange( exp );
        Unused( prev );
        ASSERT( prev == _SetLockBit( exp ));
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Cast
=================================================
*/
    template <typename R, typename T>
    ND_ constexpr RC<R>  Cast (const RC<T> &value) __NE___
    {
        StaticAssert( sizeof(R) > 0 );
        return RC<R>{ static_cast<R*>( value.get() )};
    }

/*
=================================================
    DynCast
=================================================
*/
#ifdef AE_ENABLE_RTTI
    template <typename R, typename T>
    ND_ constexpr RC<R>  DynCast (const RC<T> &value) __NE___
    {
        return RC<R>{ dynamic_cast<R*>( value.get() )};
    }
#endif

} // AE::Base


template <typename T>
struct std::hash< AE::Base::RC<T> >
{
    ND_ size_t  operator () (const AE::Base::RC<T> &x) C_NE___ {
        return std::hash< T* >{}( x.get() );
    }
};

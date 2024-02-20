// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/AllocatorHelper.h"
#include "base/Math/Byte.h"
#include "base/Math/POTValue.h"
#include "base/Utils/RefCounter.h"

namespace AE::Base
{

    //
    // Allocator interface
    //
    class IAllocator : public EnableRC<IAllocator>
    {
    public:
        template <typename T>   using StdAlloc_t = StdAllocatorRef< T, RC<IAllocator> >;

    public:
        template <typename T>
        ND_ T*              Allocate (usize count = 1)                              __NE___ { return Cast<T>( Allocate( SizeAndAlignOf<T> * count )); }

        ND_ virtual void*   Allocate (Bytes size)                                   __NE___ { return Allocate( SizeAndAlign{ size, DefaultAllocatorAlign }); }
            virtual void    Deallocate (void* ptr, Bytes size)                      __NE___ { return Deallocate( ptr, SizeAndAlign{ size, DefaultAllocatorAlign }); }
            virtual void    Deallocate (void* ptr)                                  __NE___ = 0;

        ND_ virtual void*   Allocate (const SizeAndAlign sizeAndAlign)              __NE___ = 0;
            virtual void    Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___ = 0;

            virtual void    Reserve (const SizeAndAlign sizeAndAlign)               __NE___ { Unused( sizeAndAlign ); }
            virtual void    Reserve (Bytes size)                                    __NE___ { Reserve( SizeAndAlign{ size, DefaultAllocatorAlign }); }

            virtual void    Discard ()                                              __NE___ { DBG_WARNING( "not supported" ); }

        //ND_ virtual void* Allocate (POTBytes size)                                __NE___ { return Allocate( Bytes{size} ); }
        //ND_ virtual void* Allocate (POTBytes size, POTBytes align)                __NE___ { return Allocate(SizeAndAlign{ size, align }); }

        // TODO:
        // virtual ECapabilities    GetCapabilities () = 0;
        // virtual Bytes            GetMemoryUsage () = 0;
    };



    //
    // Thread-safe Allocator interface
    //
    class IAllocatorTS : public IAllocator
    {
        //  Thread-safe:  yes
        // void*  Allocate (...)
        // void   Deallocate (...)
        // void   Reserve (...)

        //  Thread-safe:  optional
        // void  Discard ()
    };



    //
    // Is Thread-safe Allocator
    //
    namespace _hidden_
    {
        template <typename T, bool IsInterface>
        struct _IsThreadSafeAllocator {
            static constexpr bool   value = IsBaseOfNoQual< IAllocatorTS, T >;
        };

        template <typename T>
        struct _IsThreadSafeAllocator< T, false > {
            static constexpr bool   value = T::IsThreadSafe;
        };

        template <typename T>
        struct _IsThreadSafeAllocator2 {
            static constexpr bool   value = _IsThreadSafeAllocator< T, (IsBaseOf< IAllocator, T >) >::value;
        };
    }

    template <typename T>
    static constexpr bool   IsThreadSafeAllocator = Base::_hidden_::_IsThreadSafeAllocator2< RemoveAllQualifiers<T> >::value;
//-----------------------------------------------------------------------------



    //
    // Allocator Reference
    //
    template <typename AllocatorType>
    class AllocatorRef
    {
    // types
    public:
        using Allocator_t       = AllocatorType;
        using Self              = AllocatorRef< AllocatorType >;

        template <typename T>
        using StdAllocator_t    = StdAllocatorRef< T, AllocatorType* >;

        static constexpr bool   IsThreadSafe = IsThreadSafeAllocator< AllocatorType >;


    // variables
    private:
        AllocatorType&  _alloc;


    // methods
    public:
        AllocatorRef (Self &&other)                                         __NE___ : _alloc{other._alloc} {}
        AllocatorRef (const Self &other)                                    __NE___ : _alloc{other._alloc} {}
        AllocatorRef (AllocatorType &alloc)                                 __NE___ : _alloc{alloc} {}

        ND_ void*   Allocate (Bytes size)                                   __NE___ { return _alloc.Allocate( size ); }
        ND_ void*   Allocate (const SizeAndAlign sizeAndAlign)              __NE___ { return _alloc.Allocate( sizeAndAlign ); }

        template <typename T>
        ND_ T*      Allocate (usize count = 1)                              __NE___ { return Cast<T>( Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> })); }

            void    Deallocate (void* ptr)                                  __NE___ { return _alloc.Deallocate( ptr ); }
            void    Deallocate (void* ptr, Bytes size)                      __NE___ { return _alloc.Deallocate( ptr, size ); }
            void    Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___ { return _alloc.Deallocate( ptr, sizeAndAlign ); }

        ND_ bool    operator == (const Self &rhs)                           C_NE___ { return &_alloc == &rhs._alloc; }

        ND_ AllocatorType&  GetAllocatorRef ()                              C_NE___ { return _alloc; }
    };



    //
    // Aligned Allocator Reference with base align
    //
    template <typename AllocatorType, usize BaseAlign>
    class AllocatorRefBaseAlign
    {
    // types
    public:
        using Allocator_t       = AllocatorType;
        using Self              = AllocatorRefBaseAlign< AllocatorType, BaseAlign >;

        template <typename T>
        using StdAllocator_t    = StdAllocatorRef< T, AllocatorType* >;

        static constexpr usize  Align           = BaseAlign;
        static constexpr bool   IsThreadSafe    = IsThreadSafeAllocator< AllocatorType >;


    // variables
    private:
        AllocatorType&  _alloc;


    // methods
    public:
        AllocatorRefBaseAlign (Self &&other)                __NE___ : _alloc{other._alloc} {}
        AllocatorRefBaseAlign (const Self &other)           __NE___ : _alloc{other._alloc} {}
        AllocatorRefBaseAlign (AllocatorType &alloc)        __NE___ : _alloc{alloc} {}

        template <typename T>
        ND_ T*      Allocate (usize count = 1)              __NE___ { StaticAssert( alignof(T) <= BaseAlign );  return Cast<T>( Allocate( SizeOf<T> * count )); }
        ND_ void*   Allocate (Bytes size)                   __NE___ { return _alloc.Allocate( SizeAndAlign{ size, Bytes{BaseAlign} }); }

            void    Deallocate (void* ptr, Bytes size)      __NE___ { return _alloc.Deallocate( ptr, SizeAndAlign{ size, Bytes{BaseAlign} }); }

        ND_ bool    operator == (const Self &rhs)           C_NE___ { return &_alloc == &rhs._alloc; }

        ND_ AllocatorType&  GetAllocatorRef ()              C_NE___ { return _alloc; }
    };



    //
    // Std Allocator Reference
    //
    template <typename T, typename TAllocatorPtr>
    class StdAllocatorRef
    {
    // types
    public:
        StaticAssert( IsPointer<TAllocatorPtr> or IsRC<TAllocatorPtr> or IsSpecializationOf<TAllocatorPtr, Ptr> );

        using value_type        = T;
        using size_type         = usize;
        using difference_type   = ssize;
        using Self              = StdAllocatorRef< T, TAllocatorPtr >;

        // removed in C++20
        template <typename U>
        struct rebind {
            using other = StdAllocatorRef< U, TAllocatorPtr >;
        };


    // variables
    private:
        TAllocatorPtr   _alloc;


    // methods
    public:
        StdAllocatorRef (TAllocatorPtr alloc)                       __NE___ : _alloc{FwdArg<TAllocatorPtr>(alloc)} {}
        StdAllocatorRef (Self &&other)                              __NE___ : _alloc{RVRef(other._alloc)} {}
        StdAllocatorRef (const Self &other)                         __NE___ : _alloc{other._alloc} {}

        template <typename B>
        StdAllocatorRef (const StdAllocatorRef<B,TAllocatorPtr>& other) __NE___ : _alloc{other.GetAllocatorPtr()} {}

            Self&   operator = (const Self &)                               = delete;
            Self&   operator = (Self &&)                            __NE___ = default;


        // returns non-null pointer
        ND_ T*      allocate (const usize count)                    __Th___
        {
            T*  ptr = Cast<T>( _alloc->Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }));
            CHECK_THROW( ptr != null, std::bad_alloc{} );
            return ptr;
        }

            void    deallocate (T * const ptr, const usize count)   __NE___ { return _alloc->Deallocate( ptr, SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }); }

        ND_ Self    select_on_container_copy_construction ()        C_NE___ { return Self{ *_alloc }; }

        // removed in C++20
        ND_ usize   max_size ()                                     C_NE___ { return usize(UMax) / sizeof(T); }

        ND_ bool    operator == (const Self &rhs)                   C_NE___ { return _alloc == rhs._alloc; }

        ND_ TAllocatorPtr   GetAllocatorPtr ()                      C_NE___ { return _alloc; }
    };
//-----------------------------------------------------------------------------



    //
    // Allocator implementation
    //
    template <typename T>
    class IAllocatorAdaptor final :
        public Conditional< IsThreadSafeAllocator<T>, IAllocatorTS, IAllocator >
    {
    // variables
    private:
        T   _alloc;

    // methods
    public:
        IAllocatorAdaptor ()                                            __NE___ {}
        IAllocatorAdaptor (IAllocatorAdaptor &&other)                   __NE___ : _alloc{ RVRef(other._alloc) } {}

        template <typename ...Args>
        explicit IAllocatorAdaptor (Args&& ... args)                    __Th___ : _alloc{ FwdArg<Args>(args)... } {}

        using IAllocator::Allocate;

        void*  Allocate (Bytes size)                                    __NE_OV { return _alloc.Allocate( size ); }
        void   Deallocate (void* ptr, Bytes size)                       __NE_OV { return _alloc.Deallocate( ptr, size ); }
        void   Deallocate (void* ptr)                                   __NE_OV { return _alloc.Deallocate( ptr ); }

        void*  Allocate (const SizeAndAlign sizeAndAlign)               __NE_OV { return _alloc.Allocate( sizeAndAlign ); }
        void   Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)  __NE_OV { return _alloc.Deallocate( ptr, sizeAndAlign ); }
    };


    template <typename T>
    class IAllocatorAdaptor< AllocatorRef<T> > final :
        public Conditional< IsThreadSafeAllocator<T>, IAllocatorTS, IAllocator >
    {
    // variables
    private:
        T   _alloc;

    // methods
    public:
        IAllocatorAdaptor ()                                            __NE___ {}
        IAllocatorAdaptor (IAllocatorAdaptor &&other)                   __NE___ : _alloc{ RVRef(other._alloc) } {}
        explicit IAllocatorAdaptor (const AllocatorRef<T> &ref)         __NE___ : _alloc{ ref.GetAllocatorRef() } {}

        template <typename ...Args>
        explicit IAllocatorAdaptor (Args&& ... args)                    __Th___ : _alloc{ FwdArg<Args>(args)... } {}

        using IAllocator::Allocate;

        void*  Allocate (Bytes size)                                    __NE_OV { return _alloc.Allocate( size ); }
        void   Deallocate (void* ptr, Bytes size)                       __NE_OV { return _alloc.Deallocate( ptr, size ); }
        void   Deallocate (void* ptr)                                   __NE_OV { return _alloc.Deallocate( ptr ); }

        void*  Allocate (const SizeAndAlign sizeAndAlign)               __NE_OV { return _alloc.Allocate( sizeAndAlign ); }
        void   Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)  __NE_OV { return _alloc.Deallocate( ptr, sizeAndAlign ); }
    };


} // AE::Base


namespace AE
{
        void                        SetDefaultAllocator (Base::RC<Base::IAllocator>)    __NE___;
    ND_ Base::RC<Base::IAllocator>  GetDefaultAllocator ()                              __NE___;
    ND_ Base::Ptr<Base::IAllocator> GetDefaultAllocatorPtr ()                           __NE___;

} // AE


namespace std
{
    template <typename T, typename A>
    void  swap (AE::Base::StdAllocatorRef<T,A> &lhs, AE::Base::StdAllocatorRef<T,A> &rhs) __NE___
    {
        auto    tmp = AE::Base::RVRef(lhs);
        lhs = AE::Base::RVRef(rhs);
        rhs = AE::Base::RVRef(tmp);
    }

} // std

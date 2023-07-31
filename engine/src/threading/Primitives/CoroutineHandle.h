// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_HAS_COROUTINE
# include <coroutine>
# include "threading/Common.h"

namespace AE::Threading
{

    //
    // Coroutine Handle
    //

    template <typename PromiseType>
    struct CoroutineHandle
    {
    // types
    public:
        using Handle_t      = std::coroutine_handle< PromiseType >;
        using Self          = CoroutineHandle< PromiseType >;
        using Promise_t     = PromiseType;

        using promise_type  = PromiseType;


    // variables
    private:
        Handle_t    _handle;


    // methods
    private:
        explicit CoroutineHandle (Handle_t h)   __NE___ : _handle{h} {}

    public:
        CoroutineHandle ()                      __NE___ :
            _handle{ null }
        {}

        CoroutineHandle (Self && other)         __NE___ :
            _handle{ other._handle }
        {
            other._handle = null;
        }

        CoroutineHandle (const Self &)          = delete;

        ~CoroutineHandle ()                     __NE___
        {
            if ( _handle )
                _handle.destroy();
        }

        Self&  operator = (const Self &)        = delete;

        Self&  operator = (Self && rhs)         __NE___
        {
            if ( _handle )
                _handle.destroy();

            _handle = rhs._handle;

            rhs._handle = null;
            return *this;
        }

        // checks if the handle represents a coroutine
        ND_ constexpr bool  IsValid ()          C_NE___
        {
            return bool{_handle};
        }

        // resumes execution of the coroutine
        void  Resume ()                         C_NE___
        {
            ASSERT( _handle );
            return _handle.resume();
        }

        // checks if the coroutine has completed
        ND_ bool  Done ()                       C_NE___
        {
            ASSERT( _handle );
            return _handle.done();
        }

        // access the promise of a coroutine
        ND_ Promise_t&  Promise ()              C_NE___
        {
            ASSERT( _handle );
            return _handle.promise();
        }

        // creates a coroutine_handle from the promise object of a coroutine
        ND_ static constexpr Self  FromPromise (Promise_t& promise) __NE___
        {
            return Self{ Handle_t::from_promise( promise )};
        }

        // exports the underlying address, i.e. the pointer backing the coroutine
        ND_ constexpr void*  Address ()         C_NE___
        {
            ASSERT( _handle );
            return _handle.address();
        }

        ND_ constexpr void*  Release ()         __NE___
        {
            ASSERT( _handle );
            void*   addr = _handle.address();
            _handle = null;
            return addr;
        }

        // imports a coroutine from a pointer
        ND_ static constexpr Self  FromAddress (void* const addr) __NE___
        {
            return Self{ Handle_t::from_address( addr )};
        }

        void  Destroy ()                        __NE___
        {
            if ( _handle )
                _handle.destroy();

            _handle = null;
        }

        ND_ HashVal  GetHash ()                 C_NE___
        {
            return HashOf( _handle );
        }
    };


} // AE::Threading



template <typename PromiseType>
struct std::hash< AE::Threading::CoroutineHandle<PromiseType> > {
    ND_ size_t  operator () (const AE::Threading::CoroutineHandle<PromiseType> &c) C_NE___ {
        return size_t(c.GetHash());
    }
};

#endif // AE_HAS_COROUTINE

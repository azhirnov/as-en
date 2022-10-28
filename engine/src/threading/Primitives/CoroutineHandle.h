// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"

#ifdef AE_HAS_COROUTINE
# include <coroutine>

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
		using Handle_t		= std::coroutine_handle< PromiseType >;
		using Self			= CoroutineHandle< PromiseType >;
		using Promise_t		= PromiseType;

		using promise_type	= PromiseType;


	// variables
	private:
		Handle_t	_handle;


	// methods
	private:
		explicit CoroutineHandle (Handle_t h) : _handle{h} {}

	public:
		CoroutineHandle () :
			_handle{ null }
		{}

		CoroutineHandle (Self && other) :
			_handle{ other._handle }
		{
			other._handle = null;
		}

		CoroutineHandle (const Self &) = delete;

		~CoroutineHandle ()
		{
			if ( _handle )
				_handle.destroy();
		}

		Self&  operator = (const Self &) = delete;

		Self&  operator = (Self && rhs)
		{
			if ( _handle )
				_handle.destroy();

			_handle = rhs._handle;

			rhs._handle = null;
			return *this;
		}

		// checks if the handle represents a coroutine
		ND_ constexpr bool  IsValid () const
		{
			return bool{_handle};
		}

		// resumes execution of the coroutine
		void  Resume () const
		{
			ASSERT( _handle );
			return _handle.resume();
		}
		
		// checks if the coroutine has completed
		ND_ bool  Done () const
		{
			ASSERT( _handle );
			return _handle.done();
		}

		// access the promise of a coroutine
		ND_ Promise_t&  Promise () const
		{
			ASSERT( _handle );
			return _handle.promise();
		}

		// creates a coroutine_handle from the promise object of a coroutine
		ND_ static constexpr Self  FromPromise (Promise_t& promise)
		{
			return Self{ Handle_t::from_promise( promise )};
		}

		// exports the underlying address, i.e. the pointer backing the coroutine
		ND_ constexpr void*  Address () const
		{
			ASSERT( _handle );
			return _handle.address();
		}

		ND_ constexpr void*  Release ()
		{
			ASSERT( _handle );
			void*	addr = _handle.address();
			_handle = null;
			return addr;
		}
		
		// imports a coroutine from a pointer
		ND_ static constexpr Self  FromAddress (void* const addr)
		{
			return Self{ Handle_t::from_address( addr )};
		}

		void  Destroy ()
		{
			if ( _handle )
				_handle.destroy();

			_handle = null;
		}

		ND_ HashVal  GetHash () const
		{
			return HashOf( _handle );
		}
	};


} // AE::Threading


namespace std
{
	template <typename PromiseType>
	struct hash< AE::Threading::CoroutineHandle<PromiseType> > {
		ND_ size_t  operator () (const AE::Threading::CoroutineHandle<PromiseType> &c) const {
			return size_t(c.GetHash());
		}
	};

} // std

#endif // AE_HAS_COROUTINE
// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exceptions:
		- internal object may throw exceptions (in copy-ctor)
*/

#pragma once

#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	//
	// In Place Storage
	//

	template <typename T>
	struct InPlace final
	{
		// some std containers dosn't marked as noexcept
		//STATIC_ASSERT( IsNothrowMoveCtor<T> );
		//STATIC_ASSERT( IsNothrowDefaultCtor<T> );

	// types
	public:
		using Self		= InPlace< T >;
		using Value_t	= T;


	// variables
	private:
		union {
			T			_value;
			ubyte		_data [ sizeof(T) ];	// don't use it!
		};
		DEBUG_ONLY(
			bool		_isCreated = false;
		)


	// methods
	public:
		InPlace ()							__NE___
		{
			DEBUG_ONLY( DbgFreeMem( OUT _value ));
		}

		InPlace (const Self &other)			noexcept(IsNothrowCopyCtor<T>)
			DEBUG_ONLY(: _isCreated{other._isCreated})
		{
			PlacementNew<T>( OUT &_value, other._value );			// throw
		}

		InPlace (Self&& other)				__NE___
			DEBUG_ONLY(: _isCreated{other._isCreated})
		{
			PlacementNew<T>( OUT &_value, RVRef(other._value) );	// nothrow
		}

		~InPlace ()							__NE___
		{
			ASSERT( not _isCreated );
		}


		Self&  operator = (const Self &rhs) = delete;
		Self&  operator = (Self&& rhs) = delete;


		template <typename ...Args>
		Self&  Create (Args&& ...args)		noexcept(IsNothrowCtor<T, Args...>)
		{
			DEBUG_ONLY(
				ASSERT( not _isCreated );
				_isCreated = true;
			)
			PlacementNew<T>( OUT &_value, FwdArg<Args &&>( args )... );	// throw
			return *this;
		}

		void  Destroy ()					__NE___
		{
			DEBUG_ONLY(
				ASSERT( _isCreated );
				_isCreated = false;
			)
			PlacementDelete( INOUT _value );
		}


		template <typename Fn>
		Self&  CustomCtor (const Fn &fn)	noexcept(std::is_nothrow_invocable_v<Fn>)
		{
			DEBUG_ONLY(
				ASSERT( not _isCreated );
				_isCreated = true;
			)
			fn( OUT _value );	// throw
			return *this;
		}
		
		template <typename Fn>
		void  CustomDtor (const Fn &fn)		__NE___
		{
			DEBUG_ONLY(
				ASSERT( _isCreated );
				_isCreated = false;
			)
			fn( OUT _value );
		}


		ND_ T *			operator -> ()		__NE___	{ ASSERT( _isCreated );  return &_value; }
		ND_ T const*	operator -> ()		C_NE___	{ ASSERT( _isCreated );  return &_value; }

		ND_ T &			operator * ()		__NE___	{ ASSERT( _isCreated );  return _value; }
		ND_ T const&	operator * ()		C_NE___	{ ASSERT( _isCreated );  return _value; }

		ND_ T *			operator & ()		__NE___	{ ASSERT( _isCreated );  return &_value; }
		ND_ T const*	operator & ()		C_NE___	{ ASSERT( _isCreated );  return &_value; }

		DEBUG_ONLY(
			ND_ bool	IsCreated ()		C_NE___	{ return _isCreated; }
		)
	};


} // AE::Base

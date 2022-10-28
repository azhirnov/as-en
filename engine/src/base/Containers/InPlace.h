// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		InPlace ()
		{
			DEBUG_ONLY( DbgFreeMem( OUT _value ));
		}

		InPlace (const Self &other)
			DEBUG_ONLY(: _isCreated{other._isCreated})
		{
			PlacementNew<T>( OUT &_value, other._value );
		}

		InPlace (Self&& other)
			DEBUG_ONLY(: _isCreated{other._isCreated})
		{
			PlacementNew<T>( OUT &_value, RVRef(other._value) );
		}

		~InPlace ()
		{
			ASSERT( not _isCreated );
		}


		Self&  operator = (const Self &rhs) = delete;
		Self&  operator = (Self&& rhs) = delete;


		template <typename ...Args>
		Self&  Create (Args&& ...args)
		{
			DEBUG_ONLY(
				ASSERT( not _isCreated );
				_isCreated = true;
			)
			PlacementNew<T>( OUT &_value, FwdArg<Args &&>( args )... );
			return *this;
		}

		void  Destroy ()
		{
			DEBUG_ONLY(
				ASSERT( _isCreated );
				_isCreated = false;
			)
			PlacementDelete( INOUT _value );
		}


		template <typename Fn>
		Self&  CustomCtor (const Fn &fn)
		{
			DEBUG_ONLY(
				ASSERT( not _isCreated );
				_isCreated = true;
			)
			fn( OUT _value );
			return *this;
		}
		
		template <typename Fn>
		void  CustomDtor (const Fn &fn)
		{
			DEBUG_ONLY(
				ASSERT( _isCreated );
				_isCreated = false;
			)
			fn( OUT _value );
		}


		ND_ T *			operator -> ()			{ ASSERT( _isCreated );  return &_value; }
		ND_ T const*	operator -> ()	const	{ ASSERT( _isCreated );  return &_value; }

		ND_ T &			operator * ()			{ ASSERT( _isCreated );  return _value; }
		ND_ T const&	operator * ()	const	{ ASSERT( _isCreated );  return _value; }

		ND_ T *			operator & ()			{ ASSERT( _isCreated );  return &_value; }
		ND_ T const*	operator & ()	const	{ ASSERT( _isCreated );  return &_value; }

		DEBUG_ONLY(
			ND_ bool	IsCreated ()	const	{ return _isCreated; }
		)
	};


} // AE::Base

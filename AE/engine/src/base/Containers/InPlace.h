// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
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
	// types
	public:
		using Self		= InPlace< T >;
		using Value_t	= T;


	// variables
	private:
		union {
			T			_value;
			ubyte		_unused [ sizeof(T) ];	// don't use it!
		};
		DEBUG_ONLY(
			bool		_isCreated = false;
		)


	// methods
	public:
		__Cx__ InPlace ()									__NE___
		{
			if_not_consteval() {
				DEBUG_ONLY( DbgFreeMem( OUT _value ));
			}
		}

		__Cx__ InPlace (const Self &other)					__NE___ :
			_value{ other.Ref() }
			DEBUG_ONLY(, _isCreated{ true })
		{
			CheckNothrow( IsNothrowCopyCtor< T >);
		}

		__Cx__ InPlace (Self &&other)						__NE___ :
			_value{ other.AsRVRef() }
			DEBUG_ONLY(, _isCreated{ true })
		{
			CheckNothrow( IsNothrowMoveCtor< T >);
		}

		__Cx__ ~InPlace ()									__NE___
		{
			StaticAssert( alignof(Self) >= alignof(T) );
			ASSERT_MSG( not _isCreated, "must be destroyed" );
		}


		Self&  operator = (const Self &rhs)					= delete;
		Self&  operator = (Self&& rhs)						= delete;


		template <typename ...Args>
		__Cx__ Self&  Create (Args&& ...args)				__NE___
		{
			ASSERT_MSG( not _isCreated, "already created" );

			CheckNothrow( IsNoExcept( T{ FwdArg<Args>( args )... }));
			CheckNothrow( IsNoExcept( new (std::addressof(_value)) T{ FwdArg<Args>( args )... }));

			new (std::addressof(_value)) T{ FwdArg<Args>( args )... };

			DEBUG_ONLY( _isCreated = true;)
			return *this;
		}

		template <typename ...Args>
		Self&  CreateOrThrow (Args&& ...args)				__Th___
		{
			ASSERT_MSG( not _isCreated, "already created" );

			CheckNothrow( not IsNoExcept( new (std::addressof(_value)) T{ FwdArg<Args>( args )... }));

			new (std::addressof(_value)) T{ FwdArg<Args>( args )... };  // throw

			DEBUG_ONLY( _isCreated = true;)
			return *this;
		}

		__Cx__ void  Destroy ()								__NE___
		{
			Ref().~T();

			DEBUG_ONLY(
				_isCreated = false;
				DbgFreeMem( _value );
			)
		}


		template <typename Fn>
		__Cx__ Self&  CustomCtor (const Fn &fn)				__NE___
		{
			CheckNothrow( IsNoExcept( fn( _value )));
			ASSERT_MSG( not _isCreated, "already created" );

			fn( OUT _value );

			DEBUG_ONLY( _isCreated = true;)
			return *this;
		}

		template <typename Fn>
		__Cx__ void  CustomDtor (const Fn &fn)				__NE___
		{
			CheckNothrow( IsNoExcept( fn( _value )));

			fn( INOUT Ref() );

			DEBUG_ONLY(
				_isCreated = false;
				DbgFreeMem( _value );
			)
		}

		NdCx__ T *			Ptr ()							__NE___	{ ASSERT( _isCreated );  return std::launder( &_value ); }
		NdCx__ T const*		Ptr ()							C_NE___	{ ASSERT( _isCreated );  return std::launder( &_value ); }
		NdCx__ T const*		ConstPtr ()						C_NE___	{ ASSERT( _isCreated );  return std::launder( &_value ); }

		NdCx__ T &			Ref ()							__NE___	{ return *Ptr(); }
		NdCx__ T const&		Ref ()							C_NE___	{ return *Ptr(); }

		NdCx__ T &&			AsRVRef ()						rvNE___	{ return RVRef(*Ptr()); }


		NdCx__ T *			operator -> ()					__NE___	{ return Ptr(); }
		NdCx__ T const*		operator -> ()					C_NE___	{ return Ptr(); }

		NdCx__ T &			operator * ()					__NE___	{ return *Ptr(); }
		NdCx__ T const&		operator * ()					C_NE___	{ return *Ptr(); }

		NdCx__ T *			operator & ()					__NE___	{ return Ref(); }
		NdCx__ T const*		operator & ()					C_NE___	{ return Ref(); }


		DEBUG_ONLY(
			NdCx__ bool		IsCreated ()					C_NE___	{ return _isCreated; }
		)
	};


	template <typename T>	struct TMemCopyAvailable< InPlace<T> >		: CT_False {};
	template <typename T>	struct TTriviallyDestructible< InPlace<T> >	: CT_False {};


} // AE::Base

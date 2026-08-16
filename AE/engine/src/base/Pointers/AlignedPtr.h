// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Pointers/Ref.h"
#include "base/Math/Byte.h"

namespace AE::Base
{

	//
	// Aligned Pointer
	//

	template <usize Align, bool Const = false>
	struct AlignedPtr
	{
	// types
	public:
		using Self	= AlignedPtr< Align, Const >;
		using Type	= Conditional< Const, const void*, void* >;
		using CType	= Conditional< Const, const void*, const void* >;


	// variables
	private:
		Type	_value	= null;


	// methods
	public:
		__Cx__ AlignedPtr ()						__NE___ {}
		__Cx__ AlignedPtr (std::nullptr_t)			__NE___ {}
		__Cx__ explicit AlignedPtr (Type ptr)		__NE___ : _value{ AssumeAligned< Align >( ptr )} {}

		NdCx__ Type		get ()						__NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }
		NdCx__ CType	get ()						C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }

		NdCx__ operator Type ()						__NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }
		NdCx__ operator CType ()					C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }

		template <typename T>
		NdCx__ auto*	Cast ()						__NE___	{ NonNull( _value );  return AssumeAligned< Align >( Base::Cast<T>( _value )); }

		template <typename T>
		NdCx__ auto*	Cast ()						C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( Base::Cast<T>( _value )); }

		NdCx__ explicit operator bool ()			C_NE___	{ return _value != null; }

		NdCx__ Self		operator +  (Bytes offset)	C_NE___	{ ASSERT( IsMultipleOf( offset, Align ));  return Self{ _value + offset }; }
		__Cx__ Self&	operator += (Bytes offset)	__NE___	{ ASSERT( IsMultipleOf( offset, Align ));  _value += offset;  return *this; }

		NdCx__ Self		operator +  (usize offset)	C_NE___	{ ASSERT( IsMultipleOf( offset, Align ));	return Self{ _value + offset }; }
		__Cx__ Self&	operator += (usize offset)	__NE___	{ ASSERT( IsMultipleOf( offset, Align ));	_value += offset;  return *this; }

		NdCx__ bool		operator == (Self rhs)		C_NE___	{ return _value == rhs._value; }
		NdCx__ bool		operator != (Self rhs)		C_NE___	{ return not (*this == rhs); }
		NdCx__ bool		operator <  (Self rhs)		C_NE___ { return _value <  rhs._value; }
		NdCx__ bool		operator >  (Self rhs)		C_NE___ { return _value >  rhs._value; }
		NdCx__ bool		operator <= (Self rhs)		C_NE___ { return _value <= rhs._value; }
		NdCx__ bool		operator >= (Self rhs)		C_NE___ { return _value >= rhs._value; }

		NdCe__ static Bytes	PtrAlign ()				__NE___	{ return Bytes{Align}; }
	};


	template <usize A>	struct TMemCopyAvailable< AlignedPtr<A> >	: CT_True {};
	template <usize A>	struct TZeroMemAvailable< AlignedPtr<A> >	: CT_True {};

} // AE::Base


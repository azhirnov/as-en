// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Pointers/Ref.h"
#include "base/Math/Byte.h"

namespace AE::Base
{

	//
	// Aligned Pointer
	//

	template <usize Align>
	struct AlignedPtr
	{
	// types
	public:
		using Self = AlignedPtr< Align >;


	// variables
	private:
		void *		_value	= null;


	// methods
	public:
		__Cx__ AlignedPtr ()						__NE___ {}
		__Cx__ AlignedPtr (std::nullptr_t)			__NE___ {}
		__Cx__ explicit AlignedPtr (void* ptr)		__NE___ : _value{ AssumeAligned< Align >( ptr )} {}

		NdCx__ void*		get ()					__NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }
		NdCx__ void const*	get ()					C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }

		NdCx__ operator void* ()					__NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }
		NdCx__ operator void const* ()				C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( _value ); }

		template <typename T>
		NdCx__ T*		Cast ()						__NE___	{ NonNull( _value );  return AssumeAligned< Align >( Base::Cast<T>( _value )); }

		template <typename T>
		NdCx__ T const*	Cast ()						C_NE___	{ NonNull( _value );  return AssumeAligned< Align >( Base::Cast<T>( _value )); }

		NdCx__ explicit operator bool ()			C_NE___	{ return _value != null; }

		NdCx__ Self		operator +  (Bytes offset)	C_NE___	{ ASSERT( IsMultipleOf( offset, Align ));  return Self{ _value + offset }; }
		__Cx__ Self&	operator += (Bytes offset)	__NE___	{ ASSERT( IsMultipleOf( offset, Align ));  _value += offset;  return *this; }

		NdCx__ Self		operator +  (usize offset)	C_NE___	{ return Self{ _value + (PtrAlign() * offset) }; }
		__Cx__ Self&	operator += (usize offset)	__NE___	{ _value += PtrAlign() * offset;  return *this; }

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


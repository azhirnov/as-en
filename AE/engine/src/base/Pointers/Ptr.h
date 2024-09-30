// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Raw Pointer Wrapper
	//

	template <typename T>
	struct Ptr
	{
	// variables
	private:
		T *		_value	= null;

	// methods
	public:
		constexpr Ptr ()									__NE___ {}
		constexpr Ptr (std::nullptr_t)						__NE___ {}
		constexpr Ptr (T* ptr)								__NE___ : _value{ptr} {}

		template <typename B>
		constexpr Ptr (Ptr<B> other)						__NE___ : _value{static_cast<T*>( other.get() )} {}

		ND_ constexpr T *		operator -> ()				C_NE___	{ NonNull( _value );  return _value; }
		ND_ constexpr T &		operator *  ()				C_NE___	{ NonNull( _value );  return *_value; }
		ND_ constexpr T *		get ()						C_NE___	{ return _value; }

		ND_ constexpr explicit operator T * ()				C_NE___	{ return _value; }

		ND_ constexpr operator Ptr<const T> ()				C_NE___	{ return _value; }

		template <typename B>
		ND_ constexpr explicit operator B  ()				C_NE___	{ return static_cast<B>( _value ); }

		ND_ constexpr explicit operator bool ()				C_NE___	{ return _value != null; }

		ND_ constexpr bool  operator == (Ptr<T> rhs)		C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool  operator != (Ptr<T> rhs)		C_NE___	{ return not (*this == rhs); }
		ND_ constexpr bool  operator <  (Ptr<T> rhs)		C_NE___ { return _value <  rhs._value; }
		ND_ constexpr bool  operator >  (Ptr<T> rhs)		C_NE___ { return _value >  rhs._value; }
		ND_ constexpr bool  operator <= (Ptr<T> rhs)		C_NE___ { return _value <= rhs._value; }
		ND_ constexpr bool  operator >= (Ptr<T> rhs)		C_NE___ { return _value >= rhs._value; }
	};


	namespace _hidden_
	{
		template <typename T>
		struct _RemovePtr {
			using type = T;
		};

		template <typename T>
		struct _RemovePtr< Ptr<T> > {
			using type = T;
		};

	} // _hidden_

	template <typename T>
	using RemovePtr = typename Base::_hidden_::_RemovePtr<T>::type;


	template <typename T>	struct TMemCopyAvailable< Ptr<T> >	: CT_True {};
	template <typename T>	struct TZeroMemAvailable< Ptr<T> >	: CT_True {};

} // AE::Base


template <typename T>
struct std::hash< AE::Base::Ptr<T> > {
	ND_ size_t  operator () (const AE::Base::Ptr<T> &value) C_NE___ {
		return hash<T *>()( value.operator->() );
	}
};

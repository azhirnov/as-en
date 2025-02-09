// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Pointers/Ref.h"

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
		__Cx__ Ptr ()								__NE___ {}
		__Cx__ Ptr (std::nullptr_t)					__NE___ {}
		__Cx__ Ptr (T* ptr)							__NE___ : _value{ptr} {}

		__Cx__ Ptr (Ref<T> ref)						__NE___ : _value{&ref} {}

		template <typename B>
		__Cx__ Ptr (Ptr<B> other)					__NE___ : _value{static_cast<T*>( other.get() )} {}

		NdCz__ T *		operator -> ()				C_NE___	{ NonNull( _value );  return _value; }
		NdCz__ T &		operator *  ()				C_NE___	{ NonNull( _value );  return *_value; }
		NdCx__ T *		get ()						C_NE___	{ return _value; }

		NdCx__ explicit operator T * ()				C_NE___	{ return _value; }

		NdCx__ operator Ptr<const T> ()				C_NE___	{ return _value; }

		template <typename B>
		NdCx__ explicit operator B  ()				C_NE___	{ return static_cast<B>( _value ); }

		NdCx__ explicit operator bool ()			C_NE___	{ return _value != null; }

		NdCx__ bool  operator == (Ptr<T> rhs)		C_NE___	{ return _value == rhs._value; }
		NdCx__ bool  operator != (Ptr<T> rhs)		C_NE___	{ return not (*this == rhs); }
		NdCx__ bool  operator <  (Ptr<T> rhs)		C_NE___ { return _value <  rhs._value; }
		NdCx__ bool  operator >  (Ptr<T> rhs)		C_NE___ { return _value >  rhs._value; }
		NdCx__ bool  operator <= (Ptr<T> rhs)		C_NE___ { return _value <= rhs._value; }
		NdCx__ bool  operator >= (Ptr<T> rhs)		C_NE___ { return _value >= rhs._value; }
	};

	template <typename T>
	using CPtr = Ptr< const T >;


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

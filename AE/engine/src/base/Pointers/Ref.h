// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Class that wraps non-null pointer.
*/

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Reference Wrapper
	//

	template <typename T>
	struct Ref
	{
	// variables
	private:
		T *		_value;

	// methods
	public:
		Ref ()											= delete;
		Ref (const Ref<T> &)							= delete;

		__Cx__ Ref (Ref<T> &&)							__NE___ = default;
		__Cx__ explicit Ref (T& ref)					__NE___ : _value{&ref} {}

		__Cx__ Ref<T>&	operator = (const Ref<T> &)		= delete;
		__Cx__ Ref<T>&	operator = (Ref<T> &&)			__NE___	= default;

	//	__Cx__ T&		operator = (const T& rhs)		C_NE___	{ return *_value = rhs; }

		NdCx__ T*		operator -> ()					C_NE___	{ return _value; }
		NdCx__ T*		operator & ()					C_NE___	{ return _value; }

		NdCx__ operator T& ()							C_NE___	{ return *_value; }

		NdCx__ T&		get ()							C_NE___	{ return *_value; }
	};


	template <typename T>
	Ref (T&) -> Ref<T>;


/*
=================================================
	ArgRef (same as std::ref)
=================================================
*/
	template <typename T>
	NdCx__ Ref<T>  ArgRef (T &arg) __NE___
	{
		return Ref<T>{ arg };
	}


} // AE::Base

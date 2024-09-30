// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
		Ref ()												= delete;
		Ref (const Ref<T> &)								= delete;

		constexpr Ref (Ref<T> &&)							__NE___ = default;
		constexpr explicit Ref (T& ref)						__NE___ : _value{&ref} {}

			constexpr Ref<T>&	operator = (const Ref<T> &)	= delete;
			constexpr Ref<T>&	operator = (Ref<T> &&)		__NE___	= default;

			constexpr T&		operator = (const T& rhs)	C_NE___	{ return *_value = rhs; }

		ND_ constexpr T*		operator -> ()				C_NE___	{ return _value; }
		ND_ constexpr T*		operator & ()				C_NE___	{ return _value; }

		ND_ constexpr operator T& ()						C_NE___	{ return *_value; }

		ND_ constexpr T&	get ()							C_NE___	{ return *_value; }
	};


	template <typename T>
	Ref (T&) -> Ref<T>;


/*
=================================================
	ArgRef (same as std::ref)
=================================================
*/
	template <typename T>
	ND_ constexpr Ref<T>  ArgRef (T &arg) __NE___
	{
		return Ref<T>{ arg };
	}


} // AE::Base

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
		Ptr () {}
		Ptr (T *ptr) : _value{ptr} {}

		template <typename B>
		Ptr (const Ptr<B> &other) : _value{static_cast<T*>( (B*)other )} {}

		ND_ T *		operator -> ()					const	{ ASSERT( _value );  return _value; }
		ND_ T &		operator *  ()					const	{ ASSERT( _value );  return *_value; }
		ND_ T *		get ()							const	{ return _value; }

		ND_ explicit operator T * ()				const	{ return _value; }

		ND_ operator Ptr<const T> ()				const	{ return _value; }

		template <typename B>
		ND_ explicit operator B  ()					const	{ return static_cast<B>( _value ); }

		ND_ explicit operator bool ()				const	{ return _value != null; }

		ND_ bool  operator == (const Ptr<T> &rhs)	const	{ return _value == rhs._value; }
		ND_ bool  operator != (const Ptr<T> &rhs)	const	{ return not (*this == rhs); }
	};

} // AE::Base


namespace std
{

	template <typename T>
	struct hash< AE::Base::Ptr<T> > {
		ND_ size_t  operator () (const AE::Base::Ptr<T> &value) const {
			return hash<T *>()( value.operator->() );
		}
	};

} // std

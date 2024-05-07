// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe: no
*/

#pragma once

#include "networking/Common.h"
#include "pch/Serializing.h"

namespace AE::Networking
{
	using namespace AE::Base;


	//
	// Network Data Encoder (Serializer)
	//

	class DataEncoder final
	{
	// variables
	private:
		Serializing::Serializer		_ser;


	// methods
	public:
		DataEncoder (void* ptr, Bytes size)								__NE___	: _ser{FastWStream{ ptr, ptr + size }} {}
		DataEncoder (void* ptr, const void* end)						__NE___	: _ser{FastWStream{ ptr, end }} {}

		ND_ Bytes	RemainingSize ()									C_NE___	{ return _ser.stream.RemainingSize(); }
		ND_ bool	IsFull ()											C_NE___	{ return _ser.stream.Empty(); }

		// optional
		ND_ bool	Flush ()											__NE___	{ return _ser.Flush(); }

		template <typename Arg0, typename ...Args>
		ND_ bool	operator () (const Arg0 &arg0, const Args& ...args)	__NE___	{ return _ser( arg0, args... ); }
		ND_ bool	operator () ()										__NE___	{ return true; }

		ND_ bool	Write (const void* buffer, Bytes size)				__NE___	{ return _ser.stream.Write( buffer, size ); }

		template <typename T>
		ND_ bool	Encode (const T* arr, const usize count)			__NE___
		{
			bool	ok = true;
			for (usize i = 0; (i < count) and ok; ++i) { ok = _ser( arr[i] ); }
			return ok;
		}
	};


} // AE::Networking

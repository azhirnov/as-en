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
	// Network Data Decoder (Deserializer)
	//

	class DataDecoder final
	{
	// variables
	private:
		Serializing::Deserializer	_des;


	// methods
	public:
		DataDecoder (const void* ptr, Bytes size, IAllocator &alloc)	__NE___ : _des{ FastRStream{ ptr, ptr+size }, &alloc } {}

		ND_ bool	IsComplete ()										C_NE___	{ return _des.stream.Empty(); }
		ND_ Bytes	RemainingSize ()									C_NE___	{ return _des.stream.RemainingSize(); }

		template <typename Arg0, typename ...Args>
		ND_ bool	operator () (OUT Arg0& arg0, OUT Args& ...args)		__NE___	{ return _des( OUT arg0, OUT args... ); }
		ND_ bool	operator () ()										__NE___	{ return true; }

		ND_ bool	Read (OUT void* buffer, Bytes size)					__NE___	{ return _des.stream.Read( OUT buffer, size ); }
		ND_ Bytes	ReadRemaining (OUT void* buffer)					__NE___	{ return _des.stream.ReadRemaining( OUT buffer ); }

		template <typename T>
		ND_ bool	Decode (OUT T* arr, const usize count)				__NE___
		{
			bool	ok = true;
			for (usize i = 0; (i < count) and ok; ++i) { ok = _des( OUT arr[i] ); }
			return ok;
		}
	};


} // AE::Networking

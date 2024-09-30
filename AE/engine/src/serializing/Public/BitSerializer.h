// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "serializing/Public/Common.h"

namespace AE::Serializing
{

	//
	// Serializer
	//

	struct BitSerializer : Noncopyable
	{
	// types
	public:
		using BitType = PackedBits< 0, 1, int >::BitType;


	// variables
	public:
		FastWStream			stream;

		DEBUG_ONLY(
			ulong			dbgUnusedBits = 0;
		)

	private:
		BitType				_packedBits	= 0;
		uint				_bitCount	= 0;


	// methods
	public:
		explicit BitSerializer (FastWStream wstream)		__NE___ : stream{ RVRef(wstream) } {}
		explicit BitSerializer (RC<WStream> wstream)		__NE___ : stream{ RVRef(wstream) } {}

		~BitSerializer ()									__NE___	{ Unused( _FlushBits() ); }

		template <typename ...Args>
		ND_ bool  operator () (const Args& ...args)			__NE___;

		// optional
		ND_ bool  Flush ()									__NE___	{ return _FlushBits(); }

	private:
		ND_ bool  _FlushBits ()								__NE___;

		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveSerialize (const Arg0 &, const Args& ...) __NE___;

		template <uint F, uint B, typename T>
		ND_ bool  _Serialize (const PackedBits<F,B,T> &)	__NE___;
	};

} // AE::Serializing

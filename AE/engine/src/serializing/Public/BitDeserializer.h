// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "serializing/Public/BitSerializer.h"

namespace AE::Serializing
{

	//
	// Bit Deserializer
	//

	struct BitDeserializer final : Noncopyable
	{
	// types
	public:
		using BitType = PackedBits< 0, 1, int >::BitType;


	// variables
	public:
		FastRStream		stream;

	private:
		BitType			_packedBits	= 0;
		uint			_bitCount	= 0;


	// methods
	public:
		explicit BitDeserializer (FastRStream rstream)		__NE___ : stream{ RVRef(rstream) } {}
		explicit BitDeserializer (RC<RStream> rstream)		__NE___ : stream{ RVRef(rstream) } {}

			template <typename ...Args>
		ND_ bool  operator () (INOUT Args& ...args)			__NE___;

		ND_ bool  IsEnd ()									C_NE___	{ return stream.Empty(); }

	private:
		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveDeserialize (INOUT Arg0 &, INOUT Args& ...) __NE___;

		template <uint F, uint B, typename T>
		ND_ bool  _Deserialize (INOUT PackedBits<F,B,T> &)	__NE___;
	};

} // AE::Serializing

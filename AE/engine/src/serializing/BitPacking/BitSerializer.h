// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Common.h"
#include "serializing/BitPacking/PackedBits.h"

namespace AE::Serializing
{

	//
	// Bit Serializer
	//

	struct BitSerializer : Noncopyable
	{
		friend class ObjectFactory;

	// types
	public:
		static constexpr uint	MaxStringLength	= 0xFFFF;
		static constexpr uint	MaxArrayLength	= 0xFFFFFF;	// TODO: remove or limit mem size


	// variables
	public:
		FastWStream			stream;
		Ptr<ObjectFactory>	factory;


	// methods
	public:
		explicit BitSerializer (FastWStream wstream) __NE___ : stream{ RVRef(wstream) } {}
		explicit BitSerializer (RC<WStream> wstream) __NE___ : stream{ RVRef(wstream) } {}

		template <typename ...Args>
		ND_ bool  operator () (const Args& ...args) __NE___;

	private:
	};


} // AE::Serializing

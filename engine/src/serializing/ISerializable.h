// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Common.h"

namespace AE::Serializing
{

	//
	// Serializable interface
	//
	class ISerializable
	{
	// interface
	public:
		virtual ~ISerializable ()								__NE___ {}

		ND_ virtual bool  Serialize (struct Serializer &)		C_NE___ = 0;
		ND_ virtual bool  Deserialize (struct Deserializer &)	__NE___ = 0;
	};


} // AE::Serializing

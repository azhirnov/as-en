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
		ND_ virtual bool  Serialize (struct Serializer &) const = 0;
		ND_ virtual bool  Deserialize (struct Deserializer &) = 0;
	};


}	// AE::Serializing

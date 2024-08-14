// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "serializing/Common.h"

namespace AE::Serializing
{

	//
	// Serializable interface
	//
	class NO_VTABLE ISerializable
	{
	// interface
	public:
		virtual ~ISerializable ()								__NE___ {}

		ND_ virtual bool  Serialize (struct Serializer &)		C_NE___ = 0;
		ND_ virtual bool  Deserialize (struct Deserializer &)	__NE___ = 0;
	};



#define DECL_SERIALIZER( _name_, ... )																				\
	bool  _name_::Serialize (Serializing::Serializer &ser)				C_NE___	{ return ser( __VA_ARGS__ ); }		\
	bool  _name_::Deserialize (Serializing::Deserializer &des)			__NE___	{ return des( OUT __VA_ARGS__ ); }

#define DECL_SERIALIZER_T( _prefix_, _name_, ... )																	\
	_prefix_ bool  _name_::Serialize (Serializing::Serializer &ser)		C_NE___	{ return ser( __VA_ARGS__ ); }		\
	_prefix_ bool  _name_::Deserialize (Serializing::Deserializer &des)	__NE___ { return des( OUT __VA_ARGS__ ); }

#define DECL_EMPTY_SERIALIZER( _name_ )																\
	bool  _name_::Serialize (Serializing::Serializer &)					C_NE___	{ return true; }	\
	bool  _name_::Deserialize (Serializing::Deserializer &)				__NE___ { return true; }


} // AE::Serializing

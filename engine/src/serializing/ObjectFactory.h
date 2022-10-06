// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/ISerializable.h"
#include "serializing/Serializer.h"
#include "serializing/Deserializer.h"

namespace AE::Serializing
{

	//
	// Object Factory
	//

	class ObjectFactory
	{
	// types
	private:
		using Serialize_t	= bool (*) (Serializer &, const void *);
		using Deserialize_t	= bool (*) (Deserializer &, OUT void *, bool create);

		struct ObjInfo
		{
			Serialize_t		serialize	= null;
			Deserialize_t	deserialize	= null;
		};

		using ObjectMap_t	= HashMap< SerializedID, ObjInfo >;		// requires pointer stability
		using ObjectTypes_t	= FlatHashMap< std::type_index, Pair<const SerializedID, ObjInfo>* >;
		using HashToObj_t	= FlatHashMap< uint, Pair<const SerializedID, ObjInfo>* >;


	// variables
	private:
		SharedMutex				_guard;
		ObjectMap_t				_objects;
		ObjectTypes_t			_objectTypes;

		#if not AE_OPTIMIZE_IDS
			HashToObj_t			_hashToObj;
		#endif

		DEBUG_ONLY(
			NamedID_HashCollisionCheck	_hashCollisionCheck;
		)


	// methods
	public:
		ObjectFactory () {}

		template <typename T>
		bool  Register (const SerializedID &id, Serialize_t ser, Deserialize_t deser);
		
		template <typename T>
		bool  Register (const SerializedID &id);

		template <typename T>
		ND_ bool  Serialize (Serializer &, const T& obj);

		template <typename T>
		ND_ bool  Deserialize (Deserializer &, INOUT T& obj);
		ND_ bool  Deserialize (Deserializer &, INOUT void* obj);
	};


}	// AE::Serializing

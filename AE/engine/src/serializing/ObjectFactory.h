// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  yes
*/

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
		using Serialize_t	= bool (*) (Serializer &, const void *) __NE___;
		using Deserialize_t	= bool (*) (Deserializer &, INOUT void *&, Ptr<IAllocator>) __NE___;
		using ID			= SerializedID::Optimized_t;

		struct ObjInfo
		{
			Serialize_t		serialize	= null;
			Deserialize_t	deserialize	= null;
		};

		using ObjectMap_t	= HashMap< ID, ObjInfo >;		// requires pointer stability
		using ObjectTypes_t	= FlatHashMap< TypeId, Pair<const ID, ObjInfo>* >;
		using HashToObj_t	= FlatHashMap< ID, Pair<const ID, ObjInfo>* >;


	// variables
	private:
		mutable SharedMutex		_guard;
		ObjectMap_t				_objects;
		ObjectTypes_t			_objectTypes;

		DEBUG_ONLY(
			NamedID_HashCollisionCheck	_hashCollisionCheck;
		)


	// methods
	public:
		ObjectFactory ()																	__NE___ {}

		template <typename T>
			bool  Register (SerializedID::Ref id, Serialize_t ser, Deserialize_t deser)		__NE___;

		template <typename T>
			bool  Register (SerializedID::Ref id)											__NE___;


		template <typename T>
		ND_ bool  Serialize (Serializer &, const T* obj)									C_NE___;

		template <typename T>
		ND_ bool  Serialize (Serializer &, const RC<T> &obj)								C_NE___;

		template <typename T>
		ND_ bool  Serialize (Serializer &, const Unique<T> &obj)							C_NE___;


		template <typename T>
		ND_ bool  Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT RC<T> &obj)		C_NE___;

		template <typename T>
		ND_ bool  Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT Unique<T> &)	C_NE___;


	private:
		ND_ bool  _Serialize (Serializer &, TypeId, const void* obj)						C_NE___;
		ND_ bool  _Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT void* &)		C_NE___;
	};


/*
=================================================
	Register
=================================================
*/
	template <typename T>
	bool  ObjectFactory::Register (SerializedID::Ref id, Serialize_t ser, Deserialize_t des) __NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not register trivial type" );
		CHECK_ERR( id.IsDefined() );

		EXLOCK( _guard );

		TRY{
			auto [it, inserted] = _objects.emplace( id, ObjInfo{ser, des} );			// throw
			CHECK_ERR( inserted );

			CHECK_ERR( _objectTypes.emplace( TypeIdOf<T>(), it.operator->() ).second );	// throw

			DEBUG_ONLY( _hashCollisionCheck.Add( id ));
			return true;
		}
		CATCH_ALL(
			return false;
		)
	}

	template <typename T>
	bool  ObjectFactory::Register (SerializedID::Ref id) __NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not register trivial type" );
		StaticAssert( IsBaseOf< ISerializable, T >);
		CheckNothrow( IsNothrowCtor< T >);

		return Register<T>( id,
							[] (Serializer &ser, const void* ptr) __NE___ -> bool
							{
								return Cast<ISerializable>(ptr)->Serialize( ser );
							},
							[] (Deserializer &des, INOUT void* &ptr, Ptr<IAllocator> alloc) __NE___ -> bool
							{
								Unused( alloc );	// TODO: allocator can not be used with RC<>, Unique<>
								if ( ptr == null )
									ptr = new T{};

								return Cast<ISerializable>(ptr)->Deserialize( des );
							}
						  );
	}

/*
=================================================
	Serialize
=================================================
*/
	template <typename T>
	bool  ObjectFactory::Serialize (Serializer &ser, const T *obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not serialize trivial type" );

		if ( obj == null )
			return ser( SerializedID{} );

		return _Serialize( ser, TypeIdOf(*obj), obj );
	}

	template <typename T>
	bool  ObjectFactory::Serialize (Serializer &ser, const RC<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		if ( obj == null )
			return ser( SerializedID{} );

		return _Serialize( ser, TypeIdOf(*obj), obj.get() );
	}

	template <typename T>
	bool  ObjectFactory::Serialize (Serializer &ser, const Unique<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		if ( obj == null )
			return ser( SerializedID{} );

		return _Serialize( ser, TypeIdOf(*obj), obj.get() );
	}

	inline bool  ObjectFactory::_Serialize (Serializer &ser, const TypeId typeId, const void* obj) C_NE___
	{
		ASSERT( obj != null );

		ID			id;
		ObjInfo		info;
		{
			SHAREDLOCK( _guard );

			auto	it = _objectTypes.find( typeId );
			CHECK_ERR_MSG( it != _objectTypes.end(),
				"Type "s << typeId.Name() << " is not registered" );

			id   = it->second->first;
			info = it->second->second;
		}

		CHECK_ERR( ser( id ) and info.serialize( ser, obj ));
		return true;
	}

/*
=================================================
	Deserialize
=================================================
*/
	inline bool  ObjectFactory::_Deserialize (Deserializer &des, Ptr<IAllocator> alloc, INOUT void* &ptr) C_NE___
	{
		ID	id;

		if_unlikely( not des( OUT id ))
			return false;

		if ( id == Default )
		{
			ptr = null;
			return true;
		}

		ObjInfo		info;
		{
			SHAREDLOCK( _guard );

			auto	it = _objects.find( id );
			if_unlikely( it == _objects.end() )
				return false;

			info = it->second;
		}
		return info.deserialize( des, INOUT ptr, alloc );
	}

/*
=================================================
	Deserialize
=================================================
*/
	template <typename T>
	bool  ObjectFactory::Deserialize (Deserializer &des, Ptr<IAllocator> alloc, INOUT RC<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		void*	ptr = null;
		bool	res = _Deserialize( des, alloc, INOUT ptr );

		obj.reset( Cast<T>( ptr ));
		return res;
	}

/*
=================================================
	Deserialize
=================================================
*/
	template <typename T>
	bool  ObjectFactory::Deserialize (Deserializer &des, Ptr<IAllocator> alloc, INOUT Unique<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		void*	ptr = null;
		bool	res = _Deserialize( des, alloc, INOUT ptr );

		obj.reset( Cast<T>( ptr ));
		return res;
	}


} // AE::Serializing

#include "serializing/Serializer.inl.h"
#include "serializing/Deserializer.inl.h"

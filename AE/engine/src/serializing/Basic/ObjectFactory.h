// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Basic/ISerializable.h"
#include "serializing/Basic/Serializer.h"
#include "serializing/Basic/Deserializer.h"

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

		struct ObjInfo
		{
			Serialize_t		serialize	= null;
			Deserialize_t	deserialize	= null;
		};

		using ObjectMap_t	= HashMap< SerializedID, ObjInfo >;		// requires pointer stability
		using ObjectTypes_t	= FlatHashMap< TypeId, Pair<const SerializedID, ObjInfo>* >;
		using HashToObj_t	= FlatHashMap< SerializedID::Optimized_t, Pair<const SerializedID, ObjInfo>* >;


	// variables
	private:
		mutable SharedMutex		_guard;
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
		ObjectFactory ()																	__NE___ {}

		template <typename T>
			bool  Register (SerializedID::Ref id, Serialize_t ser, Deserialize_t deser)		__NE___;

		template <typename T>
			bool  Register (SerializedID::Ref id)											__NE___;


		template <typename T>
		ND_ bool  Serialize (Serializer &, const T &obj)									C_NE___;

		template <typename T>
		ND_ bool  Serialize (Serializer &, const RC<T> &obj)								C_NE___;

		template <typename T>
		ND_ bool  Serialize (Serializer &, const Unique<T> &obj)							C_NE___;


		template <typename T>
		ND_ bool  Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT T &obj)			C_NE___;

		template <typename T>
		ND_ bool  Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT RC<T> &obj)		C_NE___;

		template <typename T>
		ND_ bool  Deserialize (Deserializer &, Ptr<IAllocator> alloc, INOUT Unique<T> &)	C_NE___;


	private:
		template <typename T>
		ND_ bool  _Serialize (Serializer &, const void* obj)								C_NE___;
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

			#if not AE_OPTIMIZE_IDS
				_hashToObj.emplace( id, it.operator->() );
			#endif

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
	bool  ObjectFactory::Serialize (Serializer &ser, const T &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not serialize trivial type" );
		StaticAssert( not IsPointer<T> );
		SHAREDLOCK( _guard );

		auto	it = _objectTypes.find( TypeIdOf<T>() );
		CHECK_ERR( it != _objectTypes.end() );

		CHECK_ERR(	ser( it->second->first )				and
					it->second->second.serialize( ser, &obj ));
		return true;
	}

	template <typename T>
	bool  ObjectFactory::Serialize (Serializer &ser, const RC<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		return _Serialize<T>( ser, obj.get() );
	}

	template <typename T>
	bool  ObjectFactory::Serialize (Serializer &ser, const Unique<T> &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );

		return _Serialize<T>( ser, obj.get() );
	}

	template <typename T>
	bool  ObjectFactory::_Serialize (Serializer &ser, const void* obj) C_NE___
	{
		if ( obj == null )
			return ser( SerializedID{} );

		SHAREDLOCK( _guard );

		auto	it = _objectTypes.find( TypeIdOf<T>() );
		CHECK_ERR( it != _objectTypes.end() );

		CHECK_ERR(	ser( it->second->first )				and
					it->second->second.serialize( ser, obj ));
		return true;
	}

/*
=================================================
	Deserialize
=================================================
*/
	template <typename T>
	bool  ObjectFactory::Deserialize (Deserializer &des, Ptr<IAllocator>, INOUT T &obj) C_NE___
	{
		StaticAssert( not IsTriviallySerializable<T>, "Can not deserialize trivial type" );
		StaticAssert( not IsPointer<T> );
		SHAREDLOCK( _guard );

		auto	it = _objectTypes.find( TypeIdOf<T>() );
		CHECK_ERR( it != _objectTypes.end() );

		SerializedID	id;
		void*			ptr = &obj;
		bool			res =
			des( OUT id )					and
			id == it->second->first			and
			it->second->second.deserialize( des, INOUT ptr, null );

		ASSERT( ptr == &obj );
		return res;
	}

/*
=================================================
	Deserialize
=================================================
*/
	inline bool  ObjectFactory::_Deserialize (Deserializer &des, Ptr<IAllocator> alloc, INOUT void* &ptr) C_NE___
	{
		SHAREDLOCK( _guard );

		SerializedID	id;
		CHECK_ERR( des( OUT id ));

		if ( id == Default )
		{
			ptr = null;
			return true;
		}

		ObjInfo const*	info = null;

		#if AE_OPTIMIZE_IDS
			auto	it = _objects.find( id );
			CHECK_ERR( it != _objects.end() );
			info = &it->second;
		#else
			auto	it = _hashToObj.find( id );
			CHECK_ERR( it != _hashToObj.end() );
			info = &it->second->second;
		#endif

		return info->deserialize( des, INOUT ptr, alloc );
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

#include "serializing/Basic/Serializer.inl.h"
#include "serializing/Basic/Deserializer.inl.h"

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

	struct Serializer : Noncopyable
	{
		friend class ObjectFactory;

	// types
	public:
		static constexpr uint	MaxStringLength	= 0xFFFF;
		static constexpr uint	MaxArrayLength	= 0xFFFFFF;	// TODO: remove or limit mem size


	// variables
	public:
		FastWStream					stream;
		Ptr<const ObjectFactory>	factory;

	private:
		#if AE_DEBUG_SERIALIZER
			uint					_dbgCounter = 0;
		#endif


	// methods
	public:
		explicit Serializer (FastWStream wstream)	__NE___ : stream{ RVRef(wstream) } {}
		explicit Serializer (RC<WStream> wstream)	__NE___ : stream{ RVRef(wstream) } {}

		~Serializer ()								__NE___	{}

		template <typename ...Args>
		ND_ bool  operator () (const Args& ...args)	__NE___;


	private:
		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveSerialize (const Arg0 &arg0, const Args& ...args)					__NE___;

		template <typename T>				ND_ bool  _SerializeObj (const T &)					__NE___;
		template <typename T>				ND_ bool  _Serialize (const T &)					__NE___;
		template <typename F, typename S>	ND_ bool  _Serialize (const Pair<F,S> &)			__NE___;
		template <usize N>					ND_ bool  _Serialize (const BitSet<N> &)			__NE___;
		template <typename T>				ND_ bool  _Serialize (ArrayView<T>)					__NE___;
		template <typename T, typename A>	ND_ bool  _Serialize (const Array<T,A> &v)			__NE___	{ return _Serialize(ArrayView<T>{v}); }
		template <typename T, usize S>		ND_ bool  _Serialize (const StaticArray<T,S> &arr)	__NE___	{ return _Serialize(ArrayView<T>{arr}); }
		template <typename T>				ND_ bool  _Serialize (BasicStringView<T>)			__NE___;
		template <typename T>				ND_ bool  _Serialize (const BasicString<T> &str)	__NE___	{ return _Serialize(BasicStringView<T>{str}); }
		template <typename T>				ND_ bool  _Serialize (const Rectangle<T> &)			__NE___;
		template <typename T>				ND_ bool  _Serialize (const RGBAColor<T> &)			__NE___;
		template <typename T>				ND_ bool  _Serialize (const HSVColor &)				__NE___;
		template <typename T, usize S>		ND_ bool  _Serialize (const TFixedString<T,S> &str)	__NE___	{ return _Serialize(BasicStringView<T>{str}); }

		template <typename T, usize S, typename CP>
		ND_ bool  _Serialize (const FixedArray<T,S,CP> &arr)									__NE___	{ return _Serialize(ArrayView<T>{arr}); }

		template <typename T, int I, glm::qualifier Q>
		ND_ bool  _Serialize (const TVec<T,I,Q> &)												__NE___;

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Serialize (const NamedID<Size, UID, true, Seed> &)							__NE___;

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Serialize (const NamedID<Size, UID, false, Seed> &)							__NE___;

		#ifdef AE_ENABLE_ABSEIL
		template <typename K, typename V, typename H, typename E, typename A>
		ND_ bool  _Serialize (const FlatHashMap<K,V,H,E,A> &map)								__NE___	{ return _SerializeMap<K,V>( map ); }
		#endif
		template <typename K, typename V, typename H, typename E, typename A>
		ND_ bool  _Serialize (const HashMap<K,V,H,E,A> &map)									__NE___	{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, typename H, typename E, typename A>
		ND_ bool  _Serialize (const HashMultiMap<K,V,H,E,A> &map)								__NE___	{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, usize S, typename KCP, typename VCP>
		ND_ bool  _Serialize (const FixedMap<K,V,S,KCP,VCP> &map)								__NE___	{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, typename MapType>
		ND_ bool  _SerializeMap (const MapType &)												__NE___;

		#ifdef AE_ENABLE_ABSEIL
		template <typename T, typename H, typename E, typename A>
		ND_ bool  _Serialize (const FlatHashSet<T,H,E,A> &set)									__NE___	{ return _SerializeSet<T>( set ); }
		#endif
		template <typename T, typename H, typename E, typename A>
		ND_ bool  _Serialize (const HashSet<T,H,E,A> &set)										__NE___	{ return _SerializeSet<T>( set ); }
		template <typename T, usize S, typename CP>
		ND_ bool  _Serialize (const FixedSet<T,S,CP> &set)										__NE___	{ return _SerializeSet<T>( set ); }
		template <typename T, typename SetType>
		ND_ bool  _SerializeSet (const SetType &)												__NE___;

		template <usize S, typename ...Types>
		ND_ bool  _Serialize (const FixedTupleArray<S,Types...> &)								__NE___;

		template <typename ...Types>
		ND_ bool  _Serialize (const Tuple<Types...> &)											__NE___;

		template <typename ...Types>
		ND_ bool  _Serialize (const Union<Types...> &)											__NE___;

		template <typename T>
		ND_ bool  _Serialize (const Optional<T> &)												__NE___;

		template <typename T, typename ...Args, typename ...Types>
		ND_ bool  _RecursiveSerializeUnion (const Union<Types...> &)							__NE___;

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveSerializeTuple (const Tuple<Types...> &)							__NE___;

		template <usize I, usize S, typename ...Types>
		ND_ bool  _RecursiveSerializeTupleArray (const FixedTupleArray<S, Types...> &)			__NE___;

		template <typename ...Types>
		ND_ bool  _Serialize (const TupleArrayView<Types...> &)									__NE___;

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveSerializeTupleArrayView (const TupleArrayView<Types...> &)			__NE___;

		template <typename V, typename D, typename S>
		ND_ bool  _Serialize (const PhysicalQuantity<V,D,S> &)									__NE___;

		template <typename Qt, int I, glm::qualifier Ql>
		ND_ bool  _Serialize (const PhysicalQuantityVec<Qt,I,Ql> &)								__NE___;

		ND_ bool  _Serialize (const Path &path)													__NE___;
	};

} // AE::Serializing

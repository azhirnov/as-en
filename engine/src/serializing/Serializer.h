// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Common.h"

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
		static constexpr uint	MaxStringLenght	= 0xFFFF;
		static constexpr uint	MaxArrayLenght	= 0xFFFFFF;


	// variables
	public:
		FastWStream			stream;
		Ptr<ObjectFactory>	factory;
		
	private:
		#if AE_DEBUG_SERIALIZER
			uint _dbgCounter = (uint('A') << 16) | (uint('E') << 24);
		#endif


	// methods
	public:
		explicit Serializer (FastWStream wstream) : stream{ RVRef(wstream) } {}
		explicit Serializer (RC<WStream> wstream) : stream{ RVRef(wstream) } {}

		template <typename ...Args>
		bool  operator () (const Args& ...args);

	private:
		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveSerialize (const Arg0 &arg0, const Args& ...args);

		template <typename T>				ND_ bool  _SerializeObj (const T &);
		template <typename T>				ND_ bool  _Serialize (const T &);
		template <typename T>				ND_ bool  _Serialize (const TBytes<T> &);
		template <typename F, typename S>	ND_ bool  _Serialize (const Pair<F,S> &);
		template <usize N>					ND_ bool  _Serialize (const BitSet<N> &);
		template <typename T>				ND_ bool  _Serialize (ArrayView<T>);
		template <typename T, typename A>	ND_ bool  _Serialize (const Array<T,A> &v)				{ return _Serialize(ArrayView<T>{v}); }
		template <typename T, usize S>		ND_ bool  _Serialize (const FixedArray<T,S> &arr)		{ return _Serialize(ArrayView<T>{arr}); }
		template <typename T, usize S>		ND_ bool  _Serialize (const StaticArray<T,S> &arr)		{ return _Serialize(ArrayView<T>{arr}); }
											ND_ bool  _Serialize (StringView);
											ND_ bool  _Serialize (const String &str)				{ return _Serialize(StringView{str}); }
		template <typename T>				ND_ bool  _Serialize (const Rectangle<T> &);
		template <typename T>				ND_ bool  _Serialize (const RGBAColor<T> &);
		template <typename T>				ND_ bool  _Serialize (const HSVColor &);
		template <typename T, usize S>		ND_ bool  _Serialize (const TFixedString<T,S> &str)		{ return _Serialize(StringView{str}); }
		
		template <typename T, uint I, glm::qualifier Q>
		ND_ bool  _Serialize (const TVec<T,I,Q> &);

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Serialize (const NamedID<Size, UID, true, Seed> &);

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Serialize (const NamedID<Size, UID, false, Seed> &);
		
		#ifdef AE_ENABLE_ABSEIL
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Serialize (const FlatHashMap<K,V,H,E,A> &map)	{ return _SerializeMap<K,V>( map ); }
		#endif
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Serialize (const HashMap<K,V,H,E,A> &map)		{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Serialize (const HashMultiMap<K,V,H,E,A> &map)	{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, usize S>								ND_ bool  _Serialize (const FixedMap<K,V,S> &map)			{ return _SerializeMap<K,V>( map ); }
		template <typename K, typename V, typename MapType>						ND_ bool  _SerializeMap (const MapType &);
		
		#ifdef AE_ENABLE_ABSEIL
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Serialize (const FlatHashSet<T,H,E,A> &set)		{ return _SerializeSet<T>( set ); }
		#endif
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Serialize (const HashSet<T,H,E,A> &set)			{ return _SerializeSet<T>( set ); }
		template <typename T, usize S>											ND_ bool  _Serialize (const FixedSet<T,S> &set)				{ return _SerializeSet<T>( set ); }
		template <typename T, typename SetType>									ND_ bool  _SerializeSet (const SetType &);

		template <usize S, typename ...Types>
		ND_ bool  _Serialize (const FixedTupleArray<S,Types...> &);
		
		template <typename ...Types>
		ND_ bool  _Serialize (const Tuple<Types...> &);

		template <typename ...Types>
		ND_ bool  _Serialize (const Union<Types...> &);

		template <typename T>
		ND_ bool  _Serialize (const Optional<T> &);
		
		template <typename T, typename ...Args, typename ...Types>
		ND_ bool  _RecursiveSrializeUnion (const Union<Types...> &);
		
		template <usize I, typename ...Types>
		ND_ bool  _RecursiveSrializeTuple (const Tuple<Types...> &);

		template <usize I, usize S, typename ...Types>
		ND_ bool  _RecursiveSrializeTupleArray (const FixedTupleArray<S, Types...> &);
		
		template <typename V, typename D, typename S>
		ND_ bool  _Serialize (const PhysicalQuantity<V,D,S> &);

		template <typename Qt, int I, glm::qualifier Ql>
		ND_ bool  _Serialize (const PhysicalQuantityVec<Qt,I,Ql> &);
	};

} // AE::Serializing

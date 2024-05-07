// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:  no
*/

#pragma once

#include "serializing/Serializer.h"

namespace AE::Serializing
{

	//
	// Deserializer
	//

	struct Deserializer final : Noncopyable
	{
	// types
	public:
		static constexpr uint	MaxStringLength	= Serializer::MaxStringLength;
		static constexpr uint	MaxArrayLength	= Serializer::MaxArrayLength;

		using BitType = PackedBits< 0, 1, int >::BitType;


	// variables
	public:
		FastRStream					stream;
		Ptr<const ObjectFactory>	factory;		// optional
		Ptr<IAllocator>				allocator;		// optional

	private:
		BitType						_packedBits	= 0;
		uint						_bitCount	= 0;

		#if AE_DEBUG_SERIALIZER
			uint					_dbgCounter = 0;
		#endif


	// methods
	public:
		explicit Deserializer (FastRStream rstream, Ptr<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{alloc} {}
		explicit Deserializer (RC<RStream> rstream, Ptr<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{alloc} {}

			template <typename ...Args>
		ND_ bool  operator () (INOUT Args& ...args)	__NE___;

		ND_ bool  IsEnd ()							C_NE___	{ return stream.Empty(); }

	private:
		ND_ bool  _ReadBits ()																	__NE___;

		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveDeserialize (INOUT Arg0 &arg0, INOUT Args& ...args)					__NE___;

		template <typename T>				ND_ bool  _DeserializeObj (INOUT T &)				__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT T &)					__NE___;
		template <typename F, typename S>	ND_ bool  _Deserialize (INOUT Pair<F,S> &)			__NE___;
		template <usize N>					ND_ bool  _Deserialize (INOUT BitSet<N> &)			__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT BasicString<T> &)		__NE___;
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT TFixedString<T,S> &)	__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT Rectangle<T> &)		__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT RGBAColor<T> &)		__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT HSVColor &)			__NE___;
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT StaticArray<T,S> &)	__NE___;
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT FixedArray<T,S> &)	__NE___;
		template <typename T, typename A>	ND_ bool  _Deserialize (INOUT Array<T,A> &)			__NE___;

		// with allocator
		template <typename T>				ND_ bool  _Deserialize (INOUT ArrayView<T> &)		__NE___;
		template <typename T>				ND_ bool  _Deserialize (INOUT BasicStringView<T> &)	__NE___;

		template <typename T>				ND_ bool  _Deserialize (INOUT NtBasicStringView<T> &);	// not defined
		template <typename T>				ND_ bool  _Deserialize (INOUT StructView<T> &);			// not defined
											ND_ bool  _Deserialize (INOUT AnyTypeRef &);			// not defined
											ND_ bool  _Deserialize (INOUT AnyTypeCRef &);			// not defined

		template <uint F, uint B, typename T>
		ND_ bool  _Deserialize (INOUT PackedBits<F,B,T> &)				__NE___;

		template <typename T, int I, glm::qualifier Q>
		ND_ bool  _Deserialize (INOUT TVec<T,I,Q> &)					__NE___;

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Deserialize (INOUT NamedID<Size, UID, true, Seed> &)	__NE___;

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Deserialize (INOUT NamedID<Size, UID, false, Seed> &)__NE___;

		#ifdef AE_ENABLE_ABSEIL
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Deserialize (INOUT FlatHashMap<K,V,H,E,A> &map)	__NE___	{ return _DeserializeMap<K,V>( INOUT map, UMax ); }
		#endif
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Deserialize (INOUT HashMap<K,V,H,E,A> &map)		__NE___	{ return _DeserializeMap<K,V>( INOUT map, UMax ); }
		template <typename K, typename V, usize S>								ND_ bool  _Deserialize (INOUT FixedMap<K,V,S> &map)			__NE___	{ return _DeserializeMap<K,V>( INOUT map, uint(S) ); }
		template <typename K, typename V, typename MapType>						ND_ bool  _DeserializeMap (INOUT MapType &map, uint maxCount)__NE___;

		#ifdef AE_ENABLE_ABSEIL
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Deserialize (INOUT FlatHashSet<T,H,E,A> &set)	__NE___	{ return _DeserializeSet<T>( INOUT set, UMax ); }
		#endif
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Deserialize (INOUT HashSet<T,H,E,A> &set)		__NE___	{ return _DeserializeSet<T>( INOUT set, UMax ); }
		template <typename T, usize S>											ND_ bool  _Deserialize (INOUT FixedSet<T,S> &set)			__NE___	{ return _DeserializeSet<T>( INOUT set, uint(S) ); }
		template <typename T, typename SetType>									ND_ bool  _DeserializeSet (INOUT SetType &set, uint maxCount)__NE___;

		template <typename T>
		ND_ bool  _Deserialize (INOUT Optional<T> &)					__NE___;

		template <typename ...Types>
		ND_ bool  _Deserialize (INOUT Tuple<Types...> &)				__NE___;

		template <usize S, typename ...Types>
		ND_ bool  _Deserialize (INOUT FixedTupleArray<S,Types...> &)	__NE___;

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveDeserializeTuple (INOUT Tuple<Types...> &)	__NE___;

		template <typename ...Types>
		ND_ bool  _Deserialize (INOUT Union<Types...> &)				__NE___;

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveDeserializeUnion (INOUT Union<Types...> &, usize idx)		__NE___;

		template <usize I, usize S, typename ...Types>
		ND_ bool  _RecursiveDeserializeTupleArray (INOUT FixedTupleArray<S, Types...> &) __NE___;

		template <typename ...Types>
		ND_ bool  _Deserialize (INOUT TupleArrayView<Types...> &)		__NE___;

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveDeserializeTupleArrayView (INOUT TupleArrayView<Types...> &) __NE___;

		template <typename V, typename D, typename S>
		ND_ bool  _Deserialize (INOUT PhysicalQuantity<V,D,S> &)		__NE___;

		template <typename Qt, int I, glm::qualifier Ql>
		ND_ bool  _Deserialize (INOUT PhysicalQuantityVec<Qt,I,Ql> &)	__NE___;

		ND_ bool  _Deserialize (INOUT Path &path)						__NE___;
	};

} // AE::Serializing

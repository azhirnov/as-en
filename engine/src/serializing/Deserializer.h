// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Serializer.h"

namespace AE::Serializing
{

	//
	// Deserializer
	//

	struct Deserializer : Noncopyable
	{
	// types
	public:
		static constexpr uint	MaxStringLenght	= Serializer::MaxStringLenght;
		static constexpr uint	MaxArrayLenght	= Serializer::MaxArrayLenght;


	// variables
	public:
		FastRStream				stream;
		Ptr<ObjectFactory>		factory;
		RC<IAllocator>			allocator;

	private:
		#if AE_DEBUG_SERIALIZER
			uint _dbgCounter = 0; //(uint('A') << 16) | (uint('E') << 24);
		#endif


	// methods
	public:
		explicit Deserializer (FastRStream rstream, RC<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{ RVRef(alloc) } {}
		explicit Deserializer (RC<RStream> rstream, RC<IAllocator> alloc = Default) __NE___ : stream{ RVRef(rstream) }, allocator{ RVRef(alloc) } {}
		
			template <typename ...Args>
		ND_ bool  operator () (INOUT Args& ...args)	__NE___;
		ND_ bool  operator () (INOUT void *)		__NE___;
		
		ND_ bool  IsEnd ()							C_NE___	{ return stream.Empty(); }

	private:
		template <typename Arg0, typename ...Args>
		ND_ bool  _RecursiveDeserialize (INOUT Arg0 &arg0, INOUT Args& ...args);

		template <typename T>				ND_ bool  _DeserializeObj (INOUT T &);
		template <typename T>				ND_ bool  _Deserialize (INOUT T &);
		template <typename T>				ND_ bool  _Deserialize (INOUT TBytes<T> &);
		template <typename F, typename S>	ND_ bool  _Deserialize (INOUT Pair<F,S> &);
		template <usize N>					ND_ bool  _Deserialize (INOUT BitSet<N> &);
											ND_ bool  _Deserialize (INOUT String &);
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT TFixedString<T,S> &);
		template <typename T>				ND_ bool  _Deserialize (INOUT Rectangle<T> &);
		template <typename T>				ND_ bool  _Deserialize (INOUT RGBAColor<T> &);
		template <typename T>				ND_ bool  _Deserialize (INOUT HSVColor &);
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT StaticArray<T,S> &);
		template <typename T, usize S>		ND_ bool  _Deserialize (INOUT FixedArray<T,S> &);
		template <typename T, typename A>	ND_ bool  _Deserialize (INOUT Array<T,A> &);

		// with allocator
		template <typename T>				ND_ bool  _Deserialize (INOUT ArrayView<T> &);
		template <typename T>				ND_ bool  _Deserialize (INOUT BasicStringView<T> &);

		template <typename T>				ND_ bool  _Deserialize (INOUT NtBasicStringView<T> &);	// not defined
		template <typename T>				ND_ bool  _Deserialize (INOUT StructView<T> &);			// not defined
											ND_ bool  _Deserialize (INOUT AnyTypeRef &);			// not defined
											ND_ bool  _Deserialize (INOUT AnyTypeCRef &);			// not defined
		
		template <typename T, uint I, glm::qualifier Q>
		ND_ bool  _Deserialize (INOUT TVec<T,I,Q> &);

		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Deserialize (INOUT NamedID<Size, UID, true, Seed> &);
		
		template <usize Size, uint UID, uint Seed>
		ND_ bool  _Deserialize (INOUT NamedID<Size, UID, false, Seed> &);
		
		#ifdef AE_ENABLE_ABSEIL
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Deserialize (INOUT FlatHashMap<K,V,H,E,A> &map)	{ return _DeserializeMap<K,V>( INOUT map, UMax ); }
		#endif
		template <typename K, typename V, typename H, typename E, typename A>	ND_ bool  _Deserialize (INOUT HashMap<K,V,H,E,A> &map)		{ return _DeserializeMap<K,V>( INOUT map, UMax ); }
		template <typename K, typename V, usize S>								ND_ bool  _Deserialize (INOUT FixedMap<K,V,S> &map)			{ return _DeserializeMap<K,V>( INOUT map, uint(S) ); }
		template <typename K, typename V, typename MapType>						ND_ bool  _DeserializeMap (INOUT MapType &map, uint maxCount);
	
		#ifdef AE_ENABLE_ABSEIL
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Deserialize (INOUT FlatHashSet<T,H,E,A> &set)	{ return _DeserializeSet<T>( INOUT set, UMax ); }
		#endif
		template <typename T, typename H, typename E, typename A>				ND_ bool  _Deserialize (INOUT HashSet<T,H,E,A> &set)		{ return _DeserializeSet<T>( INOUT set, UMax ); }
		template <typename T, usize S>											ND_ bool  _Deserialize (INOUT FixedSet<T,S> &set)			{ return _DeserializeSet<T>( INOUT set, uint(S) ); }
		template <typename T, typename SetType>									ND_ bool  _DeserializeSet (INOUT SetType &set, uint maxCount);

		template <typename T>
		ND_ bool  _Deserialize (INOUT Optional<T> &);
		
		template <typename ...Types>
		ND_ bool  _Deserialize (INOUT Tuple<Types...> &);

		template <usize S, typename ...Types>
		ND_ bool  _Deserialize (INOUT FixedTupleArray<S,Types...> &);

		template <usize I, typename ...Types>
		ND_ bool  _RecursiveDeserializeTuple (INOUT Tuple<Types...> &);

		template <usize I, usize S, typename ...Types>
		ND_ bool  _RecursiveDeserializeTupleArray (INOUT FixedTupleArray<S, Types...> &);
		
		template <typename V, typename D, typename S>
		ND_ bool  _Deserialize (INOUT PhysicalQuantity<V,D,S> &);

		template <typename Qt, int I, glm::qualifier Ql>
		ND_ bool  _Deserialize (INOUT PhysicalQuantityVec<Qt,I,Ql> &);
	};

} // AE::Serializing

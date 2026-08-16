// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	UntypedStorage can be used as static variable.
	For RC object use SharedMem type.
*/

#pragma once

#include "base/CompileTime/TypeList.h"
#include "base/Math/Byte.h"
#include "base/Utils/Helpers.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	//
	// Static Untyped Storage
	//
	template <usize Size_v, usize Align_v>
	struct UntypedStorage final : Noncopyable
	{
	// types
	public:
		using Self = UntypedStorage< Size_v, Align_v >;


	// variables
	private:
		alignas(Align_v) char	_buffer [Size_v];


	// methods
	public:
		template <typename T>
		explicit UntypedStorage (const T &value)				__NE___;

		template <typename T>
		explicit UntypedStorage (TypeList<T>)					__NE___;

		UntypedStorage ()										__NE___	{ DEBUG_ONLY( DbgInitMem( _buffer, Size() )); }
		~UntypedStorage ()										__NE___	{ DEBUG_ONLY( DbgFreeMem( _buffer, Size() )); }

		void  ZeroMem ()										__NE___	{ Base::ZeroMem( _buffer, Size() ); }

		template <typename T>
		ND_ T*			Ptr (Bytes offset = 0_b)				__NE___;

		template <typename T>
		ND_ T const*	Ptr (Bytes offset = 0_b)				C_NE___	{ return const_cast<Self*>(this)->Ptr<T>( offset ); }

		template <typename T>
		ND_ T&			Ref (Bytes offset = 0_b)				__NE___	{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&	Ref (Bytes offset = 0_b)				C_NE___	{ return *Ptr<T>( offset ); }

		NdCe__ static Bytes		Size ()							__NE___	{ return Bytes{Size_v}; }
		NdCe__ static Bytes		Align ()						__NE___	{ return Bytes{Align_v}; }
	};



	//
	// Static Trivial Storage
	//
	template <usize Size_v, usize Align_v>
	struct TrivialStorage
	{
	// types
	public:
		using Self = TrivialStorage< Size_v, Align_v >;


	// variables
	private:
		alignas(Align_v) char	_buffer [Size_v];


	// methods
	public:
		template <typename T>
		explicit TrivialStorage (const T &value)				__NE___;

		template <typename T>
		explicit TrivialStorage (TypeList<T>)					__NE___;

		TrivialStorage (const Self &other)						__NE___;
		Self&  operator = (const Self &rhs)						__NE___;

		TrivialStorage ()										__NE___	{ DEBUG_ONLY( DbgInitMem( _buffer, Size() )); }
		~TrivialStorage ()										__NE___	{ DEBUG_ONLY( DbgFreeMem( _buffer, Size() )); }

		template <typename T>
		ND_ T*			Ptr (Bytes offset = 0_b)				__NE___;

		template <typename T>
		ND_ T const*	Ptr (Bytes offset = 0_b)				C_NE___	{ return const_cast<Self*>(this)->Ptr<T>( offset ); }

		template <typename T>
		ND_ T&			Ref (Bytes offset = 0_b)				__NE___	{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&	Ref (Bytes offset = 0_b)				C_NE___	{ return *Ptr<T>( offset ); }

		ND_ void*		Data ()									__NE___	{ return _buffer; }
		ND_ void const*	Data ()									C_NE___	{ return _buffer; }

		NdCe__ static Bytes		Size ()							__NE___	{ return Bytes{Size_v}; }
		NdCe__ static Bytes		Align ()						__NE___	{ return Bytes{Align_v}; }
	};



	template <typename T, usize Capacity>
	using UninitializedStaticArray = UntypedStorage< sizeof(T) * Capacity, alignof(T) >;

//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	UntypedStorage<S,A>::UntypedStorage (const T &value) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTriviallyDestructible<T> );
		StaticAssert( IsMemCopyAvailable<T> );

		std::memcpy( OUT _buffer, &value, S );
	}

	template <usize S, usize A>
	template <typename T>
	UntypedStorage<S,A>::UntypedStorage (TypeList<T>) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTriviallyDestructible<T> );

		PlacementNew<T>( OUT _buffer );
	}

/*
=================================================
	Ptr
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	T*  UntypedStorage<S,A>::Ptr (Bytes offset) __NE___
	{
		StaticAssert( sizeof(T) <= S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTriviallyDestructible<T> );

		ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
		return Cast<T>( _buffer + offset );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	TrivialStorage<S,A>::TrivialStorage (const T &value) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTrivial<T> );

		std::memcpy( OUT _buffer, &value, S );
	}

	template <usize S, usize A>
	template <typename T>
	TrivialStorage<S,A>::TrivialStorage (TypeList<T>) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTrivial<T> );

		PlacementNew<T>( OUT _buffer );
	}

/*
=================================================
	Ptr
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	T*  TrivialStorage<S,A>::Ptr (Bytes offset) __NE___
	{
		StaticAssert( sizeof(T) <= S );
		StaticAssert( alignof(T) <= A );
		StaticAssert( IsTrivial<T> );

		ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
		return Cast<T>( _buffer + offset );
	}

/*
=================================================
	Copy-ctor
=================================================
*/
	template <usize S, usize A>
	TrivialStorage<S,A>::TrivialStorage (const Self &other) __NE___
	{
		std::memcpy( OUT _buffer, other._buffer, sizeof(_buffer) );
	}

	template <usize S, usize A>
	TrivialStorage<S,A>&  TrivialStorage<S,A>::operator = (const TrivialStorage<S,A> &rhs) __NE___
	{
		std::memcpy( OUT _buffer, rhs._buffer, sizeof(_buffer) );
		return *this;
	}


} // AE::Base

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  no

	Message and chunk with message must use the same allocator,
	so we need to use shared_lock to allow allocations from multiple thread,
	but only one thread can reset allocator (see 'Produce()' method).
*/

#pragma once

#include "networking/HighLevel/MessageFactory.h"

namespace AE::Networking::_hidden_
{
	using MsgAndSync_SpinLock_t = Threading::TRWSpinLock< false, true >;


	//
	// Message with Extra data
	//
	template <typename T>
	struct MsgWithExtra : MovableOnly
	{
	// variables
	protected:
		T *				_msg	= null;
		DEBUG_ONLY(
			Bytes		_extraSize;)


	// methods
	public:
		MsgWithExtra ()												__NE___ {}
		MsgWithExtra (T* msg, Bytes extraSize)						__NE___	: _msg{msg} DEBUG_ONLY(, _extraSize{extraSize}) { Unused(extraSize); }
		MsgWithExtra (MsgWithExtra &&other)							__NE___	: _msg{other._msg} DEBUG_ONLY(, _extraSize{other.extraSize}) { other._msg = null; }

		MsgWithExtra&  operator = (MsgWithExtra &&rhs)				__NE___;

		ND_ T*  operator -> ()										__NE___	{ ASSERT( _msg != null );  return _msg; }

		ND_ explicit operator bool ()								C_NE___	{ return _msg != null; }
		ND_ explicit operator CSMessagePtr ()						C_NE___	{ return CSMessagePtr{ _msg }; }


		template <typename E>
		ND_ E*	Extra ()											__NE___;

		template <typename E>
		E*		PutExtra (const E* data, usize count)				__NE___;
		void*	PutExtra (const void* data, Bytes dataSize)			__NE___;

		template <typename E>
		BasicStringView<E>	PutExtra (BasicStringView<E> str)		__NE___	{ return BasicStringView<E>{ PutExtra( str.data(), str.size() ), str.size() }; }

		template <typename E>
		ArrayView<E>		PutExtra (ArrayView<E> arr)				__NE___	{ return ArrayView<E>{ PutExtra( arr.data(), arr.size() ), arr.size() }; }


		template <typename M>
		void	Put (M T::*, const void* src, Bytes size)			__NE___;

		template <typename M, typename E>
		void	Put (M T::*m, BasicStringView<E> str)				__NE___;

		template <typename M, typename E>
		void	Put (M T::*m, ArrayView<E> arr)						__NE___;

		template <typename M>
		void	Extract (M T::*, OUT void* dst, Bytes size)			C_NE___;

		template <typename E, typename M>
		ND_ ArrayView<E>  ExtractArray (M T::*, usize count)		C_NE___;
	};



	//
	// Message And Sync Object
	//
	template <typename T>
	struct MsgAndSync : MsgWithExtra<T>
	{
	// types
	private:
		using Base_t	= MsgWithExtra<T>;
		using Guard_t	= MsgAndSync_SpinLock_t;


	// variables
	private:
		Guard_t *		_guard	= null;


	// methods
	public:
		MsgAndSync ()												__NE___ {}
		MsgAndSync (T* msg, Guard_t &guard, Bytes extraSize)		__NE___	: Base_t{msg, extraSize}, _guard{&guard} {}
		MsgAndSync (MsgAndSync &&other)								__NE___	: Base_t{RVRef(other)}, _guard{other._guard} { other._guard = null; }
		~MsgAndSync ()												__NE___	{ if_unlikely( _guard != null ) _guard->unlock_shared(); }

		MsgAndSync&  operator = (MsgAndSync &&rhs)					__NE___;

		void	_Unlock ()											__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	MsgWithExtra<T>&  MsgWithExtra<T>::operator = (MsgWithExtra &&rhs) __NE___
	{
		_msg = rhs._msg;
		DEBUG_ONLY( _extraSize = rhs._extraSize;)
		return *this;
	}

/*
=================================================
	Extra
=================================================
*/
	template <typename T>
	template <typename E>
	E*  MsgWithExtra<T>::Extra () __NE___
	{
		ASSERT( _msg != null );

		if constexpr( alignof(E) <= alignof(T) )
			return Cast<E>( _msg + SizeOf<T> );
		else
			return Cast<E>( AlignUp( _msg, AlignOf<E> ) + SizeOf<T> );
	}

/*
=================================================
	PutExtra
=================================================
*/
	template <typename T>
	template <typename E>
	E*  MsgWithExtra<T>::PutExtra (const E* data, const usize count) __NE___
	{
		ASSERT( _msg != null );

		const Bytes  data_size = SizeOf<E> * count;
		ASSERT( data_size <= _extraSize );

		E*	dst = Extra<E>();
		ASSERT_MSG( dst + data_size <= (Cast<void>(_msg) + SizeOf<T> + _extraSize),
				    "'extraSize' must include alignment to dst type" );

		MemCopy( OUT dst, data, data_size );
		return dst;
	}

	template <typename T>
	void*  MsgWithExtra<T>::PutExtra (const void* data, const Bytes dataSize) __NE___
	{
		ASSERT( _msg != null );
		ASSERT( dataSize <= _extraSize );

		void*	dst = Extra<char>();
		MemCopy( OUT dst, data, dataSize );

		return dst;
	}

/*
=================================================
	Put
----
	'T  data []' is not in C++ standard, so we use
	'T  data [1]' to get correct alignment, actual array size may be greater.
----
	'Put' method used to copy data to the message extra storage.
=================================================
*/
	template <typename T>
	template <typename M>
	void  MsgWithExtra<T>::Put (M T::*member, const void* srcData, const Bytes srcDataSize) __NE___
	{
		ASSERT( _msg != null );

		void*	dst = &(_msg->*member);
		ASSERT( dst + srcDataSize <= (Cast<void>(_msg) + SizeOf<T> + _extraSize) );

		MemCopy( OUT dst, srcData, srcDataSize );
	}

	template <typename T>
	template <typename M, typename E>
	void  MsgWithExtra<T>::Put (M T::*m, BasicStringView<E> str) __NE___
	{
		StaticAssert( IsSameTypes< RemoveArray<M>, E >);
		Put( m, str.data(), StringSizeOf(str) );
	}

	template <typename T>
	template <typename M, typename E>
	void  MsgWithExtra<T>::Put (M T::*m, ArrayView<E> arr) __NE___
	{
		StaticAssert( IsSameTypes< RemoveArray<M>, E >);
		Put( m, arr.data(), ArraySizeOf(arr) );
	}

/*
=================================================
	Extract
----
	Used to copy data from message extra storage.
=================================================
*/
	template <typename T>
	template <typename M>
	void  MsgWithExtra<T>::Extract (M T::*member, OUT void* dst, const Bytes dstDataSize) C_NE___
	{
		ASSERT( _msg != null );

		void*	src = &(_msg->*member);
		ASSERT( src + dstDataSize <= (Cast<void>(_msg) + SizeOf<T> + _extraSize) );

		MemCopy( OUT dst, src, dstDataSize );
	}

	template <typename T>
	template <typename E, typename M>
	ArrayView<E>  MsgWithExtra<T>::ExtractArray (M T::*member, const usize count) C_NE___
	{
		StaticAssert( IsSameTypes< RemoveArray<M>, E >);

		E const*	src = &(_msg->*member);
		ASSERT( src + SizeOf<E>*count <= (Cast<void>(_msg) + SizeOf<T> + _extraSize) );

		return ArrayView<E>{ src, count };
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	MsgAndSync<T>&  MsgAndSync<T>::operator = (MsgAndSync &&rhs) __NE___
	{
		ASSERT( _guard == null );

		this->_msg		= rhs._msg;
		this->_guard	= rhs._guard;

		DEBUG_ONLY( this->_extraSize = rhs._extraSize;)

		rhs._guard	= null;
		return *this;
	}

/*
=================================================
	_Unlock
----
	used only in AsyncCSMessageProducer
=================================================
*/
	template <typename T>
	void  MsgAndSync<T>::_Unlock () __NE___
	{
		_guard->unlock_shared();

		this->_msg		= null;
		this->_guard	= null;
	}


} // AE::Networking::_hidden_

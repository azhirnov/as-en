// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
// TODO:
//	add atomic_signed_lock_free, atomic_unsigned_lock_free

#pragma once

#include "base/Math/Bytes.h"
#include "base/Utils/Threading.h"
#include "base/Platforms/ThreadUtils.h"

#ifndef AE_LFAS_ENABLED
# include "threading/Common.h"
#endif

namespace AE::Threading
{
	using AE::Base::ThreadUtils;

#  ifdef AE_LFAS_ENABLED
	template <typename T>
	using StdAtomic = LFAS::CPP::StdAtomic< T >;
	using LFAS::CPP::EMemoryOrder;
#  else
	template <typename T>
	using StdAtomic = std::atomic< T >;
#  endif


	//
	// Atomic
	//

	template <typename PublicType,
			  typename InternalType,
			  std::memory_order Success,
			  std::memory_order Failure
			 >
	struct TAtomic
	{
	// types
	public:
		using value_type	= PublicType;

	private:
		using Self	= TAtomic< PublicType, InternalType, Success, Failure >;
		using MO_t	= std::memory_order;
		using IT	= InternalType;
		using T		= PublicType;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		STATIC_ASSERT( StdAtomic<IT>::is_always_lock_free );
		STATIC_ASSERT( IsInteger<IT> or IsEnum<IT> );
		STATIC_ASSERT( sizeof(PublicType) == sizeof(InternalType) );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TAtomic ()										__NE___ {}
		explicit TAtomic (T value)						__NE___ : _value{ IT{value} } {}

		TAtomic (const Self &)							= delete;
		TAtomic (Self &&)								= delete;

			Self&	operator = (const Self &)			= delete;
			Self&	operator = (Self &&)				= delete;

			void	store (T desired)					__NE___ { _value.store( IT{desired}, OnSuccess ); }
		ND_ T		load ()								C_NE___ { return T{_value.load( OnSuccess )}; }
		ND_ T		exchange (T desired)				__NE___ { return T{_value.exchange( IT{desired}, OnSuccess )}; }
		
			void	store (T desired, MO_t memOrder)	__NE___ { _value.store( IT{desired}, memOrder ); }
		ND_ T		load (MO_t memOrder)				C_NE___ { return T{_value.load( memOrder )}; }
		ND_ T		exchange (T desired, MO_t memOrder)	__NE___ { return T{_value.exchange( IT{desired}, memOrder )}; }

		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired)				__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired)				__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		
		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, success, failure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, success, failure ); }
		
		ND_ bool	CAS (INOUT T& expected, T desired)									__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired, MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, success, failure ); }
		
		ND_ bool	CAS_Loop (INOUT T& expected, T desired)								__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, success, failure ); }

		T	fetch_add (T arg)					__NE___ { return T{_value.fetch_add( IT{arg}, OnSuccess )}; }
		T	fetch_sub (T arg)					__NE___ { return T{_value.fetch_sub( IT{arg}, OnSuccess )}; }
		T	fetch_and (T arg)					__NE___ { return T{_value.fetch_and( IT{arg}, OnSuccess )}; }
		T	fetch_or (T arg)					__NE___ { return T{_value.fetch_or(  IT{arg}, OnSuccess )}; }
		T	fetch_xor (T arg)					__NE___ { return T{_value.fetch_xor( IT{arg}, OnSuccess )}; }
		
		T	Add (T arg)							__NE___ { return Add( arg, OnSuccess ); }
		T	Sub (T arg)							__NE___ { return Sub( arg, OnSuccess ); }
		T	And (T arg)							__NE___ { return And( arg, OnSuccess ); }
		T	Or  (T arg)							__NE___ { return Or(  arg, OnSuccess ); }
		T	Xor (T arg)							__NE___ { return Xor( arg, OnSuccess ); }
		T	Inc ()								__NE___ { return Add( T{1}, OnSuccess ); }
		T	Dec ()								__NE___ { return Sub( T{1}, OnSuccess ); }

		T	fetch_add (T arg, MO_t memOrder)	__NE___ { return T{_value.fetch_add( IT{arg}, memOrder )}; }
		T	fetch_sub (T arg, MO_t memOrder)	__NE___ { return T{_value.fetch_sub( IT{arg}, memOrder )}; }
		T	fetch_and (T arg, MO_t memOrder)	__NE___ { return T{_value.fetch_and( IT{arg}, memOrder )}; }
		T	fetch_or  (T arg, MO_t memOrder)	__NE___ { return T{_value.fetch_or(  IT{arg}, memOrder )}; }
		T	fetch_xor (T arg, MO_t memOrder)	__NE___ { return T{_value.fetch_xor( IT{arg}, memOrder )}; }
		
		T	Add (T arg, MO_t memOrder)			__NE___ { return fetch_add( arg, memOrder ) + arg; }
		T	Sub (T arg, MO_t memOrder)			__NE___ { return fetch_sub( arg, memOrder ) - arg; }
		T	And (T arg, MO_t memOrder)			__NE___ { return fetch_and( arg, memOrder ) & arg; }
		T	Or  (T arg, MO_t memOrder)			__NE___ { return fetch_or(  arg, memOrder ) | arg; }
		T	Xor (T arg, MO_t memOrder)			__NE___ { return fetch_xor( arg, memOrder ) ^ arg; }
		T	Inc (MO_t memOrder)					__NE___ { return Add( T{1}, memOrder ); }
		T	Dec (MO_t memOrder)					__NE___ { return Sub( T{1}, memOrder ); }

		Self&  operator += (T arg)				__NE___ { fetch_add( arg );  return *this; }
		Self&  operator -= (T arg)				__NE___ { fetch_sub( arg );  return *this; }
		Self&  operator &= (T arg)				__NE___ { fetch_and( arg );  return *this; }
		Self&  operator |= (T arg)				__NE___ { fetch_or(  arg );  return *this; }
		Self&  operator ^= (T arg)				__NE___ { fetch_xor( arg );  return *this; }
		Self&  operator ++ ()					__NE___ { fetch_add( T{1} ); return *this; }
		Self&  operator -- ()					__NE___ { fetch_sub( T{1} ); return *this; }

		T  Max (T arg)							__NE___
		{
			T	exp = load();
			for (; (exp <= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T  Min (T arg)							__NE___
		{
			T	exp = load();
			for (; (exp >= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		template <typename From>
		ND_ static forceinline InternalType&  _Ref (From &src) __NE___
		{
			if constexpr( IsSameTypes<InternalType, From> )
				return src;
			else
				return reinterpret_cast< InternalType &>( src );
		}
	};

	

	//
	// Bitfield Atomic
	//

	template <typename T, std::memory_order Success, std::memory_order Failure>
	struct TBitAtomic
	{
	// types
	public:
		using value_type	= T;
		using uint_type		= Base::ByteSizeToUInt< sizeof(T) >;

	private:
		using Self	= TBitAtomic< T, Success, Failure >;
		using MO_t	= std::memory_order;
		using IT	= uint_type;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		STATIC_ASSERT( StdAtomic<IT>::is_always_lock_free );
		STATIC_ASSERT( IsUnion<T> or IsClass<T> );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TBitAtomic ()									__NE___ {}
		explicit TBitAtomic (T value)					__NE___ : _value{ _Cast(value) } {}

		TBitAtomic (const Self &)						= delete;
		TBitAtomic (Self &&)							= delete;

		Self&  operator = (const Self &)				= delete;
		Self&  operator = (Self &&)						= delete;

			void	store (T desired)					__NE___ { _value.store( _Cast(desired), OnSuccess ); }
		ND_ T		load ()								C_NE___ { return _Cast( _value.load( OnSuccess )); }
		ND_ T		exchange (T desired)				__NE___ { return _Cast( _value.exchange( _Cast(desired), OnSuccess )); }
		
			void	store (T desired, MO_t memOrder)	__NE___ { _value.store( _Cast(desired), memOrder ); }
		ND_ T		load (MO_t memOrder)				C_NE___ { return _Cast( _value.load( memOrder )); }
		ND_ T		exchange (T desired, MO_t memOrder)	__NE___ { return _Cast( _value.exchange( _Cast(desired), memOrder )); }

		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired)	__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		
		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		
		ND_ bool	CAS (INOUT T& expected, T desired)									__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired, MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		
		ND_ bool	CAS_Loop (INOUT T& expected, T desired)								__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		T	fetch_and (IT arg)						__NE___ { return _Cast( _value.fetch_and( arg, OnSuccess )); }
		T	fetch_or  (IT arg)						__NE___ { return _Cast( _value.fetch_or(  arg, OnSuccess )); }
		T	fetch_xor (IT arg)						__NE___ { return _Cast( _value.fetch_xor( arg, OnSuccess )); }
		
		T	And (IT arg)							__NE___ { return And( arg, OnSuccess ); }
		T	Or  (IT arg)							__NE___ { return Or(  arg, OnSuccess ); }
		T	Xor (IT arg)							__NE___ { return Xor( arg, OnSuccess ); }

		T	fetch_and (IT arg, MO_t memOrder)		__NE___ { return _Cast( _value.fetch_and( arg, memOrder )); }
		T	fetch_or  (IT arg, MO_t memOrder)		__NE___ { return _Cast( _value.fetch_or(  arg, memOrder )); }
		T	fetch_xor (IT arg, MO_t memOrder)		__NE___ { return _Cast( _value.fetch_xor( arg, memOrder )); }
		
		T	And (IT arg, MO_t memOrder)				__NE___ { return fetch_and( arg, memOrder ) & arg; }
		T	Or  (IT arg, MO_t memOrder)				__NE___ { return fetch_or(  arg, memOrder ) | arg; }
		T	Xor (IT arg, MO_t memOrder)				__NE___ { return fetch_xor( arg, memOrder ) ^ arg; }
		
		T	SetBit (usize bit)						__NE___ { return fetch_or( IT{1} << bit ); }
		T	ResetBit (usize bit)					__NE___ { return fetch_and( ~(IT{1} << bit) ); }

		T	SetBit (usize bit, MO_t memOrder)		__NE___ { return fetch_or( IT{1} << bit, memOrder ); }
		T	ResetBit (usize bit, MO_t memOrder)		__NE___ { return fetch_and( ~(IT{1} << bit), memOrder ); }
		
		T  Max (T arg)								__NE___
		{
			T	exp = load();
			for (; (exp <= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T  Min (T arg)								__NE___
		{
			T	exp = load();
			for (; (exp >= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		ND_ static forceinline IT	_Cast (const T &value)		__NE___ { return BitCast<IT>( value ); }
		ND_ static forceinline T	_Cast (const IT &value)		__NE___ { return BitCast<T>( value ); }

		ND_ static forceinline T&	_Ref (IT &value)			__NE___ { return reinterpret_cast< T &>( value ); }
		ND_ static forceinline IT&	_Ref (T &value)				__NE___ { return reinterpret_cast< IT &>( value ); }
	};



	//
	// Atomic for float point
	//
	
	template <typename T, std::memory_order Success, std::memory_order Failure>
	struct TAtomicFloat
	{
	// types
	public:
		using value_type	= T;
		
	private:
		using Self	= TAtomicFloat< T, Success, Failure >;
		using MO_t	= std::memory_order;
		using IT	= Base::ByteSizeToUInt< sizeof(T) >;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		STATIC_ASSERT( StdAtomic<T>::is_always_lock_free );
		STATIC_ASSERT( IsFloatPoint<T> );
		STATIC_ASSERT( sizeof(IT) == sizeof(T) );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TAtomicFloat ()									__NE___ {}
		explicit TAtomicFloat (T value)					__NE___ : _value{ _Cast(value) } {}

		TAtomicFloat (const Self &)						= delete;
		TAtomicFloat (Self &&)							= delete;

		Self&  operator = (const Self &)				= delete;
		Self&  operator = (Self &&)						= delete;

			void	store (T desired)					__NE___ { _value.store( _Cast(desired), OnSuccess ); }
		ND_ T		load ()								C_NE___ { return _Cast( _value.load( OnSuccess )); }
		ND_ T		exchange (T desired)				__NE___ { return _Cast( _value.exchange( _Cast(desired), OnSuccess )); }
		
			void	store (T desired, MO_t memOrder)	__NE___ { _value.store( _Cast(desired), memOrder ); }
		ND_ T		load (MO_t memOrder)				C_NE___ { return _Cast( _value.load( memOrder )); }
		ND_ T		exchange (T desired, MO_t memOrder)	__NE___ { return _Cast( _value.exchange( _Cast(desired), memOrder )); }

		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired)	__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		
		ND_ bool	compare_exchange_weak (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		ND_ bool	compare_exchange_strong (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		
		ND_ bool	CAS (INOUT T& expected, T desired)									__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired, MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }
		
		ND_ bool	CAS_Loop (INOUT T& expected, T desired)								__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		T	fetch_add (T arg)					__NE___ { return fetch_add( arg, OnSuccess ); }
		T	fetch_sub (T arg)					__NE___ { return fetch_sub( arg, OnSuccess ); }
		
		T	Add (T arg)							__NE___ { return Add( arg, OnSuccess ); }
		T	Sub (T arg)							__NE___ { return Sub( arg, OnSuccess ); }
		T	Inc ()								__NE___ { return Add( T{1}, OnSuccess ); }
		T	Dec ()								__NE___ { return Sub( T{1}, OnSuccess ); }
		
		T	fetch_add (T arg, MO_t memOrder)	__NE___
		{
			T	exp	= load();
			for (; not CAS( INOUT exp, exp + arg, memOrder, OnFailure );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	fetch_sub (T arg, MO_t memOrder)	__NE___
		{
			T	exp	= load();
			for (; not CAS( INOUT exp, exp - arg, memOrder, OnFailure );) { ThreadUtils::Pause(); }
			return exp;
		}
		
		T	Add (T arg, MO_t memOrder)			__NE___ { return fetch_add( arg, memOrder ) + arg; }
		T	Sub (T arg, MO_t memOrder)			__NE___ { return fetch_sub( arg, memOrder ) - arg; }
		T	Inc (MO_t memOrder)					__NE___ { return Add( T{1}, memOrder ); }
		T	Dec (MO_t memOrder)					__NE___ { return Sub( T{1}, memOrder ); }

		Self&  operator += (T arg)				__NE___ { fetch_add( arg );  return *this; }
		Self&  operator -= (T arg)				__NE___ { fetch_sub( arg );  return *this; }
		Self&  operator ++ ()					__NE___ { fetch_add( T{1} ); return *this; }
		Self&  operator -- ()					__NE___ { fetch_sub( T{1} ); return *this; }

		T  Max (T arg)							__NE___
		{
			T	exp = load();
			for (; (exp <= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T  Min (T arg)							__NE___
		{
			T	exp = load();
			for (; (exp >= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		ND_ static forceinline IT	_Cast (const T &value)	__NE___ { return BitCast<IT>( value ); }
		ND_ static forceinline T	_Cast (const IT &value)	__NE___ { return BitCast<T>( value ); }
		ND_ static forceinline IT&	_Ref (T &value)			__NE___ { return reinterpret_cast<IT &>( value ); }
	};


	
	//
	// Atomic for pointer
	//

	template <typename T, std::memory_order Success, std::memory_order Failure>
	struct TAtomic< T*, T*, Success, Failure >
	{
	// types
	public:
		using value_type	= T *;

	private:
		using Self	= TAtomic< value_type, value_type, Success, Failure >;
		using MO_t	= std::memory_order;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		STATIC_ASSERT( StdAtomic<value_type>::is_always_lock_free );


	// variables
	private:
		StdAtomic< value_type >		_value;


	// methods
	public:
		TAtomic ()										__NE___ {}
		explicit TAtomic (value_type value)				__NE___ : _value{ value } {}

		TAtomic (const Self &)							= delete;
		TAtomic (Self &&)								= delete;

		Self&  operator = (const Self &)				= delete;
		Self&  operator = (Self &&)						= delete;

			void		store (value_type desired)		__NE___ { _value.store( desired, OnSuccess ); }
		ND_ value_type	load ()							C_NE___ { return _value.load( OnSuccess ); }
		ND_ value_type	exchange (value_type desired)	__NE___ { return _value.exchange( desired, OnSuccess ); }
		
			void		store (value_type desired, MO_t memOrder)		__NE___ { _value.store( desired, memOrder ); }
		ND_ value_type	load (MO_t memOrder)							C_NE___ { return _value.load( memOrder ); }
		ND_ value_type	exchange (value_type desired, MO_t memOrder)	__NE___ { return _value.exchange( desired, memOrder ); }

		ND_ bool	compare_exchange_weak (INOUT value_type& expected, value_type desired)		__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, OnSuccess, OnFailure ); }
		ND_ bool	compare_exchange_strong (INOUT value_type& expected, value_type desired)	__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, OnSuccess, OnFailure ); }
		
		ND_ bool	compare_exchange_weak (INOUT value_type& expected, value_type desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, success, failure ); }
		ND_ bool	compare_exchange_strong (INOUT value_type& expected, value_type desired, MO_t success, MO_t failure)__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, success, failure ); }
		
		ND_ bool	CAS (INOUT value_type& expected, value_type desired)									__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT value_type& expected, value_type desired, MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, success, failure ); }
		
		ND_ bool	CAS_loop (INOUT value_type& expected, value_type desired)								__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, OnSuccess, OnFailure ); }
		ND_ bool	CAS_loop (INOUT value_type& expected, value_type desired, MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, success, failure ); }

		value_type	fetch_add (ssize arg)					__NE___ { return _value.fetch_add( arg, OnSuccess ); }
		value_type	fetch_sub (ssize arg)					__NE___ { return _value.fetch_sub( arg, OnSuccess ); }
		value_type	Add (ssize arg)							__NE___ { return _value.Add( arg, OnSuccess ); }
		value_type	Sub (ssize arg)							__NE___ { return _value.Sub( arg, OnSuccess ); }
		
		value_type	fetch_add (ssize arg, MO_t memOrder)	__NE___ { return _value.fetch_add( arg, memOrder ); }
		value_type	fetch_sub (ssize arg, MO_t memOrder)	__NE___ { return _value.fetch_sub( arg, memOrder ); }
		value_type	Add (ssize arg, MO_t memOrder)			__NE___ { return _value.fetch_add( arg, memOrder ) + Math::BytesSSize{arg}; }
		value_type	Sub (ssize arg, MO_t memOrder)			__NE___ { return _value.fetch_sub( arg, memOrder ) - Math::BytesSSize{arg}; }
		
		value_type  Max (value_type arg)					__NE___
		{
			value_type	exp = load();
			for (; (exp <= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		value_type  Min (value_type arg)					__NE___
		{
			value_type	exp = load();
			for (; (exp >= arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}
	};


	template <typename T>
	using Atomic = TAtomic< T, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
	
	template <typename T>
	using FAtomic = TAtomicFloat< T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;

	template <typename T>
	using BitAtomic = TBitAtomic< T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;


	namespace _hidden_
	{
		template <typename T>
		struct TBytesAtomic {
			using type = TAtomic< Math::TBytes<T>, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
		};
		
		template <typename T>
		struct TBytesAtomic< Math::TBytes<T> > {
			using type = TAtomic< Math::TBytes<T>, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
		};
	}
	
	template <typename T>
	using BytesAtomic = typename Threading::_hidden_::TBytesAtomic<T>::type;


} // AE::Threading

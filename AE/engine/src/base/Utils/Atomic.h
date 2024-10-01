// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	CAS
	CAS_loop
		Atomically compares the value representation of *this with that of expected.
		If those are bitwise-equal, replaces the former with desired (performs read-modify-write operation).
		Otherwise, loads the actual value stored in *this into expected (performs load operation).
		'CAS' is allowed to fail spuriously, that is, acts as if *this != expected even if they are equal.
		When a compare-and-exchange is in a loop, 'CAS' will yield better performance on some platforms.
		(from C++ specs)

	fetch_min
		Set minimal value of *this and argument.
		Returns previous value of *this.

	fetch_max
		Set maximal value of *this and argument.
		Returns previous value of *this.
*/
#pragma once

#include "base/Utils/Threading.h"
#include "base/Platforms/ThreadUtils.h"

namespace AE::Base
{
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

		StaticAssert( StdAtomic<IT>::is_always_lock_free );
		StaticAssert( IsInteger<IT> or IsEnum<IT> );
		StaticAssert( sizeof(PublicType) == sizeof(InternalType) );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TAtomic ()												__NE___ DEBUG_ONLY(: _value{ IT(BitCast<usize>(&_value)) }) {}
		explicit TAtomic (T value)								__NE___ : _value{ IT{value} } {}
		explicit TAtomic (Default_t)							__NE___ : _value{ IT{Default} } {}

		TAtomic (const Self &)									= delete;
		TAtomic (Self &&)										= delete;

			Self&	operator = (const Self &)					= delete;
			Self&	operator = (Self &&)						= delete;

			void	store (T desired)							__NE___ { _value.store( IT{desired}, OnSuccess ); }
		ND_ T		load ()										C_NE___ { return T{_value.load( OnSuccess )}; }
		ND_ T		exchange (T desired)						__NE___ { return T{_value.exchange( IT{desired}, OnSuccess )}; }

			void	store (T desired, MO_t memOrder)			__NE___ { _value.store( IT{desired}, memOrder ); }
		ND_ T		load (MO_t memOrder)						C_NE___ { return T{_value.load( memOrder )}; }
		ND_ T		exchange (T desired, MO_t memOrder)			__NE___ { return T{_value.exchange( IT{desired}, memOrder )}; }

		ND_ bool	CAS (INOUT T& expected, T desired)			__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired,
						 MO_t success, MO_t failure)			__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), IT{desired}, success, failure ); }

		ND_ bool	CAS_Loop (INOUT T& expected, T desired)		__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired,
							  MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), IT{desired}, success, failure ); }

		T	fetch_add (T arg)									__NE___ { return T{_value.fetch_add( IT{arg}, OnSuccess )}; }
		T	fetch_sub (T arg)									__NE___ { return T{_value.fetch_sub( IT{arg}, OnSuccess )}; }
		T	fetch_and (T arg)									__NE___ { return T{_value.fetch_and( IT{arg}, OnSuccess )}; }
		T	fetch_or (T arg)									__NE___ { return T{_value.fetch_or(  IT{arg}, OnSuccess )}; }
		T	fetch_xor (T arg)									__NE___ { return T{_value.fetch_xor( IT{arg}, OnSuccess )}; }

		T	Add (T arg)											__NE___ { return Add( arg, OnSuccess ); }
		T	Sub (T arg)											__NE___ { return Sub( arg, OnSuccess ); }
		T	And (T arg)											__NE___ { return And( arg, OnSuccess ); }
		T	Or  (T arg)											__NE___ { return Or(  arg, OnSuccess ); }
		T	Xor (T arg)											__NE___ { return Xor( arg, OnSuccess ); }
		T	Inc ()												__NE___ { return Add( T{1}, OnSuccess ); }
		T	Dec ()												__NE___ { return Sub( T{1}, OnSuccess ); }

		T	fetch_add (T arg, MO_t memOrder)					__NE___ { return T{_value.fetch_add( IT{arg}, memOrder )}; }
		T	fetch_sub (T arg, MO_t memOrder)					__NE___ { return T{_value.fetch_sub( IT{arg}, memOrder )}; }
		T	fetch_and (T arg, MO_t memOrder)					__NE___ { return T{_value.fetch_and( IT{arg}, memOrder )}; }
		T	fetch_or  (T arg, MO_t memOrder)					__NE___ { return T{_value.fetch_or(  IT{arg}, memOrder )}; }
		T	fetch_xor (T arg, MO_t memOrder)					__NE___ { return T{_value.fetch_xor( IT{arg}, memOrder )}; }

		T	Add (T arg, MO_t memOrder)							__NE___ { return fetch_add( arg, memOrder ) + arg; }
		T	Sub (T arg, MO_t memOrder)							__NE___ { return fetch_sub( arg, memOrder ) - arg; }
		T	And (T arg, MO_t memOrder)							__NE___ { return fetch_and( arg, memOrder ) & arg; }
		T	Or  (T arg, MO_t memOrder)							__NE___ { return fetch_or(  arg, memOrder ) | arg; }
		T	Xor (T arg, MO_t memOrder)							__NE___ { return fetch_xor( arg, memOrder ) ^ arg; }
		T	Inc (MO_t memOrder)									__NE___ { return Add( T{1}, memOrder ); }
		T	Dec (MO_t memOrder)									__NE___ { return Sub( T{1}, memOrder ); }

		Self&  operator += (T arg)								__NE___ { fetch_add( arg );  return *this; }
		Self&  operator -= (T arg)								__NE___ { fetch_sub( arg );  return *this; }
		Self&  operator &= (T arg)								__NE___ { fetch_and( arg );  return *this; }
		Self&  operator |= (T arg)								__NE___ { fetch_or(  arg );  return *this; }
		Self&  operator ^= (T arg)								__NE___ { fetch_xor( arg );  return *this; }
		Self&  operator ++ ()									__NE___ { fetch_add( T{1} ); return *this; }
		Self&  operator -- ()									__NE___ { fetch_sub( T{1} ); return *this; }

		T	fetch_max (T arg)									__NE___
		{
			T	exp = load();
			for (; (exp < arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	fetch_min (T arg)									__NE___
		{
			T	exp = load();
			for (; (exp > arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		template <typename From>
		Nd__IF static InternalType&  _Ref (From &src)			__NE___
		{
			if constexpr( IsSameTypes< InternalType, From >)
				return src;
			else
				return reinterpret_cast< InternalType &>( src );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Struct Atomic
	//

	template <typename T, std::memory_order Success, std::memory_order Failure>
	struct TStructAtomic
	{
	// types
	public:
		using value_type	= T;
		using uint_type		= ByteSizeToUInt< sizeof(T) >;

	private:
		using Self	= TStructAtomic< T, Success, Failure >;
		using MO_t	= std::memory_order;
		using IT	= uint_type;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		StaticAssert( StdAtomic<IT>::is_always_lock_free );
		StaticAssert( IsUnion<T> or IsClass<T> );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TStructAtomic ()												__NE___ DEBUG_ONLY(: _value{ IT(BitCast<usize>(&_value)) }) {}
		explicit TStructAtomic (T value)								__NE___ : _value{ _Cast( value ) } {}
		explicit TStructAtomic (Default_t)								__NE___ : _value{ _Cast( IT{Default} ) } {}

		TStructAtomic (const Self &)									= delete;
		TStructAtomic (Self &&)											= delete;

			Self&	operator = (const Self &)							= delete;
			Self&	operator = (Self &&)								= delete;

			void	store (T desired)									__NE___ { _value.store( _Cast(desired), OnSuccess ); }
		ND_ T		load ()												C_NE___ { return _Cast( _value.load( OnSuccess )); }
		ND_ T		exchange (T desired)								__NE___ { return _Cast( _value.exchange( _Cast(desired), OnSuccess )); }

			void	store (T desired, MO_t memOrder)					__NE___ { _value.store( _Cast(desired), memOrder ); }
		ND_ T		load (MO_t memOrder)								C_NE___ { return _Cast( _value.load( memOrder )); }
		ND_ T		exchange (T desired, MO_t memOrder)					__NE___ { return _Cast( _value.exchange( _Cast(desired), memOrder )); }

		ND_ bool	CAS (INOUT T& expected, T desired)					__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired,
						 MO_t success, MO_t failure)					__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		ND_ bool	CAS_Loop (INOUT T& expected, T desired)				__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired,
							  MO_t success, MO_t failure)				__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		T	fetch_max (T arg)											__NE___
		{
			T	exp = load();
			for (; (exp < arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	fetch_min (T arg)											__NE___
		{
			T	exp = load();
			for (; (exp > arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		Nd__IF static IT	_Cast (const T &value)						__NE___ { return BitCast<IT>( value ); }
		Nd__IF static T		_Cast (const IT &value)						__NE___ { return BitCast<T>( value ); }

		Nd__IF static T&	_Ref (IT &value)							__NE___ { return reinterpret_cast< T &>( value ); }
		Nd__IF static IT&	_Ref (T &value)								__NE___ { return reinterpret_cast< IT &>( value ); }
	};
//-----------------------------------------------------------------------------



	//
	// Bitfield Atomic
	//

	template <typename T, std::memory_order Success, std::memory_order Failure>
	struct TBitfieldAtomic
	{
	// types
	public:
		using value_type	= T;
		using Bitfield_t	= Bitfield< T >;

	private:
		using Self	= TBitfieldAtomic< T, Success, Failure >;
		using MO_t	= std::memory_order;
		using BF	= Bitfield_t;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;
		static constexpr uint	_BitCount	= CT_SizeOfInBits<T>;

		StaticAssert( StdAtomic<T>::is_always_lock_free );
		StaticAssert( IsUnsignedInteger<T> );


	// variables
	private:
		StdAtomic< T >		_value;


	// methods
	public:
		TBitfieldAtomic ()													__NE___ DEBUG_ONLY(: _value{ T(BitCast<usize>(&_value)) }) {}
		explicit TBitfieldAtomic (BF value)									__NE___ : _value{ value.Get() } {}

		TBitfieldAtomic (const Self &)										= delete;
		TBitfieldAtomic (Self &&)											= delete;

			Self&	operator = (const Self &)								= delete;
			Self&	operator = (Self &&)									= delete;

			void	Store (BF desired)										__NE___ { _value.store( desired.Get(), OnSuccess ); }
		ND_ BF		Load ()													C_NE___ { return BF{ _value.load( OnSuccess )}; }
		ND_ BF		Exchange (BF desired)									__NE___ { return BF{ _value.exchange( desired.Get(), OnSuccess )}; }

			void	Store (BF desired, MO_t memOrder)						__NE___ { _value.store( desired.Get(), memOrder ); }
		ND_ BF		Load (MO_t memOrder)									C_NE___ { return BF{ _value.load( memOrder )}; }
		ND_ BF		Exchange (BF desired, MO_t memOrder)					__NE___ { return BF{ _value.exchange( desired.Get(), memOrder )}; }

		ND_ bool	CAS (INOUT BF& expected, BF desired)					__NE___ { return _value.compare_exchange_weak( INOUT expected.Ref(), desired.Get(), OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT BF& expected, BF desired,
						 MO_t success, MO_t failure)						__NE___ { return _value.compare_exchange_weak( INOUT expected.Ref(), desired.Get(), success, failure ); }

		ND_ bool	CAS_Loop (INOUT BF& expected, BF desired)				__NE___ { return _value.compare_exchange_strong( INOUT expected.Ref(), desired.Get(), OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT BF& expected, BF desired,
							  MO_t success, MO_t failure)					__NE___ { return _value.compare_exchange_strong( INOUT expected.Ref(), desired.Get(), success, failure ); }

		ND_ bool	None ()													C_NE___	{ return Load().None(); }
		ND_ bool	Any ()													C_NE___	{ return Load().Any(); }
		ND_ bool	All ()													C_NE___	{ return Load().All(); }

		ND_ usize	BitCount ()												C_NE___	{ return Load().BitCount(); }
		ND_ usize	ZeroBitCount ()											C_NE___	{ return Load().ZeroBitCount(); }

		// 0|1 -> 1
			BF		Set (usize bit)											__NE___ { ASSERT( bit < _BitCount );  return BF{ _value.fetch_or( T{1} << bit )}; }
			BF		Set (usize bit, MO_t memOrder)							__NE___ { ASSERT( bit < _BitCount );  return BF{ _value.fetch_or( T{1} << bit, memOrder )}; }

		// 0|1 -> 0
			BF		Erase (usize bit)										__NE___ { ASSERT( bit < _BitCount );  return BF{ _value.fetch_and( ~(T{1} << bit) )}; }
			BF		Erase (usize bit, MO_t memOrder)						__NE___ { ASSERT( bit < _BitCount );  return BF{ _value.fetch_and( ~(T{1} << bit), memOrder )}; }

		ND_ bool	Has (usize bit)											C_NE___	{ return Load().Has( bit ); }

		// 0|1 -> 1
			BF		SetRange (usize first, usize count)						__NE___	{ return BF{ _value.fetch_or( _Range( first, count ))}; }
			BF		SetRange (usize first, usize count, MO_t memOrder)		__NE___	{ return BF{ _value.fetch_or( _Range( first, count ), memOrder )}; }

		// 0|1 -> 0
			BF		ResetRange (usize first, usize count)					__NE___	{ return BF{ _value.fetch_or( ~_Range( first, count ))}; }
			BF		ResetRange (usize first, usize count, MO_t memOrder)	__NE___	{ return BF{ _value.fetch_or( ~_Range( first, count ), memOrder )}; }

		ND_ bool	HasRange (usize first, usize count)						C_NE___	{ return Load().HasRange( first, count ); }

			BF		fetch_and (BF arg)										__NE___ { return BF{ _value.fetch_and( arg.Get(), OnSuccess )}; }
			BF		fetch_or (BF arg)										__NE___ { return BF{ _value.fetch_or(  arg.Get(), OnSuccess )}; }
			BF		fetch_xor (BF arg)										__NE___ { return BF{ _value.fetch_xor( arg.Get(), OnSuccess )}; }

			BF		fetch_and (BF arg, MO_t memOrder)						__NE___ { return BF{ _value.fetch_and( arg.Get(), memOrder )}; }
			BF		fetch_or  (BF arg, MO_t memOrder)						__NE___ { return BF{ _value.fetch_or(  arg.Get(), memOrder )}; }
			BF		fetch_xor (BF arg, MO_t memOrder)						__NE___ { return BF{ _value.fetch_xor( arg.Get(), memOrder )}; }

		// change first 1 bit to 0
		ND_ int		ExtractBitIndex ()										__NE___	{ return IntLog2( ExtractBit() ); }
		ND_ T		ExtractBit ()											__NE___
		{
			for (T bits = _value.load( EMemoryOrder::Relaxed );;)
			{
				T	result = bits & ~(bits - T{1});
				if_likely( _value.compare_exchange_weak( INOUT bits, bits & ~result, OnSuccess, OnFailure )) return result;
				ThreadUtils::Pause();
			}
		}

		// returns first 1 bit
		ND_ T		GetFirstBit ()											C_NE___	{ return Load().GetFirstBit(); }
		ND_ int		GetFirstBitIndex ()										C_NE___	{ return Load().GetFirstBitIndex(); }

		// change first 0 bit to 1
		ND_ int		SetFirstZeroBitIndex ()									__NE___	{ return IntLog2( SetFirstZeroBit() ); }
		ND_ T		SetFirstZeroBit ()										__NE___
		{
			for (T bits = _value.load( EMemoryOrder::Relaxed );;)
			{
				T	inv		= ~bits;
				T	result	= inv & ~(inv - T{1});
				if_likely( _value.compare_exchange_weak( INOUT bits, bits | result, OnSuccess, OnFailure )) return result;
				ThreadUtils::Pause();
			}
		}

		// returns first 0 bit
		ND_ int		GetFirstZeroBitIndex ()									C_NE___	{ return Load().GetFirstZeroBitIndex(); }
		ND_ T		GetFirstZeroBit ()										C_NE___	{ return Load().GetFirstZeroBit(); }

	private:
		Nd__IF static T  _Range (usize first, usize count)					__NE___	{ ASSERT( first < _BitCount );  ASSERT( first+count <= _BitCount );  return ToBitMask<T>( count ) << first; }
	};
//-----------------------------------------------------------------------------



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
		using IT	= ByteSizeToUInt< sizeof(T) >;

		static constexpr MO_t	OnSuccess	= Success;
		static constexpr MO_t	OnFailure	= Failure;

		StaticAssert( StdAtomic<T>::is_always_lock_free );
		StaticAssert( IsFloatPoint<T> );
		StaticAssert( sizeof(IT) == sizeof(T) );


	// variables
	private:
		StdAtomic< IT >		_value;


	// methods
	public:
		TAtomicFloat ()										__NE___ DEBUG_ONLY(: _value{ IT(BitCast<usize>(&_value)) }) {}
		explicit TAtomicFloat (T value)						__NE___ : _value{ _Cast(value) } {}

		TAtomicFloat (const Self &)							= delete;
		TAtomicFloat (Self &&)								= delete;

			Self&	operator = (const Self &)				= delete;
			Self&	operator = (Self &&)					= delete;

			void	store (T desired)						__NE___ { _value.store( _Cast(desired), OnSuccess ); }
		ND_ T		load ()									C_NE___ { return _Cast( _value.load( OnSuccess )); }
		ND_ T		exchange (T desired)					__NE___ { return _Cast( _value.exchange( _Cast(desired), OnSuccess )); }

			void	store (T desired, MO_t memOrder)		__NE___ { _value.store( _Cast(desired), memOrder ); }
		ND_ T		load (MO_t memOrder)					C_NE___ { return _Cast( _value.load( memOrder )); }
		ND_ T		exchange (T desired, MO_t memOrder)		__NE___ { return _Cast( _value.exchange( _Cast(desired), memOrder )); }

		ND_ bool	CAS (INOUT T& expected, T desired)		__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T& expected, T desired,
						 MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_weak( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		ND_ bool	CAS_Loop (INOUT T& expected, T desired)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T& expected, T desired,
							  MO_t success, MO_t failure)	__NE___ { return _value.compare_exchange_strong( INOUT _Ref(expected), _Cast(desired), success, failure ); }

		T	fetch_add (T arg)								__NE___ { return fetch_add( arg, OnSuccess ); }
		T	fetch_sub (T arg)								__NE___ { return fetch_sub( arg, OnSuccess ); }

		T	Add (T arg)										__NE___ { return Add( arg, OnSuccess ); }
		T	Sub (T arg)										__NE___ { return Sub( arg, OnSuccess ); }
		T	Inc ()											__NE___ { return Add( T{1}, OnSuccess ); }
		T	Dec ()											__NE___ { return Sub( T{1}, OnSuccess ); }

		T	fetch_add (T arg, MO_t memOrder)				__NE___
		{
			T	exp	= load();
			for (; not CAS( INOUT exp, exp + arg, memOrder, OnFailure );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	fetch_sub (T arg, MO_t memOrder)				__NE___
		{
			T	exp	= load();
			for (; not CAS( INOUT exp, exp - arg, memOrder, OnFailure );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	Add (T arg, MO_t memOrder)						__NE___ { return fetch_add( arg, memOrder ) + arg; }
		T	Sub (T arg, MO_t memOrder)						__NE___ { return fetch_sub( arg, memOrder ) - arg; }
		T	Inc (MO_t memOrder)								__NE___ { return Add( T{1}, memOrder ); }
		T	Dec (MO_t memOrder)								__NE___ { return Sub( T{1}, memOrder ); }

		T	fetch_max (T arg)								__NE___
		{
			T	exp = load();
			for (; (exp < arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T	fetch_min (T arg)								__NE___
		{
			T	exp = load();
			for (; (exp > arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

	private:
		Nd__IF static IT	_Cast (const T &value)			__NE___ { return BitCast<IT>( value ); }
		Nd__IF static T		_Cast (const IT &value)			__NE___ { return BitCast<T>( value ); }
		Nd__IF static IT&	_Ref (T &value)					__NE___ { return reinterpret_cast<IT &>( value ); }
	};
//-----------------------------------------------------------------------------



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

		StaticAssert( StdAtomic<value_type>::is_always_lock_free );


	// variables
	private:
		StdAtomic< value_type >		_value;


	// methods
	public:
		TAtomic ()												__NE___ DEBUG_ONLY(: _value{ BitCast<value_type>(&_value) }) {}
		explicit TAtomic (T* value)								__NE___ : _value{ value } {}

		TAtomic (const Self &)									= delete;
		TAtomic (Self &&)										= delete;

			Self&	operator = (const Self &)					= delete;
			Self&	operator = (Self &&)						= delete;

			void	store (T* desired)							__NE___ { _value.store( desired, OnSuccess ); }
		ND_ T*		load ()										C_NE___ { return _value.load( OnSuccess ); }
		ND_ T*		exchange (T* desired)						__NE___ { return _value.exchange( desired, OnSuccess ); }

			void	store (T* desired, MO_t memOrder)			__NE___ { _value.store( desired, memOrder ); }
		ND_ T*		load (MO_t memOrder)						C_NE___ { return _value.load( memOrder ); }
		ND_ T*		exchange (T* desired, MO_t memOrder)		__NE___ { return _value.exchange( desired, memOrder ); }

		ND_ bool	CAS (INOUT T* &expected, T* desired)		__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, OnSuccess, OnFailure ); }
		ND_ bool	CAS (INOUT T* &expected, T* desired,
						 MO_t success, MO_t failure)			__NE___ { return _value.compare_exchange_weak( INOUT expected, desired, success, failure ); }

		ND_ bool	CAS_Loop (INOUT T* &expected, T* desired)	__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, OnSuccess, OnFailure ); }
		ND_ bool	CAS_Loop (INOUT T* &expected, T* desired,
							  MO_t success, MO_t failure)		__NE___ { return _value.compare_exchange_strong( INOUT expected, desired, success, failure ); }

		T*	fetch_add (ssize arg)								__NE___ { return _value.fetch_add( arg, OnSuccess ); }
		T*	fetch_sub (ssize arg)								__NE___ { return _value.fetch_sub( arg, OnSuccess ); }
		T*	Add (ssize arg)										__NE___ { return _value.Add( arg, OnSuccess ); }
		T*	Sub (ssize arg)										__NE___ { return _value.Sub( arg, OnSuccess ); }

		T*	fetch_add (ssize arg, MO_t memOrder)				__NE___ { return _value.fetch_add( arg, memOrder ); }
		T*	fetch_sub (ssize arg, MO_t memOrder)				__NE___ { return _value.fetch_sub( arg, memOrder ); }
		T*	Add (ssize arg, MO_t memOrder)						__NE___ { return _value.fetch_add( arg, memOrder ) + Math::BytesSSize{arg}; }
		T*	Sub (ssize arg, MO_t memOrder)						__NE___ { return _value.fetch_sub( arg, memOrder ) - Math::BytesSSize{arg}; }

		T*	fetch_max (T* arg)									__NE___
		{
			T*	exp = load();
			for (; (exp < arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}

		T*	fetch_min (T* arg)									__NE___
		{
			T*	exp = load();
			for (; (exp > arg) and not CAS( INOUT exp, arg );) { ThreadUtils::Pause(); }
			return exp;
		}
	};
//-----------------------------------------------------------------------------



	//
	// Atomic State
	//

	template <typename T>
	struct AtomicState
	{
	// types
	public:
		using value_type	= T;

	private:
		using Self	= AtomicState< T >;
		using MO_t	= std::memory_order;

		StaticAssert( IsEnum<T> );
		StaticAssert( StdAtomic<value_type>::is_always_lock_free );

		static constexpr MO_t	OnSuccess	= EMemoryOrder::Relaxed;
		static constexpr MO_t	OnFailure	= EMemoryOrder::Relaxed;


	// variables
	private:
		StdAtomic< T >		_value;


	// methods
	public:
		AtomicState ()								__NE___ DEBUG_ONLY(: _value{ T(BitCast<usize>(&_value)) }) {}
		explicit AtomicState (T state)				__NE___ : _value{ state } {}

			void	store (T desired)				__NE___ { _value.store( desired, OnSuccess ); }
		ND_ T		load ()							C_NE___ { return _value.load( OnSuccess ); }

		ND_ bool	Set (T expected, T newState)	__NE___	{ return _value.compare_exchange_strong( INOUT expected, newState, OnSuccess, OnFailure ); }
		ND_	T		Set (T newState)				__NE___	{ return _value.exchange( newState, OnSuccess ); }
	};
//-----------------------------------------------------------------------------



	template <typename T>
	using Atomic = TAtomic< T, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;

	template <typename T>
	using FAtomic = TAtomicFloat< T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;	// TODO: tests

	template <typename T>
	using StructAtomic = TStructAtomic< T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;

	template <typename T>
	using BitfieldAtomic = TBitfieldAtomic< T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;


	namespace _hidden_
	{
		template <typename T>
		struct TAtomicByte {
			using type = TAtomic< Math::TByte<T>, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
		};

		template <typename T>
		struct TAtomicByte< Math::TByte<T> > {
			using type = TAtomic< Math::TByte<T>, T, EMemoryOrder::Relaxed, EMemoryOrder::Relaxed >;
		};
	}

	template <typename T>
	using AtomicByte = typename Base::_hidden_::TAtomicByte<T>::type;
//-----------------------------------------------------------------------------



	namespace _hidden_
	{
		template <typename T>
		struct _IsAtomic : CT_False {
			using type = void;
		};

		template <typename PublicType, typename InternalType, std::memory_order Success, std::memory_order Failure>
		struct _IsAtomic< TAtomic< PublicType, InternalType, Success, Failure >> : CT_True {
			using type = PublicType;
		};

		template <typename T, std::memory_order Success, std::memory_order Failure>
		struct _IsAtomic< TBitfieldAtomic< T, Success, Failure >> : CT_True {
			using type = T;
		};

		template <typename T, std::memory_order Success, std::memory_order Failure>
		struct _IsAtomic< TAtomicFloat< T, Success, Failure >> : CT_True {
			using type = T;
		};

		template <typename T, std::memory_order Success, std::memory_order Failure>
		struct _IsAtomic< TStructAtomic< T, Success, Failure >> : CT_True {
			using type = T;
		};
	}

	template <typename T>
	static constexpr bool  IsAtomic = Base::_hidden_::_IsAtomic< T >::value;

	template <typename T>
	using AtomicInternalType = typename Base::_hidden_::_IsAtomic< T >::type;
//-----------------------------------------------------------------------------


} // AE::Base

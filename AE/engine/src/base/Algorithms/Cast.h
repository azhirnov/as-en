// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Pointers/Ptr.h"

namespace AE::Base
{

/*
=================================================
	CheckPointerAlignment
=================================================
*/
	ND_ inline bool  CheckPointerAlignment (void const* ptr, usize align) __NE___
	{
		DBG_CHECK_MSG( ((align & (align - 1)) == 0), "Align must be power of 2" );

		return (usize(ptr) & (align-1)) == 0;
	}

	template <usize Align, typename T>
	NdCx__ bool  CheckPointerAlignment (T const* ptr) __NE___
	{
		StaticAssert( ((Align & (Align - 1)) == 0), "Align must be power of 2" );

		return (usize(ptr) & (Align-1)) == 0;
	}

	template <typename R, typename T>
	NdCx__ bool  CheckPointerAlignment (T const* ptr) __NE___
	{
		return CheckPointerAlignment< alignof(R) >( ptr );
	}

/*
=================================================
	CheckPointerCast
=================================================
*/
	template <typename R, typename T>
	__Cz__ void  CheckPointerCast (T const* ptr) __NE___
	{
	#if defined(AE_PLATFORM_APPLE) and AE_CXX_VER <= 17
		if constexpr( not IsVoid<R> and not IsConstEvaluated() )
		{
			// don't use 'TypeNameOf'
			CHECK( CheckPointerAlignment<R>( ptr ));
		}

	#elif defined(AE_DEBUG)
		if constexpr( not IsVoid<R> and not IsConstEvaluated() )
		{
			if ( not CheckPointerAlignment<R>( ptr ))
			{
				AE_LOGE( (std::stringstream{} << "Failed to cast pointer from '" << TypeNameOf<T>() << "' to '" << TypeNameOf<R>()
					<< "': memory address " << std::hex << usize(ptr) << " is not aligned to " << std::dec << alignof(R)
					<< ", it may cause undefined behavior").str() );
			}
		}
	#else
		Unused( ptr );
	#endif
	}

/*
=================================================
	AssumeAligned
=================================================
*/
	template <usize Align, typename T>
	NdCz__ T*  AssumeAligned (T* ptr) __NE___
	{
		ASSERT( CheckPointerAlignment< Align >( ptr ));
		if constexpr( not IsVoid<T> )
			StaticAssert( alignof(T) <= Align );

		#ifdef __cpp_lib_assume_aligned
			return std::assume_aligned<Align>( ptr );
		#else
			return ptr;
		#endif
	}
	
/*
=================================================
	NonAlignedCast (pointer)
=================================================
*/
	template <typename R, typename T>
	NdCx__ R const*  NonAlignedCast (T const* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R const *>(value);
		else
			return static_cast< R const *>( static_cast< void const *>(value) );
	}

	template <typename R, typename T>
	NdCx__ R*  NonAlignedCast (T* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R *>(value);
		else
			return static_cast< R *>( static_cast< void *>(value) );
	}

/*
=================================================
	Cast (pointer)
=================================================
*/
	template <typename R, typename T>
	NdCx__ R const*  Cast (T const* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R const *>(value);
		else
			return static_cast< R const *>( static_cast< void const *>(value) );
	}

	template <typename R, typename T>
	NdCx__ R*  Cast (T* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R *>(value);
		else
			return static_cast< R *>( static_cast< void *>(value) );
	}

	template <typename R, typename T>
	NdCx__ Ptr<R const>  Cast (Ptr<T const> value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	NdCx__ Ptr<R>  Cast (Ptr<T> value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	NdCx__ R*  Cast (const Unique<T> &value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	NdCx__ Unique<R>  Cast (Unique<T> &&value) __NE___
	{
		return Unique<R>{ Cast<R>( value.release() )};
	}

	template <typename R, typename T>
	ND_ SharedPtr<R>  Cast (const SharedPtr<T> &other) __NE___
	{
		return std::static_pointer_cast<R>( other );
	}

	template <typename R, typename T>
	ND_ SharedPtr<R>  Cast (SharedPtr<T> &&other) __NE___
	{
		return std::static_pointer_cast<R>( RVRef(other) );
	}

	template <typename T>
	ND_ T*  Cast (std::nullptr_t) __NE___
	{
		return static_cast<T*>(null);
	}

/*
=================================================
	RefCast (reference)
=================================================
*/
	template <typename R, typename T>
	NdCx__ R const&  RefCast (T const &value) __NE___
	{
		return *Cast<R>( &value );
	}

	template <typename R, typename T>
	NdCx__ R&  RefCast (T &value) __NE___
	{
		return *Cast<R>( &value );
	}

/*
=================================================
	TimeCast (chrono)
=================================================
*/
	template <typename To, typename Rep, typename Period, ENABLEIF( IsDuration<To> )>
	NdCx__ To  TimeCast (const std::chrono::duration<Rep, Period> value) __NE___
	{
		return std::chrono::duration_cast<To>( value );
	}

	template <typename ToDuration, typename Clock, typename Duration, ENABLEIF( IsDuration<ToDuration> )>
	NdCx__ std::chrono::time_point<Clock, ToDuration>  TimeCast (const std::chrono::time_point<Clock, Duration> value) __NE___
	{
		return std::chrono::time_point_cast<ToDuration>( value );
	}

/*
=================================================
	DynCast
=================================================
*/
#ifdef AE_ENABLE_RTTI
	template <typename R, typename T>
	NdCx__ R const*  DynCast (T const* value) __NE___
	{
		return dynamic_cast< R const *>( value );
	}

	template <typename R, typename T>
	NdCx__ R*  DynCast (T* value) __NE___
	{
		return dynamic_cast< R *>( value );
	}

	template <typename R, typename T>
	NdCx__ Ptr<R const>  DynCast (Ptr<T const> value) __NE___
	{
		return DynCast<R>( value.operator->() );
	}

	template <typename R, typename T>
	NdCx__ Ptr<R>  DynCast (Ptr<T> value) __NE___
	{
		return DynCast<R>( value.operator->() );
	}

	template <typename R, typename T>
	ND_ SharedPtr<R>  DynCast (const SharedPtr<T> &other) __NE___
	{
		return std::dynamic_pointer_cast<R>( other );
	}

	template <typename R, typename T>
	ND_ bool  CastAllowed (T const* value) __NE___
	{
		return (dynamic_cast<R const*>( value ) != null) == (value != null);
	}

	template <typename R, typename T>
	ND_ bool  CastNotAllowed (T const* value) __NE___
	{
		return not CastAllowed<R>( value );
	}

#else

	template <typename R, typename T>
	ND_ bool  CastAllowed (T const* value)		__NE___	{ return true; }

	template <typename R, typename T>
	ND_ bool  CastNotAllowed (T const* value)	__NE___	{ return true; }

#endif

/*
=================================================
	BitCast
=================================================
*/
	template <typename To, typename From>
	NdCx__ To  BitCast (const From& src) __NE___
	{
		StaticAssert( sizeof(To) == sizeof(From), "must be same size!" );
		StaticAssert( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//StaticAssert( not IsSame< To, From >);	// to find unnecessary cast

	  #ifdef __cpp_lib_bit_cast
		if constexpr( std::is_trivially_copyable_v<From> and std::is_trivially_copyable_v<To> )
			return std::bit_cast<To>( src );
		else
	  #endif
		{
			To	dst;
			std::memcpy( OUT &dst, &src, sizeof(To) );
			return dst;
		}
	}

/*
=================================================
	BitCastRlx
----
	bit cast with relaxed checks, allow cast between different sizes.
	example: void* -> ulong
=================================================
*/
	template <typename To, typename From>
	NdCx__ To  BitCastRlx (const From& src) __NE___
	{
		//StaticAssert( sizeof(From) <= sizeof(To), "cast will loose data!" );
		StaticAssert( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//StaticAssert( not IsSame< To, From >);	// to find unnecessary cast

	  #ifdef __cpp_lib_bit_cast
		if constexpr( sizeof(To) == sizeof(From) and std::is_trivially_copyable_v<From> and std::is_trivially_copyable_v<To> )
			return std::bit_cast<To>( src );
		else
	  #endif
		{
			To	dst = {};
			std::memcpy( OUT &dst, &src, std::min( sizeof(From), sizeof(To) ));
			return dst;
		}
	}

/*
=================================================
	BitCastPtr
=================================================
*/
	template <typename To, typename From>
	NdCx__ To  BitCastPtr (const From* src) __NE___
	{
		StaticAssert( (IsVoid<From> or IsMemCopyAvailable<From>) and IsMemCopyAvailable<To>, "must be trivial types!" );

		To	dst;
		std::memcpy( OUT &dst, src, sizeof(To) );
		return dst;
	}

/*
=================================================
	CheckCast
=================================================
*/
#ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4244)
#endif
#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wimplicit-float-conversion"
#   pragma clang diagnostic ignored "-Wimplicit-int-conversion"
#   pragma clang diagnostic ignored "-Wshorten-64-to-32"
#endif

	template <typename To, typename From>
	NdCx__ To  CheckCast (const From &src) __NE___
	{
		StaticAssert( IsAnyInteger<To> );
		StaticAssert( IsAnyInteger<From> );

		if constexpr( IsSigned<From> and IsUnsigned<To> )
			ASSERT_Cx( src >= From(0) );

		ASSERT_Cx( static_cast<From>(static_cast<To>(src)) == src );
		return static_cast<To>(src);
	}

	template <typename To, typename From>
	NdCx__ bool  CheckCast (OUT To &dst, const From &src) __NE___
	{
		dst = static_cast<To>(src);
		return static_cast<From>(static_cast<To>(src)) == src;
	}

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif
#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
#endif

/*
=================================================
	LimitCast
=================================================
*/
	template <typename To, typename From>
	NdCx__ To  LimitCast (const From& src) __NE___
	{
		StaticAssert( IsAnyInteger<To> and IsAnyInteger<From> );
		StaticAssert( MaxValue<From>() >= MaxValue<To>() );

		if constexpr( IsSigned<From> and IsUnsigned<To> )
		{
			return	src < From(0)							? To(0) :
					src > static_cast<From>(MaxValue<To>())	? MaxValue<To>() :
															  static_cast<To>(src);
		}
		else
		{
			StaticAssert( MinValue<From>() <= MinValue<To>() );

			return	src > static_cast<From>(MaxValue<To>())	? MaxValue<To>() :
					src < static_cast<From>(MinValue<To>()) ? MinValue<To>() :
															  static_cast<To>(src);
		}
	}

/*
=================================================
	ConstCast
=================================================
*/
	template <typename T>
	NdCx__ T*  ConstCast (const T* ptr) __NE___
	{
		return const_cast<T*>( ptr );
	}

	template <typename T>
	NdCx__ T&  ConstCast (const T& ref) __NE___
	{
		return const_cast<T &>( ref );
	}


} // AE::Base

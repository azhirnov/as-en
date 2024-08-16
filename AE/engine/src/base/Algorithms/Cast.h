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

	template <typename R, typename T>
	ND_ constexpr bool  CheckPointerAlignment (T const* ptr) __NE___
	{
		constexpr usize	align = alignof(R);

		StaticAssert( ((align & (align - 1)) == 0), "Align must be power of 2" );

		return (usize(ptr) & (align-1)) == 0;
	}

/*
=================================================
	CheckPointerCast
=================================================
*/
	template <typename R, typename T>
	cxx20_constexpr void  CheckPointerCast (T const* ptr) __NE___
	{
	#ifdef AE_CFG_DEBUG
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
	Cast (pointer)
=================================================
*/
	template <typename R, typename T>
	ND_ constexpr R const*  Cast (T const* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R const *>(value);
		else
			return static_cast< R const *>( static_cast< void const *>(value) );
	}

	template <typename R, typename T>
	ND_ constexpr R*  Cast (T* value) __NE___
	{
		StaticAssert( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );

		if constexpr( std::is_convertible_v< T const*, R const* > or IsBaseOf< T, R >)
			return static_cast< R *>(value);
		else
			return static_cast< R *>( static_cast< void *>(value) );
	}

	template <typename R, typename T>
	ND_ constexpr Ptr<R const>  Cast (Ptr<T const> value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	ND_ constexpr Ptr<R>  Cast (Ptr<T> value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	ND_ constexpr R*  Cast (const Unique<T> &value) __NE___
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	ND_ constexpr Unique<R>  Cast (Unique<T> &&value) __NE___
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
	ND_ constexpr R const&  RefCast (T const &value) __NE___
	{
		return *Cast<R>( &value );
	}

	template <typename R, typename T>
	ND_ constexpr R&  RefCast (T &value) __NE___
	{
		return *Cast<R>( &value );
	}

/*
=================================================
	TimeCast (chrono)
=================================================
*/
	template <typename To, typename Rep, typename Period, ENABLEIF( IsDuration<To> )>
	ND_ constexpr To  TimeCast (const std::chrono::duration<Rep, Period> value) __NE___
	{
		return std::chrono::duration_cast<To>( value );
	}

	template <typename ToDuration, typename Clock, typename Duration, ENABLEIF( IsDuration<ToDuration> )>
	ND_ constexpr std::chrono::time_point<Clock, ToDuration>  TimeCast (const std::chrono::time_point<Clock, Duration> value) __NE___
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
	ND_ constexpr R const*  DynCast (T const* value) __NE___
	{
		return dynamic_cast< R const *>( value );
	}

	template <typename R, typename T>
	ND_ constexpr R*  DynCast (T* value) __NE___
	{
		return dynamic_cast< R *>( value );
	}

	template <typename R, typename T>
	ND_ constexpr Ptr<R const>  DynCast (Ptr<T const> value) __NE___
	{
		return DynCast<R>( value.operator->() );
	}

	template <typename R, typename T>
	ND_ constexpr Ptr<R>  DynCast (Ptr<T> value) __NE___
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
	ND_ constexpr To  BitCast (const From& src) __NE___
	{
		StaticAssert( sizeof(To) == sizeof(From), "must be same size!" );
		StaticAssert( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//StaticAssert( not IsSameTypes< To, From >);	// to find unnecessary cast

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
	ND_ constexpr To  BitCastRlx (const From& src) __NE___
	{
		//StaticAssert( sizeof(From) <= sizeof(To), "cast will loose data!" );
		StaticAssert( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//StaticAssert( not IsSameTypes< To, From >);	// to find unnecessary cast

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
	ND_ constexpr To  CheckCast (const From &src) __NE___
	{
		if constexpr( IsSigned<From> and IsUnsigned<To> )
			ASSERT( src >= From(0) );

		ASSERT( static_cast<From>(static_cast<To>(src)) == src );
		return static_cast<To>(src);
	}

	template <typename To, typename From>
	ND_ constexpr bool  CheckCast (OUT To &dst, const From &src) __NE___
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
	ND_ constexpr To  LimitCast (const From& src) __NE___
	{
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
	ND_ constexpr T*  ConstCast (const T* ptr) __NE___
	{
		return const_cast<T*>( ptr );
	}

	template <typename T>
	ND_ constexpr T&  ConstCast (const T& ref) __NE___
	{
		return const_cast<T &>( ref );
	}


} // AE::Base

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/Ptr.h"

#ifdef AE_DEBUG
# include <sstream>
#endif

namespace AE::Base
{

/*
=================================================
	CheckPointerAlignment
=================================================
*/
	ND_ forceinline bool  CheckPointerAlignment (void const* ptr, usize align)
	{
		DBG_CHECK_MSG( ((align & (align - 1)) == 0), "Align must be power of 2" );

		return (usize(ptr) & (align-1)) == 0;
	}

	template <typename R, typename T>
	ND_ forceinline bool  CheckPointerAlignment (T const* ptr)
	{
		constexpr usize	align = alignof(R);

		STATIC_ASSERT( ((align & (align - 1)) == 0), "Align must be power of 2" );

		return (usize(ptr) & (align-1)) == 0;
	}
	
/*
=================================================
	CheckPointerCast
=================================================
*/
	template <typename R, typename T>
	forceinline void  CheckPointerCast (T const* ptr)
	{
		Unused( ptr );
		DEBUG_ONLY(
			if constexpr( not IsVoid<R> )
			{
				if ( not CheckPointerAlignment<R>( ptr ))
				{
					std::stringstream	str;
					str << "Failed to cast pointer from '" << typeid(T).name() << "' to '" << typeid(R).name()
						<< "': memory address " << std::hex << usize(ptr) << " is not aligned to " << std::dec << alignof(R)
						<< ", it may cause undefined behavior";
					AE_LOGE( str.str() );
				}
			}
		)
	}

/*
=================================================
	Cast
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline constexpr R const volatile*  Cast (T const volatile* value)
	{
		STATIC_ASSERT( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );
		return static_cast< R const volatile *>( static_cast< void const volatile *>(value) );
	}

	template <typename R, typename T>
	ND_ forceinline constexpr R volatile*  Cast (T volatile* value)
	{
		STATIC_ASSERT( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );
		return static_cast< R volatile *>( static_cast< void volatile *>(value) );
	}

	template <typename R, typename T>
	ND_ forceinline constexpr R const*  Cast (T const* value)
	{
		STATIC_ASSERT( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );
		return static_cast< R const *>( static_cast< void const *>(value) );
	}
	
	template <typename R, typename T>
	ND_ forceinline constexpr R*  Cast (T* value)
	{
		STATIC_ASSERT( sizeof(R*) == sizeof(T*) and sizeof(T*) == sizeof(void*) );
		CheckPointerCast<R>( value );
		return static_cast< R *>( static_cast< void *>(value) );
	}

	template <typename R, typename T>
	ND_ forceinline constexpr Ptr<R const>  Cast (Ptr<T const> value)
	{
		return Cast<R>( value.get() );
	}
	
	template <typename R, typename T>
	ND_ forceinline constexpr Ptr<R>  Cast (Ptr<T> value)
	{
		return Cast<R>( value.get() );
	}
	
	template <typename R, typename T>
	ND_ forceinline constexpr R*  Cast (const Unique<T> &value)
	{
		return Cast<R>( value.get() );
	}

	template <typename R, typename T>
	ND_ forceinline SharedPtr<R>  Cast (const SharedPtr<T> &other)
	{
		return std::static_pointer_cast<R>( other );
	}
	
/*
=================================================
	DynCast
=================================================
*/
#ifdef AE_ENABLE_RTTI
	template <typename R, typename T>
	ND_ forceinline constexpr R const*  DynCast (T const* value)
	{
		return dynamic_cast< R const *>( value );
	}
	
	template <typename R, typename T>
	ND_ forceinline constexpr R*  DynCast (T* value)
	{
		return dynamic_cast< R *>( value );
	}

	template <typename R, typename T>
	ND_ forceinline constexpr Ptr<R const>  DynCast (Ptr<T const> value)
	{
		return DynCast<R>( value.operator->() );
	}
	
	template <typename R, typename T>
	ND_ forceinline constexpr Ptr<R>  DynCast (Ptr<T> value)
	{
		return DynCast<R>( value.operator->() );
	}

	template <typename R, typename T>
	ND_ forceinline SharedPtr<R>  DynCast (const SharedPtr<T> &other)
	{
		return std::dynamic_pointer_cast<R>( other );
	}
#endif

/*
=================================================
	BitCast
=================================================
*/
	template <typename To, typename From>
	ND_ forceinline constexpr To  BitCast (const From& src)
	{
		STATIC_ASSERT( sizeof(To) == sizeof(From), "must be same size!" );
		STATIC_ASSERT( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//STATIC_ASSERT( not IsSameTypes< To, From >);	// to find unnecessary cast

	#ifdef __cpp_lib_bit_cast
		return std::bit_cast<To>( src );
	#else
		To	dst;
		std::memcpy( OUT &dst, &src, sizeof(To) );
		return dst;
	#endif
	}

/*
=================================================
	UnsafeBitCast
=================================================
*/
	template <typename To, typename From>
	ND_ inline constexpr To  UnsafeBitCast (const From& src)
	{
		//STATIC_ASSERT( sizeof(From) <= sizeof(To), "cast will lost data!" );
		STATIC_ASSERT( IsMemCopyAvailable<From> and IsMemCopyAvailable<To>, "must be trivial types!" );
		//STATIC_ASSERT( not IsSameTypes< To, From >);	// to find unnecessary cast

		To	dst = {};
		std::memcpy( OUT &dst, &src, std::min( sizeof(From), sizeof(To) ));
		return dst;
	}

/*
=================================================
	CheckCast
=================================================
*/
	template <typename To, typename From>
	ND_ inline constexpr To  CheckCast (const From& src)
	{
	#ifdef AE_COMPILER_MSVC
	#	pragma warning (push)
	#	pragma warning (disable: 4244)
	#endif

		if constexpr( IsSigned<From> and IsUnsigned<To> )
		{
			ASSERT( src >= From(0) );
		}

		ASSERT( static_cast<From>(static_cast<To>(src)) == src );

		return static_cast<To>(src);
		
	#ifdef AE_COMPILER_MSVC
	#	pragma warning (pop)
	#endif
	}
	
/*
=================================================
	LimitCast
=================================================
*/
	template <typename To, typename From>
	ND_ inline constexpr To  LimitCast (const From& src)
	{
		STATIC_ASSERT( MaxValue<From>() >= MaxValue<To>() );

		if constexpr( IsSigned<From> and IsUnsigned<To> )
		{
			return	src < From(0)							? To(0) :
					src > static_cast<From>(MaxValue<To>())	? MaxValue<To>() :
															  static_cast<To>(src);
		}
		else
		{
			STATIC_ASSERT( MinValue<From>() <= MinValue<To>() );

			return	src > static_cast<From>(MaxValue<To>())	? MaxValue<To>() :
					src < static_cast<From>(MinValue<To>()) ? MinValue<To>() :
															  static_cast<To>(src);
		}
	}


} // AE::Base

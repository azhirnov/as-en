// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"
#include "base/Defines/Undef.h"

namespace AE
{
	using sbyte		= int8_t;
	using ubyte		= uint8_t;
	using sshort	= int16_t;
	using ushort	= uint16_t;
	using sint		= int32_t;
	using uint 		= uint32_t;		// u
	using slong		= int64_t;		// ll
	using ulong		= uint64_t;		// ull
	using ssize		= intptr_t;
	using usize		= size_t;

#if defined(__cpp_char8_t) and not defined(AE_PLATFORM_APPLE)
	using CharUtf8	= char8_t;	// C++20
#else
	enum class CharUtf8 : char {};
#endif

	using CharAnsi	= char;
	using CharUtf16	= char16_t;
	using CharUtf32	= char32_t;
	
#ifdef AE_PLATFORM_WINDOWS
# if UNICODE
	using CharType			= wchar_t;
#	define TXT( _text_ )	(L"" _text_)
# else
	using CharType			= CharAnsi;
#	define TXT( _text_ )	("" _text_)
# endif
#else
	using CharType			= CharUtf8;
#	define TXT( _text_ )	(u8"" _text_)
#endif

	namespace Base {}
	namespace Math {}
}

namespace AE::Math
{
	using namespace AE::Base;
}

#include "base/Log/Log.h"
#include "base/CompileTime/TemplateUtils.h"
#include "base/CompileTime/TypeTraits.h"
#include "base/Algorithms/Hash.h"
#include "base/CompileTime/Constants.h"
#include "base/CompileTime/DefaultType.h"
#include "base/Utils/StdFunctions.h"
#include "base/Containers/Tuple.h"
#include "base/Containers/HashTable.h"

namespace AE::Base
{
	using namespace AE::Math;
	
	template <typename T,
			  typename A = std::allocator<T>>
	using BasicString	= std::basic_string< T, std::char_traits<T>, A >;

	using String		= BasicString< CharAnsi >;
	using WString		= BasicString< wchar_t >;
	using U8String		= BasicString< CharUtf8 >;
	using U16String		= BasicString< CharUtf16 >;
	using U32String		= BasicString< CharUtf32 >;

	template <typename T,
			  typename A = std::allocator<T>>
	using Array = std::vector< T, A >;

	template <typename T>	using SharedPtr		= std::shared_ptr< T >;
	template <typename T>	using WeakPtr		= std::weak_ptr< T >;

	template <typename T>	using Deque			= std::deque< T >;

	template <usize N>		using BitSet		= std::bitset< N >;
	
	template <typename T>	using Optional		= std::optional< T >;

	constexpr std::nullopt_t	NullOptional	= std::nullopt;
	
	template <typename T>
	using BasicStringView	= std::basic_string_view<T>;
	using StringView		= BasicStringView< CharAnsi >;
	using WStringView		= BasicStringView< wchar_t >;
	using U8StringView		= BasicStringView< CharUtf8 >;
	using U16StringView		= BasicStringView< CharUtf16 >;
	using U32StringView		= BasicStringView< CharUtf32 >;

	template <typename T>	using Function		= std::function< T >;


	template <typename T,
			  typename Deleter = std::default_delete<T>>
	using Unique		= std::unique_ptr< T, Deleter >;

	template <typename T,
			  usize ArraySize>
	using StaticArray	= std::array< T, ArraySize >;


	template <typename FirstT,
			  typename SecondT>
	using Pair			= std::pair< FirstT, SecondT >;
	

	

	// Uppercase names reserved by physical quantity wrappers
	using seconds		= std::chrono::seconds;
	using milliseconds	= std::chrono::milliseconds;
	using microseconds	= std::chrono::microseconds;
	using nanoseconds	= std::chrono::nanoseconds;
	using secondsf		= std::chrono::duration<float>;
	using secondsd		= std::chrono::duration<double>;
	using nanosecondsd	= std::chrono::duration<double, std::nano>;
	using minutes		= std::chrono::minutes;
	

/*
=================================================
	MakeShared
=================================================
*/
	template <typename T, typename ...Types>
	ND_ forceinline SharedPtr<T>  MakeShared (Types&&... args)  __Th___
	{
		return std::make_shared<T>( FwdArg<Types>( args )... );
	}

/*
=================================================
	MakeUnique
=================================================
*/
	template <typename T, typename ...Types>
	ND_ forceinline Unique<T>  MakeUnique (Types&&... args)  __Th___
	{
		return std::make_unique<T>( FwdArg<Types>( args )... );
	}
	
/*
=================================================
	MakePair
=================================================
*/
	template <typename A, typename B>
	ND_ forceinline Pair<A,B>  MakePair (A&& first, B&& second) __NE___
	{
		return Pair<A,B>{ FwdArg<A>(first), FwdArg<B>(second) };
	}

} // AE::Base

namespace AE
{
	class Exception final
	{
	// types
	private:
		const char*		_what = "";

	// methods
	public:
		explicit Exception (const char *str)		__NE___ : _what{str} {}
		explicit Exception (const std::string &str)	__NE___ : _what{str.c_str()} {}

		ND_ const char*  what ()					C_NE___ { return _what; }
	};

} // AE

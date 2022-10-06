// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Config.h"
#include "base/Defines/Attribs.h"
#include "base/Defines/Errors.h"
#include "base/Defines/Helpers.h"
#include "base/Defines/MacroChecks.h"


#include <type_traits>
#include <cstdint>
#include <utility>
#include <tuple>
#include <variant>
#include <vector>
#include <string>
#include <array>
#include <memory>		// shared_ptr, weak_ptr, unique_ptr
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <cstring>
#include <cmath>
#include <optional>
#include <string_view>
#include <typeindex>
#include <type_traits>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>
#include <atomic>

namespace AE
{
	using sbyte		= int8_t;
	using ubyte		= uint8_t;
	using sshort	= int16_t;
	using ushort	= uint16_t;
	using sint		= int32_t;
	using uint 		= uint32_t;
	using slong		= int64_t;
	using ulong		= uint64_t;
	using ssize		= intptr_t;
	using usize		= size_t;
	
#if defined(AE_PLATFORM_WINDOWS) and UNICODE
	using CharType			= wchar_t;
#	define TXT( _text_ )	(L"" _text_)
#else
	using CharType			= char;
#	define TXT( _text_ )	_text_	// TODO: u8 ?
#endif

	namespace Base {}
	namespace Math {}
}

namespace AE::Math
{
	using namespace AE::Base;
}

#include "base/Log/Log.h"
#include "base/Algorithms/Hash.h"
#include "base/CompileTime/TemplateUtils.h"
#include "base/CompileTime/TypeTraits.h"
#include "base/CompileTime/Constants.h"
#include "base/CompileTime/DefaultType.h"
#include "base/Utils/StdFunctions.h"
#include "base/Containers/Tuple.h"
#include "base/Containers/HashTable.h"

namespace AE::Base
{
	using namespace AE::Math;

	using String	= std::string;
	using WString	= std::wstring;
	
	template <typename T,
			  typename A = std::allocator<T>>
	using BasicString = std::basic_string< T, std::char_traits<T>, A >;

	template <typename T,
			  typename A = std::allocator<T>>
	using Array = std::vector< T, A >;

	template <typename T>	using SharedPtr		= std::shared_ptr< T >;
	template <typename T>	using WeakPtr		= std::weak_ptr< T >;

	template <typename T>	using Deque			= std::deque< T >;

	template <usize N>		using BitSet		= std::bitset< N >;
	
	template <typename T>	using Optional		= std::optional< T >;

	constexpr std::nullopt_t	NullOptional	= std::nullopt;
	
							using StringView		= std::string_view;
							using WStringView		= std::wstring_view;
	template <typename T>	using BasicStringView	= std::basic_string_view<T>;

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
	

/*
=================================================
	MakeShared
=================================================
*/
	template <typename T, typename ...Types>
	ND_ forceinline SharedPtr<T>  MakeShared (Types&&... args)
	{
		return std::make_shared<T>( FwdArg<Types>( args )... );
	}

/*
=================================================
	MakeUnique
=================================================
*/
	template <typename T, typename ...Types>
	ND_ forceinline Unique<T>  MakeUnique (Types&&... args)
	{
		return std::make_unique<T>( FwdArg<Types>( args )... );
	}
	
}	// AE::Base

namespace AE
{
#	if AE_NO_EXCEPTIONS == 0
	class Exception final : public std::runtime_error
	{
	public:
		explicit Exception (Base::StringView sv) : runtime_error{ Base::String{sv} } {}
		explicit Exception (Base::String str) : runtime_error{ Base::RVRef(str) } {}
		explicit Exception (const char *str) : runtime_error{ Base::String{str} } {}
	};
#	endif

}	// AE

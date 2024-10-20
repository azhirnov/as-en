// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Base.pch.h"
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

#if defined(__cpp_char8_t)
	using CharUtf8	= char8_t;		// u8''  (C++20)
#else
	enum class CharUtf8 : char {};
#endif

	using CharAnsi	= char;
	using CharUtf16	= char16_t;		// u''
	using CharUtf32	= char32_t;		// U''

#ifdef AE_PLATFORM_WINDOWS
# if UNICODE
	using CharType			= wchar_t;	// L''
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
#include "base/CompileTime/Constants.h"

namespace AE::Base
{
	using namespace AE::Math;

	template <typename T,
			  typename A = std::allocator<T>>
	using BasicString		= std::basic_string< T, std::char_traits<T>, A >;

	using String			= BasicString< CharAnsi >;
	using WString			= BasicString< wchar_t >;
	using U8String			= BasicString< CharUtf8 >;
	using U16String			= BasicString< CharUtf16 >;
	using U32String			= BasicString< CharUtf32 >;


	template <typename T>
	using BasicStringView	= std::basic_string_view<T>;
	using StringView		= BasicStringView< CharAnsi >;
	using WStringView		= BasicStringView< wchar_t >;
	using U8StringView		= BasicStringView< CharUtf8 >;
	using U16StringView		= BasicStringView< CharUtf16 >;
	using U32StringView		= BasicStringView< CharUtf32 >;


	template <typename T>	using SharedPtr		= std::shared_ptr< T >;
	template <typename T>	using WeakPtr		= std::weak_ptr< T >;

	template <typename T>	using Deque			= std::deque< T >;

	template <usize N>		using BitSet		= std::bitset< N >;

	template <typename T>	using Optional		= std::optional< T >;

	static constexpr inline std::nullopt_t	NullOptional = std::nullopt;

	template <typename T>	using Function		= std::function< T >;


	template <typename T,
			  typename A = std::allocator<T>>
	using Array			= std::vector< T, A >;

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
	using nanosecondsf	= std::chrono::duration<float, std::nano>;
	using nanosecondsd	= std::chrono::duration<double, std::nano>;
	using minutes		= std::chrono::minutes;

/*
=================================================
	TypeNameOf
=================================================
*/
#ifdef AE_ENABLE_RTTI
	template <typename T>
	ND_ constexpr StringView  TypeNameOf () __NE___
	{
		return StringView{ typeid(T).name() };
	}

# if defined(__cpp_char8_t) and defined(AE_PLATFORM_APPLE)
	// bugfix: link error in MacOS clang14-15
	template <>
	ND_ constexpr StringView  TypeNameOf<char8_t> () __NE___
	{
		return StringView{"char8_t"};
	}
# endif
#endif

/*
=================================================
	Unused
=================================================
*/
	template <typename... Args>
	constexpr void  Unused (Args&& ...) __NE___ {}

/*
=================================================
	RVRef (same as std::move)
=================================================
*/
	template <typename T>
	ND_ AE_INTRINSIC constexpr RemoveReference<T>&&  RVRef (T &arg) __NE___
	{
		return static_cast< RemoveReference<T>&& >( arg );
	}

/*
=================================================
	FwdArg (same as std::forward)
=================================================
*/
	template <typename T>
	ND_ AE_INTRINSIC constexpr T&&  FwdArg (RemoveReference<T> &arg) __NE___
	{
		return static_cast< T&& >( arg );
	}

	template <typename T>
	ND_ AE_INTRINSIC constexpr T&&  FwdArg (RemoveReference<T> &&arg) __NE___
	{
		StaticAssert( not IsLValueRef<T> );
		return static_cast< T&& >( arg );
	}

/*
=================================================
	MakeShared
=================================================
*/
	template <typename T, typename ...Args>
	ND_ SharedPtr<T>  MakeShared (Args&&... args) __Th___
	{
		StaticAssert( IsConstructible< T, Args... >);
		return std::make_shared<T>( FwdArg<Args>( args )... );
	}

/*
=================================================
	MakeUnique
=================================================
*/
	template <typename T, typename ...Args>
	ND_ Unique<T>  MakeUnique (Args&&... args) __NE___
	{
		StaticAssert( IsConstructible< T, Args... >);
		CheckNothrow( IsNothrowCtor< T, Args... >);
		//CheckNothrow( IsNoExcept( new T{ FwdArg<Args>(args)... }));

		return Unique<T>( new T{ FwdArg<Args>( args )... });
	}

/*
=================================================
	MakePair
=================================================
*/
	template <typename A, typename B>
	ND_ constexpr Pair<A,B>  MakePair (A&& first, B&& second) __NE___
	{
		return Pair<A,B>{ FwdArg<A>(first), FwdArg<B>(second) };
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< BasicStringView<T> >		: CT_True	{};
	template <typename T>	struct TZeroMemAvailable< BasicStringView<T> >		: CT_False	{};
	template <typename T>	struct TTriviallySerializable< BasicStringView<T> >	: CT_False	{};
	template <typename T>	struct TTriviallyDestructible< BasicStringView<T> >	: CT_True	{};


	template <usize N>	struct TMemCopyAvailable< BitSet<N> >		: CT_True	{};
	template <usize N>	struct TZeroMemAvailable< BitSet<N> >		: CT_True	{};
	template <usize N>	struct TTriviallySerializable< BitSet<N> >	: CT_False	{};
	template <usize N>	struct TTriviallyDestructible< BitSet<N> >	: CT_True	{};

	template <typename F, typename S>	struct TMemCopyAvailable< Pair<F,S> >		: CT_Bool< IsMemCopyAvailable<F> and IsMemCopyAvailable<S> >			{};
	template <typename F, typename S>	struct TZeroMemAvailable< Pair<F,S> >		: CT_Bool< IsZeroMemAvailable<F> and IsZeroMemAvailable<S> >			{};
	template <typename F, typename S>	struct TTriviallySerializable< Pair<F,S> >	: CT_Bool< IsTriviallySerializable<F> and IsTriviallySerializable<S> >	{};
	template <typename F, typename S>	struct TTriviallyDestructible< Pair<F,S> >	: CT_Bool< IsTriviallyDestructible<F> and IsTriviallyDestructible<S> >	{};

} // AE::Base

#include "base/Algorithms/Hash.h"
#include "base/Containers/Tuple.h"
#include "base/Containers/HashTable.h"

namespace AE
{
	class Exception final
	{
	// types
	private:
		const char*		_what = "";

	// methods
	public:
		explicit Exception (const char* str)		__NE___ : _what{str} {}
		explicit Exception (const std::string &str)	__NE___ : _what{str.c_str()} {}

		ND_ const char*  what ()					C_NE___ { return _what; }
	};

} // AE

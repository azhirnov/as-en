// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_ENABLE_GFS
#  include "filesystem.hpp"
	namespace _ae_fs_ = ghc::filesystem;
#else
#	include <filesystem>
	namespace _ae_fs_ = std::filesystem;
#endif
#include "base/Defines/Undef.h"
#include "base/Algorithms/Cast.h"

namespace AE::Base
{
	using Path = _ae_fs_::path;

	
/*
=================================================
	ToPath
=================================================
*/
	template <typename T>
	Nd__In Path  ToPath (const T* str) __Th___
	{
	  #if not defined(__cpp_char8_t)
		if constexpr( IsSame< T, CharUtf8 >)
			return Path{ Cast<char>( str )};
		else
	  #endif
			return Path{str};
	}
	
	template <typename T>
	Nd__In Path  ToPath (BasicStringView<T> str) __Th___
	{
	  #if not defined(__cpp_char8_t)
		if constexpr( IsSame< T, CharUtf8 >)
			return Path{ StringView{ Cast<char>( str.data() ), str.length() }};
		else
	  #endif
			return Path{str};
	}
	
	template <typename T>
	Nd__In Path  ToPath (const BasicString<T> &str) __Th___
	{
	  #if not defined(__cpp_char8_t)
		if constexpr( IsSame< T, CharUtf8 >)
			return Path{ StringView{ Cast<char>( str.data() ), str.length() }};
		else
	  #endif
			return Path{str};
	}

	template <typename T>
	Nd__In Path  ToPath (BasicString<T> &&str) __Th___
	{
	  #if not defined(__cpp_char8_t)
		if constexpr( IsSame< T, CharUtf8 >)
			return Path{ StringView{ Cast<char>( str.data() ), str.length() }};
		else
	  #endif
			return Path{ RVRef(str) };
	}

} // AE::Base

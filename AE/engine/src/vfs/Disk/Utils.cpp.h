// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_ENABLE_UTF8PROC
#	error Utf8Proc is required
#endif

namespace AE::VFS
{
namespace
{

/*
=================================================
	Convert
=================================================
*/
	template <typename T, typename Alloc>
	ND_ bool  Convert (OUT StringView &result, BasicStringView<T> path, Alloc &allocator)
	{
		if constexpr( IsSameTypes< T, CharAnsi > or IsSameTypes< T, CharUtf8 >)
		{
			const usize	count = Utf8CharCount( BasicStringView<CharUtf8>{ Cast<CharUtf8>(path.data()), path.size() });
			CHECK_ERR( count <= FileName::MaxStringLength() );

			char*	str = allocator.template Allocate<char>( count+1 );
			CHECK_ERR( str != null );

			usize	len = path.length();
			bool	res = Utf8ToAnsi( OUT str, Cast<CharUtf8>(path.data()), INOUT len );
			ASSERT( count == len );

			FindAndReplace( INOUT MutableArrayView<char>{str, len}, '\\', '/' );

			result = StringView{ str, len };
			return res;
		}
		else
		if constexpr( IsSameTypes< T, wchar_t > or IsSameTypes< T, CharUtf32 >)
		{
			CHECK_ERR( path.length() <= FileName::MaxStringLength() );

			char*	str = allocator.template Allocate<char>( path.length()+1 );
			CHECK_ERR( str != null );

			bool	res = WCharToAnsi( OUT str, path.data(), path.length() );

			FindAndReplace( INOUT MutableArrayView<char>{str, path.length()}, '\\', '/' );

			result = StringView{ str, path.length() };
			return res;
		}
	}

	template <typename Alloc>
	ND_ bool  Convert (OUT StringView &result, const Path &path, Alloc &allocator)
	{
		return Convert( OUT result, BasicStringView<Path::value_type>{path.native()}, allocator );
	}

/*
=================================================
	Convert
=================================================
*/
	template <typename T>
	ND_ bool  Convert (OUT String &result, BasicStringView<T> path)
	{
		result.clear();

		if constexpr( IsSameTypes< T, CharAnsi > or IsSameTypes< T, CharUtf8 >)
		{
			const usize	count = Utf8CharCount( BasicStringView<CharUtf8>{ Cast<CharUtf8>(path.data()), path.size() });
			CHECK_ERR( count <= FileName::MaxStringLength() );

			result.resize( count );

			usize	len = path.length();
			bool	res = Utf8ToAnsi( OUT result.data(), Cast<CharUtf8>(path.data()), INOUT len );
			ASSERT( count == len );

			FindAndReplace( INOUT result, '\\', '/' );
			return res;
		}
		else
		if constexpr( IsSameTypes< T, wchar_t > or IsSameTypes< T, CharUtf32 >)
		{
			CHECK_ERR( path.length() <= FileName::MaxStringLength() );

			result.resize( path.length() );

			bool	res = WCharToAnsi( OUT result.data(), path.data(), path.length() );

			FindAndReplace( INOUT result, '\\', '/' );
			return res;
		}
	}

	ND_ bool  Convert (OUT String &result, const Path &path)
	{
		return Convert( OUT result, BasicStringView<Path::value_type>{path.native()} );
	}

} // namespace
} // AE::VFS

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Abseil docs: https://abseil.io/docs/cpp/guides/container
*/

#pragma once

#ifdef AE_ENABLE_ABSEIL

# ifdef AE_COMPILER_MSVC
#	pragma warning(push, 1)
#	pragma warning(disable: 4458)
#	pragma warning(disable: 4127)
#	pragma warning(disable: 4100)
#	pragma warning(disable: 4946)
# endif

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdouble-promotion"
#  if __clang_major__ >= 15
#   pragma clang diagnostic ignored "-Wdeprecated-builtins"
#  endif
# endif

# include "absl/container/flat_hash_map.h"
# include "absl/container/flat_hash_set.h"

# include "absl/container/node_hash_map.h"
# include "absl/container/node_hash_set.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning(pop)
# endif
# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif

#endif // AE_ENABLE_ABSEIL

namespace AE::Base
{

	template <typename T,
			  typename Hasher = std::hash<T>,
			  typename KeyEq  = std::equal_to<T>,
			  typename Alloc  = std::allocator<T>>
	using HashSet		= std::unordered_set< T, Hasher, KeyEq, Alloc >;


	template <typename Key,
			  typename Value,
			  typename Hasher = std::hash<Key>,
			  typename KeyEq  = std::equal_to<Key>,
			  typename Alloc  = std::allocator<std::pair<const Key, Value>>>
	using HashMap		= std::unordered_map< Key, Value, Hasher, KeyEq, Alloc >;

	template <typename Key,
			  typename Value,
			  typename Hasher = std::hash<Key>,
			  typename KeyEq  = std::equal_to<Key>,
			  typename Alloc  = std::allocator<std::pair<const Key, Value>>>
	using HashMultiMap	= std::unordered_multimap< Key, Value, Hasher, KeyEq, Alloc >;


#ifdef AE_ENABLE_ABSEIL

	//
	// Flat Hash Map
	//
	template <typename Key,
			  typename Value,
			  typename Hasher	= std::hash< Key >,
			  typename KeyEq	= std::equal_to< Key >,
			  typename Alloc	= std::allocator< std::pair< const Key, Value >>
			 >
	using FlatHashMap = absl::flat_hash_map< Key, Value, Hasher, KeyEq, Alloc >;


	//
	// Flat Hash Set
	//
	template <typename Key,
			  typename Hasher	= std::hash< Key >,
			  typename KeyEq	= std::equal_to< Key >,
			  typename Alloc	= std::allocator< Key >
			 >
	using FlatHashSet = absl::flat_hash_set< Key, Hasher, KeyEq, Alloc >;

#else

	// fallback

	template <typename Key,
			  typename Value,
			  typename Hasher = std::hash<Key>,
			  typename KeyEq  = std::equal_to<Key>,
			  typename Alloc  = std::allocator<std::pair<const Key, Value>>>
	using FlatHashMap = HashMap< Key, Value, Hasher, KeyEq, Alloc >;

	template <typename T,
			  typename Hasher = std::hash<T>,
			  typename KeyEq  = std::equal_to<T>,
			  typename Alloc  = std::allocator<T>>
	using FlatHashSet = HashSet< T, Hasher, KeyEq, Alloc >;

#endif // AE_ENABLE_ABSEIL

} // AE::Base


#ifdef AE_ENABLE_ABSEIL
template <typename Key,
		  typename Value,
		  typename Hasher,
		  typename KeyEq,
		  typename Alloc
		 >
struct std::hash< AE::Base::FlatHashMap< Key, Value, Hasher, KeyEq, Alloc >>
{
	size_t  operator () (const AE::Base::FlatHashMap< Key, Value, Hasher, KeyEq, Alloc > &map) C_NE___
	{
		using namespace AE::Base;
		HashVal	h = HashOf( map.size() );
		for (auto& [key, val] : map) {
			h << HashOf( key ) << HashOf( val );
		}
		return size_t(h);
	}
};


template <typename Key,
		  typename Hasher,
		  typename KeyEq,
		  typename Alloc
		 >
struct std::hash< AE::Base::FlatHashSet< Key, Hasher, KeyEq, Alloc >>
{
	size_t  operator () (const AE::Base::FlatHashSet< Key, Hasher, KeyEq, Alloc > &set) C_NE___
	{
		using namespace AE::Base;
		HashVal	h = HashOf( set.size() );
		for (auto& item : set) {
			h << HashOf( item );
		}
		return size_t(h);
	}
};
#endif


// check definitions
// TODO: move to Abseil-lib
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef ABSL_HAVE_EXCEPTIONS
#	pragma detect_mismatch( "ABSL_HAVE_EXCEPTIONS", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_EXCEPTIONS", "0" )
#  endif

#  ifdef ABSL_IS_LITTLE_ENDIAN
#	pragma detect_mismatch( "ABSL_IS_LITTLE_ENDIAN", "1" )
#  else
#	pragma detect_mismatch( "ABSL_IS_LITTLE_ENDIAN", "0" )
#  endif

#  ifdef ABSL_USES_STD_ANY
#	pragma detect_mismatch( "ABSL_USES_STD_ANY", "1" )
#  else
#	pragma detect_mismatch( "ABSL_USES_STD_ANY", "0" )
#  endif

#  ifdef ABSL_USES_STD_OPTIONAL
#	pragma detect_mismatch( "ABSL_USES_STD_OPTIONAL", "1" )
#  else
#	pragma detect_mismatch( "ABSL_USES_STD_OPTIONAL", "0" )
#  endif

#  ifdef ABSL_USES_STD_VARIANT
#	pragma detect_mismatch( "ABSL_USES_STD_VARIANT", "1" )
#  else
#	pragma detect_mismatch( "ABSL_USES_STD_VARIANT", "0" )
#  endif

#  ifdef ABSL_USES_STD_STRING_VIEW
#	pragma detect_mismatch( "ABSL_USES_STD_STRING_VIEW", "1" )
#  else
#	pragma detect_mismatch( "ABSL_USES_STD_STRING_VIEW", "0" )
#  endif

#  ifdef ABSL_HAVE_MEMORY_SANITIZER
#	pragma detect_mismatch( "ABSL_HAVE_MEMORY_SANITIZER", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_MEMORY_SANITIZER", "0" )
#  endif

#  ifdef ABSL_HAVE_THREAD_SANITIZER
#	pragma detect_mismatch( "ABSL_HAVE_THREAD_SANITIZER", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_THREAD_SANITIZER", "0" )
#  endif

#  ifdef ABSL_HAVE_ADDRESS_SANITIZER
#	pragma detect_mismatch( "ABSL_HAVE_ADDRESS_SANITIZER", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_ADDRESS_SANITIZER", "0" )
#  endif

#  ifdef ABSL_HAVE_HWADDRESS_SANITIZER
#	pragma detect_mismatch( "ABSL_HAVE_HWADDRESS_SANITIZER", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_HWADDRESS_SANITIZER", "0" )
#  endif

#  ifdef ABSL_HAVE_LEAK_SANITIZER
#	pragma detect_mismatch( "ABSL_HAVE_LEAK_SANITIZER", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_LEAK_SANITIZER", "0" )
#  endif

#  ifdef ABSL_HAVE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION
#	pragma detect_mismatch( "ABSL_HAVE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION", "1" )
#  else
#	pragma detect_mismatch( "ABSL_HAVE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION", "0" )
#  endif

#  ifdef ABSL_INTERNAL_HAS_RTTI
#	pragma detect_mismatch( "ABSL_INTERNAL_HAS_RTTI", "1" )
#  else
#	pragma detect_mismatch( "ABSL_INTERNAL_HAS_RTTI", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

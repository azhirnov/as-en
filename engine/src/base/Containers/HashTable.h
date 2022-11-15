// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_ABSEIL

# ifdef AE_COMPILER_MSVC
#	pragma warning(push, 1)
#	pragma warning(disable: 4458)
#	pragma warning(disable: 4127)
#	pragma warning(disable: 4100)
#	pragma warning(disable: 4946)
# endif

# ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdouble-promotion"
# endif

# include "absl/container/flat_hash_map.h"
# include "absl/container/flat_hash_set.h"

# include "absl/container/node_hash_map.h"
# include "absl/container/node_hash_set.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning(pop)
# endif
# ifdef AE_COMPILER_CLANG
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

	
	/*//
	// Hash Map
	//
	template <typename Key,
			  typename Value,
			  typename Hasher	= std::hash< Key >,
			  typename KeyEq	= std::equal_to< Key >,
			  typename Alloc	= std::allocator< std::pair< const Key, Value >>
			 >
	using HashMap = absl::node_hash_map< Key, Value, Hasher, KeyEq, Alloc >;
	
	template <typename Key,
			  typename Value,
			  typename Hasher = std::hash<Key>,
			  typename KeyEq  = std::equal_to<Key>,
			  typename Alloc  = std::allocator<std::pair<const Key, Value>>>
	using HashMultiMap	= std::unordered_multimap< Key, Value, Hasher, KeyEq, Alloc >;


	//
	// Hash Set
	//
	template <typename Key,
			  typename Hasher	= std::hash< Key >,
			  typename KeyEq	= std::equal_to< Key >,
			  typename Alloc	= std::allocator< Key >
			 >
	using HashSet = absl::node_hash_set< Key, Hasher, KeyEq, Alloc >;*/

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


namespace std
{

#ifdef AE_ENABLE_ABSEIL
	template <typename Key,
			  typename Value,
			  typename Hasher,
			  typename KeyEq,
			  typename Alloc
			 >
	struct hash< AE::Base::FlatHashMap< Key, Value, Hasher, KeyEq, Alloc >> {
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
	struct hash< AE::Base::FlatHashSet< Key, Hasher, KeyEq, Alloc >> {
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

} // std

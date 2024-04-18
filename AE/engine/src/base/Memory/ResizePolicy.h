// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	//
	// Default Resize Policy
	//

	struct DefaultResizePolicy
	{
	private:
		static constexpr usize	_MinSize	= 16;

	public:
		ND_ static constexpr usize  Resize (usize count, bool allowReserve) __NE___
		{
			return Max( (allowReserve ? count * 2 : count), _MinSize );
		}

		ND_ static constexpr usize  Resize (Bytes elementSizeOf, usize count, bool allowReserve) __NE___
		{
			const usize		new_count	= Resize( count, allowReserve );
			const Bytes		new_size	= ResizeInBytes( elementSizeOf * new_count );
			const usize		result		= usize(new_size / elementSizeOf);
			ASSERT( result >= new_count );
			return result;
		}

		ND_ static constexpr Bytes  ResizeInBytes (Bytes newSize) __NE___
		{
			const bool		use_large_blocks	= (usize(newSize) >> 20) > 5;
			const Bytes		new_size			= AlignUp( newSize, use_large_blocks ? LargeAllocationSize : SmallAllocationSize );
			return new_size;
		}

		template <typename T>
		ND_ static constexpr usize  Resize (usize count, bool allowReserve) __NE___
		{
			return Resize( SizeOf<T>, count, allowReserve );
		}
	};


	//
	// Resize Policy template
	//

	template <typename Allocator>
	struct TResizePolicy {
		using type = DefaultResizePolicy;
	};

	template <typename Allocator>
	using ResizePolicy = typename TResizePolicy< Allocator >::type;


} // AE::Base

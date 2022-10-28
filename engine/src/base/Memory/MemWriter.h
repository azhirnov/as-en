// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Math.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{

	//
	// Memory Writer
	//

	struct MemWriter
	{
	// variables
	private:
		void *		_ptr	= null;
		usize		_offset	= 0;
		usize		_size;

		static constexpr Bytes	_MaxAlign {1u << 8};


	// methods
	public:
		//MemWriter ()														{}
		MemWriter (void *ptr, Bytes size) : _ptr{ptr}, _size{usize(size)}	{ ASSERT( Bytes{ptr} > _MaxAlign ); }	// IsAllocated() == true
		explicit MemWriter (Bytes align) : _ptr{align}, _size{UMax}			{ ASSERT( align <= _MaxAlign ); }		// IsAllocated() == false


		void  AlignTo (Bytes align)
		{
			Unused( Reserve( 0_b, align ));
		}

		ND_ void*  Reserve (Bytes size, Bytes align)
		{
			ASSERT( _ptr != null );
			usize	result = AlignUp( usize(_ptr) + _offset, usize(align) );

			_offset = (result - usize(_ptr)) + usize(size);
			ASSERT( _offset <= _size );

			return BitCast<void *>( result );
		}


		template <typename T>
		ND_ T&  Reserve ()
		{
			return *Cast<T>( Reserve( SizeOf<T>, AlignOf<T> ));
		}

		template <typename T, typename ...Args>
		ND_ T&  Emplace (Args&& ...args)
		{
			ASSERT( IsAllocated() );
			return *PlacementNew<T>( &Reserve<T>(), FwdArg<Args>( args )... );
		}

		template <typename T, typename ...Args>
		ND_ T&  EmplaceSized (Bytes size, Args&& ...args)
		{
			ASSERT( IsAllocated() );
			ASSERT( size >= SizeOf<T> );
			return *PlacementNew<T>( Reserve( size, AlignOf<T> ), FwdArg<Args>( args )... );
		}


		template <typename T>
		ND_ T*  ReserveArray (usize count)
		{
			return count ? Cast<T>( Reserve( SizeOf<T> * count, AlignOf<T> )) : null;
		}

		template <typename T, typename ...Args>
		ND_ T*  EmplaceArray (usize count, Args&& ...args)
		{
			ASSERT( IsAllocated() );
			T*	result = ReserveArray<T>( count );

			for (usize i = 0; i < count; ++i) {
				PlacementNew<T>( result + i, FwdArg<Args>( args )... );
			}
			return result;
		}


		void  Clear ()
		{
			ASSERT( IsAllocated() );
			ZeroMem( _ptr, Bytes{_size} );
		}


		ND_ Bytes  OffsetOf (void *ptr, Bytes defaultValue = UMax) const
		{
			if ( ptr ) {
				ASSERT( ptr >= _ptr and ptr < _ptr + Bytes{_size} );
				return Bytes{usize(ptr) - usize(_ptr)};
			}
			return defaultValue;
		}

		ND_ bool	IsAllocated ()		const	{ return Bytes{_ptr} > _MaxAlign; }
		ND_ Bytes	AllocatedSize ()	const	{ return Bytes{_offset}; }
		ND_ Bytes	MaxSize ()			const	{ return Bytes{_size}; }
		ND_ void*	Data ()				const	{ return _ptr; }
	};


} // AE::Base

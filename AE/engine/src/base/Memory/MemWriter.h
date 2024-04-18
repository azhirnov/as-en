// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	1. Without allocation:
		- Create 'MemWriter' with base align.
		- use only Reserve methods.
		- get current size with 'AllocatedSize()'

	2. With allocation:
		- Create 'MemWriter' with preallocated memory.
		- use Reserve/Emplace/Clear methods.
*/

#pragma once

#include "base/Math/Vec.h"
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

		static constexpr Bytes	_MaxAlign {1u << 10};


	// methods
	public:
		MemWriter (void* ptr, Bytes size)							__NE___;
		explicit MemWriter (Bytes align)							__NE___;

			void	AlignTo (Bytes align)							__NE___	{ Unused( Reserve( 0_b, align )); }

		ND_ void*	Reserve (Bytes size, Bytes align)				__NE___;
		ND_ void*	Reserve (const SizeAndAlign sa)					__NE___	{ return Reserve( sa.size, sa.align ); }

		template <typename T>
		ND_ T&		Reserve ()										__NE___	{ return *Cast<T>( Reserve( SizeAndAlignOf<T> )); }

		template <typename T>
		ND_ T*		ReserveArray (usize count)						__NE___;


		template <typename T, typename ...Args>
		ND_ T&		Emplace (Args&& ...args)						__NE___;

		template <typename T, typename ...Args>
		ND_ T&		EmplaceSized (Bytes size, Args&& ...args)		__NE___;

		template <typename T, typename ...Args>
		ND_ T*		EmplaceArray (usize count, Args&& ...args)		__NE___;


			void	Clear ()										__NE___;

		ND_ Bytes	OffsetOf (void* ptr, Bytes defaultValue = UMax)	C_NE___;

		ND_ bool	IsAllocated ()									C_NE___	{ return Bytes{_ptr} > _MaxAlign; }
		ND_ Bytes	AllocatedSize ()								C_NE___	{ return Bytes{_offset}; }
		ND_ Bytes	MaxSize ()										C_NE___	{ return Bytes{_size}; }
		ND_ void*	Data ()											C_NE___	{ return _ptr; }
	};



/*
=================================================
	constructor
=================================================
*/
	inline MemWriter::MemWriter (void* ptr, Bytes size) __NE___ :
		_ptr{ptr}, _size{usize(size)}
	{
		ASSERT( Bytes{ptr} > _MaxAlign );
		ASSERT( IsAllocated() );
	}

	inline MemWriter::MemWriter (Bytes align) __NE___ :
		_ptr{align}, _size{UMax}
	{
		ASSERT( align <= _MaxAlign );
		ASSERT( not IsAllocated() );
	}

/*
=================================================
	Reserve
=================================================
*/
	inline void*  MemWriter::Reserve (Bytes size, Bytes align) __NE___
	{
		ASSERT( _ptr != null );
		usize	result = AlignUp( usize(_ptr) + _offset, usize(align) );

		_offset = (result - usize(_ptr)) + usize(size);
		ASSERT( _offset <= _size );		// TODO: throw?

		return BitCast<void *>( result );
	}

/*
=================================================
	Emplace
=================================================
*/
	template <typename T, typename ...Args>
	T&  MemWriter::Emplace (Args&& ...args) __NE___
	{
		ASSERT( IsAllocated() );
		return *PlacementNew<T>( OUT &Reserve<T>(), FwdArg<Args>( args )... );
	}

/*
=================================================
	EmplaceSized
=================================================
*/
	template <typename T, typename ...Args>
	T&  MemWriter::EmplaceSized (Bytes size, Args&& ...args) __NE___
	{
		ASSERT( IsAllocated() );
		ASSERT( size >= SizeOf<T> );
		return *PlacementNew<T>( OUT Reserve( size, AlignOf<T> ), FwdArg<Args>( args )... );
	}

/*
=================================================
	ReserveArray
=================================================
*/
	template <typename T>
	T*  MemWriter::ReserveArray (usize count) __NE___
	{
		return	count > 0 ?
					Cast<T>( Reserve( SizeOf<T> * count, AlignOf<T> )) :
					null;
	}

/*
=================================================
	EmplaceArray
=================================================
*/
	template <typename T, typename ...Args>
	T*  MemWriter::EmplaceArray (usize count, Args&& ...args) __NE___
	{
		ASSERT( IsAllocated() );
		T*	result = ReserveArray<T>( count );

		for (usize i = 0; i < count; ++i) {
			PlacementNew<T>( OUT result + i, FwdArg<Args>( args )... );
		}
		return result;
	}

/*
=================================================
	Clear
=================================================
*/
	inline void  MemWriter::Clear () __NE___
	{
		ASSERT( IsAllocated() );
		ZeroMem( OUT _ptr, Bytes{_size} );
	}

/*
=================================================
	OffsetOf
=================================================
*/
	inline Bytes  MemWriter::OffsetOf (void* ptr, Bytes defaultValue) C_NE___
	{
		if_likely( ptr != null )
		{
			ASSERT( ptr >= _ptr and ptr < _ptr + Bytes{_size} );
			return Bytes{usize(ptr) - usize(_ptr)};
		}
		return defaultValue;
	}


} // AE::Base

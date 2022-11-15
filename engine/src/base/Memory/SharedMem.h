// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/Memory/IAllocator.h"
#include "base/Containers/ArrayView.h"
#include "base/Math/Math.h"

namespace AE::Base
{

	//
	// Shared Memory
	//

	class SharedMem final : public EnableRC< SharedMem >
	{
	// types
	public:
		using Allocator_t	= RC<IAllocator>;


	// variables
	private:
		void*			_ptr		= null;
		Bytes32u		_size;
		Bytes32u		_align;
		Allocator_t		_allocator;


	// methods
	public:
		ND_ void*	Data ()			const	{ return _ptr; }
		ND_ Bytes	Size ()			const	{ return _size; }
		ND_ auto	Allocator ()	const	{ return _allocator; }

		ND_ bool  Contains (const void* ptr, Bytes size = 0_b) const
		{
			return IsIntersects<const void*>( _ptr, _ptr + _size, ptr, ptr + size );
		}

		template <typename T>
		ND_ ArrayView<T>  ToView () const
		{
			ASSERT( _size % SizeOf<T> == 0 );
			ASSERT( AlignOf<T> <= _align );
			return ArrayView<T>{ Cast<T>(_ptr), _size / SizeOf<T> };
		}
		

		ND_ static RC<SharedMem>  Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign)
		{
			return Create( alloc, sizeAndAlign.size, sizeAndAlign.align );
		}

		ND_ static RC<SharedMem>  Create (Allocator_t alloc, Bytes size, Bytes align = SizeOf<void*>)
		{
			if_likely( alloc and size > 0 )
			{
				void*	ptr = alloc->Allocate( SizeAndAlign{ size, align });
				if_likely( ptr != null )
					return RC<SharedMem>{ new SharedMem{ ptr, size, align, alloc }};
			}
			return Default;
		}
		
		// implemented in 'threading' lib
		ND_ static RC<SharedMem>  Create (Bytes size, Bytes align = SizeOf<void*>);

		ND_ static Allocator_t  CreateAllocator ()
		{
			return SharedMem::Allocator_t{ new AllocatorImpl< UntypedAllocator >{} };
		}


	private:
		SharedMem (void* ptr, Bytes size, Bytes align, Allocator_t alloc) :
			_ptr{ptr}, _size{size}, _align{align}, _allocator{alloc}
		{}
		
		~SharedMem () override
		{
			if ( _ptr != null )
				_allocator->Deallocate( _ptr, SizeAndAlign{ _size, _align });
		}
	};


} // AE::Base

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Math/Bytes.h"
#include "base/Algorithms/Cast.h"
#include "base/Utils/RefCounter.h"

namespace AE::Base
{

	//
	// Allocator Reference
	//
	
	template <typename AllocatorType>
	struct AllocatorRef
	{
	// types
	public:
		using Allocator_t		= AllocatorType;
		using Self				= AllocatorRef< AllocatorType >;

		template <typename T>
		using StdAllocator_t	= StdAllocatorRef< T, AllocatorType* >;
		
		static constexpr bool	IsThreadSafe = AllocatorType::IsThreadSafe;


	// variables
	private:
		AllocatorType&	_alloc;
		

	// methods
	public:
		AllocatorRef (Self &&other) : _alloc{other._alloc} {}
		AllocatorRef (const Self &other) : _alloc{other._alloc} {}
		AllocatorRef (AllocatorType &alloc) : _alloc{alloc} {}

		ND_ AE_ALLOCATOR void*  Allocate (Bytes size)
		{
			return _alloc.Allocate( size );
		}
		
		template <typename T>
		ND_ T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count ));
		}

		void  Deallocate (void* ptr)
		{
			return _alloc.Deallocate( ptr );
		}

		void  Deallocate (void* ptr, Bytes size)
		{
			return _alloc.Deallocate( ptr, size );
		}

		ND_ AllocatorType&  GetAllocatorRef () const
		{
			return _alloc;
		}

		ND_ bool  operator == (const Self &rhs) const
		{
			return &_alloc == &rhs._alloc;
		}
	};



	//
	// Aligned Allocator Reference
	//
	
	template <typename AllocatorType>
	struct AlignedAllocatorRef
	{
	// types
	public:
		using Allocator_t		= AllocatorType;
		using Self				= AlignedAllocatorRef< AllocatorType >;

		template <typename T>
		using StdAllocator_t	= StdAllocatorRef< T, AllocatorType* >;
		
		static constexpr bool	IsThreadSafe = AllocatorType::IsThreadSafe;


	// variables
	private:
		AllocatorType&	_alloc;
		

	// methods
	public:
		AlignedAllocatorRef (Self &&other) : _alloc{other._alloc} {}
		AlignedAllocatorRef (const Self &other) : _alloc{other._alloc} {}
		AlignedAllocatorRef (AllocatorType &alloc) : _alloc{alloc} {}

		ND_ AE_ALLOCATOR void*  Allocate (Bytes size, Bytes align)
		{
			return _alloc.Allocate( size, align );
		}
		
		template <typename T>
		ND_ T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count, AlignOf<T> ));
		}

		void  Deallocate (void* ptr, Bytes size, Bytes align)
		{
			return _alloc.Deallocate( ptr, size, align );
		}

		ND_ AllocatorType&  GetAllocatorRef () const
		{
			return _alloc;
		}

		ND_ bool  operator == (const Self &rhs) const
		{
			return &_alloc == &rhs._alloc;
		}
	};



	//
	// Aligned Allocator Reference with base align
	//
	
	template <typename AllocatorType, usize BaseAlign>
	struct AlignedAllocatorRefBaseAlign
	{
	// types
	public:
		using Allocator_t		= AllocatorType;
		using Self				= AlignedAllocatorRefBaseAlign< AllocatorType, BaseAlign >;

		template <typename T>
		using StdAllocator_t	= StdAllocatorRef< T, AllocatorType* >;
		
		static constexpr usize	Align			= BaseAlign;
		static constexpr bool	IsThreadSafe	= AllocatorType::IsThreadSafe;


	// variables
	private:
		AllocatorType&	_alloc;
		

	// methods
	public:
		AlignedAllocatorRefBaseAlign (Self &&other) : _alloc{other._alloc} {}
		AlignedAllocatorRefBaseAlign (const Self &other) : _alloc{other._alloc} {}
		AlignedAllocatorRefBaseAlign (AllocatorType &alloc) : _alloc{alloc} {}

		ND_ AE_ALLOCATOR void*  Allocate (Bytes size)
		{
			return _alloc.Allocate( size, Bytes{BaseAlign} );
		}
		
		template <typename T>
		ND_ T*  Allocate (usize count = 1)
		{
			return Cast<T>( Allocate( SizeOf<T> * count ));
		}

		void  Deallocate (void* ptr, Bytes size)
		{
			return _alloc.Deallocate( ptr, size, Bytes{BaseAlign} );
		}

		ND_ AllocatorType&  GetAllocatorRef () const
		{
			return _alloc;
		}

		ND_ bool  operator == (const Self &rhs) const
		{
			return &_alloc == &rhs._alloc;
		}
	};
	


	//
	// Std Allocator Reference
	//

	template <typename T, typename TAllocatorPtr>
	struct StdAllocatorRef
	{
	// types
	public:
		using value_type		= T;
		using size_type			= usize;
		using difference_type	= ssize;
		using Self				= StdAllocatorRef< T, TAllocatorPtr >;
		
		// removed in C++20
		template <typename U>
		struct rebind {
			using other = StdAllocatorRef< U, TAllocatorPtr >;
		};

		STATIC_ASSERT( IsPointer<TAllocatorPtr> or IsRC<TAllocatorPtr> or IsSpecializationOf<TAllocatorPtr, Ptr> );


	// variables
	private:
		TAllocatorPtr	_alloc;


	// methods
	public:
		StdAllocatorRef (TAllocatorPtr alloc) : _alloc{FwdArg<TAllocatorPtr>(alloc)} {}
		StdAllocatorRef (Self &&other) : _alloc{RVRef(other._alloc)} {}
		StdAllocatorRef (const Self &other) : _alloc{other._alloc} {}
		
		template <typename B>
		StdAllocatorRef (const StdAllocatorRef<B,TAllocatorPtr>& other) : _alloc{other.GetAllocatorPtr()} {}

		Self&  operator = (const Self &) = delete;
		Self&  operator = (Self &&) = default;

		
		ND_ AE_ALLOCATOR T*  allocate (const usize count)
		{
			return Cast<T>( _alloc->Allocate( SizeOf<T> * count, AlignOf<T> ));
		}

		void  deallocate (T * const ptr, const usize count)
		{
			return _alloc->Deallocate( ptr, SizeOf<T> * count, AlignOf<T> );
		}

		ND_ Self  select_on_container_copy_construction () const
		{
			return Self{ *_alloc };
		}
		
		// removed in C++20
		ND_ usize  max_size () const
		{
			return usize(UMax) / sizeof(T);
		}

		ND_ TAllocatorPtr   GetAllocatorPtr () const
		{
			return _alloc;
		}

		ND_ bool  operator == (const Self &rhs) const
		{
			return _alloc == rhs._alloc;
		}
	};

} // AE::Base


namespace std
{

	template <typename T, typename A>
	void  swap (AE::Base::StdAllocatorRef<T,A> &lhs, AE::Base::StdAllocatorRef<T,A> &rhs)
	{
		auto	tmp = AE::Base::RVRef(lhs);
		lhs = AE::Base::RVRef(rhs);
		rhs = AE::Base::RVRef(tmp);
	}

} // std

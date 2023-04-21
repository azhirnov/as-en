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
	class AllocatorRef
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
		AllocatorRef (Self &&other)											__NE___ : _alloc{other._alloc} {}
		AllocatorRef (const Self &other)									__NE___ : _alloc{other._alloc} {}
		AllocatorRef (AllocatorType &alloc)									__NE___ : _alloc{alloc} {}

		ND_ void*  Allocate (Bytes size)									__NE___	{ return _alloc.Allocate( size ); }
		ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE___ { return _alloc.Allocate( sizeAndAlign ); }

		template <typename T>
		ND_ T*  Allocate (usize count = 1)									__NE___ { return Cast<T>( Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> })); }

		void  Deallocate (void* ptr)										__NE___	{ return _alloc.Deallocate( ptr ); }
		void  Deallocate (void* ptr, Bytes size)							__NE___	{ return _alloc.Deallocate( ptr, size ); }
		void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)		__NE___	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }

		ND_ AllocatorType&  GetAllocatorRef ()								C_NE___	{ return _alloc; }

		ND_ bool  operator == (const Self &rhs)								C_NE___	{ return &_alloc == &rhs._alloc; }
	};



	//
	// Allocator Reference
	//
	
	template <typename AllocatorType>
	class AllocatorRef2
	{
	// types
	public:
		using Allocator_t		= AllocatorType;
		using Self				= AllocatorRef2< AllocatorType >;

		template <typename T>
		using StdAllocator_t	= StdAllocatorRef< T, AllocatorType* >;
		
		static constexpr bool	IsThreadSafe = AllocatorType::IsThreadSafe;
		static constexpr Bytes	_BaseAlign	{sizeof(void*)};


	// variables
	private:
		AllocatorType&	_alloc;
		

	// methods
	public:
		AllocatorRef2 (Self &&other)										__NE___ : _alloc{other._alloc} {}
		AllocatorRef2 (const Self &other)									__NE___ : _alloc{other._alloc} {}
		AllocatorRef2 (AllocatorType &alloc)								__NE___ : _alloc{alloc} {}

		ND_ void*  Allocate (Bytes size)									__NE___	{ return _alloc.Allocate( SizeAndAlign{ size, _BaseAlign }); }
		ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)				__NE___	{ return _alloc.Allocate( sizeAndAlign ); }

		template <typename T>
		ND_ T*  Allocate (usize count = 1)									__NE___ { return Cast<T>( Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> })); }

		void  Deallocate (void* ptr)										__NE___ { return _alloc.Deallocate( ptr ); }
		void  Deallocate (void* ptr, Bytes size)							__NE___	{ return _alloc.Deallocate( ptr, SizeAndAlign{ size, _BaseAlign }); }
		void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)		__NE___	{ return _alloc.Deallocate( ptr, sizeAndAlign ); }

		ND_ AllocatorType&  GetAllocatorRef ()								C_NE___	{ return _alloc; }

		ND_ bool  operator == (const Self &rhs)								C_NE___	{ return &_alloc == &rhs._alloc; }
	};



	//
	// Aligned Allocator Reference with base align
	//
	
	template <typename AllocatorType, usize BaseAlign>
	class AlignedAllocatorRefBaseAlign
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
		AlignedAllocatorRefBaseAlign (Self &&other)			__NE___ : _alloc{other._alloc} {}
		AlignedAllocatorRefBaseAlign (const Self &other)	__NE___ : _alloc{other._alloc} {}
		AlignedAllocatorRefBaseAlign (AllocatorType &alloc)	__NE___ : _alloc{alloc} {}

		template <typename T>
		ND_ T*		Allocate (usize count = 1)				__NE___	{ STATIC_ASSERT( alignof(T) <= BaseAlign );  return Cast<T>( Allocate( SizeOf<T> * count )); }
		ND_ void*	Allocate (Bytes size)					__NE___	{ return _alloc.Allocate( SizeAndAlign{ size, Bytes{BaseAlign} }); }

		void  Deallocate (void* ptr, Bytes size)			__NE___	{ return _alloc.Deallocate( ptr, SizeAndAlign{ size, Bytes{BaseAlign} }); }

		ND_ AllocatorType&  GetAllocatorRef ()				C_NE___	{ return _alloc; }

		ND_ bool  operator == (const Self &rhs)				C_NE___	{ return &_alloc == &rhs._alloc; }
	};
	


	//
	// Std Allocator Reference
	//

	template <typename T, typename TAllocatorPtr>
	class StdAllocatorRef
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
		StdAllocatorRef (TAllocatorPtr alloc)				__NE___ : _alloc{FwdArg<TAllocatorPtr>(alloc)} {}
		StdAllocatorRef (Self &&other)						__NE___ : _alloc{RVRef(other._alloc)} {}
		StdAllocatorRef (const Self &other)					__NE___ : _alloc{other._alloc} {}
		
		template <typename B>
		StdAllocatorRef (const StdAllocatorRef<B,TAllocatorPtr>& other) __NE___ : _alloc{other.GetAllocatorPtr()} {}

		Self&  operator = (const Self &)							= delete;
		Self&  operator = (Self &&)							__NE___	= default;

		
		// returns non-null pointer
		ND_ T*  allocate (const usize count)				__Th___
		{
			T*	ptr = Cast<T>( _alloc->Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }));
			if_unlikely( ptr == null )
				throw std::bad_alloc{};
			return ptr;
		}

		void  deallocate (T * const ptr, const usize count)	__NE___	{ return _alloc->Deallocate( ptr, SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }); }

		ND_ Self  select_on_container_copy_construction ()	C_NE___	{ return Self{ *_alloc }; }
		
		// removed in C++20
		ND_ usize  max_size ()								C_NE___	{ return usize(UMax) / sizeof(T); }

		ND_ TAllocatorPtr   GetAllocatorPtr ()				C_NE___	{ return _alloc; }

		ND_ bool  operator == (const Self &rhs)				C_NE___	{ return _alloc == rhs._alloc; }
	};

} // AE::Base


namespace std
{

	template <typename T, typename A>
	void  swap (AE::Base::StdAllocatorRef<T,A> &lhs, AE::Base::StdAllocatorRef<T,A> &rhs) __NE___
	{
		auto	tmp = AE::Base::RVRef(lhs);
		lhs = AE::Base::RVRef(rhs);
		rhs = AE::Base::RVRef(tmp);
	}

} // std

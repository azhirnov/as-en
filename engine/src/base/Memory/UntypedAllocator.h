// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/MemUtils.h"
#include "base/Memory/AllocatorFwdDecl.h"

namespace AE::Base
{

	//
	// Untyped Default Allocator
	//

	struct UntypedAllocator
	{
	// types
		static constexpr bool	IsThreadSafe = true;

	// methods
		ND_ bool  operator == (const UntypedAllocator &)		C_NE___
		{
			return true;
		}

		// with default alignment
		ND_ AE_ALLOCATOR static void*  Allocate (Bytes size)	__NE___
		{
			return ::operator new ( usize(size), std::nothrow_t{} );
		}
		
		static void  Deallocate (void *ptr)						__NE___
		{
			::operator delete ( ptr, std::nothrow_t() );
		}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)			__NE___
		{
			//#if defined(AE_PLATFORM_LINUX) and defined(AE_COMPILER_GCC)
			//	::operator delete ( ptr );
			//#else
				::operator delete ( ptr, usize(size) );
			//#endif
		}


		// with custom alignment
		ND_ AE_ALLOCATOR static void*  Allocate (const SizeAndAlign sizeAndAlign) __NE___
		{
			return ::operator new ( usize(sizeAndAlign.size), std::align_val_t(usize(sizeAndAlign.align)), std::nothrow_t{} );
		}
		
		//static void  Deallocate (void *ptr, Bytes align) __NE___
		//{
		//	::operator delete ( ptr, std::align_val_t(usize(align)), std::nothrow_t() );
		//}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign) __NE___
		{
			//#if defined(AE_PLATFORM_LINUX) and defined(AE_COMPILER_GCC)
			//	::operator delete ( ptr, std::align_val_t(usize(sizeAndAlign.align)) );
			//#else
				::operator delete ( ptr, usize(sizeAndAlign.size), std::align_val_t(usize(sizeAndAlign.align)) );
			//#endif
		}
	};


	
	//
	// Untyped Allocator with base alignment
	//

	template <usize BaseAlign>
	struct UntypedAllocatorBaseAlign
	{
	// types
		static constexpr usize	Align			= BaseAlign;
		static constexpr bool	IsThreadSafe	= true;
		
	// methods
		ND_ AE_ALLOCATOR static void*  Allocate (Bytes size)	__NE___
		{
			return ::operator new ( usize(size), std::align_val_t(BaseAlign), std::nothrow_t{} );
		}
		
		static void  Deallocate (void *ptr)						__NE___
		{
			::operator delete ( ptr, std::align_val_t(BaseAlign), std::nothrow_t() );
		}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)			__NE___
		{
			#if defined(AE_PLATFORM_LINUX) and defined(AE_COMPILER_GCC)
				::operator delete ( ptr, std::align_val_t(BaseAlign) );
			#else
				::operator delete ( ptr, usize(size), std::align_val_t(BaseAlign) );
			#endif
		}

		ND_ bool  operator == (const UntypedAllocatorBaseAlign<BaseAlign> &) C_NE___
		{
			return true;
		}
	};


	
	//
	// On Stack Allocator
	//

	struct UntypedOnStackAllocator
	{
	// types
		static constexpr bool	IsThreadSafe = true;

	// methods
		ND_ AE_ALLOCATOR static void*	Allocate (Bytes size)		__NE___	{ return alloca( usize(size) ); }

			static void					Deallocate (void *)			__NE___	{}
	};

} // AE::Base

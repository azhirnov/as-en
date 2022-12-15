// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/MemUtils.h"
#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/AllocatorHelper.h"

namespace AE::Base
{

	//
	// Untyped Default Allocator
	//

	class UntypedAllocator
	{
	// types
	public:
		static constexpr bool	IsThreadSafe = true;
	private:
		using Helper_t = AllocatorHelper< EAllocatorType::Global >;

	// methods
	public:
		ND_ bool  operator == (const UntypedAllocator &)		C_NE___
		{
			return true;
		}

		// with default alignment
		ND_ static void*  Allocate (Bytes size)					__NE___
		{
			void*	ptr = ::operator new ( usize(size), std::nothrow_t{} );
			Helper_t::OnAllocate( ptr, size );
			return ptr;
		}
		
		static void  Deallocate (void *ptr)						__NE___
		{
			Helper_t::OnDeallocate( ptr );
			::operator delete ( ptr, std::nothrow_t() );
		}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)			__NE___
		{
			Helper_t::OnDeallocate( ptr, size );
			//#if defined(AE_PLATFORM_LINUX) and defined(AE_COMPILER_GCC)
			//	::operator delete ( ptr );
			//#else
				::operator delete ( ptr, usize(size) );
			//#endif
		}


		// with custom alignment
		ND_ static void*  Allocate (const SizeAndAlign sizeAndAlign) __NE___
		{
			void*	ptr = ::operator new ( usize(sizeAndAlign.size), std::align_val_t(usize(sizeAndAlign.align)), std::nothrow_t{} );
			Helper_t::OnAllocate( ptr, sizeAndAlign );
			return ptr;
		}
		
		//static void  Deallocate (void *ptr, Bytes align) __NE___
		//{
		//	::operator delete ( ptr, std::align_val_t(usize(align)), std::nothrow_t() );
		//}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign) __NE___
		{
			Helper_t::OnDeallocate( ptr, sizeAndAlign );
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
	class UntypedAllocatorBaseAlign
	{
	// types
	public:
		static constexpr usize	Align			= BaseAlign;
		static constexpr bool	IsThreadSafe	= true;
	private:
		using Helper_t = AllocatorHelper< EAllocatorType::Global >;
		
	// methods
	public:
		ND_ static void*  Allocate (Bytes size)					__NE___
		{
			void*	ptr = ::operator new ( usize(size), std::align_val_t(BaseAlign), std::nothrow_t{} );
			Helper_t::OnAllocate( ptr, size );
			return ptr;
		}
		
		static void  Deallocate (void *ptr)						__NE___
		{
			Helper_t::OnDeallocate( ptr );
			::operator delete ( ptr, std::align_val_t(BaseAlign), std::nothrow_t() );
		}
		
		// deallocation with explicit size may be faster
		static void  Deallocate (void *ptr, Bytes size)			__NE___
		{
			Helper_t::OnDeallocate( ptr, size );
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

	class UntypedOnStackAllocator
	{
	// types
	public:
		static constexpr bool	IsThreadSafe = true;

	// methods
	public:
		ND_ static void*	Allocate (Bytes size)		__NE___
		{
			void*	ptr = alloca( usize(size) );
			AllocatorHelper< EAllocatorType::OnStack >::OnAllocate( ptr, size );
			return ptr;
		}

			static void		Deallocate (void *)			__NE___	{}
	};

} // AE::Base

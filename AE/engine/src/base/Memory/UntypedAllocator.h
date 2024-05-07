// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"
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
		template <typename T>
		ND_ static T*     Allocate (usize count = 1)							__NE___
		{
			return Cast<T>( Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> }));
		}

		// with default alignment
		ND_ static void*  Allocate (Bytes size)									__NE___
		{
			void*	ptr = ::operator new( usize(size), std::nothrow );
			Helper_t::OnAllocate( ptr, size );
			return ptr;
		}

		static void  Deallocate (void* ptr)										__NE___
		{
			Helper_t::OnDeallocate( ptr );
			::operator delete( ptr );
			CheckNothrow( IsNoExcept( ::operator delete( ptr )));
		}

		// deallocation with explicit size may be faster
		static void  Deallocate (void* ptr, Bytes size)							__NE___
		{
			Helper_t::OnDeallocate( ptr, size );
			::operator delete( ptr, usize(size) );
			CheckNothrow( IsNoExcept( ::operator delete( ptr, usize(size) )));
		}


		// with custom alignment
		ND_ static void*  Allocate (const SizeAndAlign sizeAndAlign)			__NE___
		{
			void*	ptr = ::operator new( usize(sizeAndAlign.size), std::align_val_t(usize(sizeAndAlign.align)), std::nothrow );
			Helper_t::OnAllocate( ptr, sizeAndAlign );
			return ptr;
		}

		// deallocation with explicit size may be faster
		static void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign)	__NE___
		{
			Helper_t::OnDeallocate( ptr, sizeAndAlign );
			::operator delete( ptr, usize(sizeAndAlign.size), std::align_val_t(usize(sizeAndAlign.align)) );
			CheckNothrow( IsNoExcept( ::operator delete( ptr, 0, std::align_val_t{0} )));
		}

		ND_ bool  operator == (const UntypedAllocator &)						C_NE___
		{
			return true;
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
		template <typename T>
		ND_ static T*     Allocate (usize count = 1)							__NE___
		{
			StaticAssert( alignof(T) <= BaseAlign );
			return Cast<T>( Allocate( SizeOf<T> * count ));
		}

		ND_ static void*  Allocate (Bytes size)									__NE___
		{
			void*	ptr = ::operator new( usize(size), std::align_val_t(BaseAlign), std::nothrow );
			Helper_t::OnAllocate( ptr, size );
			return ptr;
		}

		static void  Deallocate (void* ptr)										__NE___
		{
			Helper_t::OnDeallocate( ptr );
			::operator delete( ptr, std::align_val_t(BaseAlign) );
			CheckNothrow( IsNoExcept( ::operator delete( ptr, std::align_val_t{0} )));
		}

		// deallocation with explicit size may be faster
		static void  Deallocate (void* ptr, Bytes size)							__NE___
		{
			Helper_t::OnDeallocate( ptr, size );
			::operator delete( ptr, usize(size), std::align_val_t(BaseAlign) );
			CheckNothrow( IsNoExcept( ::operator delete( ptr, 0, std::align_val_t{0} )));
		}

		ND_ bool  operator == (const UntypedAllocatorBaseAlign<BaseAlign> &)	C_NE___
		{
			return true;
		}
	};



	//
	// On Stack Allocator
	//

#ifdef AE_COMPILER_MSVC
# if 0 //def AE_ENABLE_EXCEPTIONS
#	define AllocateOnStack2( _outPtr_, _sizeInBytes_ )												\
		{																							\
			StaticAssert( IsBytes< RemoveCV< decltype(_sizeInBytes_) >>);							\
			_outPtr_ = null;																		\
			__try {																					\
				ASSERT( _sizeInBytes_ <= 1_Kb );			/* _ALLOCA_S_THRESHOLD */				\
				_outPtr_ = Cast< RemovePointer<decltype(_outPtr_)> >(_alloca( usize(_sizeInBytes_) ));\
				AllocatorHelper< EAllocatorType::OnStack >::OnAllocate( _outPtr_, _sizeInBytes_ );	\
			}																						\
			__except( GetExceptionCode() == AE_SEH_STACK_OVERFLOW ?									\
						EXCEPTION_EXECUTE_HANDLER :													\
						EXCEPTION_CONTINUE_SEARCH )													\
			{																						\
				_resetstkoflw();																	\
			}																						\
		}
# else
#	define AllocateOnStack2( _outPtr_, _sizeInBytes_ )												\
		{																							\
			StaticAssert( IsBytes< RemoveCV< decltype(_sizeInBytes_) >>);							\
			ASSERT( (_sizeInBytes_) <= 1_Kb );			/* _ALLOCA_S_THRESHOLD */					\
			_outPtr_ = Cast< RemovePointer<decltype(_outPtr_)> >(_alloca( usize(_sizeInBytes_) ));	\
			AllocatorHelper< EAllocatorType::OnStack >::OnAllocate( _outPtr_, _sizeInBytes_ );		\
		}
# endif // AE_ENABLE_EXCEPTIONS

#else
#	define AllocateOnStack2( _outPtr_, _sizeInBytes_ )												\
		{																							\
			StaticAssert( IsBytes< RemoveCV< decltype(_sizeInBytes_) >>);							\
			ASSERT( (_sizeInBytes_) <= 1_Kb );			/* _ALLOCA_S_THRESHOLD */					\
			_outPtr_ = Cast< RemovePointer<decltype(_outPtr_)> >(alloca( usize(_sizeInBytes_) ));	\
			AllocatorHelper< EAllocatorType::OnStack >::OnAllocate( _outPtr_, _sizeInBytes_ );		\
		}

#endif


#	define AllocateOnStack( _outPtr_, _count_ )														\
		StaticAssert( IsInteger< RemoveCV< decltype(_count_) >>);									\
		AllocateOnStack2( _outPtr_, (SizeOf<RemovePointer<decltype(_outPtr_)>> * _count_) );

#	define AllocateOnStack_WithCtor( _outPtr_, _count_ )											\
		{																							\
			StaticAssert( IsInteger< RemoveCV< decltype(_count_) >>);								\
			AllocateOnStack2( _outPtr_, (SizeOf<RemovePointer<decltype(_outPtr_)>> * _count_) );	\
			for (usize i = 0, cnt = _count_; i < cnt; ++i) {										\
				PlacementNew< RemovePointer<decltype(_outPtr_)> >( OUT _outPtr_ + i );				\
			}																						\
		}

#	define AllocateOnStack_ZeroMem( _outPtr_, _count_ )												\
		{																							\
			StaticAssert( IsInteger< RemoveCV< decltype(_count_) >>);								\
			AllocateOnStack2( _outPtr_, (SizeOf<RemovePointer<decltype(_outPtr_)>> * _count_) );	\
			ZeroMem( OUT _outPtr_, _count_ );														\
		}


} // AE::Base

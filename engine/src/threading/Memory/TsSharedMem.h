// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/SharedMem.h"
#include "threading/Primitives/SpinLock.h"

namespace AE::Threading
{

	//
	// Thread-safe Shared Memory
	//

	class alignas(AE_CACHE_LINE) TsSharedMem final : public EnableRC< TsSharedMem >
	{
	// types
	public:
		using Allocator_t	= RC<IAllocator>;

	private:

		//
		// Write Access
		//
		struct WriteAccess : MovableOnly
		{
		private:
			TsSharedMem *	_ref;
			
			friend class TsSharedMem;
			WriteAccess (TsSharedMem* ref)	__NE___ : _ref{ref} {}

		public:
			~WriteAccess ()					__NE___	{ if ( _ref != null ) _ref->_guard.unlock(); }

			ND_ void*	Data ()				C_NE___	{ return _ref != null ? _ref->_ptr : null; }
			ND_ Bytes	Size ()				C_NE___	{ return _ref != null ? Bytes{_ref->_size} : 0_b; }
			ND_ Bytes	Align ()			C_NE___	{ return _ref != null ? Bytes{_ref->_align} : 0_b; }
		};


		//
		// Read Access
		//
		struct ReadAccess : MovableOnly
		{
		private:
			TsSharedMem *	_ref;
			
			friend class TsSharedMem;
			ReadAccess (TsSharedMem* ref)	__NE___ : _ref{ref} {}

		public:
			~ReadAccess ()					__NE___	{ if ( _ref != null ) _ref->_guard.unlock_shared(); }
			
			ND_ void*	Data ()				C_NE___	{ return _ref != null ? _ref->_ptr : null; }
			ND_ Bytes	Size ()				C_NE___	{ return _ref != null ? Bytes{_ref->_size} : 0_b; }
			ND_ Bytes	Align ()			C_NE___	{ return _ref != null ? Bytes{_ref->_align} : 0_b; }

			template <typename T>
			ND_ ArrayView<T>  ToView ()		C_NE___
			{
				ASSERT( Size() % SizeOf<T> == 0 );
				ASSERT( AlignOf<T> <= Align() );
				return ArrayView<T>{ Cast<T>(Data()), Size() / SizeOf<T> };
			}
		};
		

	// variables
	private:
		RWSpinLock		_guard;		// used to protect access to '_ptr'
		void*			_ptr		= null;
		Bytes32u		_size;
		Bytes32u		_align;
		Allocator_t		_allocator;


	// methods
	public:
		ND_ Bytes		Size ()			C_NE___	{ return _size; }
		ND_ auto		Allocator ()	C_NE___	{ return _allocator; }

		ND_ ReadAccess	Read ()			__NE___	{ return { _guard.try_lock_shared() ? this : null }; }
		ND_ WriteAccess	Write ()		__NE___	{ return { _guard.try_lock() ? this : null }; }

		ND_ static RC<TsSharedMem>  Create (Allocator_t alloc, Bytes size, Bytes align = SizeOf<void*>);
		ND_ static RC<TsSharedMem>  Create (Bytes size, Bytes align = SizeOf<void*>);


	private:
		TsSharedMem (void* ptr, Bytes size, Bytes align, Allocator_t alloc) __NE___ :
			_ptr{ptr}, _size{size}, _align{align}, _allocator{alloc}
		{}
		
		~TsSharedMem () __NE_OV
		{
			if ( _ptr != null )
				_allocator->Deallocate( _ptr, SizeAndAlign{ _size, _align });
		}
	};


} // AE::Threading

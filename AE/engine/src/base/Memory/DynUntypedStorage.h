// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	DynUntypedStorage can be used as static variable.
	For RC object use SharedMem type.
*/

#pragma once

#include "base/Pointers/RefCounter.h"
#include "base/Pointers/RestrictPtr.h"
#include "base/Math/POTValue.h"
#include "base/Memory/IAllocator.h"

namespace AE::Base
{

	//
	// Dynamic Untyped Storage
	//
	struct DynUntypedStorage final : MovableOnly
	{
	// variables
	private:
		RstPtr<void>	_ptr;

	#if AE_PLATFORM_BITS == 32
		uint			_size		: 28;
		uint			_alignPOT	: 4;
	#elif AE_PLATFORM_BITS == 64
		usize			_size		: 56;
		usize			_alignPOT	: 8;
	#endif

		DEBUG_ONLY( RC<IAllocator>	_dbgAllocator;)


	// methods
	public:
		DynUntypedStorage ()											__NE___ : _size{0}, _alignPOT{0} {}
		~DynUntypedStorage ()											__NE___ { Dealloc( null ); }

		DynUntypedStorage (DynUntypedStorage &&)						__NE___;
		DynUntypedStorage&  operator = (DynUntypedStorage &&)			__NE___;

		explicit DynUntypedStorage (Bytes size, Bytes align = DefaultAllocatorAlign, IAllocator* alloc = null)	__NE___;
		explicit DynUntypedStorage (SizeAndAlign sizeAndAlign, IAllocator* alloc = null)						__NE___;

		ND_ Bytes				Size ()									C_NE___	{ return Bytes{usize{_size} << _alignPOT}; }
		ND_ POTBytes			AlignPOT ()								C_NE___	{ return POTBytes{PowerOfTwo( _alignPOT )}; }
		ND_ Bytes				Align ()								C_NE___	{ return Bytes{ AlignPOT() }; }
		ND_ bool				Empty ()								C_NE___	{ return _ptr == null; }

		ND_ RstPtr<void>		Data ()									__NE___	{ return _ptr; }
		ND_ RstPtr<const void>	Data ()									C_NE___	{ return _ptr; }

		template <typename T>
		ND_ RstPtr<T>			Data ()									__NE___	{ return RstPtr<T>{ Cast<T>( _ptr.get() )}; }

		template <typename T>
		ND_ RstPtr<const T>		Data ()									C_NE___	{ return RstPtr<const T>{ Cast<T>( _ptr.get() )}; }

		ND_ const void*			Begin ()								C_NE___	{ return _ptr.get(); }
		ND_ const void*			End ()									C_NE___	{ return _ptr.get() + Size(); }

		ND_ explicit operator bool ()									C_NE___	{ return _ptr != null; }

			bool  Alloc (Bytes size, Bytes align, IAllocator* alloc)	__NE___	{ return Alloc( SizeAndAlign{size,align}, alloc ); }
			bool  Alloc (SizeAndAlign sizeAndAlign, IAllocator* alloc)	__NE___;
			void  Dealloc (IAllocator* alloc)							__NE___;

		template <typename T>
		ND_ T*			Ptr (Bytes offset = 0_b)						__NE___	{ ASSERT( SizeOf<T> + offset <= Size() );  return Cast<T>( _ptr.get() + offset ); }

		template <typename T>
		ND_ T const*	Ptr (Bytes offset = 0_b)						C_NE___	{ ASSERT( SizeOf<T> + offset <= Size() );  return Cast<T>( _ptr.get() + offset ); }

		ND_ void*		Ptr (Bytes offset = 0_b)						__NE___ { ASSERT( offset <= Size() );  return _ptr.get() + offset; }
		ND_ void const*	Ptr (Bytes offset = 0_b)						C_NE___	{ ASSERT( offset <= Size() );  return _ptr.get() + offset; }

		template <typename T>
		ND_ T&			Ref (Bytes offset = 0_b)						__NE___	{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&	Ref (Bytes offset = 0_b)						C_NE___	{ return *Ptr<T>( offset ); }
	};

	StaticAssertDbg( sizeof(DynUntypedStorage) == sizeof(void*)*2 + sizeof(RC<IAllocator>) );
	StaticAssertRel( sizeof(DynUntypedStorage) == sizeof(void*)*2 );
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline DynUntypedStorage::DynUntypedStorage (Bytes size, Bytes align, IAllocator* allocator) __NE___ :
		_size{0}, _alignPOT{0}
	{
		Alloc( SizeAndAlign{ size, align }, allocator );
	}

	inline DynUntypedStorage::DynUntypedStorage (SizeAndAlign sizeAndAlign, IAllocator* allocator) __NE___ :
		_size{0}, _alignPOT{0}
	{
		Alloc( sizeAndAlign, allocator );
	}

	inline DynUntypedStorage::DynUntypedStorage (DynUntypedStorage &&other) __NE___ :
		_ptr{ other._ptr }, _size{ other._size }, _alignPOT{ other._alignPOT }
		DEBUG_ONLY(, _dbgAllocator{ RVRef(other._dbgAllocator) })
	{
		other._ptr = null;
	}

	inline DynUntypedStorage&  DynUntypedStorage::operator = (DynUntypedStorage &&rhs) __NE___
	{
		_ptr		= rhs._ptr;
		_size		= rhs._size;
		_alignPOT	= rhs._alignPOT;
		DEBUG_ONLY( _dbgAllocator = RVRef(rhs._dbgAllocator); )
		rhs._ptr = null;
		return *this;
	}

/*
=================================================
	Alloc
=================================================
*/
	inline bool  DynUntypedStorage::Alloc (const SizeAndAlign sizeAndAlign, IAllocator* allocator) __NE___
	{
		if_unlikely( _ptr != null )
			return false;

		if_unlikely( allocator == null )
			allocator = AE::GetDefaultAllocatorPtr().get();

		_alignPOT	= POTBytes{ sizeAndAlign.align }.GetPOT();
		_size		= (usize(sizeAndAlign.size) + ((1u<<_alignPOT)-1)) >> _alignPOT;

		CHECK_ERR( Size() >= sizeAndAlign.size );
		CHECK_ERR( Align() == sizeAndAlign.align );

		_ptr = RstPtr<void>{ allocator->Allocate( SizeAndAlign{ Size(), Align() })};

		DEBUG_ONLY( _dbgAllocator = allocator->GetRC(); )
		return _ptr != null;
	}

/*
=================================================
	Dealloc
=================================================
*/
	inline void  DynUntypedStorage::Dealloc (IAllocator* allocator) __NE___
	{
		if_likely( _ptr != null )
		{
			if_unlikely( allocator == null )
				allocator = AE::GetDefaultAllocatorPtr().get();

			ASSERT( _dbgAllocator == allocator );
			allocator->Deallocate( _ptr.get(), SizeAndAlign{ Size(), Align() });
		}

		_ptr		= null;
		_size		= 0;
		_alignPOT	= 0;
	}

} // AE::Base

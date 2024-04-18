// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	UntypedStorage and DynUntypedStorage can be used as static variable.
	For RC object use SharedMem type.
*/

#pragma once

#include "base/CompileTime/TypeList.h"
#include "base/Math/Byte.h"
#include "base/Math/POTValue.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Memory/IAllocator.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/RestrictPtr.h"

namespace AE::Base
{

	//
	// Static Untyped Storage
	//
	template <usize Size_v, usize Align_v>
	struct UntypedStorage final : Noncopyable
	{
	// types
	public:
		using Self = UntypedStorage< Size_v, Align_v >;


	// variables
	private:
		alignas(Align_v) char	_buffer [Size_v];


	// methods
	public:
		template <typename T>
		explicit UntypedStorage (const T &value)				__NE___;

		template <typename T>
		explicit UntypedStorage (TypeList<T>)					__NE___;

		UntypedStorage ()										__NE___	{ DEBUG_ONLY( DbgInitMem( _buffer, Sizeof(_buffer) )); }
		~UntypedStorage ()										__NE___	{ DEBUG_ONLY( DbgFreeMem( _buffer, Sizeof(_buffer) )); }

		template <typename T>
		ND_ T*			Ptr (Bytes offset = 0_b)				__NE___;

		template <typename T>
		ND_ T const*	Ptr (Bytes offset = 0_b)				C_NE___	{ return const_cast<Self*>(this)->Ptr<T>( offset ); }

		template <typename T>
		ND_ T&			Ref (Bytes offset = 0_b)				__NE___	{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&	Ref (Bytes offset = 0_b)				C_NE___	{ return *Ptr<T>( offset ); }

		ND_ static constexpr Bytes		Size ()					__NE___	{ return Bytes{Size_v}; }
		ND_ static constexpr Bytes		Align ()				__NE___	{ return Bytes{Align_v}; }
	};



	//
	// Dynamic Untyped Storage
	//
	struct DynUntypedStorage final : MovableOnly
	{
	// variables
	private:
		RstPtr<void>	_ptr;
		usize			_size	: 28;
		usize			_align	: 4;

		DEBUG_ONLY( RC<IAllocator>	_dbgAllocator;)


	// methods
	public:
		DynUntypedStorage ()											__NE___ : _size{0}, _align{0} {}
		~DynUntypedStorage ()											__NE___ { Dealloc( null ); }

		DynUntypedStorage (DynUntypedStorage &&)						__NE___;
		DynUntypedStorage&  operator = (DynUntypedStorage &&)			__NE___;

		explicit DynUntypedStorage (Bytes size, Bytes align = DefaultAllocatorAlign, IAllocator* alloc = null)	__NE___;
		explicit DynUntypedStorage (SizeAndAlign sizeAndAlign, IAllocator* alloc = null)						__NE___;

		ND_ Bytes				Size ()									C_NE___	{ return Bytes{_size}; }
		ND_ POTBytes			AlignPOT ()								C_NE___	{ return POTBytes{PowerOfTwo( _align )}; }
		ND_ Bytes				Align ()								C_NE___	{ return Bytes{ AlignPOT() }; }
		ND_ bool				Empty ()								C_NE___	{ return _ptr == null; }

		ND_ RstPtr<void>		Data ()									__NE___	{ return _ptr; }
		ND_ RstPtr<const void>	Data ()									C_NE___	{ return RstPtr<const void>{ _ptr.get() }; }

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


	template <typename T, usize Capacity>
	using UninitializedStaticArray = UntypedStorage< sizeof(T) * Capacity, alignof(T) >;

//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	UntypedStorage<S,A>::UntypedStorage (const T &value) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );

		std::memcpy( OUT _buffer, &value, S );
	}

	template <usize S, usize A>
	template <typename T>
	UntypedStorage<S,A>::UntypedStorage (TypeList<T>) __NE___
	{
		StaticAssert( sizeof(T) == S );
		StaticAssert( alignof(T) <= A );

		PlacementNew<T>( OUT _buffer );
	}

/*
=================================================
	Ptr
=================================================
*/
	template <usize S, usize A>
	template <typename T>
	T*  UntypedStorage<S,A>::Ptr (Bytes offset) __NE___
	{
		StaticAssert( sizeof(T) <= S );
		StaticAssert( alignof(T) <= A );

		ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
		return Cast<T>( _buffer + offset );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline DynUntypedStorage::DynUntypedStorage (Bytes size, Bytes align, IAllocator* allocator) __NE___ :
		_size{0}, _align{0}
	{
		Alloc( SizeAndAlign{ size, align }, allocator );
	}

	inline DynUntypedStorage::DynUntypedStorage (SizeAndAlign sizeAndAlign, IAllocator* allocator) __NE___ :
		_size{0}, _align{0}
	{
		Alloc( sizeAndAlign, allocator );
	}

	inline DynUntypedStorage::DynUntypedStorage (DynUntypedStorage &&other) __NE___ :
		_ptr{ other._ptr }, _size{ other._size }, _align{ other._align }
		DEBUG_ONLY(, _dbgAllocator{ RVRef(other._dbgAllocator) })
	{
		other._ptr = null;
	}

	inline DynUntypedStorage&  DynUntypedStorage::operator = (DynUntypedStorage &&rhs) __NE___
	{
		_ptr	= rhs._ptr;
		_size	= rhs._size;
		_align	= rhs._align;
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

		_size	= usize(sizeAndAlign.size);
		_align	= POTBytes( sizeAndAlign.align ).GetPOT();

		ASSERT( Size() == sizeAndAlign.size );
		ASSERT( Align() == sizeAndAlign.align );

		_ptr = RstPtr<void>{ allocator->Allocate( sizeAndAlign )};

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

		_ptr	= null;
		_size	= 0;
		_align	= 0;
	}

} // AE::Base

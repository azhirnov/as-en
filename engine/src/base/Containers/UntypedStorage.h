// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/POTValue.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Utils/Helpers.h"

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
		UntypedStorage ()
		{
			DEBUG_ONLY( DbgInitMem( _buffer, Sizeof(_buffer) ));
		}
		
		//explicit UntypedStorage (_hidden_::_Zero) : _buffer{}
		//{}
		
		template <typename T>
		explicit UntypedStorage (const T &value)
		{
			STATIC_ASSERT( sizeof(T) == Size_v );
			STATIC_ASSERT( alignof(T) <= Align_v );

			std::memcpy( OUT _buffer, &value, Size_v );
		}

		~UntypedStorage ()
		{
			DEBUG_ONLY( DbgFreeMem( _buffer, Sizeof(_buffer) ));
		}

		template <typename T>
		ND_ T*  Ptr (Bytes offset = 0_b)
		{
			ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
			return Base::Cast<T>( _buffer + offset );
		}

		template <typename T>
		ND_ T const*  Ptr (Bytes offset = 0_b) const
		{
			ASSERT( SizeOf<T> + offset <= sizeof(_buffer) );
			return Base::Cast<T>( _buffer + offset );
		}

		template <typename T>
		ND_ T&  Ref (Bytes offset = 0_b)				{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&  Ref (Bytes offset = 0_b) const	{ return *Ptr<T>( offset ); }

		ND_ Bytes	Size ()		const	{ return Bytes{Size_v}; }
		ND_ Bytes	Align ()	const	{ return Bytes{Align_v}; }
	};


	
	//
	// Dynamic Untyped Storage
	//

	template <typename AllocType = UntypedAllocator>
	struct DynUntypedStorage final : Noncopyable
	{
	// types
	public:
		using Self			= DynUntypedStorage< AllocType >;
		using Allocator_t	= AllocType;


	// variables
	private:
		void * RST	_ptr	= null;
		usize		_size	: 28;
		usize		_align	: 4;


	// methods
	public:
		DynUntypedStorage () __NE___ {}
		DynUntypedStorage (Bytes size, Bytes align) __NE___ { Alloc( SizeAndAlign{ size, align }); }
		explicit DynUntypedStorage (const SizeAndAlign sizeAndAlign) __NE___ { Alloc( sizeAndAlign ); }

		~DynUntypedStorage ()				__NE___ { Dealloc(); }

		ND_ Bytes			Size ()			C_NE___	{ return Bytes{_size}; }
		ND_ POTValue		AlignPOT ()		C_NE___	{ return POTValue{ubyte( _align )}; }
		ND_ Bytes			Align ()		C_NE___	{ return Bytes{usize{ AlignPOT() }}; }
		
		NDRST(void * )		Data ()			__NE___	{ return _ptr; }
		NDRST(const void*)	Data ()			C_NE___	{ return _ptr; }


		ND_ explicit operator bool ()		C_NE___	{ return _ptr != null; }


		bool  Alloc (const SizeAndAlign sizeAndAlign) __NE___
		{
			Dealloc();
			
			_size	= usize(sizeAndAlign.size);
			_align	= POTValue::From( usize(sizeAndAlign.align) ).Get();

			ASSERT( Size() == sizeAndAlign.size );
			ASSERT( Align() == sizeAndAlign.align );

			_ptr = Allocator_t::Allocate( sizeAndAlign );
			return _ptr != null;
		}

		void  Dealloc ()						__NE___
		{
			if ( _ptr != null )
				Allocator_t::Deallocate( _ptr, SizeAndAlign{ Size(), Align() });

			_ptr	= null;
			_size	= 0;
			_align	= 0;
		}


		template <typename T>
		ND_ T*  Ptr (Bytes offset = 0_b)		__NE___
		{
			ASSERT( SizeOf<T> + offset <= Size() );
			return Base::Cast<T>( _ptr + offset );
		}

		template <typename T>
		ND_ T const*  Ptr (Bytes offset = 0_b) C_NE___
		{
			ASSERT( SizeOf<T> + offset <= Size() );
			return Base::Cast<T>( _ptr + offset );
		}

		template <typename T>
		ND_ T&  Ref (Bytes offset = 0_b)		__NE___	{ return *Ptr<T>( offset ); }

		template <typename T>
		ND_ T const&  Ref (Bytes offset = 0_b)	C_NE___	{ return *Ptr<T>( offset ); }
	};


} // AE::Base

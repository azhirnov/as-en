// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/RefCounter.h"
#include "base/Memory/IAllocator.h"
#include "base/Containers/ArrayView.h"
#include "base/Math/Math.h"
#include "base/Math/POTValue.h"

namespace AE::Base
{
namespace _hidden_
{
	template <typename T>
	struct TSharedMem_Extra {
		T		data {};
	};

	template <>
	struct TSharedMem_Extra<void> {};
}


	//
	// Shared Memory
	//

	template <typename ExtraDataType>
	class TSharedMem final :
		public EnableRC< TSharedMem<ExtraDataType> >,
		public NonAllocatable
	{
	// types
	public:
		using Allocator_t	= RC<IAllocator>;
		using Self			= TSharedMem< ExtraDataType >;

	private:
		using Extra_t		= Base::_hidden_::TSharedMem_Extra< ExtraDataType >;

		static constexpr bool	_HasExtra = not IsVoid< ExtraDataType >;


	// variables
	private:
		Bytes32u		_size;
		POTValue		_align;
		Extra_t			_extra;
		Allocator_t		_allocator;


	// methods
	public:
		ND_ void const*		Data ()			C_NE___	{ return this + AlignUp( SizeOf<Self>, _align ); }
		ND_ void*			Data ()			__NE___	{ return this + AlignUp( SizeOf<Self>, _align ); }
		ND_ Bytes			Size ()			C_NE___	{ return _size; }
		ND_ Bytes			Align ()		C_NE___	{ return Bytes{ _align.ToValue<usize>() }; }
		ND_ auto			Allocator ()	C_NE___	{ return _allocator; }
		
		ND_ auto&			Extra ()		__NE___	{ if constexpr( _HasExtra ) return &_extra.data; }
		ND_ auto&			Extra ()		C_NE___	{ if constexpr( _HasExtra ) return &_extra.data; }

		ND_ bool			Contains (const void* ptr, Bytes size = 0_b) C_NE___;

		template <typename T>
		ND_ ArrayView<T>	ToView ()		C_NE___;
		

		ND_ static Allocator_t	CreateAllocator ()															__NE___;
		ND_ static RC<Self>		Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign)					__NE___;
		ND_ static RC<Self>		Create (Allocator_t alloc, Bytes size, Bytes align = DefaultAllocatorAlign)	__NE___;


	private:
		TSharedMem (Bytes size, POTValue align, Allocator_t alloc)		__NE___ : _size{size}, _align{align}, _allocator{alloc} {}
		~TSharedMem ()													__NE_OV	{}

		void  _ReleaseObject ()											__NE_OV;

		ND_ static SizeAndAlign  _CalcSize (Bytes size, POTValue align)	__NE___;
	};

	using SharedMem = TSharedMem<void>;

	
/*
=================================================
	Contains
=================================================
*/
	template <typename E>
	bool  TSharedMem<E>::Contains (const void* ptr, Bytes size) C_NE___
	{
		return IsIntersects<const void*>( Data(), Data() + _size, ptr, ptr + size );
	}
	
/*
=================================================
	ToView
=================================================
*/
	template <typename E>
	template <typename T>
	ArrayView<T>  TSharedMem<E>::ToView () C_NE___
	{
		ASSERT( _size % SizeOf<T> == 0 );
		ASSERT( POTAlignOf<T> <= _align );
		return ArrayView<T>{ Cast<T>(Data()), _size / SizeOf<T> };
	}
		
/*
=================================================
	Create
=================================================
*/
	template <typename E>
	RC<TSharedMem<E>>  TSharedMem<E>::Create (Allocator_t alloc, const SizeAndAlign sizeAndAlign) __NE___
	{
		return Create( alloc, sizeAndAlign.size, sizeAndAlign.align );
	}
	
/*
=================================================
	Create
=================================================
*/
	template <typename E>
	RC<TSharedMem<E>>  TSharedMem<E>::Create (Allocator_t alloc, Bytes size, Bytes align) __NE___
	{
		if_likely( alloc and size > 0 )
		{
			auto	align_pot	= POTValue::From( align );
			void*	self		= alloc->Allocate( _CalcSize( size, align_pot ));

			if_likely( self != null )
				return RC<Self>{ new(self) Self{ size, align_pot, RVRef(alloc) }};
		}
		return Default;
	}
	
/*
=================================================
	CreateAllocator
=================================================
*/
	template <typename E>
	typename TSharedMem<E>::Allocator_t  TSharedMem<E>::CreateAllocator () __NE___
	{
		CATCH_ERR(
			return Self::Allocator_t{ new AllocatorImpl< UntypedAllocator >{} };
		)
		return null;
	}
	
/*
=================================================
	_CalcSize
=================================================
*/
	template <typename E>
	SizeAndAlign  TSharedMem<E>::_CalcSize (Bytes size, POTValue align) __NE___
	{
		return SizeAndAlign{ AlignUp( SizeOf<Self>, align ) + size, Bytes{Max( POTAlignOf<Self>, align ).template ToValue<usize>()} };
	}
	
/*
=================================================
	_ReleaseObject
=================================================
*/
	template <typename E>
	void  TSharedMem<E>::_ReleaseObject () __NE___
	{
		std::atomic_thread_fence( std::memory_order_acquire );
		CHECK( _allocator );

		auto	alloc	= RVRef(_allocator);
		void*	ptr		= this;
		auto	sa		= _CalcSize( _size, _align );

		_extra.~Extra_t();
		_allocator.~RC();

		alloc->Deallocate( ptr, sa );

		// don't call 'delete this'

		// TODO: flush cache ?
	}


} // AE::Base

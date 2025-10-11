// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Byte.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/MutableArrayView.h"
#include "base/Containers/FixedString.h"

namespace AE::Base
{

	//
	// Memory View
	//

	template <bool IsConst>
	struct TMemoryView
	{
	// types
	public:
		using Self		= TMemoryView;
		using Ptr_t		= Conditional< IsConst, const void*, void* >;


	// variables
	public:
		Ptr_t	data	= null;
		Bytes	size;


	// methods
	public:
		TMemoryView ()											__NE___ {}
		TMemoryView (Ptr_t inData, Bytes inSize)				__NE___	: data{inData}, size{inSize} {}
		TMemoryView (const Self &)								__NE___ = default;
		TMemoryView (Self &&)									__NE___ = default;

		// cast mutable to immutable
	//	template <typename P = Ptr_t>
	//	  requires( Base::IsConst<P> )
	//	TMemoryView (TMemoryView<false> other)					__NE___	: data{other.data}, size{other.size} {}


		// ArrayView
		template <typename T>
		explicit TMemoryView (MutableArrayView<T> other)		__NE___	: data{other.data()}, size{ArraySizeOf(other)} {}

		template <typename T, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (ArrayView<T> other)				__NE___	: data{other.data()}, size{ArraySizeOf(other)} {}

		// Array
		template <typename T, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (Array<T> &other)					__NE___	: data{other.data()}, size{ArraySizeOf(other)} {}

		template <typename T, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const Array<T> &other)			__NE___	: data{other.data()}, size{ArraySizeOf(other)} {}

		// C-style array
		template <typename T, usize S, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (T (&other)[S])					__NE___	: data{other}, size{ArraySizeOf(other)} {}

		template <typename T, usize S, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const T (&other)[S])				__NE___	: data{other}, size{ArraySizeOf(other)} {}

		// FixedArray
		template <typename T, usize S, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (FixedArray<T,S> &other)			__NE___	: data{other}, size{ArraySizeOf(other)} {}

		template <typename T, usize S, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const FixedArray<T,S> &other)		__NE___	: data{other}, size{ArraySizeOf(other)} {}

		// StaticArray
		template <typename T, usize S, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (StaticArray<T,S> &other)			__NE___	: data{other}, size{ArraySizeOf(other)} {}

		template <typename T, usize S, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const StaticArray<T,S> &other)	__NE___	: data{other}, size{ArraySizeOf(other)} {}

		// String
		template <typename T, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (BasicString<T> &other)			__NE___	: data{other.data()}, size{StringSizeOf(other)} {}

		template <typename T, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const BasicString<T> &other)		__NE___	: data{other.data()}, size{StringSizeOf(other)} {}

		// StringView
		template <typename T, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (BasicStringView<T> other)			__NE___	: data{other.data()}, size{StringSizeOf(other)} {}

		// FixedString
		template <typename T, usize S, typename P = Ptr_t>
		  requires( not Base::IsConst<P> )
		explicit TMemoryView (TFixedString<T,S> &other)			__NE___	: data{other.data()}, size{StringSizeOf(other)} {}

		template <typename T, usize S, typename P = Ptr_t>
		  requires( Base::IsConst<P> )
		explicit TMemoryView (const TFixedString<T,S> &other)	__NE___	: data{other.data()}, size{StringSizeOf(other)} {}


		ND_ Ptr_t	Begin ()									C_NE___	{ return data; }
		ND_ Ptr_t	End ()										C_NE___	{ return data + size; }

		ND_ Self	SubRange (Bytes offset)						C_NE___;
		ND_ Self	SubRange (Bytes offset, Bytes range)		C_NE___;

		ND_ bool	Contains (const void* ptr)					C_NE___;
		ND_ bool	Contains (Self other)						C_NE___;

		ND_ bool	Intersects (Self other)						C_NE___;
	};


	using MemoryView	= TMemoryView< false >;
	using ConstMemView	= TMemoryView< true >;
//-----------------------------------------------------------------------------



/*
=================================================
	SubRange
=================================================
*/
	template <bool C>
	TMemoryView<C>  TMemoryView<C>::SubRange (Bytes offset) C_NE___
	{
		ASSERT( offset <= size );

		offset = Min( offset, size );

		return Self{ data + offset, size - offset };
	}

	template <bool C>
	TMemoryView<C>  TMemoryView<C>::SubRange (Bytes offset, Bytes range) C_NE___
	{
		ASSERT( offset <= size );
		ASSERT( offset + range <= size );

		offset = Min( offset, size );
		range  = Min( offset + size, offset + range ) - offset;

		return Self{ data + offset, range };
	}

/*
=================================================
	Contains
=================================================
*/
	template <bool C>
	bool  TMemoryView<C>::Contains (const void* ptr) C_NE___
	{
		return	ptr >= Begin() and ptr <= End();
	}

	template <bool C>
	bool  TMemoryView<C>::Contains (Self other) C_NE___
	{
		return	other.Begin()	>= Begin() and
				other.End()		<= End();
	}

/*
=================================================
	Intersects
=================================================
*/
	template <bool C>
	bool  TMemoryView<C>::Intersects (Self other) C_NE___
	{
		return	other.End()		<= Begin()	or
				other.Begin()	>= End();
	}


} // AE::Base

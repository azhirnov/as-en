// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Exceptions:
		- array elements may throw exceptions (in copy-ctor)
		- allocation failed - std::bad_alloc
		- too big array size - std::bad_array_new_length
*/

#pragma once

#include "base/Algorithms/Cast.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Containers/ArrayView.h"
#include "base/Memory/ResizePolicy.h"
#include "base/Memory/CopyPolicy.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/CompileTime/Math.h"

namespace AE::Base
{

	//
	// Ring Buffer
	//

	template <typename T,
			  typename TAllocator		= UntypedAllocator,
			  typename TCopyPolicy		= CopyPolicy::AutoDetect<T>,
			  typename TResizePolicy	= ResizePolicy< TAllocator >
			 >
	struct RingBuffer : NothrowAllocatable
	{
		//CheckNothrow( IsNothrowMoveCtor< T >);
		//CheckNothrow( IsNothrowDefaultCtor< T >);

	// types
	public:
		using Self				= RingBuffer< T, TAllocator, TCopyPolicy, TResizePolicy >;
		using Allocator_t		= TAllocator;
		using value_type		= T;

	private:
		using CopyPolicy_t		= TCopyPolicy;
		using ResizePolicy_t	= TResizePolicy;
		using Offset_t			= uint;

		template <bool IsConst>
		struct TIterator
		{
			friend struct RingBuffer;

		// types
		private:
			using Iter	= TIterator< IsConst >;
			using RBPtr	= Conditional< IsConst, const Self *, Self *>;

		// variables
		private:
			RBPtr	_rbPtr	= null;
			usize	_index	= UMax;

		public:
			TIterator ()								__NE___ {}
			TIterator (const Iter &)					__NE___ = default;
			TIterator (Iter &&)							__NE___ = default;
			TIterator (RBPtr ptr, usize idx)			__NE___ : _rbPtr{ptr}, _index{idx} { NonNull( _rbPtr ); }

				Iter&	operator = (const Iter &)		__NE___ = default;
				Iter&	operator = (Iter &&)			__NE___ = default;

			ND_ bool	operator != (const Iter &rhs)	C_NE___	{ return not (*this == rhs); }
			ND_ bool	operator == (const Iter &rhs)	C_NE___ { return (_rbPtr == rhs._rbPtr) and (_index == rhs._index); }

				Iter&	operator ++ ()					__NE___	{ NonNull( _rbPtr );	_index = Min( _index + 1, _rbPtr->size() );	return *this; }
				Iter	operator ++ (int)				__NE___	{ Iter res{ *this };	this->operator++();							return res; }
				Iter&	operator += (usize x)			__NE___	{ NonNull( _rbPtr );	_index = Min( _index + x, _rbPtr->size() );	return *this; }
			ND_ Iter	operator +  (usize x)			C_NE___	{ return (Iter{*this} += x); }

			ND_ T &			operator * ()				__NE___	{ NonNull( _rbPtr );	return (*_rbPtr)[_index]; }
			ND_ T const&	operator * ()				C_NE___	{ NonNull( _rbPtr );	return (*_rbPtr)[_index]; }

			ND_ T *			operator -> ()				__NE___	{ NonNull( _rbPtr );	return &(*_rbPtr)[_index]; }
			ND_ T const*	operator -> ()				C_NE___	{ NonNull( _rbPtr );	return &(*_rbPtr)[_index]; }
		};

		static constexpr Offset_t	_EmptyBit = Offset_t{1} << (CT_SizeOfInBits<Offset_t> - 1);

	public:
		using iterator			= TIterator< false >;
		using const_iterator	= TIterator< true >;


	// variables
	private:
		T *				_array		= null;
		Offset_t		_first		= 0;
		Offset_t		_end		= 0;
		Offset_t		_packed		= _EmptyBit;	// 31 bits for capacity, 1 bit for empty flag

		NO_UNIQUE_ADDRESS
		 Allocator_t	_allocator;

		DEBUG_ONLY(
			T const		(*_dbg_first) [400]	= null;
			T const		(*_dbg_end)   [400]	= null;
		)

		//  _____________ __________ _____________
		// | used memory | reserved | used memory |
		// 0           _end      _first         _size
		// ^-second part-^          ^--first part-^


	// methods
	public:
		RingBuffer ()										__NE___ {}
		RingBuffer (const Self &other)						__Th___;
		RingBuffer (Self &&other)							__NE___;

		~RingBuffer ()										__NE___	{ _Release(); }

			Self&		operator = (const Self &rhs)		__Th___;
			Self&		operator = (Self &&rhs)				__NE___;

		ND_ bool		operator == (ArrayView<T> rhs)		C_NE___;
		ND_ bool		operator == (const Self &rhs)		C_NE___;

		ND_ bool		operator != (ArrayView<T> rhs)		C_NE___	{ return not (*this == rhs); }
		ND_ bool		operator != (const Self &rhs)		C_NE___	{ return not (*this == rhs); }

		ND_ T		&	operator [] (usize i)				__NE___;
		ND_ T const	&	operator [] (usize i)				C_NE___;

		ND_ T &			front ()							__NE___;
		ND_ T const&	front ()							C_NE___;

		ND_ T &			back ()								__NE___;
		ND_ T const&	back ()								C_NE___;

		ND_ usize		size ()								C_NE___;
		ND_ bool		empty ()							C_NE___	{ return _packed & _EmptyBit; }
		ND_ usize		capacity ()							C_NE___	{ return _packed & ~_EmptyBit; }

			void		push_front (const T &value)			__Th___;
			void		push_front (T&& value)				__Th___;

			void		push_back (const T &value)			__Th___;
			void		push_back (T&& value)				__Th___;

			void		pop_front ()						__NE___;
			void		pop_back ()							__NE___;

			template <typename ...Types>
			T &			emplace_front (Types&& ...args)		__Th___;

			template <typename ...Types>
			T &			emplace_back (Types&& ...args)		__Th___;

		ND_ T			ExtractFront ()						__NE___;
		ND_ T			ExtractBack ()						__NE___;

			void		EraseFront (usize count)			__NE___;
			void		EraseBack (usize count)				__NE___;

			iterator	FastErase (iterator it)				__NE___;
			void		FastErase (usize idx)				__NE___;

		ND_	iterator		begin ()						__NE___	{ return iterator{ this, 0 }; }
		ND_	const_iterator	begin ()						C_NE___	{ return const_iterator{ this, 0 }; }
		ND_	iterator		end ()							__NE___	{ return begin() + size(); }
		ND_	const_iterator	end ()							C_NE___	{ return begin() + size(); }

			void		reserve (usize newSize)				__Th___;
			void		clear ()							__NE___;

		ND_ HashVal		CalcHash ()							C_NE___;
			void		GetParts (OUT ArrayView<T> &part0,
								  OUT ArrayView<T> &part1)	C_NE___;

			void		AppendFront (ArrayView<T> value)	__Th___;
			void		AppendBack (ArrayView<T> value)		__Th___;

	private:
		void  _Release ()									__NE___;
		void  _Copy (const Self &other)						__Th___;

		void  _Reallocate (usize newSize, bool allowReserve)__Th___;	// bad_array_new_length, bad_alloc

		ND_ Offset_t  _WrapIndex (ssize i)					C_NE___;
		ND_ Offset_t  _SizeMask ()							C_NE___	{ return empty() ? 0 : ~Offset_t{0}; }

		template <typename B>
		void  _AppendFront (B* src, usize count)			__Th___;

		template <typename B>
		void  _AppendBack (B* src, usize count)				__Th___;

		void  _UpdateDbgView ()								__NE___;
	};



/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	RingBuffer<T,A,CP,RP>::RingBuffer (const Self &other) __Th___
	{
		_Copy( other );	// throw
	}

/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	RingBuffer<T,A,CP,RP>::RingBuffer (Self &&other) __NE___ :
		_array{ other._array },
		_first{ other._first },
		_end{ other._end },
		_packed{ other._packed },
		_allocator{ RVRef(other._allocator) }
	{
		other._array = null;
		other._Release();
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	RingBuffer<T,A,CP,RP>&  RingBuffer<T,A,CP,RP>::operator = (const Self &rhs) __Th___
	{
		clear();
		_Copy( rhs );	// throw
		return *this;
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	RingBuffer<T,A,CP,RP>&  RingBuffer<T,A,CP,RP>::operator = (Self &&rhs) __NE___
	{
		_Release();

		_array		= rhs._array;
		_first		= rhs._first;
		_end		= rhs._end;
		_packed		= rhs._packed;
		_allocator	= RVRef(rhs._allocator);

		rhs._array = null;
		rhs._Release();

		_UpdateDbgView();
		return *this;
	}

/*
=================================================
	_Release
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::_Release () __NE___
	{
		clear();

		if ( _array != null )
			_allocator.Deallocate( _array, SizeAndAlign{ SizeOf<T> * capacity(), AlignOf<T> });

		_packed	= 0;
		_array	= null;

		DEBUG_ONLY(
			_dbg_first	= null;
			_dbg_end	= null;
		)
	}

/*
=================================================
	_Copy
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::_Copy (const Self &other) __Th___
	{
		clear();
		AppendBack( other );
	}

/*
=================================================
	_Reallocate
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::_Reallocate (usize newCapacity, bool allowReserve) __Th___
	{
		ASSERT( newCapacity > size() );

		const usize		old_size	= capacity();
		const usize		old_count	= size();
		T *	const		old_ptr		= _array;
		const usize		new_cap		= ResizePolicy_t::template Resize<T>( newCapacity, allowReserve );

		CHECK_THROW( new_cap < _EmptyBit, std::bad_array_new_length{} );

		_packed = Offset_t(new_cap) | (_packed & _EmptyBit);
		ASSERT( capacity() == new_cap );
		ASSERT( empty() == (old_count == 0) );

		const Bytes	new_size = SizeOf<T> * capacity();

		T*	new_ptr = Cast<T>( _allocator.Allocate( SizeAndAlign{ new_size, AlignOf<T> }));
		CHECK_THROW( new_ptr != null, std::bad_alloc{} );

		_array = new_ptr;
		DEBUG_ONLY( DbgInitMem( _array, new_size ));

		if ( old_ptr != null )
		{
			// replace
			if ( _first >= _end )
			{
				usize	off = old_size - _first;
				CopyPolicy_t::Replace( OUT _array,		 INOUT old_ptr + _first, off );
				CopyPolicy_t::Replace( OUT _array + off, INOUT old_ptr,			_end );
			}
			else
			{
				CopyPolicy_t::Replace( OUT _array, INOUT old_ptr + _first, _end - _first );
			}

			_allocator.Deallocate( old_ptr, SizeAndAlign{ SizeOf<T> * old_size, AlignOf<T> });
		}

		_first = 0;
		_end   = Offset_t(old_count);

		ASSERT( size() == old_count );
		_UpdateDbgView();
	}

/*
=================================================
	_WrapIndex
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	typename RingBuffer<T,A,CP,RP>::Offset_t  RingBuffer<T,A,CP,RP>::_WrapIndex (const ssize i) C_NE___
	{
		const ssize	cap = ssize(capacity());
		return Offset_t( i < 0 ? (cap + i) : (i >= cap ? (i - cap) : i) ) & _SizeMask();
	}

/*
=================================================
	size
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	usize  RingBuffer<T,A,CP,RP>::size () C_NE___
	{
		return	Offset_t(_first < _end ?
					(_end - _first) :
					(capacity() - _first + _end))
				& _SizeMask();
	}

/*
=================================================
	front
----
	UB if empty
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	T &  RingBuffer<T,A,CP,RP>::front () __NE___
	{
		ASSERT( not empty() );
		return _array[ _first ];
	}

	template <typename T, typename A, typename CP, typename RP>
	T const &  RingBuffer<T,A,CP,RP>::front () C_NE___
	{
		ASSERT( not empty() );
		return _array[ _first ];
	}

/*
=================================================
	back
----
	UB if empty
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	T &  RingBuffer<T,A,CP,RP>::back () __NE___
	{
		ASSERT( not empty() );
		return _array[ _WrapIndex( ssize(_end) - 1 )];
	}

	template <typename T, typename A, typename CP, typename RP>
	T const &  RingBuffer<T,A,CP,RP>::back () C_NE___
	{
		ASSERT( not empty() );
		return _array[ _WrapIndex( ssize(_end) - 1 )];
	}

/*
=================================================
	operator []
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	T &  RingBuffer<T,A,CP,RP>::operator [] (const usize i) __NE___
	{
		ASSERT( i < size() );
		return _array[ _WrapIndex( i + _first )];
	}

	template <typename T, typename A, typename CP, typename RP>
	T const &  RingBuffer<T,A,CP,RP>::operator [] (const usize i) C_NE___
	{
		ASSERT( i < size() );
		return _array[ _WrapIndex( i + _first )];
	}

/*
=================================================
	push_back
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::push_back (const T &value) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		CopyPolicy_t::Copy( OUT _array + _end, AddressOf(value), 1 );	// throw
		_packed	&= ~_EmptyBit;
		_end	= _WrapIndex( _end + 1 );

		_UpdateDbgView();
	}

	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::push_back (T&& value) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		CopyPolicy_t::Move( OUT _array + _end, INOUT AddressOf(value), 1 );
		_packed	&= ~_EmptyBit;
		_end	= _WrapIndex( _end + 1 );

		_UpdateDbgView();
	}

/*
=================================================
	push_front
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::push_front (const T &value) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		CopyPolicy_t::Copy( OUT _array + _first, AddressOf(value), 1 );	// throw

		_UpdateDbgView();
	}

	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::push_front (T&& value) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		CopyPolicy_t::Move( OUT _array + _first, INOUT AddressOf(value), 1 );

		_UpdateDbgView();
	}

/*
=================================================
	pop_back
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::pop_back () __NE___
	{
		if_likely( not empty() )
		{
			_end = _WrapIndex( ssize(_end) - 1 );
			CopyPolicy_t::Destroy( INOUT _array + _end, 1 );

			if_unlikely( _first == _end )
				_packed |= _EmptyBit;

			_UpdateDbgView();
		}
	}

/*
=================================================
	pop_front
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::pop_front () __NE___
	{
		if_likely( not empty() )
		{
			CopyPolicy_t::Destroy( INOUT _array + _first, 1 );
			_first = _WrapIndex( _first + 1 );

			if_unlikely( _first == _end )
				_packed |= _EmptyBit;

			_UpdateDbgView();
		}
	}

/*
=================================================
	emplace_front
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	template <typename ...Types>
	T&  RingBuffer<T,A,CP,RP>::emplace_front (Types&& ...args) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		T*	res = PlacementNew<T>( OUT _array + _first, FwdArg<Types>(args)... );	// throw

		_UpdateDbgView();
		return *res;
	}

/*
=================================================
	emplace_back
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	template <typename ...Types>
	T&  RingBuffer<T,A,CP,RP>::emplace_back (Types&& ...args) __Th___
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );	// throw

		T*	res = PlacementNew<T>( OUT _array + _end, FwdArg<Types>(args)... );	// throw

		_packed	&= ~_EmptyBit;
		_end	= _WrapIndex( _end + 1 );

		_UpdateDbgView();
		return *res;
	}

/*
=================================================
	ExtractFront
----
	UB if empty
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	T  RingBuffer<T,A,CP,RP>::ExtractFront () __NE___
	{
		T	tmp = RVRef(front());
		pop_front();
		return tmp;
	}

/*
=================================================
	ExtractBack
----
	UB if empty
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	T  RingBuffer<T,A,CP,RP>::ExtractBack () __NE___
	{
		T	tmp = RVRef(back());
		pop_back();
		return tmp;
	}

/*
=================================================
	reserve
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::reserve (usize newSize) __Th___
	{
		if ( newSize <= capacity() )
			return;

		_Reallocate( newSize, false );	// throw
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	bool  RingBuffer<T,A,CP,RP>::operator == (ArrayView<T> rhs) C_NE___
	{
		if ( size() != rhs.size() )
			return false;

		for (usize i = 0, cnt = size(); i < cnt; ++i)
		{
			if_unlikely( not ( (*this)[i] == rhs[i] ))
				return false;
		}
		return true;
	}

/*
=================================================
	operator ==
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	bool  RingBuffer<T,A,CP,RP>::operator == (const Self &rhs) C_NE___
	{
		if ( size() != rhs.size() )
			return false;

		for (usize i = 0, cnt = size(); i < cnt; ++i)
		{
			if_unlikely( not ( (*this)[i] == rhs[i] ))
				return false;
		}
		return true;
	}

/*
=================================================
	clear
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::clear () __NE___
	{
		if ( _array != null )
		{
			if ( empty() )
			{}
			else
			if ( _first > _end )
			{
				CopyPolicy_t::Destroy( INOUT _array + _first, capacity() - _first );
				CopyPolicy_t::Destroy( INOUT _array, _end );
			}
			else
			{
				CopyPolicy_t::Destroy( INOUT _array + _first, size() );
			}
		}
		_first = _end = 0;
		_packed |= _EmptyBit;

		_UpdateDbgView();
	}

/*
=================================================
	GetParts
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::GetParts (OUT ArrayView<T> &part0, OUT ArrayView<T> &part1) C_NE___
	{
		if ( empty() )
		{
			part0 = ArrayView<T>();
			part1 = ArrayView<T>();
		}
		else
		if ( _first > _end )
		{
			part0 = ArrayView<T>( _array + _first, capacity() - _first );
			part1 = ArrayView<T>( _array, _end );
		}
		else
		{
			part0 = ArrayView<T>( _array + _first, size() );
			part1 = ArrayView<T>();
		}
	}

/*
=================================================
	CalcHash
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	HashVal  RingBuffer<T,A,CP,RP>::CalcHash () C_NE___
	{
		ArrayView<T>	part0;
		ArrayView<T>	part1;
		GetParts( part0, part1 );
		return HashOf( part0 ) + HashOf( part1 );
	}

/*
=================================================
	_AppendFront
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	template <typename B>
	void  RingBuffer<T,A,CP,RP>::_AppendFront (B* src, usize count) __Th___
	{
		if_unlikely( src == null or count == 0 )
			return;

		if_unlikely( not empty() and IsIntersects<const void *>( _array, _array + capacity(), src, src + count ))
		{
			DBG_WARNING( "memory must not intersects" );
			return;
		}

		const usize	old_count = size();

		if_unlikely( old_count + count >= capacity() )
			_Reallocate( old_count + count, true );	// throw

		if_unlikely( empty() )
		{
			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array, src, count );	// throw
			else						CopyPolicy_t::Move( OUT _array, INOUT src, count );

			_first = 0;
			_end   = Offset_t(count);
			_packed &= ~_EmptyBit;
		}
		else
		if ( _first < _end )
		{
			const usize	cnt = Min( count, _first );

			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array + _first - cnt, src + count - cnt, cnt );	// throw
			else						CopyPolicy_t::Move( OUT _array + _first - cnt, INOUT src + count - cnt, cnt );

			_first = Offset_t(_first - cnt);

			if ( cnt != count )
			{
				_first = Offset_t( capacity() - (count - cnt) );

				if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array + _first, src, count - cnt );	// throw
				else						CopyPolicy_t::Move( OUT _array + _first, INOUT src, count - cnt );
			}
		}
		else
		{
			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array + _first - count, src, count );	// throw
			else						CopyPolicy_t::Move( OUT _array + _first - count, INOUT src, count );

			_first = Offset_t(_first - count);
		}

		ASSERT( old_count + count == size() );
		_UpdateDbgView();
	}

/*
=================================================
	_AppendBack
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	template <typename B>
	void  RingBuffer<T,A,CP,RP>::_AppendBack (B* src, usize count) __Th___
	{
		if_unlikely( src == null or count == 0 )
			return;

		if_unlikely( not empty() and IsIntersects<const void *>( _array, _array + capacity(), src, src + count ))
		{
			DBG_WARNING( "memory must not intersects" );
			return;
		}

		const usize	old_count = size();

		if ( old_count + count >= capacity() )
			_Reallocate( old_count + count, true );	// throw

		if_unlikely( empty() )
		{
			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array,                       src, count );	// throw
			else						CopyPolicy_t::Move( OUT _array, INOUT const_cast<T*>(src), count );

			_first = 0;
			_end   = Offset_t(count);
			_packed &= ~_EmptyBit;
		}
		else
		if ( _first < _end )
		{
			const usize	cnt = Min( count, capacity() - _end );

			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array + _end,       src, cnt );	// throw
			else						CopyPolicy_t::Move( OUT _array + _end, INOUT src, cnt );

			_end = Offset_t(_end + cnt);

			if ( cnt != count )
			{
				_end = Offset_t(count - cnt);

				if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array,       src + cnt, count - cnt );	// throw
				else						CopyPolicy_t::Move( OUT _array, INOUT src + cnt, count - cnt );
			}
		}
		else
		{
			if constexpr( IsConst<B> )	CopyPolicy_t::Copy( OUT _array + _end,	     src, count );	// throw
			else						CopyPolicy_t::Move( OUT _array + _end, INOUT src, count );

			_end = Offset_t(_end + count);
		}

		ASSERT( old_count + count == size() );
		_UpdateDbgView();
	}

/*
=================================================
	AppendFront
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::AppendFront (ArrayView<T> value) __Th___
	{
		return _AppendFront<const T>( value.data(), value.size() );	// throw
	}

/*
=================================================
	AppendBack
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::AppendBack (ArrayView<T> value) __Th___
	{
		return _AppendBack<const T>( value.data(), value.size() );	// throw
	}

/*
=================================================
	EraseFront
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::EraseFront (usize count) __NE___
	{
		count = Min( count, size() );

		for (usize i = 0; i < count; ++i)
		{
			CopyPolicy_t::Destroy( INOUT _array + _first, 1 );
			_first = _WrapIndex( _first + 1 );
		}

		if ( _first == _end )
			_packed |= _EmptyBit;

		_UpdateDbgView();
	}

/*
=================================================
	EraseBack
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::EraseBack (usize count) __NE___
	{
		count = Min( count, size() );

		for (usize i = 0; i < count; ++i)
		{
			_end = _WrapIndex( ssize(_end) - 1 );
			CopyPolicy_t::Destroy( INOUT _array + _end, 1 );
		}

		if ( _first == _end )
			_packed |= _EmptyBit;

		_UpdateDbgView();
	}

/*
=================================================
	FastErase
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	typename RingBuffer<T,A,CP,RP>::iterator
		RingBuffer<T,A,CP,RP>::FastErase (iterator it) __NE___
	{
		ASSERT( it._rbPtr == this );
		FastErase( it._index );
		return it;
	}

	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::FastErase (usize idx) __NE___
	{
		ASSERT( not empty() );

		idx = _WrapIndex( idx + _first );
		auto	dst = _WrapIndex( ssize(_end) - 1 );

		if ( _first	!= _end		and
			 idx	!= _first	and
			 idx	!= dst )
		{
			_end = dst;
			CopyPolicy_t::Destroy( _array + idx, 1 );
			CopyPolicy_t::Replace( OUT _array + idx, INOUT _array + dst, 1 );
			return;
		}

		if ( idx == _first )
		{
			pop_front();
			return;
		}

		if ( idx == dst )
		{
			pop_back();
			return;
		}

		ASSERT( _first == _end );
		clear();
	}

/*
=================================================
	_UpdateDbgView
=================================================
*/
	template <typename T, typename A, typename CP, typename RP>
	void  RingBuffer<T,A,CP,RP>::_UpdateDbgView () __NE___
	{
		DEBUG_ONLY(
			_dbg_first = BitCast<decltype(_dbg_first)>( std::addressof( _array[_first] ));

			if ( _end < _first )
				_dbg_end = BitCast<decltype(_dbg_end)>( std::addressof( _array[_end] ));
			else
				_dbg_end = null;
		)
	}

} // AE::Base


template <typename T, typename A, typename CP, typename RP>
struct std::hash< AE::Base::RingBuffer<T,A,CP,RP> >
{
	ND_ size_t  operator () (const AE::Base::RingBuffer<T,A,CP,RP> &value) C_NE___
	{
		return size_t(value.CalcHash());
	}
};

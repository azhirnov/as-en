// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
			  typename Policy = CopyPolicy::AutoDetect<T>
			 >
	struct RingBuffer
	{
	// types
	private:
		using Self			= RingBuffer< T, Policy >;
		using CPolicy_t		= Policy;
		using Offset_t		= uint;

		template <bool IsConst>
		struct TIterator
		{
		// types
		private:
			using Iter	= TIterator< IsConst >;
			using RBPtr	= Conditional< IsConst, const Self *, Self *>;

		// variables
		private:
			RBPtr	_rbPtr	= null;
			usize	_index	= UMax;

		public:
			TIterator () {}
			TIterator (const Iter &) = default;
			TIterator (Iter &&) = default;
			TIterator (RBPtr ptr, usize idx) : _rbPtr{ptr}, _index{idx} { ASSERT( _rbPtr != null ); }

			Iter& operator = (const Iter &) = default;
			Iter& operator = (Iter &&) = default;

			ND_ bool operator != (const Iter &rhs) const	{ return not (*this == rhs); }
			
			ND_ bool operator == (const Iter &rhs) const
			{
				return _rbPtr == rhs._rbPtr and _index == rhs._index;
			}

			Iter& operator ++ ()
			{
				ASSERT( _rbPtr != null );
				_index = Min( _index + 1, _rbPtr->size() );
				return *this;
			}

			Iter  operator ++ (int)
			{
				Iter	res{ *this };
				this->operator++();
				return res;
			}

			Iter&  operator += (usize x)
			{
				ASSERT( _rbPtr != null );
				_index = Min( _index + x, _rbPtr->size() );
				return *this;
			}

			ND_ Iter  operator + (usize x) const
			{
				return (Iter{*this} += x);
			}

			ND_ T &			operator * ()			{ ASSERT( _rbPtr != null );	return (*_rbPtr)[_index]; }
			ND_ T const&	operator * ()	const	{ ASSERT( _rbPtr != null );	return (*_rbPtr)[_index]; }

			ND_ T *			operator -> ()			{ ASSERT( _rbPtr != null );	return &(*_rbPtr)[_index]; }
			ND_ T const*	operator -> ()	const	{ ASSERT( _rbPtr != null );	return &(*_rbPtr)[_index]; }
		};
		
		static constexpr Offset_t	_EmptyBit = Offset_t{1} << (CT_SizeOfInBits<Offset_t> - 1);

	public:
		using Allocator_t		= UntypedAlignedAllocator;
		using value_type		= T;
		using iterator			= TIterator< false >;
		using const_iterator	= TIterator< true >;


	// variables
	private:
		T * RST			_array		= null;
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
		// 0           _end      _first        _size


	// methods
	public:
		RingBuffer () {}
		RingBuffer (const Self &other);
		RingBuffer (Self &&other);

		~RingBuffer ()	{ _Release(); }

			Self&		operator = (const Self &rhs);
			Self&		operator = (Self &&rhs);

		ND_ bool		operator == (ArrayView<T> rhs)	const;
		ND_ bool		operator == (const Self &rhs)	const;
		
		ND_ bool		operator != (ArrayView<T> rhs)	const	{ return not (*this == rhs); }
		ND_ bool		operator != (const Self &rhs)	const	{ return not (*this == rhs); }
		
		ND_ T		&	operator [] (usize i);
		ND_ T const	&	operator [] (usize i) const;

		ND_ T &			front ();
		ND_ T const&	front ()	const;

		ND_ T &			back ();
		ND_ T const&	back ()		const;
		
		ND_ usize		size ()		const;
		ND_ bool		empty ()	const		{ return _packed & _EmptyBit; }
		ND_ usize		capacity ()	const		{ return _packed & ~_EmptyBit; }
		
			void		push_front (const T &value);
			void		push_front (T&& value);

			void		push_back (const T &value);
			void		push_back (T&& value);

			void		pop_front ();
			void		pop_back ();

			template <typename ...Types>
			void		emplace_front (Types&& ...args);
			
			template <typename ...Types>
			void		emplace_back (Types&& ...args);

		ND_ T			ExtractFront ();
		ND_ T			ExtractBack ();

		ND_	iterator		begin ()			{ return iterator{ this, 0 }; }
		ND_	const_iterator	begin ()	const	{ return const_iterator{ this, 0 }; }
		ND_	iterator		end ()				{ return begin() + size(); }
		ND_	const_iterator	end ()		const	{ return begin() + size(); }
		
			void		reserve (usize newSize);
			void		clear ();

		ND_ HashVal		CalcHash () const;
			void		GetParts (OUT ArrayView<T> &part0, OUT ArrayView<T> &part1) const;
			
			void		AppendFront (ArrayView<T> value);
			void		AppendBack (ArrayView<T> value);

	private:
		void  _Release ();
		void  _Copy (const Self &other);
		void  _Reallocate (usize newSize, bool allowReserve);

		ND_ Offset_t  _WrapIndex (ssize i)	const;
		ND_ Offset_t  _SizeMask ()			const	{ return empty() ? 0 : ~Offset_t{0}; }

		template <typename B>
		void  _AppendFront (B* src, usize count);
		
		template <typename B>
		void  _AppendBack (B* src, usize count);

		void  _UpdateDbgView ();
	};

	

/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S>
	RingBuffer<T,S>::RingBuffer (const Self &other)
	{
		_Copy( other );
	}
	
/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename S>
	RingBuffer<T,S>::RingBuffer (Self &&other) :
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
	template <typename T, typename S>
	RingBuffer<T,S>&  RingBuffer<T,S>::operator = (const Self &rhs)
	{
		clear();
		_Copy( rhs );
		return *this;
	}
	
/*
=================================================
	operator =
=================================================
*/
	template <typename T, typename S>
	RingBuffer<T,S>&  RingBuffer<T,S>::operator = (Self &&rhs)
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
	template <typename T, typename S>
	void  RingBuffer<T,S>::_Release ()
	{
		clear();

		if ( _array != null )
			_allocator.Deallocate( _array, SizeOf<T> * capacity(), AlignOf<T> );

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
	template <typename T, typename S>
	void  RingBuffer<T,S>::_Copy (const Self &other)
	{
		clear();
		AppendBack( other );
	}
	
/*
=================================================
	_Reallocate
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::_Reallocate (usize newCapacity, bool allowReserve)
	{
		ASSERT( newCapacity > size() );

		const usize		old_size	= capacity();
		const usize		old_count	= size();
		T *	const		old_ptr		= _array;
		const usize		new_cap		= ResizePolicy::Resize<T>( newCapacity, allowReserve );

		_packed = Offset_t(new_cap) | (_packed & _EmptyBit);
		ASSERT( capacity() == new_cap );
		ASSERT( empty() == (old_count == 0) );

		const Bytes	new_size = SizeOf<T> * capacity();

		_array = Cast<T>( _allocator.Allocate( new_size, AlignOf<T> ));
		CHECK( _array != null );
		DEBUG_ONLY( DbgInitMem( _array, new_size ));
		
		// replace
		if ( _first >= _end )
		{
			usize	off = old_size - _first;
			CPolicy_t::Replace( _array, old_ptr + _first, off );
			CPolicy_t::Replace( _array + off, old_ptr, _end );
		}
		else
		{
			CPolicy_t::Replace( _array, old_ptr + _first, _end - _first );
		}

		if ( old_ptr != null )
			_allocator.Deallocate( old_ptr, SizeOf<T> * old_size, AlignOf<T> );
		
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
	template <typename T, typename S>
	typename RingBuffer<T,S>::Offset_t  RingBuffer<T,S>::_WrapIndex (const ssize i) const
	{
		const ssize	cap = ssize(capacity());
		return Offset_t( i < 0 ? (cap + i) : (i >= cap ? (i - cap) : i) ) & _SizeMask();
	}
	
/*
=================================================
	size
=================================================
*/
	template <typename T, typename S>
	usize  RingBuffer<T,S>::size () const
	{
		return	Offset_t(_first < _end ?
					(_end - _first) :
					(capacity() - _first + _end))
				& _SizeMask();
	}
	
/*
=================================================
	front
=================================================
*/
	template <typename T, typename S>
	T &  RingBuffer<T,S>::front ()
	{
		ASSERT( not empty() );
		return _array[ _first ];
	}
	
	template <typename T, typename S>
	T const &  RingBuffer<T,S>::front () const
	{
		ASSERT( not empty() );
		return _array[ _first ];
	}
	
/*
=================================================
	back
=================================================
*/
	template <typename T, typename S>
	T &  RingBuffer<T,S>::back ()
	{
		ASSERT( not empty() );
		return _array[ _WrapIndex( ssize(_end) - 1 )];
	}
	
	template <typename T, typename S>
	T const &  RingBuffer<T,S>::back () const
	{
		ASSERT( not empty() );
		return _array[ _WrapIndex( ssize(_end) - 1 )];
	}
		
/*
=================================================
	operator []
=================================================
*/
	template <typename T, typename S>
	T &  RingBuffer<T,S>::operator [] (const usize i)
	{
		ASSERT( i < size() );
		return _array[ _WrapIndex( i + _first )];
	}
	
	template <typename T, typename S>
	T const &  RingBuffer<T,S>::operator [] (const usize i) const
	{
		ASSERT( i < size() );
		return _array[ _WrapIndex( i + _first )];
	}
		
/*
=================================================
	push_back
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::push_back (const T &value)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		CPolicy_t::Copy( _array + _end, AddressOf(value), 1 );
		_end	= _WrapIndex( _end + 1 );
		_packed	&= ~_EmptyBit;

		_UpdateDbgView();
	}
	
	template <typename T, typename S>
	void  RingBuffer<T,S>::push_back (T&& value)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		CPolicy_t::Move( _array + _end, AddressOf(value), 1 );
		_packed	&= ~_EmptyBit;
		_end	= _WrapIndex( _end + 1 );
		
		_UpdateDbgView();
	}
	
/*
=================================================
	push_front
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::push_front (const T &value)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		CPolicy_t::Copy( _array + _first, AddressOf(value), 1 );
		
		_UpdateDbgView();
	}
	
	template <typename T, typename S>
	void  RingBuffer<T,S>::push_front (T&& value)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		CPolicy_t::Move( _array + _first, AddressOf(value), 1 );
		
		_UpdateDbgView();
	}
		
/*
=================================================
	pop_back
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::pop_back ()
	{
		if_likely( not empty() )
		{
			_end = _WrapIndex( ssize(_end) - 1 );
			CPolicy_t::Destroy( _array + _end, 1 );
			
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
	template <typename T, typename S>
	void  RingBuffer<T,S>::pop_front ()
	{
		if_likely( not empty() )
		{
			CPolicy_t::Destroy( _array + _first, 1 );
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
	template <typename T, typename S>
	template <typename ...Types>
	void  RingBuffer<T,S>::emplace_front (Types&& ...args)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		_packed	&= ~_EmptyBit;
		_first	= _WrapIndex( ssize(_first) - 1 );
		PlacementNew<T>( _array + _first, FwdArg<Types>(args)... );
		
		_UpdateDbgView();
	}
	
/*
=================================================
	emplace_back
=================================================
*/
	template <typename T, typename S>
	template <typename ...Types>
	void  RingBuffer<T,S>::emplace_back (Types&& ...args)
	{
		if_unlikely( size() + 1 > capacity() )
			_Reallocate( capacity() + 1, true );
		
		PlacementNew<T>( _array + _end, FwdArg<Types>(args)... );
		_packed	&= ~_EmptyBit;
		_end	= _WrapIndex( _end + 1 );
		
		_UpdateDbgView();
	}

/*
=================================================
	ExtractFront
=================================================
*/
	template <typename T, typename S>
	T  RingBuffer<T,S>::ExtractFront ()
	{
		T	tmp = RVRef(front());
		pop_front();
		return tmp;
	}
	
/*
=================================================
	ExtractBack
=================================================
*/
	template <typename T, typename S>
	T  RingBuffer<T,S>::ExtractBack ()
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
	template <typename T, typename S>
	void  RingBuffer<T,S>::reserve (usize newSize)
	{
		if ( newSize <= capacity() )
			return;

		_Reallocate( newSize, false );
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename T, typename S>
	bool  RingBuffer<T,S>::operator == (ArrayView<T> rhs) const
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
	template <typename T, typename S>
	bool  RingBuffer<T,S>::operator == (const Self &rhs) const
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
	template <typename T, typename S>
	void  RingBuffer<T,S>::clear ()
	{
		if ( _array != null )
		{
			if ( empty() )
			{}
			else
			if ( _first > _end )
			{
				CPolicy_t::Destroy( _array + _first, capacity() - _first );
				CPolicy_t::Destroy( _array, _end );
			}
			else
			{
				CPolicy_t::Destroy( _array + _first, size() );
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
	template <typename T, typename S>
	void  RingBuffer<T,S>::GetParts (OUT ArrayView<T> &part0, OUT ArrayView<T> &part1) const
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
	template <typename T, typename S>
	HashVal  RingBuffer<T,S>::CalcHash () const
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
	template <typename T, typename S>
	template <typename B>
	void  RingBuffer<T,S>::_AppendFront (B* src, usize count)
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
			_Reallocate( old_count + count, true );

		if_unlikely( empty() )
		{
			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array, src, count );
			else						CPolicy_t::Move( _array, src, count );

			_first = 0;
			_end   = Offset_t(count);
			_packed &= ~_EmptyBit;
		}
		else
		if ( _first < _end )
		{
			const usize	cnt = Min( count, _first );

			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array + _first - cnt, src + count - cnt, cnt );
			else						CPolicy_t::Move( _array + _first - cnt, src + count - cnt, cnt );

			_first = Offset_t(_first - cnt);
			
			if ( cnt != count )
			{
				_first = Offset_t( capacity() - (count - cnt) );

				if constexpr( IsConst<B> )	CPolicy_t::Copy( _array + _first, src, count - cnt );
				else						CPolicy_t::Move( _array + _first, src, count - cnt );
			}
		}
		else
		{
			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array + _first - count, src, count );
			else						CPolicy_t::Move( _array + _first - count, src, count );

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
	template <typename T, typename S>
	template <typename B>
	void  RingBuffer<T,S>::_AppendBack (B* src, usize count)
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
			_Reallocate( old_count + count, true );

		if_unlikely( empty() )
		{
			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array, src, count );
			else						CPolicy_t::Move( _array, const_cast<T*>(src), count );

			_first = 0;
			_end   = Offset_t(count);
			_packed &= ~_EmptyBit;
		}
		else
		if ( _first < _end )
		{
			const usize	cnt = Min( count, capacity() - _end );

			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array + _end, src, cnt );
			else						CPolicy_t::Move( _array + _end, src, cnt );

			_end = Offset_t(_end + cnt);
			
			if ( cnt != count )
			{
				_end = Offset_t(count - cnt);

				if constexpr( IsConst<B> )	CPolicy_t::Copy( _array, src + cnt, count - cnt );
				else						CPolicy_t::Move( _array, src + cnt, count - cnt );
			}
		}
		else
		{
			if constexpr( IsConst<B> )	CPolicy_t::Copy( _array + _end, src, count );
			else						CPolicy_t::Move( _array + _end, src, count );

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
	template <typename T, typename S>
	void  RingBuffer<T,S>::AppendFront (ArrayView<T> value)
	{
		return _AppendFront<const T>( value.data(), value.size() );
	}

/*
=================================================
	AppendBack
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::AppendBack (ArrayView<T> value)
	{
		return _AppendBack<const T>( value.data(), value.size() );
	}
	
/*
=================================================
	_UpdateDbgView
=================================================
*/
	template <typename T, typename S>
	void  RingBuffer<T,S>::_UpdateDbgView ()
	{
		DEBUG_ONLY(
			_dbg_first = BitCast<decltype(_dbg_first)>( &_array[_first] );

			if ( _end < _first )
				_dbg_end = BitCast<decltype(_dbg_end)>( &_array[_end] );
			else
				_dbg_end = null;
		)
	}

} // AE::Base


namespace std
{
	template <typename T, typename S>
	struct hash< AE::Base::RingBuffer<T,S> >
	{
		ND_ size_t  operator () (const AE::Base::RingBuffer<T,S> &value) const
		{
			return size_t(value.CalcHash());
		}
	};

} // std


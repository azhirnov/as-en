// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

/*
=================================================
	Reverse
----
	iterate array from back to front
----
	example:  for (auto& e : Reverse(arr)) {}
=================================================
*/
	namespace _hidden_
	{
		template <typename Container>
		class ReverseContainerView
		{
		private:
			Container &		_container;

		public:
			explicit constexpr ReverseContainerView (Container& container)	__NE___	: _container{container} {}

			ND_ constexpr auto  begin ()									__NE___	{ return std::rbegin( _container ); }
			ND_ constexpr auto  end ()										__NE___	{ return std::rend( _container ); }
		};

	} // _hidden_

	template <typename Container>
	ND_ constexpr auto  Reverse (Container& container) __NE___
	{
		return Base::_hidden_::ReverseContainerView<Container>{ container };
	}

	template <typename Container>
	ND_ constexpr auto  Reverse (const Container& container) __NE___
	{
		return Base::_hidden_::ReverseContainerView<const Container>{ container };
	}

/*
=================================================
	IndicesOnly
----
	example:  for (usize i : IndicesOnly(arr)) {}
=================================================
*/
	namespace _hidden_
	{
		struct IndicesOnly_End
		{
			usize	_size;

			explicit constexpr IndicesOnly_End (usize s)					__NE___ : _size{s} {}
		};

		struct IndicesOnly_Iter
		{
			using Self = IndicesOnly_Iter;

			usize	_index;

			explicit constexpr IndicesOnly_Iter (usize i)					__NE___ : _index{i} {}

				constexpr Self&		operator = (const Self &)				__NE___ = default;
				constexpr Self&		operator = (Self &&)					__NE___ = default;

			ND_ constexpr bool		operator != (const Self &rhs)			C_NE___	{ return _index != rhs._index; }
			ND_ constexpr bool		operator == (const Self &rhs)			C_NE___	{ return _index == rhs._index; }

			ND_ constexpr bool		operator != (const IndicesOnly_End &rhs)C_NE___	{ return _index < rhs._size; }

				constexpr Self&		operator ++ ()							__NE___	{ ++_index;  return *this; }
				constexpr Self		operator ++ (int)						__NE___	{ return Self{_index++}; }
			ND_ constexpr usize		operator * ()							__NE___	{ return _index; }
		};


		class IndicesOnlyRange
		{
		private:
			const usize		_begin;
			const usize		_end;

		public:
			explicit constexpr IndicesOnlyRange (usize b, usize e)			__NE___	: _begin{b}, _end{e} {}

			ND_ constexpr IndicesOnly_Iter	begin ()						C_NE___	{ return IndicesOnly_Iter{ _begin }; }
			ND_ constexpr IndicesOnly_End	end ()							C_NE___	{ return IndicesOnly_End{ _end }; }
		};

	} // _hidden_


	template <typename Container, ENABLEIF( IsClass<Container> )>
	ND_ constexpr auto  IndicesOnly (const Container& container) __NE___
	{
		return Base::_hidden_::IndicesOnlyRange{ 0, container.size() };
	}

	ND_ constexpr inline auto  IndicesOnly (usize begin, usize end) __NE___
	{
		ASSERT( begin <= end );
		return Base::_hidden_::IndicesOnlyRange{ begin, end };
	}

	ND_ constexpr inline auto  IndicesOnly (usize count) __NE___
	{
		return Base::_hidden_::IndicesOnlyRange{ 0, count };
	}

	template <typename T, ENABLEIF( IsEnum<T> )>
	ND_ constexpr inline auto  IndicesOnly () __NE___
	{
		return Base::_hidden_::IndicesOnlyRange{ 0, usize(T::_Count) };
	}

/*
=================================================
	ReverseIndices
----
	example:  for (usize i : ReverseIndices(arr)) {}
=================================================
*/
	namespace _hidden_
	{
		struct ReverseIndices_End
		{
			usize	_size;

			explicit constexpr ReverseIndices_End (usize s)						__NE___ : _size{s} {}
		};

		struct ReverseIndices_Iter
		{
			using Self = ReverseIndices_Iter;

			usize	_index;

			explicit constexpr ReverseIndices_Iter (usize i)					__NE___ : _index{i} {}

				constexpr Self&		operator = (const Self &)					__NE___ = default;
				constexpr Self&		operator = (Self &&)						__NE___ = default;

			ND_ constexpr bool		operator != (const Self &rhs)				C_NE___	{ return _index != rhs._index; }
			ND_ constexpr bool		operator == (const Self &rhs)				C_NE___	{ return _index == rhs._index; }

			ND_ constexpr bool		operator != (const ReverseIndices_End &rhs)	C_NE___	{ return _index < rhs._size; }

				constexpr Self&		operator ++ ()								__NE___	{ --_index;  return *this; }
				constexpr Self		operator ++ (int)							__NE___	{ return Self{_index--}; }
			ND_ constexpr usize		operator * ()								__NE___	{ return _index; }
		};


		class ReverseIndicesRange
		{
		private:
			const usize		_begin;
			const usize		_end;

		public:
			explicit constexpr ReverseIndicesRange (usize b, usize e)			__NE___	: _begin{b}, _end{e} {}

			ND_ constexpr ReverseIndices_Iter		begin ()					__NE___	{ return ReverseIndices_Iter{ _begin }; }
			ND_ constexpr ReverseIndices_End		end ()						__NE___	{ return ReverseIndices_End{ _end }; }
		};

	} // _hidden_


	template <typename Container, ENABLEIF( IsClass<Container> )>
	ND_ constexpr auto  ReverseIndices (const Container& container) __NE___
	{
		return Base::_hidden_::ReverseIndicesRange{ container.size()-1, container.size() };
	}

	ND_ constexpr inline auto  ReverseIndices (usize count) __NE___
	{
		return Base::_hidden_::ReverseIndicesRange{ count-1, count };
	}

/*
=================================================
	WithIndex
----
	example:
		for (auto [e, i] : WithIndex(arr))		- const&
		for (auto&& [e, i] : WithIndex(arr))	- &
=================================================
*/
	namespace _hidden_
	{
		template <typename Iter>
		struct WithIndex_Iter
		{
			using Self	= WithIndex_Iter<Iter>;

			Iter	_it;
			usize	_index;

			constexpr WithIndex_Iter (Iter it, usize idx)					__NE___	: _it{it}, _index{idx} {}

			ND_ constexpr bool	operator != (const Self &rhs)				C_NE___	{ return _it != rhs._it; }
			ND_ constexpr bool	operator == (const Self &rhs)				C_NE___	{ return _it == rhs._it; }

			ND_ constexpr auto	operator * ()								__NE___	{ return TupleRef{ &(*_it), &_index }; }

				constexpr Self&	operator ++ ()								__NE___	{ ++_it;  ++_index;  return *this; }
				constexpr Self	operator ++ (int)							__NE___	{ return Self{ ++_it, ++_index }; }
		};


		template <typename Container, typename Iter>
		class WithIndexContainerView
		{
		private:
			Container &		_container;

		public:
			explicit constexpr WithIndexContainerView (Container& container)__NE___	: _container{container} {}

			ND_ constexpr auto	begin ()									__NE___	{ return WithIndex_Iter<Iter>{ _container.begin(), 0 }; }
			ND_ constexpr auto	end ()										__NE___	{ return WithIndex_Iter<Iter>{ _container.end(),   UMax }; }
		};

	} // _hidden_

	template <typename Container>
	ND_ constexpr auto  WithIndex (Container& container) __NE___
	{
		return Base::_hidden_::WithIndexContainerView< Container, typename Container::iterator >{ container };
	}

	template <typename Container>
	ND_ constexpr auto  WithIndex (const Container& container) __NE___
	{
		return Base::_hidden_::WithIndexContainerView< const Container, typename Container::const_iterator >{ container };
	}

/*
=================================================
	BitfieldIterate
----
	From low to high bit.
	Replacement for loop with 'ExtractBit()'.
----
	example:  for (uint bit : BitfieldIterate( bits ))
=================================================
*/
	namespace _hidden_
	{
		struct BitfieldIterate_End
		{};

		template <typename T>
		struct BitfieldIterate_Iter
		{
		private:
			using Self	= BitfieldIterate_Iter<T>;
			using U		= ToUnsignedInteger<T>;

			T &		_bits;
			T		_current;	// may be invalid

		public:
			explicit constexpr BitfieldIterate_Iter (T& bits)			__NE___	: _bits{bits}, _current{ _ExtractBit( _bits )} {}

			ND_ constexpr bool		operator != (BitfieldIterate_End)	C_NE___	{ return _bits != Zero; }

			ND_ constexpr T			operator * ()						C_NE___	{ return _current; }

				constexpr Self&		operator ++ ()						__NE___	{ _bits = T(U(_bits) & ~U(_current));  _current = _ExtractBit( _bits );  return *this; }

		private:
			ND_ static constexpr T  _ExtractBit (T bits)				__NE___	{ return T( U(bits) & ~(U(bits) - U{1}) ); }
		};

		template <typename T>
		struct BitfieldIterateView
		{
		private:
			T	_bits;

		public:
			explicit constexpr BitfieldIterateView (T bits)				__NE___ : _bits{bits} {}

			ND_ constexpr auto	begin ()								__NE___	{ return BitfieldIterate_Iter<T>{ _bits }; }
			ND_ constexpr auto	end ()									__NE___	{ return BitfieldIterate_End{}; }

		};

	} // _hidden_

	template <typename T,
			  ENABLEIF( IsEnum<T> or IsUnsignedInteger<T> )>
	ND_ constexpr auto  BitfieldIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitfieldIterateView<T>{ bits };
	}

	template <usize C>
	ND_ constexpr auto  BitfieldIterate (const BitSet<C> &bits) __NE___
	{
		if constexpr( C <= 32 )
			return Base::_hidden_::BitfieldIterateView<uint>{ uint(bits.to_ulong()) };
		else
		if constexpr( C <= 64 )
			return Base::_hidden_::BitfieldIterateView<ulong>{ bits.to_ullong() };
	}

	template <typename T,
			  ENABLEIF( IsUnsignedInteger<T> )>
	ND_ constexpr auto  BitfieldIterate (const Bitfield<T> &bits) __NE___
	{
		return Base::_hidden_::BitfieldIterateView<T>{ T{bits} };
	}

/*
=================================================
	BitIndexIterate
----
	From low to high bit.
	Replacement for loop with 'ExtractBitIndex()'.
----
	example:
		for (uint idx : BitIndexIterate( bits ))
		for (Enum idx : BitIndexIterate<Enum>( bits ))
		for (Enum idx : BitIndexIterate( EnumSet<Enum>{...} ))
=================================================
*/
	namespace _hidden_
	{
		struct BitIndexIterate_End
		{};

		template <typename R, typename T>
		struct BitIndexIterate_Iter
		{
		private:
			using Self	= BitIndexIterate_Iter< R, T >;
			using U		= ToUnsignedInteger<T>;

			T &		_bits;
			R		_current;	// may be invalid

		public:
			explicit constexpr BitIndexIterate_Iter (T& bits)			__NE___	: _bits{bits}, _current{ _ExtractBitLog2( _bits )} {}

			ND_ constexpr bool		operator != (BitIndexIterate_End)	C_NE___	{ return _bits != Zero; }

			ND_ constexpr R			operator * ()						C_NE___	{ return _current; }

				constexpr Self&		operator ++ ()						__NE___	{ _bits = T(U(_bits) & ~SafeLeftBitShift( U{1}, uint(_current) ));  _current = _ExtractBitLog2( _bits );  return *this; }

		private:
			ND_ static constexpr R  _ExtractBitLog2 (T bits)			__NE___	{ return R(IntLog2( U(bits) & ~(U(bits) - U{1}) )); }
		};

		template <typename R, typename T>
		struct BitIndexIterateView
		{
		private:
			T	_bits;

		public:
			explicit constexpr BitIndexIterateView (T bits)				__NE___ : _bits{bits} {}

			ND_ constexpr auto	begin ()								__NE___	{ return BitIndexIterate_Iter<R,T>{ _bits }; }
			ND_ constexpr auto	end ()									__NE___	{ return BitIndexIterate_End{}; }

		};

	} // _hidden_

	template <typename T,
			  ENABLEIF( IsUnsignedInteger<T> )>
	ND_ constexpr auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< uint, T >{ bits };
	}

	template <typename T,
			  ENABLEIF( IsEnum<T> )>
	ND_ constexpr auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< T, T >{ bits };
	}

	template <typename R, typename T,
			  ENABLEIF( IsEnum<T> or IsUnsignedInteger<T> )>
	ND_ constexpr auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< R, T >{ bits };
	}

	template <usize C>
	ND_ constexpr auto  BitIndexIterate (const BitSet<C> &bits) __NE___
	{
		if constexpr( C <= 32 )
			return Base::_hidden_::BitIndexIterateView< uint, uint >{ uint(bits.to_ulong()) };
		else
		if constexpr( C <= 64 )
			return Base::_hidden_::BitIndexIterateView< uint, ulong >{ bits.to_ullong() };
	}

	template <typename T,
			  ENABLEIF( IsUnsignedInteger<T> )>
	ND_ constexpr auto  BitIndexIterate (const Bitfield<T> &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< uint, T >{ T{bits} };
	}

} // AE::Base

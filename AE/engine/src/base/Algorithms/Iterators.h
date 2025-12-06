// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/Bitfield.h"
#include "base/Utils/EnumSet.h"

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
		template <typename Iterator>
		class ReverseContainerView
		{
		private:
			const Iterator	_begin;
			const Iterator	_end;

		public:
			__Cx__ ReverseContainerView (Iterator b, Iterator e)__NE___	: _begin{b}, _end{e} {}

			NdCx__ auto  begin ()								__NE___	{ return _begin; }
			NdCx__ auto  end ()									__NE___	{ return _end; }
		};

	} // _hidden_

	template <typename Container>
	NdCx__ auto  Reverse (Container&& container) __NE___
	{
		return Base::_hidden_::ReverseContainerView{ std::rbegin(container), std::rend(container) };
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
		template <typename T>
		struct IndicesOnly_End
		{
			const T		_size;

			__Cx__ explicit IndicesOnly_End (T s)					__NE___ : _size{s} {}
		};

		template <typename T>
		struct IndicesOnly_Iter
		{
			using Self	= IndicesOnly_Iter<T>;
			using U		= ToUnsignedInteger<T>;

			T	_index;

			__Cx__ explicit IndicesOnly_Iter (T i)					__NE___ : _index{i} {}

			__Cx__ Self&	operator = (const Self &)				__NE___ = default;
			__Cx__ Self&	operator = (Self &&)					__NE___ = default;

			NdCx__ bool		operator != (const Self &rhs)			C_NE___	{ return _index != rhs._index; }
			NdCx__ bool		operator == (const Self &rhs)			C_NE___	{ return _index == rhs._index; }

			NdCx__ bool		operator != (IndicesOnly_End<T> rhs)	C_NE___	{ return _index < rhs._size; }

			NdCx__ T		operator * ()							__NE___	{ return _index; }

			__Cx__ Self&	operator ++ ()							__NE___
			{
				if constexpr( IsEnum<T> )
					_index = T(U(_index) + 1);
				else
					++_index;
				return *this;
			}

			__Cx__ Self		operator ++ (int)						__NE___
			{
				if constexpr( IsEnum<T> )
				{
					T	tmp = _index;
					_index	= T(U(_index) + 1);
					return Self{tmp};
				}
				else
					return Self{_index++};
			}
		};


		template <typename T>
		class IndicesOnlyRange
		{
		private:
			const T		_begin;
			const T		_end;

		public:
			__Cx__ explicit IndicesOnlyRange (T b, T e)				__NE___	: _begin{b}, _end{e} {}

			NdCx__ IndicesOnly_Iter<T>	begin ()					C_NE___	{ return IndicesOnly_Iter<T>{ _begin }; }
			NdCx__ IndicesOnly_End<T>	end ()						C_NE___	{ return IndicesOnly_End<T>{ _end }; }
		};

	} // _hidden_


	template <typename Container> requires( IsClass<Container> )
	NdCx__ auto  IndicesOnly (const Container& container) __NE___
	{
		return Base::_hidden_::IndicesOnlyRange<usize>{ 0, container.size() };
	}

	NdCxIn auto  IndicesOnly (usize begin, usize end) __NE___
	{
		ASSERT_Cx( begin <= end );
		return Base::_hidden_::IndicesOnlyRange<usize>{ begin, end };
	}

	NdCxIn auto  IndicesOnly (usize count) __NE___
	{
		return Base::_hidden_::IndicesOnlyRange<usize>{ 0, count };
	}

	template <typename T> requires( IsEnum<T> )
	NdCx__ auto  IndicesOnly () __NE___
	{
		return Base::_hidden_::IndicesOnlyRange<T>{ T{0}, T::_Count };
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

			__Cx__ explicit ReverseIndices_End (usize s)				__NE___ : _size{s} {}
		};

		struct ReverseIndices_Iter
		{
			using Self = ReverseIndices_Iter;

			usize	_index;

			__Cx__ explicit ReverseIndices_Iter (usize i)				__NE___ : _index{i} {}

			__Cx__ Self&	operator = (const Self &)					__NE___ = default;
			__Cx__ Self&	operator = (Self &&)						__NE___ = default;

			NdCx__ bool		operator != (const Self &rhs)				C_NE___	{ return _index != rhs._index; }
			NdCx__ bool		operator == (const Self &rhs)				C_NE___	{ return _index == rhs._index; }

			NdCx__ bool		operator != (const ReverseIndices_End &rhs)	C_NE___	{ return _index < rhs._size; }

			__Cx__ Self&	operator ++ ()								__NE___	{ --_index;  return *this; }
			__Cx__ Self		operator ++ (int)							__NE___	{ return Self{_index--}; }
			NdCx__ usize	operator * ()								__NE___	{ return _index; }
		};


		class ReverseIndicesRange
		{
		private:
			const usize		_begin;
			const usize		_end;

		public:
			__Cx__ explicit ReverseIndicesRange (usize b, usize e)		__NE___	: _begin{b}, _end{e} {}

			NdCx__ ReverseIndices_Iter		begin ()					__NE___	{ return ReverseIndices_Iter{ _begin }; }
			NdCx__ ReverseIndices_End		end ()						__NE___	{ return ReverseIndices_End{ _end }; }
		};

	} // _hidden_


	template <typename Container> requires( IsClass<Container> )
	NdCx__ auto  ReverseIndices (const Container& container) __NE___
	{
		return Base::_hidden_::ReverseIndicesRange{ container.size()-1, container.size() };
	}

	NdCxIn auto  ReverseIndices (usize count) __NE___
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
		template <typename Iterator>
		struct WithIndex_Iter
		{
			using Self	= WithIndex_Iter< Iterator >;

			Iterator	_it;
			usize		_index;

			__Cx__ WithIndex_Iter (Iterator it, usize idx)			__NE___	: _it{it}, _index{idx} {}

			NdCx__ bool		operator != (const Self &rhs)			C_NE___	{ return _it != rhs._it; }
			NdCx__ bool		operator == (const Self &rhs)			C_NE___	{ return _it == rhs._it; }

			NdCx__ auto		operator * ()							__NE___	{ return TupleRef{ &(*_it), static_cast<usize const*>(&_index) }; }

			__Cx__ Self&	operator ++ ()							__NE___	{ ++_it;  ++_index;  return *this; }
			__Cx__ Self		operator ++ (int)						__NE___	{ return Self{ ++_it, ++_index }; }
		};


		template <typename Iterator>
		class WithIndexContainerView
		{
		private:
			const Iterator		_begin;
			const Iterator		_end;

		public:
			__Cx__ WithIndexContainerView (Iterator b, Iterator e)	__NE___	: _begin{b}, _end{e} {}

			NdCx__ auto	begin ()									__NE___	{ return WithIndex_Iter{ _begin, 0 }; }
			NdCx__ auto	end ()										__NE___	{ return WithIndex_Iter{ _end,   UMax }; }
		};

	} // _hidden_

	template <typename Container>
	NdCx__ auto  WithIndex (Container&& container) __NE___
	{
		return Base::_hidden_::WithIndexContainerView{ std::begin(container), std::end(container) };
	}

/*
=================================================
	BitfieldIterate
----
	From low to high bit.
	Replacement for loop with 'ExtractBit()'.
----
	example:
		for (uint bit : BitfieldIterate( bits ))
		for (Enum bit : BitfieldIterate( Enum(bits) ))
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
			__Cx__ explicit BitfieldIterate_Iter (T& bits)		__NE___	: _bits{bits}, _current{ _ExtractBit( _bits )} {}

			NdCx__ bool		operator != (BitfieldIterate_End)	C_NE___	{ return _bits != Zero; }

			NdCx__ T		operator * ()						C_NE___	{ return _current; }

			__Cx__ Self&	operator ++ ()						__NE___	{ _bits = T(U(_bits) & ~U(_current));  _current = _ExtractBit( _bits );  return *this; }

		private:
			NdCx__ static T  _ExtractBit (T bits)				__NE___	{ return T( U(bits) & ~(U(bits) - U{1}) ); }
		};

		template <typename T>
		struct BitfieldIterateView
		{
		private:
			T	_bits;

		public:
			__Cx__ explicit BitfieldIterateView (T bits)		__NE___ : _bits{bits} {}

			NdCx__ auto	begin ()								__NE___	{ return BitfieldIterate_Iter<T>{ _bits }; }
			NdCx__ auto	end ()									__NE___	{ return BitfieldIterate_End{}; }

		};

	} // _hidden_

	template <typename T> requires( IsEnum<T> or IsUnsignedInteger<T> )
	NdCx__ auto  BitfieldIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitfieldIterateView<T>{ bits };
	}

	template <usize C>
	NdCx__ auto  BitfieldIterate (const BitSet<C> &bits) __NE___
	{
		if constexpr( C <= 32 )
			return Base::_hidden_::BitfieldIterateView<uint>{ uint(bits.to_ulong()) };
		else
		if constexpr( C <= 64 )
			return Base::_hidden_::BitfieldIterateView<ulong>{ bits.to_ullong() };
	}

	template <typename T> requires( IsUnsignedInteger<T> )
	NdCx__ auto  BitfieldIterate (const Bitfield<T> &bits) __NE___
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
		for (uint idx : BitIndexIterate<uint>( bits ))
		for (Enum idx : BitIndexIterate<Enum>( bits ))
		for (Enum idx : BitIndexIterate<uint>( Bitfield{bits} ))
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
			__Cx__ explicit BitIndexIterate_Iter (T& bits)		__NE___	: _bits{bits}, _current{ _ExtractBitLog2( _bits )} {}

			NdCx__ bool		operator != (BitIndexIterate_End)	C_NE___	{ return _bits != Zero; }

			NdCx__ R		operator * ()						C_NE___	{ return _current; }

			__Cx__ Self&	operator ++ ()						__NE___	{ _bits = T(U(_bits) & ~SafeLeftBitShift( U{1}, uint(_current) ));  _current = _ExtractBitLog2( _bits );  return *this; }

		private:
			NdCx__ static R  _ExtractBitLog2 (T bits)			__NE___	{ return R(IntLog2( U(bits) & ~(U(bits) - U{1}) )); }
		};

		template <typename R, typename T>
		struct BitIndexIterateView
		{
		private:
			T	_bits;

		public:
			__Cx__ explicit BitIndexIterateView (T bits)		__NE___ : _bits{bits} {}

			NdCx__ auto	begin ()								__NE___	{ return BitIndexIterate_Iter<R,T>{ _bits }; }
			NdCx__ auto	end ()									__NE___	{ return BitIndexIterate_End{}; }
		};

	} // _hidden_

	template <typename T>
	  requires( IsUnsignedInteger<T> )
	NdCx__ auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< uint, T >{ bits };
	}

	template <typename R, typename T>
	  requires( IsUnsignedInteger<R> and IsUnsignedInteger<T> )
	NdCx__ auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< R, T >{ bits };
	}

	template <typename R, typename T = R>
	  requires( IsEnum<R> and IsEnum<T> )
	NdCx__ auto  BitIndexIterate (const T &bits) __NE___
	{
		return Base::_hidden_::BitIndexIterateView< R, T >{ bits };
	}

	template <usize C>
	NdCx__ auto  BitIndexIterate (const BitSet<C> &bits) __NE___
	{
		if constexpr( C <= 32 )
			return Base::_hidden_::BitIndexIterateView< uint, uint >{ uint(bits.to_ulong()) };
		else
		if constexpr( C <= 64 )
			return Base::_hidden_::BitIndexIterateView< uint, ulong >{ bits.to_ullong() };
	}

	template <typename BF>
	  requires( IsSpecializationOf< BF, Bitfield >)
	NdCx__ auto  BitIndexIterate (const BF &bits) __NE___
	{
		using T = typename BF::Value_t;
		return Base::_hidden_::BitIndexIterateView< uint, T >{ T{bits} };
	}

	template <typename ES>
	  requires( IsSpecializationOf< ES, EnumSet >)
	NdCx__ auto  BitIndexIterate (const ES &bitArray) __NE___
	{
		using T = typename ES::Elem_t;
		using R = typename ES::Value_t;
		return Base::_hidden_::BitIndexIterateView< R, T >{ bitArray.AsBits() };
	}

} // AE::Base

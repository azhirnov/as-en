// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Bytes.h"

namespace AE::Base
{

	//
	// Array View
	//

	template <typename T>
	struct ArrayView
	{
	// types
	public:
		using value_type		= T;
		using iterator			= T *;
		using const_iterator	= T const *;


	// variables
	private:
		union {
			T const *	_array;
			T const		(*_dbgView)[400];		// debug viewer, don't use this field!
		};
		usize		_count	= 0;


	// methods
	public:
		ArrayView () : _array{null} {}
		
		ArrayView (T const* ptr, usize count) : _array{ptr}, _count{count}
		{
			ASSERT( (_count == 0) or (_array != null) ); 
		}

		ArrayView (std::initializer_list<T> list) : _array{list.begin()}, _count{list.size()} {}

		template <typename AllocT>
		ArrayView (const Array<T,AllocT> &vec) : _array{vec.data()}, _count{vec.size()}
		{
			ASSERT( (_count == 0) or (_array != null) ); 
		}

		template <usize S>
		ArrayView (const StaticArray<T,S> &arr) : _array{arr.data()}, _count{arr.size()} {}

		template <usize S>
		ArrayView (const T (&arr)[S]) : _array{arr}, _count{S} {}

		ND_ explicit operator Array<T> ()			const	{ return Array<T>{ begin(), end() }; }

		ND_ usize			size ()					const	{ return _count; }
		ND_ bool			empty ()				const	{ return _count == 0; }
		ND_ T const *		data ()					const	{ return _array; }

		ND_ T const &		operator [] (usize i)	const	{ ASSERT( i < _count );  return _array[i]; }

		ND_ const_iterator	begin ()				const	{ return _array; }
		ND_ const_iterator	end ()					const	{ return _array + _count; }

		ND_ T const&		front ()				const	{ ASSERT( _count > 0 );  return _array[0]; }
		ND_ T const&		back ()					const	{ ASSERT( _count > 0 );  return _array[_count-1]; }


		ND_ bool  operator == (ArrayView<T> rhs) const
		{
			if ( (_array == rhs._array) & (_count == rhs._count) )
				return true;

			if ( size() != rhs.size() )
				return false;

			for (usize i = 0; i < size(); ++i)
			{
				if_unlikely( not (_array[i] == rhs[i]) )
					return false;
			}
			return true;
		}

		ND_ bool  operator >  (ArrayView<T> rhs) const
		{
			if ( size() != rhs.size() )
				return size() > rhs.size();

			for (usize i = 0; i < size(); ++i)
			{
				if_unlikely( not (_array[i] == rhs[i]) )
					return _array[i] > rhs[i];
			}
			return true;
		}
		
		ND_ bool  operator != (ArrayView<T> rhs) const	{ return not (*this == rhs); }
		ND_ bool  operator <  (ArrayView<T> rhs) const	{ return (rhs > *this); }
		ND_ bool  operator >= (ArrayView<T> rhs) const	{ return not (*this < rhs); }
		ND_ bool  operator <= (ArrayView<T> rhs) const	{ return not (*this > rhs); }


		ND_ ArrayView<T> section (usize first, usize count) const
		{
			return first < size() ?
					ArrayView<T>{ data() + first, Min( size() - first, count )} :
					ArrayView<T>{};
		}

		template <typename R>
		ND_ EnableIf< (IsTrivial<T> and IsTrivial<R>), ArrayView<R> >  Cast () const
		{
			STATIC_ASSERT( alignof(R) >= alignof(T) );
			STATIC_ASSERT( sizeof(R) > sizeof(T) ? (sizeof(R) % sizeof(T) == 0) : (sizeof(T) % sizeof(R) == 0) );

			return ArrayView<R>{ static_cast<const R*>(static_cast<const void *>( _array )), (_count * sizeof(T)) / sizeof(R) };
		}
	};
	

	template <typename T>
	ArrayView (T) -> ArrayView<T>;

}	// AE::Base


namespace std
{
	template <typename T>
	struct hash< AE::Base::ArrayView<T> >
	{
		ND_ size_t  operator () (const AE::Base::ArrayView<T> &value) const
		{
			if constexpr( AE_FAST_HASH and AE::Base::IsTrivial<T> )
			{
				return size_t(AE::Base::HashOf( value.data(), value.size() * sizeof(T) ));
			}
			else
			{
				AE::Base::HashVal	result = AE::Base::HashOf( value.size() );

				for (auto& item : value) {
					result << AE::Base::HashOf( item );
				}
				return size_t(result);
			}
		}
	};


	template <typename T>
	struct hash< vector<T> >
	{
		ND_ size_t  operator () (const vector<T> &value) const
		{
			return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
		}
	};


	template <typename T, size_t S>
	struct hash< array<T,S> >
	{
		ND_ size_t  operator () (const array<T,S> &value) const
		{
			return size_t(AE::Base::HashOf( AE::Base::ArrayView<T>{ value }));
		}
	};

}	// std

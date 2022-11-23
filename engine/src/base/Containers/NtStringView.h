// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	This string_view type guaranties that contains non-null pointer to null-terminated string (C-style string).
	Use NtStringView only as function argument.
*/

#pragma once

#include "base/Memory/MemUtils.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Containers/FixedString.h"

namespace AE::Base
{

	//
	// Null-terminated String View
	//

	template <typename T>
	struct NtBasicStringView
	{
	// types
	public:
		using Value_t		= T;
		using Self			= NtBasicStringView< T >;
		using Allocator_t	= UntypedAllocator;

	private:
		static constexpr T	NullChar = T{0};


	// variables
	private:
		T const *	_data;
		usize		_length;
		T			_buffer [32];
		bool		_isAllocated	= false;


	// methods
	public:
		NtBasicStringView ()								__NE___;
		NtBasicStringView (Self &&other)					__NE___;
		NtBasicStringView (const Self &other)				__NE___;
		NtBasicStringView (BasicStringView<T> str)			__NE___;
		NtBasicStringView (const T* str)					__NE___;
		NtBasicStringView (const T* str, usize length)		__NE___;
		template <typename A> NtBasicStringView (const BasicString<T,A> &str)__NE___;
		template <usize S> NtBasicStringView (const TFixedString<T,S> &str)	 __NE___;
		~NtBasicStringView ()								__NE___;

		Self& operator = (Self &&)							= delete;
		Self& operator = (const Self &)						= delete;
		Self& operator = (BasicStringView<T>)				= delete;
		template <typename A> Self& operator = (const BasicString<T,A> &) = delete;
		Self& operator = (const T*)							= delete;

		explicit operator BasicStringView<T> ()				C_NE___	{ return BasicStringView<T>{ _data, _length }; }
		explicit operator BasicString<T> ()					C_NE___	{ return BasicString<T>{ _data, _length }; }

		ND_ T const*	c_str ()							C_NE___	{ return _data; }
		ND_ usize		size ()								C_NE___	{ return _length; }
		ND_ usize		length ()							C_NE___	{ return _length; }
		ND_ bool		empty ()							C_NE___	{ return _length == 0; }

	private:
		bool  _Validate ()									__NE___;
		bool  _IsStatic ()									C_NE___	{ return _data == &_buffer[0]; }
	};


	using NtStringView  = NtBasicStringView< char >;
	using NtWStringView = NtBasicStringView< wchar_t >;
	


	template <typename T>
	NtBasicStringView<T>::NtBasicStringView () __NE___ :
		_data{ _buffer }, _length{ 0 }, _buffer{ 0 }
	{}

	template <typename T>
	NtBasicStringView<T>::NtBasicStringView (BasicStringView<T> str) __NE___ :
		_data{ str.data() }, _length{ str.size() }
	{
		_Validate();
	}

	template <typename T>
	template <typename A>
	NtBasicStringView<T>::NtBasicStringView (const BasicString<T,A> &str) __NE___ :
		_data{ str.data() }, _length{ str.size() }
	{
		_Validate();
	}

	template <typename T>
	NtBasicStringView<T>::NtBasicStringView (const Self &other) __NE___ :
		_data{ other._data }, _length{ other._length }
	{
		if ( other._IsStatic() )
		{
			_data = _buffer;
			std::memcpy( _buffer, other._buffer, sizeof(_buffer) );
		}
		else
			_Validate();
	}
	
	template <typename T>
	NtBasicStringView<T>::NtBasicStringView (Self &&other) __NE___ :
		_data{ other._data }, _length{ other._length }, _isAllocated{ other._isAllocated }
	{
		if ( other._IsStatic() )
		{
			_data = _buffer;
			std::memcpy( _buffer, other._buffer, sizeof(_buffer) );
		}
		other._isAllocated = false;
	}

	template <typename T>
	NtBasicStringView<T>::NtBasicStringView (const T* str) __NE___ : _data{ str }
	{
		if constexpr( IsSameTypes< T, wchar_t >)
			_length = (str ? std::wcslen(str) : 0);
		else
			_length = (str ? std::strlen(str) : 0);

		_Validate();
	}

	template <typename T>
	NtBasicStringView<T>::NtBasicStringView (const T* str, usize length) __NE___ :
		_data{ str }, _length{ length }
	{
		_Validate();
	}
	
	template <typename T>
	template <usize S>
	NtBasicStringView<T>::NtBasicStringView (const TFixedString<T,S> &str) __NE___ :
		_data{ str.c_str() }, _length{ str.length() }
	{}

	template <typename T>
	NtBasicStringView<T>::~NtBasicStringView () __NE___
	{
		if ( _isAllocated )
			Allocator_t::Deallocate( const_cast<T *>(_data), SizeOf<T> * (_length+1) );
	}
		
	template <typename T>
	bool  NtBasicStringView<T>::_Validate () __NE___
	{
		if ( not _data )
		{
			_buffer[0]	= 0;
			_data		= _buffer;
			_length		= 0;
			return false;
		}

		if ( _data[_length] == NullChar )
			return false;
		
		T *		new_data;
		Bytes	size	= SizeOf<T> * (_length+1);

		if ( size > sizeof(_buffer) )
		{
			_isAllocated= true;
			new_data	= Cast<T>( Allocator_t::Allocate( size ));
			CHECK( new_data != null );
		}
		else
			new_data = _buffer;

		std::memcpy( OUT new_data, _data, usize(size) );
		new_data[_length] = NullChar;
		_data = new_data;

		return true;
	}

} // AE::Base

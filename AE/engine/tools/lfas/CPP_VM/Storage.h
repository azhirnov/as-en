// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "CPP_VM/VirtualMachine.h"

namespace LFAS::CPP::_hidden_
{

	//
	// Value Storage
	//

	template <typename T>
	class ValueStorage
	{
	// variables
	protected:
		T	_value;


	// methods
	public:
		ValueStorage ();
		ValueStorage (const T &val);
		ValueStorage (const ValueStorage<T> &);
		ValueStorage (ValueStorage<T> &&);
		~ValueStorage ();

		ValueStorage<T>&  operator = (const ValueStorage<T> &);
		ValueStorage<T>&  operator = (ValueStorage<T> &&);

		ND_ const T  Read () const;

		template <typename V>
		void  Write (V&& value);
	};



	//
	// Class Storage
	//

	template <typename T>
	class ClassStorage : public ValueStorage<T>
	{
		StaticAssert( IsClass<T> );

	// methods
	public:
		ClassStorage ()											__NE___	= default;
		ClassStorage (const ClassStorage<T> &)							= default;
		ClassStorage (ClassStorage<T> &&)						__NE___	= default;
		ClassStorage (const T &val)										: ValueStorage<T>{val} {}

		~ClassStorage ()										__NE___ {}

		ClassStorage<T>&  operator = (const ClassStorage<T> &)			= default;
		ClassStorage<T>&  operator = (ClassStorage<T> &&)		__NE___	= default;

		ND_ const T  Read ()									const	{ return ValueStorage<T>::Read(); }

		template <typename M>
		ND_ const M  Read (M T::*member)						const;

		template <typename V>
		void  Write (V&& value)										{ return ValueStorage<T>::Write( FwdArg<V>( value )); }

		template <typename M, typename V>
		void  Write (M T::*member, V&& value);
	};



	//
	// Array Value Storage
	//

	template <typename T, typename Count>
	class ArrayValueStorage
	{
	// types
	private:
		using Self = ArrayValueStorage< T, Count >;


	// variables
	protected:
		static constexpr usize		_Count	= Count::value;

		StaticArray< T, _Count >	_arr	= {};


	// methods
	public:
		ArrayValueStorage ();
		ArrayValueStorage (const Self &)	= delete;
		ArrayValueStorage (Self &&)			= delete;
		~ArrayValueStorage ();

		Self&  operator = (const Self &)	= delete;
		Self&  operator = (Self &&)			= delete;

		ND_ const T  Read (usize idx)		const;

		template <typename V>
		void  Write (usize idx, V&& value);
	};



	//
	// Array Class Storage
	//

	template <typename T, typename Count>
	class ArrayClassStorage : public ArrayValueStorage< T, Count >
	{
		StaticAssert( IsClass<T> );

	// methods
	public:
		template <typename M>
		ND_ const M  Read (usize idx, M T::*member) const;

		template <typename M, typename V>
		void  Write (usize idx, M T::*member, V&& value);
	};


} // LFAS::CPP::_hidden_
//-----------------------------------------------------------------------------


namespace LFAS::CPP
{

	template <typename T>
	using Storage = typename Conditional< IsClass<T>,
						DeferredTemplate< LFAS::CPP::_hidden_::ClassStorage, T >,
						DeferredTemplate< LFAS::CPP::_hidden_::ValueStorage, T > >::type;

	template <typename T, usize Count>
	using ArrayStorage = typename Conditional< IsClass<T>,
							DeferredTemplate< LFAS::CPP::_hidden_::ArrayClassStorage, T, ValueToType<Count> >,
							DeferredTemplate< LFAS::CPP::_hidden_::ArrayValueStorage, T, ValueToType<Count> > >::type;

} // LFAS::CPP
//-----------------------------------------------------------------------------


namespace LFAS::CPP::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename T>
	ValueStorage<T>::ValueStorage ()
	{
		VirtualMachine::Instance().StorageCreate( AddressOf(_value), SizeOf<T> );
	}

	template <typename T>
	ValueStorage<T>::ValueStorage (const T &val) : _value{ val }
	{
		VirtualMachine::Instance().StorageCreate( AddressOf(_value), SizeOf<T> );
	}

	template <typename T>
	ValueStorage<T>::ValueStorage (const ValueStorage<T> &other) : _value{ other.Read() }
	{
		VirtualMachine::Instance().StorageCreate( AddressOf(_value), SizeOf<T> );
	}

	template <typename T>
	ValueStorage<T>::ValueStorage (ValueStorage<T> &&other) : _value{ other.Read() }
	{
		VirtualMachine::Instance().StorageCreate( AddressOf(_value), SizeOf<T> );
	}

/*
=================================================
	destructor
=================================================
*/
	template <typename T>
	ValueStorage<T>::~ValueStorage ()
	{
		VirtualMachine::Instance().StorageDestroy( AddressOf(_value) );
	}

/*
=================================================
	operator =
=================================================
*/
	template <typename T>
	ValueStorage<T>&  ValueStorage<T>::operator = (const ValueStorage<T> &rhs)
	{
		Write( rhs.Read() );
		return *this;
	}

	template <typename T>
	ValueStorage<T>&  ValueStorage<T>::operator = (ValueStorage<T> &&rhs)
	{
		Write( rhs.Read() );
		return *this;
	}

/*
=================================================
	Read
=================================================
*/
	template <typename T>
	const T  ValueStorage<T>::Read () const
	{
		VirtualMachine::Instance().StorageReadAccess( AddressOf(_value), 0_b, SizeOf<T> );
		return _value;
	}

/*
=================================================
	Write
=================================================
*/
	template <typename T>
	template <typename V>
	void  ValueStorage<T>::Write (V&& value)
	{
		VirtualMachine::Instance().StorageWriteAccess( AddressOf(_value), 0_b, SizeOf<T> );
		_value = FwdArg<V>( value );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Read
=================================================
*/
	template <typename T>
	template <typename M>
	const M  ClassStorage<T>::Read (M T::*member) const
	{
		VirtualMachine::Instance().StorageReadAccess( AddressOf(this->_value), OffsetOf(member), SizeOf<M> );
		return this->_value.*member;
	}

/*
=================================================
	Write
=================================================
*/
	template <typename T>
	template <typename M, typename V>
	void  ClassStorage<T>::Write (M T::*member, V&& value)
	{
		VirtualMachine::Instance().StorageWriteAccess( AddressOf(this->_value), OffsetOf(member), SizeOf<M> );
		this->_value.*member = FwdArg<V>( value );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename T, typename C>
	ArrayValueStorage<T,C>::ArrayValueStorage ()
	{
		VirtualMachine::Instance().StorageCreate( AddressOf(_arr), SizeOf<decltype(_arr)> );
	}

/*
=================================================
	destructor
=================================================
*/
	template <typename T, typename C>
	ArrayValueStorage<T,C>::~ArrayValueStorage ()
	{
		VirtualMachine::Instance().StorageDestroy( AddressOf(_arr) );
	}

/*
=================================================
	Read
=================================================
*/
	template <typename T, typename C>
	const T  ArrayValueStorage<T,C>::Read (usize idx) const
	{
		CHECK( idx < _arr.size() );
		VirtualMachine::Instance().StorageReadAccess( AddressOf(_arr), AddressDistance( _arr[idx], _arr ), SizeOf<T> );
		return _arr[idx];
	}

/*
=================================================
	Write
=================================================
*/
	template <typename T, typename C>
	template <typename V>
	void  ArrayValueStorage<T,C>::Write (usize idx, V&& value)
	{
		VirtualMachine::Instance().StorageWriteAccess( AddressOf(_arr), AddressDistance( _arr[idx], _arr ), SizeOf<T> );
		_arr[idx] = FwdArg<V>( value );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Read
=================================================
*/
	template <typename T, typename C>
	template <typename M>
	const M  ArrayClassStorage<T,C>::Read (usize idx, M T::*member) const
	{
		CHECK( idx < this->_arr.size() );
		VirtualMachine::Instance().StorageReadAccess( AddressOf(this->_arr), AddressDistance( this->_arr[idx], this->_arr ) + OffsetOf(member), SizeOf<M> );
		return this->_arr[idx].*member;
	}

/*
=================================================
	Write
=================================================
*/
	template <typename T, typename C>
	template <typename M, typename V>
	void  ArrayClassStorage<T,C>::Write (usize idx, M T::*member, V&& value)
	{
		VirtualMachine::Instance().StorageWriteAccess( AddressOf(this->_arr), AddressDistance( this->_arr[idx], this->_arr ) + OffsetOf(member), SizeOf<M> );
		this->_arr[idx].*member = FwdArg<V>( value );
	}

} // LFAS::CPP::_hidden_

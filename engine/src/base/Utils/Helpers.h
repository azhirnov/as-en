// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// On Destroy
	//
	struct OnDestroy
	{
	private:
		Function<void ()>	_fn;

	public:
		explicit OnDestroy (Function<void ()> &&fn) : _fn{ RVRef(fn) } {}
		~OnDestroy ()	{ _fn(); }
	};
	

	//
	// Noncopyable
	//
	class Noncopyable
	{
	public:
		Noncopyable () = default;

		Noncopyable (const Noncopyable &) = delete;
		Noncopyable (Noncopyable &&) = delete;

		Noncopyable& operator = (const Noncopyable &) = delete;
		Noncopyable& operator = (Noncopyable &&) = delete;
	};



	//
	// Movable Only
	//
	class MovableOnly
	{
	public:
		MovableOnly () = default;

		MovableOnly (MovableOnly &&) = default;
		MovableOnly& operator = (MovableOnly &&) = default;

		MovableOnly (const MovableOnly &) = delete;
		MovableOnly& operator = (const MovableOnly &) = delete;
	};


	
	//
	// Non-instancable base class
	//
	class Noninstancable
	{
	protected:
		Noninstancable () = delete;

		//~Noninstancable () = delete;

		Noninstancable (const Noninstancable &) = delete;

		Noninstancable (Noninstancable &&) = delete;

		Noninstancable& operator = (const Noninstancable &) = delete;

		Noninstancable& operator = (Noninstancable &&) = delete;
	};



	//
	// Non Allocatable
	//
	class NonAllocatable
	{
	public:
		ND_ static void*  operator new   (usize) noexcept { return null; }
		ND_ static void*  operator new[] (usize) noexcept { return null; }
			
		ND_ static void*  operator new (usize, void* where) noexcept { return where; }

			static void  operator delete   (void*, usize)  {}
			static void  operator delete[] (void*, usize)  {}
	};



	//
	// As Pointer
	//
	template <typename T>
	struct AsPointer
	{
	private:
		T	_value;

	public:
		template <typename B>
		explicit AsPointer (B && val) : _value{ FwdArg<B>(val) } {}

		AsPointer () = delete;
		AsPointer (AsPointer<T> &&) = default;
		AsPointer (const AsPointer<T> &) = default;

		AsPointer<T>&  operator = (AsPointer<T> &&) = default;
		AsPointer<T>&  operator = (const AsPointer<T> &) = default;

		ND_ explicit constexpr operator bool () const	{ return true; }

		ND_ T*			operator -> ()			{ return &_value; }
		ND_ T const*	operator -> ()	const	{ return &_value; }
	};

	namespace _hidden_
	{
		template <typename T>
		struct _ToPtr {
			using type	= AsPointer<T>;
		};
		
		template <typename T>
		struct _ToPtr< T* > {
			using type	= T*;
		};
	}

	template <typename T>
	using ToPointer = typename _hidden_::_ToPtr<T>::type;



	//
	// All Combinations
	//
	template <auto FirstValue, auto AllBits>
	struct AllCombinationsInRange
	{
	private:
		using EnumType = decltype(AllBits);

		struct Iterator
		{
			template <auto A, auto B>
			friend struct AllCombinationsInRange;

		private:
			EnumType	_value;

			explicit Iterator (EnumType val) : _value{val} {}

		public:
			Iterator (const Iterator &) = default;

				Iterator&	operator ++ ()						{ _value = EnumType( ulong(_value) + 1 );  return *this; }
				Iterator	operator ++ (int)					{ auto tmp = _value;  ++(*this);  return Iterator{ tmp }; }
			ND_ EnumType	operator * ()				const	{ return _value; }

			ND_ bool		operator == (Iterator rhs)	const	{ return _value == rhs._value; }
			ND_ bool		operator != (Iterator rhs)	const	{ return _value != rhs._value; }
		};


	public:
		ND_ Iterator	begin ()	{ return Iterator{ FirstValue }; }
		ND_ Iterator	end ()		{ return Iterator{ AllBits }; }
	};
	
	template <auto AllBits>
	struct AllCombinations : AllCombinationsInRange< decltype(AllBits){0}, AllBits >
	{};

} // AE::Base

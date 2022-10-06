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

}	// AE::Base

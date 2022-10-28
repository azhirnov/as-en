// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	UMax constant is maximum value of unsigned integer type.
*/

#pragma once

namespace AE::Base
{

	//
	// UMax
	//
	namespace _hidden_
	{
		struct _UMax
		{
			template <typename T>
			ND_ constexpr operator const T () const
			{
				STATIC_ASSERT( T(~T{0}) > T{0} );
				return T(~T{0});
			}
			
			template <typename T>
			ND_ friend constexpr bool  operator == (const T& left, const _UMax &right)
			{
				return T(right) == left;
			}
			
			template <typename T>
			ND_ friend constexpr bool  operator != (const T& left, const _UMax &right)
			{
				return T(right) != left;
			}
		};
	}
	static constexpr Base::_hidden_::_UMax		UMax {};


	
	//
	// Zero
	//
	namespace _hidden_
	{
		struct _Zero
		{
			template <typename T>
			ND_ constexpr operator const T () const
			{
				//STATIC_ASSERT( std::is_integral_v<T> || std::is_enum_v<T> );
				return T(0);
			}
			
			template <typename T>
			ND_ friend constexpr auto  operator == (const T& left, const _Zero &right)
			{
				return T(right) == left;
			}
			
			template <typename T>
			ND_ friend constexpr auto  operator != (const T& left, const _Zero &right)
			{
				return T(right) != left;
			}

			template <typename T>
			ND_ friend constexpr auto  operator > (const T& left, const _Zero &right)
			{
				return left > T(right);
			}

			template <typename T>
			ND_ friend constexpr auto  operator < (const T& left, const _Zero &right)
			{
				return left < T(right);
			}

			template <typename T>
			ND_ friend constexpr auto  operator >= (const T& left, const _Zero &right)
			{
				return left >= T(right);
			}

			template <typename T>
			ND_ friend constexpr auto  operator <= (const T& left, const _Zero &right)
			{
				return left <= T(right);
			}
		};
	}
	static constexpr Base::_hidden_::_Zero		Zero {};



	//
	// Any Float Constant
	//

	struct AnyFloatConst
	{
	private:
		const double	_d;
		const float		_f;


	public:
		constexpr AnyFloatConst (double val) : _d{val}, _f{float(val)} {}
		
		template <typename T>
		ND_ constexpr operator const T () const
		{
			if constexpr( std::is_same_v< T, double >)
				return _d;
			
			if constexpr( std::is_same_v< T, float >)
				return _f;

			// fail to compile
		}
	};



	//
	// Boolean with comment
	//
	namespace _hidden_
	{
		struct BoolType
		{
		private:
			bool	_value;

		public:
			constexpr BoolType (bool value) : _value{ value } {}
			constexpr BoolType (const BoolType &other) : _value{ other._value } {}
			
			constexpr BoolType (bool value, const char* description) : _value{ value } { (void)(description); }

			constexpr BoolType& operator = (const BoolType &rhs)	{ _value = rhs._value;  return *this; }
			constexpr BoolType& operator = (bool rhs)				{ _value = rhs;  return *this; }

			ND_ bool*		operator & ()							{ return &_value; }
			ND_ bool const*	operator & ()					const	{ return &_value; }

			ND_ constexpr operator bool ()					const	{ return _value; }
		};

		template <bool Value>
		struct NamedBoolean
		{
			constexpr NamedBoolean ()						{}
			explicit constexpr NamedBoolean (const char*)	{}

			ND_ constexpr operator bool () const			{ return Value; }
			ND_ constexpr operator BoolType () const		{ return BoolType{Value}; }
		};
	}

	using False = Base::_hidden_::NamedBoolean<false>;
	using True  = Base::_hidden_::NamedBoolean<true>;
	using Bool  = Base::_hidden_::BoolType;


} // AE::Base

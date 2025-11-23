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
			NdCx__ operator const T ()											C_NE___
			{
				StaticAssert( T(~T{0}) > T{0} );
				return T(~T{0});
			}

			template <typename T>
			NdCx__ friend bool  operator == (const T& left, const _UMax &right)	__NE___
			{
				return T(right) == left;
			}

			template <typename T>
			NdCx__ friend bool  operator != (const T& left, const _UMax &right)	__NE___
			{
				return T(right) != left;
			}
		};
	}

	using UMax_t = Base::_hidden_::_UMax;
	static constexpr inline UMax_t	UMax {};



	//
	// Zero
	//
	namespace _hidden_
	{
		struct _Zero
		{
			template <typename T>
			NdCx__ operator const T ()											C_NE___
			{
				if constexpr( IsClass<T> )
					return T{};
				else
				if constexpr( IsAnyScalar<T> )
					return T(0);
				else
					return T{};
			}

			template <typename T>
			NdCx__ friend auto  operator == (const T& left, const _Zero &right)	__NE___
			{
				return T(right) == left;
			}

			template <typename T>
			NdCx__ friend auto  operator != (const T& left, const _Zero &right)	__NE___
			{
				return T(right) != left;
			}

			template <typename T>
			NdCx__ friend auto  operator > (const T& left, const _Zero &right)	__NE___
			{
				return left > T(right);
			}

			template <typename T>
			NdCx__ friend auto  operator < (const T& left, const _Zero &right)	__NE___
			{
				return left < T(right);
			}

			template <typename T>
			NdCx__ friend auto  operator >= (const T& left, const _Zero &right)	__NE___
			{
				return left >= T(right);
			}

			template <typename T>
			NdCx__ friend auto  operator <= (const T& left, const _Zero &right)	__NE___
			{
				return left <= T(right);
			}
		};
	}

	using Zero_t = Base::_hidden_::_Zero;
	static constexpr inline Zero_t	Zero {};


	//
	// Default
	//
	namespace _hidden_
	{
		template <typename T, bool IsEnum>
		struct _IsEnumWithUnknown2 : CT_False {};

		template <typename T>
		struct _IsEnumWithUnknown2< T, true > : CT_True {};

		template <typename T>
		static constexpr bool	_IsEnumWithUnknown = _IsEnumWithUnknown2< T, std::is_enum_v<T> >::value;


		template <typename T, int Index>
		struct _GetDefaultValueForUninitialized2 {};

		template <typename T>
		struct _GetDefaultValueForUninitialized2< T, 0 > {
			NdCx__ static T  Get ()													__NE___	{ return T{}; }
		};

		template <typename T>
		struct _GetDefaultValueForUninitialized2< T, /*int, float, pointer*/2 > {
			NdCx__ static T  Get ()													__NE___	{ return T(0); }
		};

		template <typename T>
		struct _GetDefaultValueForUninitialized2< T, /*enum*/1 > {
			NdCx__ static T  Get ()													__NE___	{ return T::Unknown; }
		};


		template <typename T>
		struct _GetDefaultValueForUninitialized
		{
			static constexpr int GetIndex ()										__NE___
			{
				return	_IsEnumWithUnknown<T>		? 1 :
							std::is_floating_point<T>::value or
							std::is_integral<T>::value		 or
							std::is_pointer<T>::value		 or
							std::is_enum<T>::value  ? 2 :
													  0;
			}

			NdCx__ static T  GetDefault ()											__NE___
			{
				return _GetDefaultValueForUninitialized2< T, GetIndex() >::Get();
			}
		};


		struct DefaultType final
		{
			__Cx__ DefaultType ()													__NE___
			{}

			template <typename T>
			NdCx__ operator T ()													C_NE___
			{
				return _GetDefaultValueForUninitialized<T>::GetDefault();
			}

			template <typename T>
			NdCx__ friend bool  operator == (const T& lhs, const DefaultType &)		__NE___
			{
				return lhs == _GetDefaultValueForUninitialized<T>::GetDefault();
			}

			template <typename T>
			NdCx__ friend bool  operator != (const T& lhs, const DefaultType &rhs)	__NE___
			{
				return not (lhs == rhs);
			}
		};

	} // _hidden_

	using Default_t	= Base::_hidden_::DefaultType;
	static constexpr inline Default_t	Default = {};



	//
	// Any Float Constant
	//

	struct AnyFloatConst
	{
	private:
		const double	_d;
		const float		_f;


	public:
		__Cx__ explicit AnyFloatConst (double val)	__NE___	: _d{val}, _f{float(val)} {}

		template <typename T>
		NdCx__ operator const T ()					C_NE___
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
			__Cx__			BoolType (const BoolType &other)				__NE___	= default;

			template <typename T>
			__Cx__ explicit	BoolType (const T &value)						__NE___	: _value{bool{ value }} {}

			__Cx__ explicit	BoolType (bool value, const char* description)	__NE___	: _value{ value } { (void)(description); }

			__Cx__ BoolType&	operator = (const BoolType &rhs)			__NE___	= default;
			__Cx__ BoolType&	operator = (bool rhs)						__NE___	{ _value = rhs;  return *this; }

			Nd____ bool*		operator & ()								__NE___	{ return &_value; }
			Nd____ bool const*	operator & ()								C_NE___	{ return &_value; }

			NdCx__ operator bool ()											C_NE___	{ return _value; }
		};

		template <bool Value>
		struct NamedBoolean
		{
			__Cx__ NamedBoolean ()						__NE___	{}
			__Cx__ explicit NamedBoolean (const char*)	__NE___	{}

			NdCx__ operator bool ()						C_NE___	{ return Value; }
			NdCx__ operator BoolType ()					C_NE___	{ return BoolType{Value}; }
		};
	}


	using False = Base::_hidden_::NamedBoolean<false>;
	using True  = Base::_hidden_::NamedBoolean<true>;
	using Bool  = Base::_hidden_::BoolType;



	//
	// Math
	//
	template <typename T>
	static constexpr T EulerNumber = T( 2.71828182845904523536 );


} // AE::Base

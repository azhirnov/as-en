// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


namespace AE::Base::_hidden_
{
	template <typename T, bool IsEnum>
	struct _IsEnumWithUnknown2 {
		static constexpr bool	value = false;
	};

	template <typename T>
	struct _IsEnumWithUnknown2< T, true > {
		static constexpr bool	value = true; //Detect_Unknown<T>::value;
	};

	template <typename T>
	static constexpr bool	_IsEnumWithUnknown = _IsEnumWithUnknown2< T, std::is_enum_v<T> >::value;


	template <typename T, int Index>
	struct _GetDefaultValueForUninitialized2 {};

	template <typename T>
	struct _GetDefaultValueForUninitialized2< T, 0 > {
		static constexpr T Get ()														__NE___	{ return T(); }
	};

	template <typename T>
	struct _GetDefaultValueForUninitialized2< T, /*int, float, pointer*/2 > {
		static constexpr T Get ()														__NE___	{ return T(0); }
	};

	template <typename T>
	struct _GetDefaultValueForUninitialized2< T, /*enum*/1 > {
		static constexpr T Get ()														__NE___	{ return T::Unknown; }
	};


	template <typename T>
	struct _GetDefaultValueForUninitialized
	{
		static constexpr int GetIndex ()												__NE___
		{
			return	_IsEnumWithUnknown<T>  ? 1 :
						std::is_floating_point<T>::value or
						std::is_integral<T>::value		 or
						std::is_pointer<T>::value		 or
						std::is_enum<T>::value  ? 2 :
							0;
		}

		static constexpr T GetDefault ()												__NE___
		{
			return _GetDefaultValueForUninitialized2< T, GetIndex() >::Get();
		}
	};


	struct DefaultType final
	{
		constexpr DefaultType ()														__NE___
		{}

		template <typename T>
		ND_ constexpr operator T ()														C_NE___
		{
			return _GetDefaultValueForUninitialized<T>::GetDefault();
		}

		template <typename T>
		ND_ friend constexpr bool  operator == (const T& lhs, const DefaultType &)		__NE___
		{
			return lhs == _GetDefaultValueForUninitialized<T>::GetDefault();
		}

		template <typename T>
		ND_ friend constexpr bool  operator != (const T& lhs, const DefaultType &rhs)	__NE___
		{
			return not (lhs == rhs);
		}
	};

} // AE::Base::_hidden_


namespace AE::Base
{
	static constexpr inline Base::_hidden_::DefaultType		Default = {};

} // AE::Base

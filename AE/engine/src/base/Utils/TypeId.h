// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base::_hidden_
{
	//
	// Static Type ID
	//
	template <uint UID>
	struct StaticTypeID final
	{
	private:
		usize	_value;

	public:
		constexpr StaticTypeID ()									__NE___ : _value{~usize{0}} {}

		ND_ constexpr bool  operator == (StaticTypeID<UID> rhs)		C_NE___	{ return _value == rhs._value; }
		ND_ constexpr bool  operator != (StaticTypeID<UID> rhs)		C_NE___	{ return _value != rhs._value; }
		ND_ constexpr bool  operator >  (StaticTypeID<UID> rhs)		C_NE___	{ return _value >  rhs._value; }
		ND_ constexpr bool  operator <  (StaticTypeID<UID> rhs)		C_NE___	{ return _value <  rhs._value; }
		ND_ constexpr bool  operator >= (StaticTypeID<UID> rhs)		C_NE___	{ return _value >= rhs._value; }
		ND_ constexpr bool  operator <= (StaticTypeID<UID> rhs)		C_NE___	{ return _value <= rhs._value; }

		ND_ constexpr usize			Get ()							C_NE___	{ return _value; }
		ND_ constexpr StringView	Name ()							C_NE___	{ return ""; }
	};

	template <uint UID>
	struct BaseStaticTypeIdOf
	{
	protected:
		static inline usize	_counter = 0;
	};

	template <typename T, uint UID>
	struct StaticTypeIdOf : BaseStaticTypeIdOf<UID>
	{
		ND_ static StaticTypeID<UID>  Get () __NE___
		{
			static usize id = BaseStaticTypeIdOf<UID>::_counter++;
			return BitCast< StaticTypeID<UID> >( id );
		}
	};

	template <typename T, uint UID>	struct StaticTypeIdOf< const T, UID > final : StaticTypeIdOf<T, UID> {};

} // AE::Base::_hidden_


template <uint32_t UID>
struct std::hash< AE::Base::_hidden_::StaticTypeID<UID> >
{
	ND_ size_t  operator () (const AE::Base::_hidden_::StaticTypeID<UID> &value) C_NE___
	{
		return value.Get();
	}
};
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_RTTI

namespace AE::Base::_hidden_
{
	//
	// STD Type ID
	//
	struct StdTypeID final
	{
	private:
		enum UnknownType {};

		std::type_index		_value;

	public:
		StdTypeID ()								__NE___	: _value{ typeid(UnknownType) } {}
		StdTypeID (const std::type_index &value)	__NE___	: _value{ value } {}

		ND_ bool  operator == (StdTypeID rhs)		C_NE___	{ return _value == rhs._value; }
		ND_ bool  operator != (StdTypeID rhs)		C_NE___	{ return _value != rhs._value; }
		ND_ bool  operator >  (StdTypeID rhs)		C_NE___	{ return _value >  rhs._value; }
		ND_ bool  operator <  (StdTypeID rhs)		C_NE___	{ return _value <  rhs._value; }
		ND_ bool  operator >= (StdTypeID rhs)		C_NE___	{ return _value >= rhs._value; }
		ND_ bool  operator <= (StdTypeID rhs)		C_NE___	{ return _value <= rhs._value; }

		ND_ std::type_index		Get ()				C_NE___	{ return _value; }
		ND_ StringView			Name ()				C_NE___	{ return _value.name(); }
	};


	template <typename T>
	struct StdTypeIdOf final
	{
		ND_ static StdTypeID  Get () __NE___
		{
			return StdTypeID( typeid(T) );
		}
	};

} // AE::Base::_hidden_


template <>
struct std::hash< AE::Base::_hidden_::StdTypeID >
{
	ND_ size_t  operator () (const AE::Base::_hidden_::StdTypeID &value) C_NE___
	{
		return std::hash< std::type_index >{}( value.Get() );
	}
};


namespace AE::Base
{
	using TypeId = Base::_hidden_::StdTypeID;
/*
=================================================
	TypeIdOf
=================================================
*/
	template <typename T>
	ND_ TypeId  TypeIdOf () __NE___
	{
		return Base::_hidden_::StdTypeIdOf<T>::Get();
	}

	template <typename T>
	ND_ TypeId  TypeIdOf (const T&) __NE___
	{
		return TypeIdOf<T>();
	}

} // AE::Base

#else

namespace AE::Base
{
	using TypeId = Base::_hidden_::StaticTypeID<0>;

/*
=================================================
	TypeIdOf
=================================================
*/
	template <typename T>
	ND_ TypeId  TypeIdOf () __NE___
	{
		return Base::_hidden_::StaticTypeIdOf<T,0>::Get();
	}

	template <typename T>
	ND_ TypeId  TypeIdOf (const T&) __NE___
	{
		return TypeIdOf<T>();
	}

} // AE::Base

#endif

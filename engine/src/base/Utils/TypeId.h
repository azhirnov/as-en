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
		constexpr StaticTypeID () : _value{~usize{0}} {}

		ND_ constexpr bool  operator == (StaticTypeID<UID> rhs) const	{ return _value == rhs._value; }
		ND_ constexpr bool  operator != (StaticTypeID<UID> rhs) const	{ return _value != rhs._value; }
		ND_ constexpr bool  operator >  (StaticTypeID<UID> rhs) const	{ return _value >  rhs._value; }
		ND_ constexpr bool  operator <  (StaticTypeID<UID> rhs) const	{ return _value <  rhs._value; }
		ND_ constexpr bool  operator >= (StaticTypeID<UID> rhs) const	{ return _value >= rhs._value; }
		ND_ constexpr bool  operator <= (StaticTypeID<UID> rhs) const	{ return _value <= rhs._value; }

		ND_ constexpr usize			Get ()	const						{ return _value; }
		ND_ constexpr const char *	Name ()	const						{ return ""; }
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
		ND_ static StaticTypeID<UID>  Get ()
		{
			static usize id = BaseStaticTypeIdOf<UID>::_counter++;
			return BitCast< StaticTypeID<UID> >( id );
		}
	};

	template <typename T, uint UID>	struct StaticTypeIdOf< const T,          UID > final : StaticTypeIdOf<T, UID> {};
	template <typename T, uint UID>	struct StaticTypeIdOf< volatile T,       UID > final : StaticTypeIdOf<T, UID> {};
	template <typename T, uint UID>	struct StaticTypeIdOf< const volatile T, UID > final : StaticTypeIdOf<T, UID> {};

}	// AE::Base::_hidden_

namespace std
{
	template <uint32_t UID>
	struct hash< AE::Base::_hidden_::StaticTypeID<UID> >
	{
		ND_ size_t  operator () (const AE::Base::_hidden_::StaticTypeID<UID> &value) const
		{
			return value.Get();
		}
	};

}	// std
//-----------------------------------------------------------------------------

	
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
		StdTypeID () : _value{ typeid(UnknownType) } {}
		StdTypeID (const std::type_index &value) : _value{ value } {}
			
		ND_ bool  operator == (StdTypeID rhs) const		{ return _value == rhs._value; }
		ND_ bool  operator != (StdTypeID rhs) const		{ return _value != rhs._value; }
		ND_ bool  operator >  (StdTypeID rhs) const		{ return _value >  rhs._value; }
		ND_ bool  operator <  (StdTypeID rhs) const		{ return _value <  rhs._value; }
		ND_ bool  operator >= (StdTypeID rhs) const		{ return _value >= rhs._value; }
		ND_ bool  operator <= (StdTypeID rhs) const		{ return _value <= rhs._value; }

		ND_ std::type_index		Get ()	const			{ return _value; }
		ND_ const char *		Name ()	const			{ return _value.name(); }
	};

		
	template <typename T>
	struct StdTypeIdOf final
	{
		ND_ static StdTypeID  Get ()
		{
			return StdTypeID( typeid(T) );
		}
	};

}	// AE::Base::_hidden_

namespace std
{
	template <>
	struct hash< AE::Base::_hidden_::StdTypeID >
	{
		ND_ size_t  operator () (const AE::Base::_hidden_::StdTypeID &value) const
		{
			return std::hash< std::type_index >{}( value.Get() );
		}
	};

}	// std


#if 0
namespace AE::Base
{
	using TypeId = Base::_hidden_::StaticTypeID<0>;
	
/*
=================================================
	TypeIdOf
=================================================
*/
	template <typename T>
	ND_ forceinline static TypeId  TypeIdOf ()
	{
		return Base::_hidden_::StaticTypeIdOf<T,0>::Get();
	}

	template <typename T>
	ND_ forceinline static TypeId  TypeIdOf (const T&)
	{
		return TypeIdOf<T>();
	}

}	// AE::Base

#else
namespace AE::Base
{
	using TypeId = Base::_hidden_::StdTypeID;
/*
=================================================
	TypeIdOf
=================================================
*/
	template <typename T>
	ND_ forceinline static TypeId  TypeIdOf ()
	{
		return Base::_hidden_::StdTypeIdOf<T>::Get();
	}

	template <typename T>
	ND_ forceinline static TypeId  TypeIdOf (const T&)
	{
		return TypeIdOf<T>();
	}

}	// AE::Base

#endif
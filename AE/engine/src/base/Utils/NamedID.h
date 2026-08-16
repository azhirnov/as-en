// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Containers/FixedString.h"
#include "base/CompileTime/Hash.h"

namespace AE::Base
{

	//
	// ID With String
	//

	template <usize Size, uint UID, bool Optimize, uint Seed = UMax>
	struct NamedID
	{
	// types
	public:
		using Self			= NamedID< Size, UID, Optimize, Seed >;
		using WithString_t	= NamedID< Size, UID, false, Seed >;
		using Optimized_t	= NamedID< Size, UID, true, Seed >;
		using Ref			= const Self;

		using Hasher_t		= std::hash< Self >;

		struct EqualTo_t {
			ND_ bool  operator () (const Self &lhs, const Self &rhs) C_NE___ {
				return lhs._hash == rhs._hash;
			}
		};


	// variables
	private:
		HashVal32	_hash;

		static constexpr THashVal<uint>	_emptyHash	= CT_Hash( "", 0, Seed );


	// methods
	public:
		__Cx__ NamedID ()								__NE___ : _hash{_emptyHash}										{}
		__Cx__ explicit NamedID (HashVal32 hash)		__NE___ : _hash{hash}											{}
		__Cx__ explicit NamedID (StringView name)		__NE___ : _hash{CT_Hash( name.data(), name.length(), Seed )}	{}
		__Cx__ explicit NamedID (const char* name)		__NE___ : _hash{CT_Hash( name, UMax, Seed )}					{}

		__Cx__ NamedID (Self &&)						__NE___	= default;
		__Cx__ NamedID (const Self &)					__NE___	= default;

		__Cx__ Self&	operator = (Self &&)			__NE___ = default;
		__Cx__ Self&	operator = (const Self &)		__NE___ = default;

		NdCx__ bool		operator == (const Self &rhs)	C_NE___	{ return _hash == rhs._hash; }
		NdCx__ bool		operator != (const Self &rhs)	C_NE___	{ return not (*this == rhs); }
		NdCx__ bool		operator >  (const Self &rhs)	C_NE___	{ return _hash > rhs._hash; }
		NdCx__ bool		operator <  (const Self &rhs)	C_NE___	{ return rhs > *this; }
		NdCx__ bool		operator >= (const Self &rhs)	C_NE___	{ return not (*this <  rhs); }
		NdCx__ bool		operator <= (const Self &rhs)	C_NE___	{ return not (*this >  rhs); }

		NdCx__ explicit operator uint ()				C_NE___	{ return uint{_hash}; }

		NdCx__ HashVal		GetHash ()					C_NE___	{ return HashVal{uint{_hash}}; }
		NdCx__ HashVal32	GetHash32 ()				C_NE___	{ return _hash; }
		NdCx__ bool			IsDefined ()				C_NE___	{ return _hash != _emptyHash; }
		NdCx__ static bool	IsOptimized ()				__NE___	{ return true; }
		NdCx__ static uint	GetSeed ()					__NE___	{ return Seed; }
		NdCx__ static uint	GetUID ()					__NE___	{ return UID; }
		NdCx__ static usize	MaxStringLength ()			__NE___	{ return Size; }
	};



	//
	// ID With String
	//

	template <usize Size, uint UID, uint Seed>
	struct NamedID< Size, UID, false, Seed >
	{
	// types
	public:
		using Self			= NamedID< Size, UID, false, Seed >;
		using Optimized_t	= NamedID< Size, UID, true, Seed >;
		using WithString_t	= NamedID< Size, UID, false, Seed >;
		using Ref			= Self const &;

		using Hasher_t		= std::hash< Self >;

		struct EqualTo_t {
			ND_ bool  operator () (const Self &lhs, const Self &rhs) C_NE___ {
				return	lhs._hash == rhs._hash and
						(not lhs._name.empty() and not rhs._name.empty()  ? lhs._name == rhs._name : true);
			}
		};


	// variables
	private:
		HashVal32			_hash;
		FixedString<Size>	_name;

		static constexpr THashVal<uint>	_emptyHash	= CT_Hash( "", 0, Seed );


	// methods
	public:
		__Cx__ NamedID ()									__NE___ : _hash{_emptyHash}														{}
		__Cx__ explicit NamedID (HashVal32 hash)			__NE___ :  _hash{hash}															{}
		__Cx__ explicit NamedID (StringView name)			__NE___ : _hash{ CT_Hash( name.data(), name.length(), Seed )}, _name{name}		{}
		__Cx__ explicit NamedID (const char* name)			__NE___ : _hash{ CT_Hash( name, UMax, Seed )}, _name{name}						{}

		template <usize StrSize>
		__Cx__ explicit NamedID (const FixedString<StrSize> &name) __NE___ : _hash{CT_Hash( name.data(), name.length(), Seed )}, _name{name}{}

		__Cx__ explicit NamedID (const Optimized_t &other)	__NE___ : _hash{other.GetHash32()}												{}

		__Cx__ NamedID (Self &&)							__NE___	= default;
		__Cx__ NamedID (const Self &)						__NE___	= default;

		__Cx__ Self&	operator = (Self &&)				__NE___ = default;
		__Cx__ Self&	operator = (const Self &)			__NE___ = default;

		NdCx__ bool		operator == (const Self &rhs)		C_NE___	{ return _hash == rhs._hash; }
		NdCx__ bool		operator != (const Self &rhs)		C_NE___	{ return not (*this == rhs); }
		NdCx__ bool		operator >  (const Self &rhs)		C_NE___	{ return _hash > rhs._hash; }
		NdCx__ bool		operator <  (const Self &rhs)		C_NE___	{ return rhs > *this; }
		NdCx__ bool		operator >= (const Self &rhs)		C_NE___	{ return not (*this <  rhs); }
		NdCx__ bool		operator <= (const Self &rhs)		C_NE___	{ return not (*this >  rhs); }

		NdCx__ operator Optimized_t ()						C_NE___	{ return Optimized_t{ GetHash32() }; }
		NdCx__ explicit operator uint ()					C_NE___	{ return uint{_hash}; }

		NdCx__ StringView	GetName ()						C_NE___	{ return _name; }
		NdCx__ HashVal		GetHash ()						C_NE___	{ return HashVal{uint{_hash}}; }
		NdCx__ HashVal32	GetHash32 ()					C_NE___	{ return _hash; }
		NdCx__ bool			IsDefined ()					C_NE___	{ return _hash != _emptyHash; }
		NdCx__ static bool	IsOptimized ()					__NE___	{ return false; }
		NdCx__ static uint	GetSeed ()						__NE___	{ return Seed; }
		NdCx__ static uint	GetUID ()						__NE___	{ return UID; }
		NdCx__ static usize	MaxStringLength ()				__NE___	{ return Size; }
	};


	template <usize Size, uint UID, uint Seed>
	Nd__In bool  operator == (const NamedID< Size, UID, false, Seed > &lhs, const NamedID< Size, UID, true, Seed > &rhs) __NE___ {
		return lhs.GetHash32() == rhs.GetHash32();
	}

	template <usize Size, uint UID, uint Seed>
	Nd__In bool  operator == (const NamedID< Size, UID, true, Seed > &lhs, const NamedID< Size, UID, false, Seed > &rhs) __NE___ {
		return lhs.GetHash32() == rhs.GetHash32();
	}


	template <usize Size, uint UID, uint Seed>	struct TMemCopyAvailable<		NamedID< Size, UID, true, Seed >>	: CT_True  {};
	template <usize Size, uint UID, uint Seed>	struct TZeroMemAvailable<		NamedID< Size, UID, true, Seed >>	: CT_False {};
	template <usize Size, uint UID, uint Seed>	struct TTriviallySerializable<	NamedID< Size, UID, true, Seed >>	: CT_True  {};
	template <usize Size, uint UID, uint Seed>	struct TTriviallyDestructible<	NamedID< Size, UID, true, Seed >>	: CT_True  {};

	template <usize Size, uint UID, uint Seed>	struct TMemCopyAvailable<		NamedID< Size, UID, false, Seed >>	: CT_True  {};
	template <usize Size, uint UID, uint Seed>	struct TZeroMemAvailable<		NamedID< Size, UID, false, Seed >>	: CT_False {};
	template <usize Size, uint UID, uint Seed>	struct TTriviallySerializable<	NamedID< Size, UID, false, Seed >>	: CT_False {};
	template <usize Size, uint UID, uint Seed>	struct TTriviallyDestructible<	NamedID< Size, UID, false, Seed >>	: CT_True  {};

} // AE::Base


template <size_t Size, uint32_t UID, bool Optimize, uint32_t Seed>
struct std::hash< AE::Base::NamedID<Size, UID, Optimize, Seed> >
{
	ND_ size_t  operator () (const AE::Base::NamedID<Size, UID, Optimize, Seed> &value) C_NE___ {
		return size_t(value.GetHash());
	}
};

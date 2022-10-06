// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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


	// variables
	private:
		HashVal32	_hash;

		static constexpr THashVal<uint>	_emptyHash	= CT_Hash( "", 0, Seed );


	// methods
	public:
		constexpr NamedID () : _hash{_emptyHash} {}
		explicit constexpr NamedID (HashVal32 hash) : _hash{hash} {}
		explicit constexpr NamedID (StringView name)  : _hash{CT_Hash( name.data(), name.length(), Seed )} {}
		explicit constexpr NamedID (const char *name) : _hash{CT_Hash( name, UMax, Seed )} {}

		ND_ constexpr bool operator == (const Self &rhs) const		{ return _hash == rhs._hash; }
		ND_ constexpr bool operator != (const Self &rhs) const		{ return not (*this == rhs); }
		ND_ constexpr bool operator >  (const Self &rhs) const		{ return _hash > rhs._hash; }
		ND_ constexpr bool operator <  (const Self &rhs) const		{ return rhs > *this; }
		ND_ constexpr bool operator >= (const Self &rhs) const		{ return not (*this <  rhs); }
		ND_ constexpr bool operator <= (const Self &rhs) const		{ return not (*this >  rhs); }

		ND_ constexpr HashVal		GetHash ()			const		{ return HashVal{uint{_hash}}; }
		ND_ constexpr HashVal32		GetHash32 ()		const		{ return _hash; }
		ND_ constexpr bool			IsDefined ()		const		{ return _hash != _emptyHash; }
		ND_ constexpr static bool	IsOptimized ()					{ return true; }
		ND_ constexpr static uint	GetSeed ()						{ return Seed; }
		ND_ constexpr static uint	GetUID ()						{ return UID; }
		ND_ constexpr static usize	MaxStringLength ()				{ return Size; }
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

		using Hasher_t		= std::hash< Self >;

		struct EqualTo_t {
			ND_ bool  operator () (const Self &lhs, const Self &rhs) const {
				return lhs._hash == rhs._hash and lhs._name == rhs._name;
			}
		};


	// variables
	private:
		HashVal32			_hash;
		FixedString<Size>	_name;

		static constexpr THashVal<uint>	_emptyHash	= CT_Hash( "", 0, Seed );


	// methods
	public:
		constexpr NamedID () : _hash{_emptyHash} {}
		explicit constexpr NamedID (HashVal32 hash) :  _hash{hash} {}
		explicit constexpr NamedID (StringView name)  : _hash{CT_Hash( name.data(), name.length(), Seed )}, _name{name} {}
		explicit constexpr NamedID (const char *name) : _hash{CT_Hash( name, UMax, Seed )}, _name{name} {}

		template <usize StrSize>
		explicit constexpr NamedID (const FixedString<StrSize> &name) : _hash{CT_Hash( name.data(), name.length(), Seed )}, _name{name} {}
		
		explicit constexpr NamedID (const Optimized_t &other) : _hash{other.GetHash32()} {}

		ND_ constexpr bool operator == (const Self &rhs) const		{ return _hash == rhs._hash; }
		ND_ constexpr bool operator != (const Self &rhs) const		{ return not (*this == rhs); }
		ND_ constexpr bool operator >  (const Self &rhs) const		{ return _hash > rhs._hash; }
		ND_ constexpr bool operator <  (const Self &rhs) const		{ return rhs > *this; }
		ND_ constexpr bool operator >= (const Self &rhs) const		{ return not (*this <  rhs); }
		ND_ constexpr bool operator <= (const Self &rhs) const		{ return not (*this >  rhs); }

		ND_ constexpr operator Optimized_t ()			const		{ return Optimized_t{ GetHash32() }; }

		ND_ constexpr StringView	GetName ()			const		{ return _name; }
		ND_ constexpr HashVal		GetHash ()			const		{ return HashVal{uint{_hash}}; }
		ND_ constexpr HashVal32		GetHash32 ()		const		{ return _hash; }
		ND_ constexpr bool			IsDefined ()		const		{ return _hash != _emptyHash; }
		ND_ constexpr static bool	IsOptimized ()					{ return false; }
		ND_ constexpr static uint	GetSeed ()						{ return Seed; }
		ND_ constexpr static uint	GetUID ()						{ return UID; }
		ND_ constexpr static usize	MaxStringLength ()				{ return Size; }
	};

	
	template <usize Size, uint UID, uint Seed>
	ND_ inline bool  operator == (const NamedID< Size, UID, false, Seed > &lhs, const NamedID< Size, UID, true, Seed > &rhs) {
		return lhs.GetHash32() == rhs.GetHash32();
	}
	
	template <usize Size, uint UID, uint Seed>
	ND_ inline bool  operator == (const NamedID< Size, UID, true, Seed > &lhs, const NamedID< Size, UID, false, Seed > &rhs) {
		return lhs.GetHash32() == rhs.GetHash32();
	}

	
	template <usize Size, uint UID, uint Seed>	struct TMemCopyAvailable<		NamedID< Size, UID, true, Seed >>	{ static constexpr bool  value = true;  };
	template <usize Size, uint UID, uint Seed>	struct TZeroMemAvailable<		NamedID< Size, UID, true, Seed >>	{ static constexpr bool  value = false; };
	template <usize Size, uint UID, uint Seed>	struct TTrivialySerializable<	NamedID< Size, UID, true, Seed >>	{ static constexpr bool  value = true;  };

	template <usize Size, uint UID, uint Seed>	struct TMemCopyAvailable<		NamedID< Size, UID, false, Seed >>	{ static constexpr bool  value = true;  };
	template <usize Size, uint UID, uint Seed>	struct TZeroMemAvailable<		NamedID< Size, UID, false, Seed >>	{ static constexpr bool  value = false; };
	template <usize Size, uint UID, uint Seed>	struct TTrivialySerializable<	NamedID< Size, UID, false, Seed >>	{ static constexpr bool  value = false; };
	
}	// AE::Base

namespace std
{
	template <size_t Size, uint32_t UID, bool Optimize, uint32_t Seed>
	struct hash< AE::Base::NamedID<Size, UID, Optimize, Seed> >
	{
		ND_ size_t  operator () (const AE::Base::NamedID<Size, UID, Optimize, Seed> &value) const {
			return size_t(value.GetHash());
		}
	};

}	// std

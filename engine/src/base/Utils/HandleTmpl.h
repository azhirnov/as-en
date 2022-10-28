// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base//Math/BitMath.h"

namespace AE::Base
{

	//
	// Handle template
	//

	template <usize IndexSize, usize GenerationSize, uint UID>
	struct HandleTmpl
	{
	// types
	public:
		using Self			= HandleTmpl< IndexSize, GenerationSize, UID >;
		using Index_t		= BitSizeToUInt< IndexSize >;
		using Generation_t	= BitSizeToUInt< GenerationSize >;
		using Value_t		= BitSizeToUInt< IndexSize + GenerationSize >;


	// variables
	private:
		union {
			Value_t		_value	= UMax;
			struct {
				Value_t		index	: IndexSize;
				Value_t		gen		: GenerationSize;
			}			_packed;
		};

		static constexpr Value_t	_IndexMask	= Math::ToBitMask<Value_t>( IndexSize );
		static constexpr Value_t	_GenMask	= Math::ToBitMask<Value_t>( GenerationSize );
		static constexpr Value_t	_GenOffset	= IndexSize;


	// methods
	private:
		explicit constexpr HandleTmpl (Value_t val) : _value{val} {}

	public:
		constexpr HandleTmpl () {}
		constexpr HandleTmpl (const Self &other) : _value{other._value} {}

		constexpr HandleTmpl (Value_t index, Value_t gen) :
			_value{Value_t(index) | (Value_t(gen) << _GenOffset)}
		{
			ASSERT( _packed.index == index );
			ASSERT( _packed.gen == gen );
		}

		ND_ constexpr bool					IsValid ()						const	{ return _value != UMax; }
		ND_ constexpr Index_t				Index ()						const	{ return _value & _IndexMask; }
		ND_ constexpr Generation_t			Generation ()					const	{ return _value >> _GenOffset; }
		ND_ HashVal							GetHash ()						const;
		ND_ constexpr Value_t				Data ()							const	{ return _value; }

		ND_ constexpr bool					operator == (const Self &rhs)	const	{ return _value == rhs._value; }
		ND_ constexpr bool					operator != (const Self &rhs)	const	{ return not (*this == rhs); }

		ND_ explicit constexpr				operator bool ()				const	{ return IsValid(); }

		ND_ static constexpr Index_t		MaxIndex ()								{ return _IndexMask; }
		ND_ static constexpr Generation_t	MaxGeneration ()						{ return _GenMask; }
		ND_ static constexpr Self			FromData (Value_t value)				{ return Self{ value }; }
	};
	


	//
	// Debuggable Handle template
	//

	template <usize IndexSize, usize GenerationSize, uint UID>
	struct HandleTmplDbg : HandleTmpl< IndexSize, GenerationSize, UID >
	{
	// types
	public:
		using Base_t		= HandleTmpl< IndexSize, GenerationSize, UID >;
		using Self			= HandleTmplDbg< IndexSize, GenerationSize, UID >;
		using Index_t		= typename Base_t::Index_t;
		using Generation_t	= typename Base_t::Generation_t;
		using Value_t		= typename Base_t::Value_t;

	public:
		constexpr HandleTmplDbg () {}
		constexpr HandleTmplDbg (const Self &other) : HandleTmplDbg{ other } {}

		constexpr HandleTmplDbg (Value_t index, Value_t gen) : HandleTmplDbg{ index, gen } {}

	// debugging
	public:
		DEBUG_ONLY(
			class IDbgBaseClass;
			IDbgBaseClass*	_class = null;
		)
	};



	//
	// Strong Reference
	//

	template <typename IDType>
	struct Strong;

	
	template <usize IndexSize, usize GenerationSize, uint UID>
	struct Strong < HandleTmpl< IndexSize, GenerationSize, UID >>
	{
	// types
	public:
		using ID_t			= HandleTmpl< IndexSize, GenerationSize, UID >;
		using Self			= Strong< ID_t >;
		using Index_t		= typename ID_t::Index_t;
		using Generation_t	= typename ID_t::Generation_t;
		using Value_t		= typename ID_t::Value_t;


	// variables
	private:
		ID_t	_id;


	// methods
	public:
		constexpr Strong ()													{}
		constexpr Strong (Self &&other) : _id{other._id}					{ other._id = Default; }
		constexpr explicit Strong (const ID_t &id) : _id{id}				{}
		constexpr Strong (Value_t index, Value_t gen) : _id{index, gen}		{}
		constexpr ~Strong ()												{ ASSERT(not IsValid()); } // handle must be released
		
		constexpr Self&				Attach (ID_t id)						{ ASSERT(not IsValid());  _id = id;  return *this; }

		constexpr Self&				operator = (Self &&rhs)					{ ASSERT(not IsValid());  _id = rhs._id;  rhs._id = Default;  return *this; }
		constexpr Self&				operator = (const Self &rhs)			{ ASSERT(not IsValid());  _id = rhs._id;  rhs._id = Default;  return *this; }
		
		ND_ constexpr ID_t			Get ()							const	{ return _id; }
		ND_ constexpr ID_t			Release ()								{ ID_t temp{_id};  _id = Default;  return temp; }
		ND_ constexpr bool			IsValid ()						const	{ return bool(_id); }

		ND_ constexpr ID_t const&	operator * ()					const	{ return _id; }
		ND_ constexpr ID_t const*	operator -> ()					const	{ return &_id; }

		ND_ constexpr bool			operator == (const Self &rhs)	const	{ return _id == rhs._id; }
		ND_ constexpr bool			operator != (const Self &rhs)	const	{ return _id != rhs._id; }

		ND_ constexpr explicit		operator bool ()				const	{ return IsValid(); }

		ND_ constexpr				operator ID_t ()				const	{ return _id; }
	};

	
	template <usize IdxSz, usize Gen, uint UID>	struct TMemCopyAvailable< HandleTmpl< IdxSz, Gen, UID >> { static constexpr bool  value = true; };



	//
	// Atomic Strong Reference
	//
	
	template <typename IDType>
	struct StrongAtom
	{
	// types
	public:
		using ID_t			= IDType;
		using StrongID_t	= Strong< ID_t >;
		using Self			= StrongAtom< IDType >;
		using Index_t		= typename IDType::Index_t;
		using Generation_t	= typename IDType::Generation_t;
		using Value_t		= typename IDType::Value_t;
		

	// variables
	private:
		std::atomic<Value_t>	_id;


	// methods
	public:
		StrongAtom () : _id{ ID_t{}.Data() }					{}
		explicit StrongAtom (ID_t id) : _id{id.Data()}			{}
		~StrongAtom ()											{ ASSERT(not IsValid()); } // handle must be released
		
		ND_ ID_t		Attach (ID_t id)						{ return ID_t::FromData( _id.exchange( id.Data(), std::memory_order_relaxed )); }
		ND_ ID_t		Attach (StrongID_t id)					{ return Attach( id.Release() ); }

		ND_ StrongID_t	Release ()								{ return StrongID_t{ ID_t::FromData( _id.exchange( ID_t{}.Data(), std::memory_order_relaxed ))}; }

		ND_ ID_t		Get ()							const	{ return ID_t::FromData( _id.load( std::memory_order_relaxed )); }
		ND_ bool		IsValid ()						const	{ return bool(Get()); }

		ND_ explicit	operator bool ()				const	{ return IsValid(); }
		ND_ 			operator ID_t ()				const	{ return Get(); }
		
		ND_ bool		operator == (const ID_t &rhs)	const	{ return Get() == rhs._id; }
		ND_ bool		operator != (const ID_t &rhs)	const	{ return Get() != rhs._id; }
	};

	
/*
=================================================
	GetHash
=================================================
*/
	template <usize IdxSz, usize Gen, uint UID>
	HashVal  HandleTmpl< IdxSz, Gen, UID >::GetHash () const
	{
		if constexpr( sizeof(Value_t) <= sizeof(HashVal) )
			return HashVal{_value};
		else
			return HashOf(_value);
	}

} // AE::Base

namespace std
{
	template <size_t IndexSize, size_t GenerationSize, uint32_t UID>
	struct hash< AE::Base::HandleTmpl<IndexSize, GenerationSize, UID> >
	{
		ND_ size_t  operator () (const AE::Base::HandleTmpl<IndexSize, GenerationSize, UID> &value) const
		{
			return size_t(value.GetHash());
		}
	};

} // std

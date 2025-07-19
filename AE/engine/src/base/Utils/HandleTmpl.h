// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base//Math/BitMath.h"
#include "base/Utils/Atomic.h"

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

		static constexpr Value_t	_IndexMask	= Base::ToBitMask<Value_t>( IndexSize );
		static constexpr Value_t	_GenMask	= Base::ToBitMask<Value_t>( GenerationSize );
		static constexpr Value_t	_GenOffset	= IndexSize;


	// methods
	private:
		explicit constexpr HandleTmpl (Value_t val) : _value{val} {}

	public:
		__Cx__ HandleTmpl ()										__NE___ = default;
		__Cx__ HandleTmpl (const Self &other)						__NE___ : _value{other._value} {}

		__Cx__ HandleTmpl (Value_t index, Value_t gen)				__NE___ :
			_value{Value_t(index) | (Value_t(gen) << _GenOffset)}
		{
			ASSERT( _packed.index == index );
			ASSERT( _packed.gen == gen );
		}

		__Cx__ Self&  operator = (const Self &rhs)					__NE___ { _value = rhs._value;  return *this; }

		NdCx__ bool					IsValid ()						C_NE___	{ return _value != UMax; }
		NdCx__ Index_t				Index ()						C_NE___	{ return _value & _IndexMask; }
		NdCx__ Generation_t			Generation ()					C_NE___	{ return _value >> _GenOffset; }
		Nd____ HashVal				GetHash ()						C_NE___;
		NdCx__ Value_t				Data ()							C_NE___	{ return _value; }

		NdCx__ bool					operator == (const Self &rhs)	C_NE___	{ return _value == rhs._value; }
		NdCx__ bool					operator != (const Self &rhs)	C_NE___	{ return not (*this == rhs); }
		NdCx__ bool					operator <  (const Self &rhs)	C_NE___	{ return _value < rhs._value; }

		NdCx__ explicit				operator bool ()				C_NE___	{ return IsValid(); }

		NdCx__ static Index_t		MaxIndex ()						__NE___	{ return _IndexMask; }
		NdCx__ static Generation_t	MaxGeneration ()				__NE___	{ return _GenMask; }
		NdCx__ static Self			FromData (Value_t value)		__NE___	{ return Self{ value }; }
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
		__Cx__ HandleTmplDbg ()								__NE___ = default;
		__Cx__ HandleTmplDbg (const Self &other)			__NE___ : Base_t{ other } {}
		__Cx__ HandleTmplDbg (Value_t index, Value_t gen)	__NE___ : Base_t{ index, gen } {}

	// debugging
	public:
		DEBUG_ONLY(
			class IDbgBaseClass;
			IDbgBaseClass*	_class = null;
		)
	};



	//
	// Strong Reference for HandleTmpl
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
		__Cx__ Strong ()									__NE___	= default;
		__Cx__ Strong (Self &&other)						__NE___ : _id{other._id}	{ other._id = Default; }
		__Cx__ explicit Strong (const ID_t &id)				__NE___ : _id{id}			{}
		__Cx__ Strong (Value_t index, Value_t gen)			__NE___ : _id{index, gen}	{}
		__Cx__ ~Strong ()									__NE___	{ ASSERT_MSG( not IsValid(), "handle must be released" ); }

		__Cx__ Self			Attach (ID_t id)				__NE___	{ ID_t  temp{_id};  _id = id;  return Self{temp}; }

		__Cx__ Self&		operator = (Self &&rhs)			__NE___	{ ASSERT(not IsValid());  _id = rhs._id;  rhs._id = Default;  return *this; }
		__Cx__ Self&		operator = (const Self &rhs)	__NE___	{ ASSERT(not IsValid());  _id = rhs._id;  rhs._id = Default;  return *this; }

		NdCx__ ID_t			Get ()							C_NE___	{ return _id; }
		NdCx__ ID_t			Release ()						__NE___	{ ID_t temp{_id};  _id = Default;  return temp; }
		NdCx__ bool			IsValid ()						C_NE___	{ return bool(_id); }

		NdCx__ ID_t const&	operator * ()					C_NE___	{ return _id; }
		NdCx__ ID_t const*	operator -> ()					C_NE___	{ return &_id; }

		NdCx__ bool			operator == (const Self &rhs)	C_NE___	{ return _id == rhs._id; }
		NdCx__ bool			operator != (const Self &rhs)	C_NE___	{ return _id != rhs._id; }
		NdCx__ bool			operator <  (const Self &rhs)	C_NE___	{ return _id <  rhs._id; }

		NdCx__ explicit		operator bool ()				C_NE___	{ return IsValid(); }

		NdCx__				operator ID_t ()				C_NE___	{ return _id; }
	};



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
		Atomic<Value_t>		_id;


	// methods
	public:
		StrongAtom ()									__NE___ : _id{ ID_t{}.Data() }			{}
		explicit StrongAtom (StrongID_t id)				__NE___ : _id{ id.Release().Data() }	{}
		~StrongAtom ()									__NE___	{ ASSERT(not IsValid()); } // handle must be released

		StrongAtom (Self &&)							= delete;
		StrongAtom (const Self &)						= delete;

		Self&  operator = (Self &&)						= delete;
		Self&  operator = (const Self &)				= delete;

		ND_ StrongID_t	Attach (ID_t id)				__NE___	{ return StrongID_t{ ID_t::FromData( _id.exchange( id.Data() ))}; }
		ND_ StrongID_t	Attach (StrongID_t id)			__NE___	{ return Attach( id.Release() ); }

		ND_ bool		SetIfEmpty (ID_t id)			__NE___	{ Value_t  exp = ID_t{}.Data();  return _id.CAS_Loop( INOUT exp, id.Data() ); }

		ND_ StrongID_t	Release ()						__NE___	{ return StrongID_t{ ID_t::FromData( _id.exchange( ID_t{}.Data() ))}; }

		ND_ ID_t		Get ()							C_NE___	{ return ID_t::FromData( _id.load() ); }
		ND_ bool		IsValid ()						C_NE___	{ return bool(Get()); }

		ND_ explicit	operator bool ()				C_NE___	{ return IsValid(); }
		ND_ 			operator ID_t ()				C_NE___	{ return Get(); }

		ND_ bool		operator == (const ID_t &rhs)	C_NE___	{ return Get() == rhs; }
		ND_ bool		operator != (const ID_t &rhs)	C_NE___	{ return Get() != rhs; }
	};
//-----------------------------------------------------------------------------


	template <usize IdxSz, usize Gen, uint UID>	struct TMemCopyAvailable     < HandleTmpl< IdxSz, Gen, UID >>	: CT_True  {};
	template <usize IdxSz, usize Gen, uint UID> struct TTriviallySerializable< HandleTmpl< IdxSz, Gen, UID >>	: CT_True  {};
	template <usize IdxSz, usize Gen, uint UID> struct TZeroMemAvailable     < HandleTmpl< IdxSz, Gen, UID >>	: CT_False {};
	template <usize IdxSz, usize Gen, uint UID> struct TTriviallyDestructible< HandleTmpl< IdxSz, Gen, UID >>	: CT_True  {};

/*
=================================================
	IsHandleTmpl
=================================================
*/
namespace _hidden_
{
	template <typename T>
	struct _IsHandleTmpl : CT_False {};

	template <usize IndexSize, usize GenerationSize, uint UID>
	struct _IsHandleTmpl< HandleTmpl< IndexSize, GenerationSize, UID >> : CT_True {};
}
	template <typename T>
	static constexpr bool	IsHandleTmpl = Base::_hidden_::_IsHandleTmpl< RemoveAllQualifiers< T >>::value;

/*
=================================================
	GetHash
=================================================
*/
	template <usize IdxSz, usize Gen, uint UID>
	HashVal  HandleTmpl< IdxSz, Gen, UID >::GetHash () C_NE___
	{
		if constexpr( sizeof(Value_t) <= sizeof(HashVal) )
			return HashVal{_value};
		else
			return HashOf(_value);
	}

} // AE::Base


template <size_t IndexSize, size_t GenerationSize, uint32_t UID>
struct std::hash< AE::Base::HandleTmpl<IndexSize, GenerationSize, UID> >
{
	ND_ size_t  operator () (const AE::Base::HandleTmpl<IndexSize, GenerationSize, UID> &value) C_NE___
	{
		return size_t(value.GetHash());
	}
};

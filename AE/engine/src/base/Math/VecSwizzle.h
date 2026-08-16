// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/Color.h"

namespace AE::Base
{

	//
	// Vector Swizzle
	//

	struct VecSwizzle
	{
	// variables
	private:
		ushort		_value	= 0;

		static constexpr uint	_Mask = 0x7;


	// methods
	public:
		__Cx__ VecSwizzle ()																__NE___	{}
		explicit VecSwizzle (const PackedVec<uint,4> &comp)									__NE___;

		template <typename T, int I, glm::qualifier Q>
		ND_ TVec<T,4,Q>			Transform (const TVec<T,I,Q> &src)							C_NE___;

		template <typename T>
		ND_ RGBAColor<T>		Transform (const RGBAColor<T> &src)							C_NE___;

		ND_ PackedVec<uint,4>	ToVec ()													C_NE___;

		NdCx__ uint		Get ()																C_NE___	{ return _value; }
		NdCx__ bool		IsUndefined ()														C_NE___	{ return _value == 0; }

		NdCx__ bool		operator == (VecSwizzle rhs)										C_NE___	{ return _value == rhs._value; }
		NdCx__ bool		operator >  (VecSwizzle rhs)										C_NE___	{ return _value >  rhs._value; }

		NdCx__ static VecSwizzle  VecDefault (usize size)									__NE___;

		friend __Cx__ VecSwizzle  operator ""_vecSwizzle (const char* str, const usize len)__NE___;


	private:
		__Cx__ explicit VecSwizzle (ushort val)												__NE___	: _value{val} {}

		NdCx__ static uint  _CharToValue (char c)											__NE___;
	};


/*
=================================================
	_CharToValue
=================================================
*/
	constexpr uint  VecSwizzle::_CharToValue (char c) __NE___
	{
		return	((c == 'x') or (c == 'X'))	? 1 :
				((c == 'y') or (c == 'Y'))	? 2 :
				((c == 'z') or (c == 'Z'))	? 3 :
				((c == 'w') or (c == 'W'))	? 4 :
				(c == '0')					? 5 :
				((c == '1') or (c == '+'))	? 6 :
				(c == '-')					? 7 :
											  0;
	}

/*
=================================================
	operator ""
=================================================
*/
	NdCx__ VecSwizzle  operator ""_vecSwizzle (const char* str, const usize len) __NE___
	{
		ASSERT_Cx( len > 0 and len <= 4 );

		VecSwizzle	res;
		uint		sw = 0;

		for (usize i = 0; (i < len) and (i < 4); ++i)
		{
			const char	c = str[i];
			const uint	v = VecSwizzle::_CharToValue( c );

			ASSERT_Cx( v != 0 );	// 'c' must be X, Y, Z, W, 0, 1, +, -
			sw |= (v << (3 - i) * 4);
		}
		res._value = CheckCast{ sw };
		return res;
	}

/*
=================================================
	constructor
=================================================
*/
	inline VecSwizzle::VecSwizzle (const PackedVec<uint,4> &comp) __NE___ : _value{0}
	{
		ASSERT(All( comp <= 7u ));

		_value |= (comp.x & _Mask) << 12;
		_value |= (comp.y & _Mask) << 8;
		_value |= (comp.z & _Mask) << 4;
		_value |= (comp.w & _Mask);
	}

/*
=================================================
	Transform
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	TVec<T,4,Q>  VecSwizzle::Transform (const TVec<T,I,Q> &src) C_NE___
	{
		T	temp [8] = {};
		for (int i = 0; i < I; ++i) temp[i+1] = src[i];
		temp[6] = T{1};
		temp[7] = T{-1};

		const PackedVec<uint,4>	sw = ToVec();
		TVec<T,4,Q>				result;

		for (int i = 0; i < 4; ++i) {
			result[i] = temp[ sw[i] ];
		}
		return result;
	}

/*
=================================================
	Transform
=================================================
*/
	template <typename T>
	RGBAColor<T>  VecSwizzle::Transform (const RGBAColor<T> &src) C_NE___
	{
		T	temp [8] = {};
		for (int i = 0; i < 4; ++i) temp[i+1] = src[i];
		temp[6] = T{1};
		temp[7] = T{-1};

		const PackedVec<uint,4>	sw = ToVec();
		RGBAColor<T>			result;

		for (int i = 0; i < 4; ++i) {
			result[i] = temp[ sw[i] ];
		}
		return result;
	}

/*
=================================================
	ToVec
=================================================
*/
	inline PackedVec<uint,4>  VecSwizzle::ToVec () C_NE___
	{
		return PackedVec<uint,4>{ (_value >> 12) & _Mask, (_value >> 8) & _Mask, (_value >> 4) & _Mask, _value & _Mask };
	}

/*
=================================================
	operator *
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ TVec<T,4,Q>  operator * (const TVec<T,I,Q> &lhs, VecSwizzle rhs) __NE___
	{
		return rhs.Transform( lhs );
	}

/*
=================================================
	VecDefault
=================================================
*/
	__CxIn VecSwizzle  VecSwizzle::VecDefault (usize size) __NE___
	{
		switch ( size )
		{
			case 1 :	return VecSwizzle{ 0x1000 };
			case 2 :	return VecSwizzle{ 0x1200 };
			case 3 :	return VecSwizzle{ 0x1230 };
			case 4 :	return VecSwizzle{ 0x1234 };
		}
		return VecSwizzle{};
	}
//-----------------------------------------------------------------------------


	template <> struct TMemCopyAvailable< AE::Base::VecSwizzle >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Base::VecSwizzle >	: CT_True {};

} // AE::Base


template <>
struct std::hash< AE::Base::VecSwizzle >
{
	ND_ size_t  operator () (const AE::Base::VecSwizzle &value) C_NE___
	{
		return size_t( value.Get() );
	}
};

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	enum class EVertexType : ushort
	{
		// vector size
		_VecOffset			= 0,
		_Vec2				= 1 << _VecOffset,
		_Vec3				= 2 << _VecOffset,
		_Vec4				= 3 << _VecOffset,
		_VecMask			= 0x3 << _VecOffset,

		// type
		_TypeOffset			= 2,
		_Byte				=  1 << _TypeOffset,
		_UByte				=  2 << _TypeOffset,
		_Short				=  3 << _TypeOffset,
		_UShort				=  4 << _TypeOffset,
		_Int				=  5 << _TypeOffset,
		_UInt				=  6 << _TypeOffset,
		_Long				=  7 << _TypeOffset,
		_ULong				=  8 << _TypeOffset,
		_Half				=  9 << _TypeOffset,
		_Float				= 10 << _TypeOffset,
		_Double				= 11 << _TypeOffset,
		_Int_10_10_10_2		= 12 << _TypeOffset,
		_TypeMask			= 0xF << _TypeOffset,

		// flags
		_FlagsOffset		= 6,
		NormalizedFlag		= 1 << _FlagsOffset,	// convert integer to normalized float value (-1..+1)
		ScaledFlag			= 2 << _FlagsOffset,	// convert integer to float value (min..max)
		_FlagMask			= 0x3 << _FlagsOffset,

		_Count				= (_VecMask | _TypeMask | _FlagMask) + 1,


		// default types
	#define AE_VERTEXTYPE_LIST( _visitor_ )\
		_visitor_( Byte,				_Byte )\
		_visitor_( Byte2,				_Byte | _Vec2 )\
		_visitor_( Byte3,				_Byte | _Vec3 )\
		_visitor_( Byte4,				_Byte | _Vec4 )\
		\
		_visitor_( Byte_Norm,			Byte  | NormalizedFlag )\
		_visitor_( Byte2_Norm,			Byte2 | NormalizedFlag )\
		_visitor_( Byte3_Norm,			Byte3 | NormalizedFlag )\
		_visitor_( Byte4_Norm,			Byte4 | NormalizedFlag )\
		\
		_visitor_( Byte_Scaled,			Byte  | ScaledFlag )\
		_visitor_( Byte2_Scaled,		Byte2 | ScaledFlag )\
		_visitor_( Byte3_Scaled,		Byte3 | ScaledFlag )\
		_visitor_( Byte4_Scaled,		Byte4 | ScaledFlag )\
		\
		_visitor_( UByte,				_UByte )\
		_visitor_( UByte2,				_UByte | _Vec2 )\
		_visitor_( UByte3,				_UByte | _Vec3 )\
		_visitor_( UByte4,				_UByte | _Vec4 )\
		\
		_visitor_( UByte_Norm,			UByte  | NormalizedFlag )\
		_visitor_( UByte2_Norm,			UByte2 | NormalizedFlag )\
		_visitor_( UByte3_Norm,			UByte3 | NormalizedFlag )\
		_visitor_( UByte4_Norm,			UByte4 | NormalizedFlag )\
		\
		_visitor_( UByte_Scaled,		UByte  | ScaledFlag )\
		_visitor_( UByte2_Scaled,		UByte2 | ScaledFlag )\
		_visitor_( UByte3_Scaled,		UByte3 | ScaledFlag )\
		_visitor_( UByte4_Scaled,		UByte4 | ScaledFlag )\
		\
		_visitor_( Short,				_Short )\
		_visitor_( Short2,				_Short | _Vec2 )\
		_visitor_( Short3,				_Short | _Vec3 )\
		_visitor_( Short4,				_Short | _Vec4 )\
		\
		_visitor_( Short_Norm,			Short  | NormalizedFlag )\
		_visitor_( Short2_Norm,			Short2 | NormalizedFlag )\
		_visitor_( Short3_Norm,			Short3 | NormalizedFlag )\
		_visitor_( Short4_Norm,			Short4 | NormalizedFlag )\
		\
		_visitor_( Short_Scaled,		Short  | ScaledFlag )\
		_visitor_( Short2_Scaled,		Short2 | ScaledFlag )\
		_visitor_( Short3_Scaled,		Short3 | ScaledFlag )\
		_visitor_( Short4_Scaled,		Short4 | ScaledFlag )\
		\
		_visitor_( UShort,				_UShort )\
		_visitor_( UShort2,				_UShort | _Vec2 )\
		_visitor_( UShort3,				_UShort | _Vec3 )\
		_visitor_( UShort4,				_UShort | _Vec4 )\
		\
		_visitor_( UShort_Norm,			UShort  | NormalizedFlag )\
		_visitor_( UShort2_Norm,		UShort2 | NormalizedFlag )\
		_visitor_( UShort3_Norm,		UShort3 | NormalizedFlag )\
		_visitor_( UShort4_Norm,		UShort4 | NormalizedFlag )\
		\
		_visitor_( UShort_Scaled,		UShort  | ScaledFlag )\
		_visitor_( UShort2_Scaled,		UShort2 | ScaledFlag )\
		_visitor_( UShort3_Scaled,		UShort3 | ScaledFlag )\
		_visitor_( UShort4_Scaled,		UShort4 | ScaledFlag )\
		\
		_visitor_( Int,					_Int )\
		_visitor_( Int2,				_Int | _Vec2 )\
		_visitor_( Int3,				_Int | _Vec3 )\
		_visitor_( Int4,				_Int | _Vec4 )\
		\
		_visitor_( UInt,				_UInt )\
		_visitor_( UInt2,				_UInt | _Vec2 )\
		_visitor_( UInt3,				_UInt | _Vec3 )\
		_visitor_( UInt4,				_UInt | _Vec4 )\
		\
		_visitor_( Long,				_Long )\
		_visitor_( Long2,				_Long | _Vec2 )\
		_visitor_( Long3,				_Long | _Vec3 )\
		_visitor_( Long4,				_Long | _Vec4 )\
		\
		_visitor_( ULong,				_ULong )\
		_visitor_( ULong2,				_ULong | _Vec2 )\
		_visitor_( ULong3,				_ULong | _Vec3 )\
		_visitor_( ULong4,				_ULong | _Vec4 )\
		\
		_visitor_( Half,				_Half )\
		_visitor_( Half2,				_Half | _Vec2 )\
		_visitor_( Half3,				_Half | _Vec3 )\
		_visitor_( Half4,				_Half | _Vec4 )\
		\
		_visitor_( Float,				_Float )\
		_visitor_( Float2,				_Float | _Vec2 )\
		_visitor_( Float3,				_Float | _Vec3 )\
		_visitor_( Float4,				_Float | _Vec4 )\
		\
		_visitor_( Double,				_Double )\
		_visitor_( Double2,				_Double | _Vec2 )\
		_visitor_( Double3,				_Double | _Vec3 )\
		_visitor_( Double4,				_Double | _Vec4 )\
		\
		_visitor_( UInt_2_10_10_10,			_Int_10_10_10_2 | _Vec4 )\
		_visitor_( UInt_2_10_10_10_Norm,	_Int_10_10_10_2 | _Vec4 | NormalizedFlag )\
		_visitor_( UInt_2_10_10_10_Scaled,	_Int_10_10_10_2 | _Vec4 | ScaledFlag )\

		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )	_name_ = (_value_),
		AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT )
		#undef AE_VERTEXTYPE_VISIT

		Unknown				= 0,
	};
	AE_BIT_OPERATORS( EVertexType );


	enum class EVertexInputRate : ubyte
	{
		Vertex,
		Instance,
		_Count,
		Unknown	= 0xFF,
	};


	enum class EIndex : ubyte
	{
		UShort,
		UInt,
		_Count,
		Unknown = 0xFF,
	};


} // AE::Graphics

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/EnumBitSet.h"
#include "graphics/Private/EnumUtils.h"
#include "UnitTest_Common.h"

namespace
{
	static void PixelFormat_Test1 ()
	{
		for (EPixelFormat fmt = EPixelFormat(0); fmt < EPixelFormat::_Count; fmt = EPixelFormat(uint(fmt) + 1))
		{
			auto&	info = EPixelFormat_GetInfo( fmt );
			TEST( info.format == fmt );
		}
	}


	static void VertexType_Test1 ()
	{
		HashSet<EVertexType>	unique;
		EnumBitSet<EVertexType>	bits;

		#define AE_VERTEXTYPE_VISIT( _name_, _value_ )											\
			STATIC_ASSERT( EVertexType::_name_ < EVertexType::_Count );							\
			TEST( unique.insert( EVertexType::_name_ ).second );								\
			TEST( (bits & EnumBitSet<EVertexType>{}.insert( EVertexType::_name_ )).None() );	\
			bits.insert( EVertexType::_name_ );

		AE_VERTEXTYPE_LIST( AE_VERTEXTYPE_VISIT );
		#undef AE_VERTEXTYPE_VISIT
	}
}


extern void UnitTest_PixelFormat ()
{
	PixelFormat_Test1();
	VertexType_Test1();

	TEST_PASSED();
}

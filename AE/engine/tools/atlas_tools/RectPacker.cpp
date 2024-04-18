// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "RectPacker.h"

namespace AE::AtlasTools
{
namespace
{
/*
=================================================
	BestDimensionForArea
=================================================
*/
	ND_ static uint2  BestDimensionForArea (ulong area, const uint2 minSize)
	{
		const uint	side	= Max( 1, IntLog2( area ) / 2 );
		uint2		result	{1u << side};

		ASSERT( All( minSize <= result ));
		Unused( minSize );

		for (; result.x * result.y < area;)
		{
			if ( result.x <= result.y )
				result.x *= 2;
			else
				result.y *= 2;
		}
		return result;
	}
}

/*
=================================================
	Pack
=================================================
*/
	bool  RectPacker::Pack ()
	{
		uint2	size = BestDimensionForArea( _maxArea, _maxSize );

		std::sort( _rects.begin(), _rects.end(),
					[] (auto& lhs, auto& rhs)
					{
						auto	l_area = Area( lhs.size );
						auto	r_area = Area( rhs.size );
						return lhs.size.x == rhs.size.x ? l_area > r_area : lhs.size.x > rhs.size.x;
					});


		for (uint i = 0; i < 100; ++i)
		{


			// increase texture size
			if ( size.x <= size.y )
				size.x *= 2;
			else
				size.y *= 2;
		}
		return false;
	}

/*
=================================================
	Reset
=================================================
*/
	void  RectPacker::Reset ()
	{
		_rects.clear();
		_maxArea	= 0;
		_maxSize	= uint2{0};
		_targetSize	= uint2{0};
	}


} // AE::AtlasTools

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_STB
# include "RectPackerSTB.h"

namespace AE::AtlasTools
{

/*
=================================================
	_BestDimensionForArea
=================================================
*/
	uint2  RectPackerSTB::_BestDimensionForArea (const ulong area, const uint targetW, const uint2 minSize)
	{
		if ( targetW > 0 )
		{
			ASSERT( targetW >= minSize.x );

			uint2	result	{targetW, uint(area / targetW)};
			return Max( result, minSize );
		}

		const uint	side	= Max( 1, IntLog2( area ) / 2 );
		uint2		result	{1u << side};

		for (; Any( result < minSize ) or (result.x * result.y < area);)
		{
			if ( result.x <= result.y )
				result.x *= 2;
			else
				result.y *= 2;
		}

		ASSERT( All( minSize <= result ));
		return result;
	}

/*
=================================================
	_FindMaxY
=================================================
*/
	void  RectPackerSTB::_FindMaxY (ArrayView<stbrp_rect> rects, OUT uint &maxY)
	{
		maxY = 0;

		for (auto& r : rects) {
			maxY = Max( maxY, uint(r.y + r.w) );
		}
	}

/*
=================================================
	Pack
=================================================
*/
	bool  RectPackerSTB::Pack ()
	{
		uint2				size = _BestDimensionForArea( _maxArea, _targetWidth, _maxSize );
		Array<stbrp_node>	packer_nodes;

		CHECK_ERR( _targetWidth == 0 or _targetWidth == size.x );

		for (uint i = 0; i < 100; ++i)
		{
			stbrp_context	packer_ctx;

			packer_nodes.clear();
			packer_nodes.resize( size.x );

			::stbrp_init_target( &packer_ctx, size.x, size.y, packer_nodes.data(), int(packer_nodes.size()) );
			::stbrp_setup_heuristic( &packer_ctx, STBRP_HEURISTIC_Skyline_BF_sortHeight );

			if ( ::stbrp_pack_rects( &packer_ctx, _rects.data(), int(_rects.size()) ) == 1 )
			{
				_targetSize = size;
				_FindMaxY( _rects, OUT _targetSize.y );
				return true;
			}

			// increase texture size
			if ( _targetWidth > 0 )
				size.y *= 2;
			else
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
	void  RectPackerSTB::Reset ()
	{
		_rects.clear();
		_maxArea		= 0;
		_maxSize		= uint2{0};
		_targetSize		= uint2{0};
		_targetWidth	= 0;
	}


} // AE::AtlasTools

# define STB_RECT_PACK_IMPLEMENTATION
# include "stb_rect_pack.h"

#endif // AE_ENABLE_STB

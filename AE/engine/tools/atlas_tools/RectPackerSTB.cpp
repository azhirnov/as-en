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
	uint2  RectPackerSTB::_BestDimensionForArea (ulong area, const uint2 minSize)
	{
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
	Pack
=================================================
*/
	bool  RectPackerSTB::Pack ()
	{
		uint2				size = _BestDimensionForArea( _maxArea, _maxSize );
		Array<stbrp_node>	packer_nodes;

		for (uint i = 0; i < 100; ++i)
		{
			stbrp_context	packer_ctx;

			packer_nodes.clear();
			packer_nodes.resize( size.x );

			stbrp_init_target( &packer_ctx, size.x, size.y, packer_nodes.data(), int(packer_nodes.size()) );
			stbrp_setup_heuristic( &packer_ctx, STBRP_HEURISTIC_Skyline_BF_sortHeight );

			if ( stbrp_pack_rects( &packer_ctx, _rects.data(), int(_rects.size()) ) == 1 )
			{
				_targetSize = size;
				return true;
			}

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
	void  RectPackerSTB::Reset ()
	{
		_rects.clear();
		_maxArea	= 0;
		_maxSize	= uint2{0};
		_targetSize	= uint2{0};
	}


} // AE::AtlasTools

# define STB_RECT_PACK_IMPLEMENTATION
# include "stb_rect_pack.h"

#endif // AE_ENABLE_STB

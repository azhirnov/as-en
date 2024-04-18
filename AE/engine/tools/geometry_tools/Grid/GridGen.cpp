// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Algorithms/ArrayUtils.h"
#include "geometry_tools/Grid/GridGen.h"

namespace AE::GeometryTools
{

/*
=================================================
	Create
=================================================
*/
	bool  GridGen::Create (const uint vertsPerEdge, const uint patchSize) __NE___
	{
		_vertices.clear();
		_indices.clear();

		CHECK_ERR( vertsPerEdge > 1 );
		CHECK_ERR( patchSize == 3 or patchSize == 4 or patchSize == 16 );

		_vertices.reserve( vertsPerEdge * vertsPerEdge * 3 );
		_indices.reserve( vertsPerEdge * vertsPerEdge * 4 );

		const float	vert_scale = 1.f / float(vertsPerEdge - 1);

		for (uint y = 0; y < vertsPerEdge; ++y)
		{
			for (uint x = 0; x < vertsPerEdge; ++x)
			{
				Vertex&	vert = _vertices.emplace_back();
				vert.uv = float2{uint2{ x, y }} * vert_scale;

				if ( x != 0 and y != 0 )
				{
					if ( patchSize == 4 )
					{
						_indices.push_back( Index( (y + 0) * vertsPerEdge  + (x + 0) ));
						_indices.push_back( Index( (y + 0) * vertsPerEdge  + (x - 1) ));
						_indices.push_back( Index( (y - 1) * vertsPerEdge  + (x - 1) ));
						_indices.push_back( Index( (y - 1) * vertsPerEdge  + (x + 0) ));
					}
					else
					if ( patchSize == 3 )
					{
						_indices.push_back( Index( (y + 0) * vertsPerEdge  + (x + 0) ));
						_indices.push_back( Index( (y + 0) * vertsPerEdge  + (x - 1) ));
						_indices.push_back( Index( (y - 1) * vertsPerEdge  + (x - 1) ));

						_indices.push_back( Index( (y - 1) * vertsPerEdge  + (x - 1) ));
						_indices.push_back( Index( (y - 1) * vertsPerEdge  + (x + 0) ));
						_indices.push_back( Index( (y + 0) * vertsPerEdge  + (x + 0) ));
					}
					else
					if ( patchSize == 16 and IsMultipleOf( x, 3 ) and IsMultipleOf( y, 3 ))
					{
						for (uint i = 0; i < 4; ++i)
						for (uint j = 0; j < 4; ++j)
							_indices.push_back( Index( (y + i - 3) * vertsPerEdge + (x + j - 3) ));
					}
				}
			}
		}

		CHECK_ERR( _vertices.size() < MaxValue<Index>() );

		return true;
	}


} // AE::GeometryTools

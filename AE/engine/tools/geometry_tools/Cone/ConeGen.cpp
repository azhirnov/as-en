// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "geometry_tools/Cone/ConeGen.h"
#include "geometry_tools/Utils/TBN.h"

namespace AE::GeometryTools
{

/*
=================================================
	Create
=================================================
*/
	bool  ConeGen::Create (const uint segmentCount, const float radius, const float height) __NE___
	{
		CHECK_ERR( segmentCount >= 3 );
		CHECK_ERR( (segmentCount*2)*3 < MaxValue<Index>() );

		_positions.resize( segmentCount + 2 );
		_indices.resize( segmentCount * 2 * 3 );

		// circle (base)
		for (uint i = 0; i < segmentCount; ++i)
		{
			Rad		a = 2.f * Pi * float(i) / float(segmentCount);
			float	x = radius * Cos( a );
			float	y = radius * Sin( a );
			_positions[i] = float3{ x, y, height };
		}

		_positions[segmentCount]	= float3{ 0.f, 0.f, 0.f };		// apex
		_positions[segmentCount+1]	= float3{ 0.f, 0.f, height };	// circle center

		const uint	apex_idx	= segmentCount;
		const uint	base_idx	= segmentCount + 1;

		// side
		for (uint i = 0, j = 0; i < segmentCount; ++i, j += 3)
		{
			uint	next	= (i + 1) % segmentCount;
			_indices[j]		= Index(i);
			_indices[j+1]	= Index(next);
			_indices[j+2]	= Index(apex_idx);
		}

		// base
		for (uint i = 0, j = segmentCount*3; i < segmentCount; ++i, j += 3)
		{
			uint	next	= (i + 1) % segmentCount;

			_indices[j]		= Index(i);
			_indices[j+1]	= Index(base_idx);
			_indices[j+2]	= Index(next);
		}

		return true;
	}

} // AE::GeometryTools

// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "geometry_tools/Common.h"

namespace AE::GeometryTools
{

	//
	// Cone Generator
	//

	class ConeGen final
	{
	// types
	public:
		using Index = ushort;


	// variables
	private:
		Array<float3>		_positions;
		Array<Index>		_indices;


	// methods
	public:

		ND_	bool  Create (uint segments, float radius, float height)	__NE___;

		ND_ ArrayView<float3>	GetPositions ()							C_NE___	{ return _positions; }
		ND_ ArrayView<Index>	GetIndices ()							C_NE___	{ return _indices; }
	};


} // AE::GeometryTools

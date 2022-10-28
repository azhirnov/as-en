// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"

namespace AE::GeometryTools
{
	using namespace AE::Base;


	//
	// Geometry Generator
	//

	struct GeometryGenerator
	{
		static bool  CreateGrid (OUT Array<float2> &vertices, OUT Array<uint> &indices, uint numVertInSide, uint patchSize = 3, float scale = 1.0f);

		static bool  CreateCube (OUT ArrayView<float> &positions,
								 OUT ArrayView<float> &texcoords,
								 OUT ArrayView<float> &normals,
								 OUT ArrayView<uint>  &indices);
	};

} // AE::GeometryTools

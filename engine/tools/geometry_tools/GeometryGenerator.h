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
    };

} // AE::GeometryTools

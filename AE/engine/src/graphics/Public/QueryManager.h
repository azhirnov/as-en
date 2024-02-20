// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Graphics
{

    //
    // Query type
    //
    enum class EQueryType : ubyte
    {
        Timestamp,
        PipelineStatistic,
        Performance,
        AccelStructCompactedSize,
        AccelStructSize,                // require 'VK_KHR_ray_tracing_maintenance1'    // TODO: add to FeatureSet ?
        AccelStructSerializationSize,
        _Count,
        Unknown = 0xFF,
    };



    //
    // Query Manager interface
    //

    class NO_VTABLE IQueryManager
    {
    };


} // AE::Graphics

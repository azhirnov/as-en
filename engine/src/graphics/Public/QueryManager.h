// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/FrameUID.h"

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
        AccelStructSize,                // require 'VK_KHR_ray_tracing_maintenance1'
        AccelStructSerializationSize,
        _Count,
        Unknown = 0xFF,
    };



    //
    // Query Manager interface
    //

    class IQueryManager
    {
    };


} // AE::Graphics

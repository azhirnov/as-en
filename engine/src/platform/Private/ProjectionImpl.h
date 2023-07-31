// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Projection.h"

namespace AE::App
{

    //
    // Projection implementation
    //

    class ProjectionImpl final : public IProjection
    {
    // methods
    public:
        void  GenerateRays (RaysGrid &grid)         C_NE_OV;
    };



/*
=================================================
    GenerateRays
=================================================
*/
    inline void  ProjectionImpl::GenerateRays (RaysGrid &) C_NE___
    {
        /*ASSERT( All( grid.Dimension() >= 2 ));

        const float4x4  mvp = proj * view * rotation;

        pos   = mvp * float4(0.0f, 0.0f, 0.0f, 1.0f);
        pos.w = 0.0f;

        for (uint y = 0; y < grid.Dimension().y; ++y)
        {
            for (uint x = 0; x < grid.Dimension().x; ++x)
            {
                float4  ray{    (float(x) / (grid.Dimension().x - 1)) * 2.0f - 1.0f,
                                (float(y) / (grid.Dimension().y - 1)) * 2.0f - 1.0f,
                                -1.0f,
                                0.0f };

                ray = float4{ Normalize( float3{ mvp * ray }), 0.0f };

                grid(x, y) = ray;
            }
        }*/
    }


} // AE::App

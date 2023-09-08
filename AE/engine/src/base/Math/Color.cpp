// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Math/Color.h"

namespace AE::Math
{

/*
=================================================
    WavelengthToRGB
----
    from https://stackoverflow.com/questions/3407942/rgb-values-of-visible-spectrum/22681410#22681410
=================================================
*/
    RGBA32f  WavelengthToRGB (float wl) __NE___
    {
        ASSERT( wl >= 400.0f and wl <= 700.0f );

        RGBA32f     res {0.0f, 0.0f, 0.0f, 1.f};

        // red
        if ( (wl >= 400.0f) & (wl < 410.0f) )
        {
            float t = (wl - 400.0f) / (410.0f - 400.0f);
            res.r   = (0.33f * t) - (0.20f * t * t);
        }
        else
        if ( (wl >= 410.0f) & (wl < 475.0f) )
        {
            float t = (wl - 410.0f) / (475.0f - 410.0f);
            res.r   = 0.14f - (0.13f * t * t);
        }
        else
        if ( (wl >= 545.0f) & (wl < 595.0f) )
        {
            float t = (wl - 545.0f) / (595.0f - 545.0f);
            res.r   = (1.98f * t) - (t * t);
        }
        else
        if ( (wl >= 595.0f) & (wl < 650.0f) )
        {
            float t = (wl - 595.0f) / (650.0f - 595.0f);
            res.r   = 0.98f + (0.06f * t) - (0.40f * t * t);
        }
        else
        if ( (wl >= 650.0f) & (wl <= 700.0f) )
        {
            float t = (wl - 650.0f) / (700.0f - 650.0f);
            res.r   = 0.65f - (0.84f * t) + (0.20f * t * t);
        }

        // green
        if ( (wl >= 415.0f) & (wl < 475.0f) )
        {
            float t = (wl - 415.0f) / (475.0f - 415.0f);
            res.g   = (0.80f * t * t);
        }
        else
        if ( (wl >= 475.0f) & (wl < 590.0f) )
        {
            float t = (wl - 475.0f) / (590.0f - 475.0f);
            res.g   = 0.8f + (0.76f * t) - (0.80f * t * t);
        }
        else
        if ( (wl >= 585.0f) & (wl < 639.0f) )
        {
            float t = (wl - 585.0f) / (639.0f - 585.0f);
            res.g   = 0.84f - (0.84f * t);
        }

        // blue
        if ( (wl >= 400.0f) & (wl < 475.0f) )
        {
            float t = (wl - 400.0f) / (475.0f - 400.0f);
            res.b   = (2.20f * t) - (1.50f * t * t);
        }
        else
        if ( (wl >= 475.0f) & (wl < 560.0f) )
        {
            float t = (wl - 475.0f) / (560.0f - 475.0f);
            res.b   = 0.7f - (t) + (0.30f * t * t);
        }

        return res;
    }

} // AE::Math

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Helper class to emulate:
        - image blit
        - image resolve
        - clear color attachment
        - clear color image
        - clear depth/stencil image
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/MCommon.h"

namespace AE::Graphics::_hidden_
{

    //
    // Metal Image Operations Helper
    //

    class MImageOpHelper
    {
    };


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL

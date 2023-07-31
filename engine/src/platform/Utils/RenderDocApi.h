// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{

    //
    // RenderDoc API
    //

    class RenderDocApi final
    {
    // variables
    private:
        Library     _lib;
        void*       _api        = null;     // RENDERDOC_API_x_x_x

        void*       _vkDevice   = null;
        void*       _wndHandle  = null;

        DRC_ONLY(
            RWDataRaceCheck     _drCheck;
        )


    // methods
    public:
        RenderDocApi ()                                 __NE___ {}
        ~RenderDocApi ()                                __NE___;

        ND_ bool  Initialize ()                         __NE___;

        // manual frame capture
        ND_ bool  BeginFrame ()                         C_NE___;
            bool  CancelFrame ()                        C_NE___;
            bool  EndFrame ()                           C_NE___;
        ND_ bool  IsFrameCapturing ()                   C_NE___;

        // next frame
        ND_ bool  TriggerFrameCapture ()                C_NE___;
        ND_ bool  TriggerMultiFrameCapture (uint count) C_NE___;
    };


} // AE::App

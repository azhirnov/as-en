// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:  yes
*/

#pragma once

#include "platform/Public/InputActions.h"

namespace AE::AppV2
{
    using namespace AE::Base;


    //
    // View Mode interface
    //

    class IViewMode : public EnableRC<IViewMode>
    {
    // types
    public:
        using InputModeName     = App::InputModeName;
        using ActionQueueReader = App::IInputActions::ActionQueueReader;
        using IOutputSurface    = App::IOutputSurface;
        using AsyncTask         = Threading::AsyncTask;


    // interface
    public:

        // TODO
        //
        ND_ virtual AsyncTask       Update (const ActionQueueReader &, ArrayView<AsyncTask> deps)   __NE___ = 0;

        // TODO
        //
        ND_ virtual AsyncTask       Draw (Ptr<IOutputSurface>, ArrayView<AsyncTask> deps)           __NE___ = 0;

        // TODO
        //
        ND_ virtual InputModeName   GetInputMode ()                                                 C_NE___ = 0;
    };


} // AE::AppV2

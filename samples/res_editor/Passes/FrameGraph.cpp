// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/FrameGraph.h"

namespace AE::ResEditor
{

    FrameGraphImpl*     FrameGraphImpl::_instance = null;

/*
=================================================
    constructor
=================================================
*/
    FrameGraphImpl::FrameGraphImpl () __Th___ :
        _rg{ RenderTaskScheduler().GetRenderGraph() }
    {
        CHECK_THROW( _instance == null );
        _instance = this;
    }

/*
=================================================
    destructor
=================================================
*/
    FrameGraphImpl::~FrameGraphImpl ()
    {
        CHECK( _rg.WaitAll() );

        CHECK( _instance == this );
        _instance = null;
    }


} // AE::ResEditor

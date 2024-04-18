// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Core/RenderGraph.h"

namespace AE::ResEditor
{
namespace {
	static RenderGraphImpl*  _rgInstance = null;
}

/*
=================================================
	constructor
=================================================
*/
	RenderGraphImpl::RenderGraphImpl () __Th___ :
		_rg{ GraphicsScheduler().GetRenderGraph() }
	{
		CHECK_THROW( _rgInstance == null );
		_rgInstance = this;
	}

/*
=================================================
	destructor
=================================================
*/
	RenderGraphImpl::~RenderGraphImpl ()
	{
		CHECK( _rg.WaitAll( AE::DefaultTimeout ));

		CHECK( _rgInstance == this );
		_rgInstance = null;
	}

/*
=================================================
	RenderGraph
=================================================
*/
	RenderGraphImpl&  RenderGraph ()
	{
		ASSERT( _rgInstance != null );
		return *_rgInstance;
	}


} // AE::ResEditor

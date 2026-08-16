// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Resources/BufferView.h"
#include "Core/RenderGraph.h"
#include "Passes/Renderer.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	BufferView::BufferView (RC<Buffer>				baseBuf,
							Strong<BufferViewID>	viewId,
							EPixelFormat			format,
							Renderer &				renderer,
							StringView				dbgName) __NE___ :
		IResource{ renderer },
		_view{ RVRef(viewId) },
		_baseBuffer{ RVRef(baseBuf) },
		_format{ format },
		_dbgName{ dbgName }
	{}

/*
=================================================
	destructor
=================================================
*/
	BufferView::~BufferView () __NE___
	{
		auto	id = _view.Release();
		GraphicsScheduler().GetResourceManager().ReleaseResource( id );
	}

/*
=================================================
	_OnBufferResized
=================================================
*/
	void  BufferView::_OnBufferResized (BufferID bufId, Bytes newSize) __Th___
	{
		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();

		BufferViewDesc	desc;
		desc.format		= _format;
		desc.size		= newSize;

		Strong<BufferViewID>	view_id = res_mngr.CreateBufferView( desc, bufId, _dbgName );
		CHECK_THROW( view_id );

		view_id = _view.Attach( RVRef(view_id) );

		res_mngr.ReleaseResource( view_id );
	}

} // AE::ResEditor

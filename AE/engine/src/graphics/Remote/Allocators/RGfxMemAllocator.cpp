// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Allocators/RGfxMemAllocator.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RGfxMemAllocator::~RGfxMemAllocator () __NE___
	{
		using namespace AE::RemoteGraphics;

		Msg::ResMngr_ReleaseResource	msg {_gfxAllocId};

		CHECK( GraphicsScheduler().GetDevice().Send( msg ));
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS

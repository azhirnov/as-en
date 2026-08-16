// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Allocators/RGfxMemAllocator.h"
# include "graphics_rhi/Remote/RRenderTaskScheduler.h"

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

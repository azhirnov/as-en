// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Descriptors/RDescriptorAllocator.h"
# include "graphics_rhi/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	RDescriptorAllocator::~RDescriptorAllocator () __NE___
	{
		using namespace AE::RemoteGraphics;

		Msg::ResMngr_ReleaseResource	msg {_dsAllocId};

		CHECK( GraphicsScheduler().GetDevice().Send( msg ));
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS

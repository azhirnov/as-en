// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#if defined(AE_ENABLE_GLFW) and defined(AE_PLATFORM_MACOS)
# include "graphics/Public/MetalTypes.h"
# undef null
# include <GLFW/glfw3.h>
# include <Cocoa/Cocoa.h>
# include <GLFW/glfw3native.h>
# define null nullptr

using AE::Graphics::MetalNSViewRC;
using AE::Graphics::MetalCALayerRC;

extern bool  GetNSWindowView (GLFWwindow* wnd, OUT MetalNSViewRC &outNSView, OUT MetalCALayerRC &outMetalLayer)
{
	id	ms_window = glfwGetCocoaWindow( wnd );	// NSWindow
	CHECK_ERR( ms_window != nil );

	id	ns_view = [ms_window contentView];	// NSView
	CHECK_ERR( ns_view != nil );

	NSBundle*	ns_bundle = [NSBundle bundleWithPath : @"/System/Library/Frameworks/QuartzCore.framework"];
	CHECK_ERR( ns_bundle != null );

	id	ns_layer = [[ns_bundle classNamed:@"CAMetalLayer"] layer];
	CHECK_ERR( ns_layer != null );

	[ns_view setLayer : ns_layer];
	[ns_view setWantsLayer : YES];

	// Disable retina display scaling
	{
		CGSize	view_scale = [ns_view convertSizeToBacking : CGSizeMake(1.0, 1.0)];
		[ns_layer setContentsScale : MIN(view_scale.width, view_scale.height)];
	}

	outNSView.Retain( (__bridge void *)ns_view );
	outMetalLayer.Retain( (__bridge void *)ns_layer );
	return true;
}

#endif // AE_ENABLE_GLFW and AE_PLATFORM_MACOS

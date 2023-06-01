// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Random.h"

#include "platform/Public/InputActions.h"
#include "platform/Public/InputSurface.h"
#include "platform/Public/OutputSurface.h"

#include "platform/DefaultV1/AppConfig.h"
#include "platform/DefaultV1/DefaultAppV1.h"

#include "graphics/Public/GraphicsImpl.h"

#include "graphics_hl/Canvas/Canvas.h"

#include "vfs/VirtualFileSystem.h"


namespace AE::Samples::Demo
{
	using namespace AE::Graphics;
	using namespace AE::App;

	using AE::Threading::AsyncTask;
	
} // AE::Samples::Demo

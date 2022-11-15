// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI

# include "base/StdInclude.h"
# include "imgui.h"

# include "base/Containers/RingBuffer.h"
# include "base/Math/PseudoRandom.h"
# include "base/Math/Color.h"
# include "base/Math/Rectangle.h"

# include "threading/Common.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	
	using AE::Threading::SharedMutex;

} // AE::Profiler

#endif // AE_ENABLE_IMGUI

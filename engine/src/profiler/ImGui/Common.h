// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_IMGUI

# include "base/Defines/StdInclude.h"

# include "base/Containers/RingBuffer.h"
# include "base/Math/PseudoRandom.h"
# include "base/Math/Color.h"
# include "base/Math/Rectangle.h"

# include "threading/Primitives/DataRaceCheck.h"

namespace AE::Profiler
{
	using namespace AE::Base;
	
	#if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::DataRaceCheck;
	#endif

} // AE::Profiler

#endif // AE_ENABLE_IMGUI

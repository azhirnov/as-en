// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/GraphicsImpl.h"
#include "platform/Public/InputSurface.h"
#include "platform/Public/OutputSurface.h"

namespace AE::RG
{
namespace _hidden_
{
	using namespace AE::Graphics;
	
	#if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::SingleThreadCheck;
	#endif
	

	#if defined(AE_ENABLE_VULKAN)
	using PrimaryCmdBufState_t	= VPrimaryCmdBufState;

	#elif defined(AE_ENABLE_METAL)
	using PrimaryCmdBufState_t	= MPrimaryCmdBufState;

	#else
	#	error not implemented
	#endif

} // _hidden_

	using namespace AE::Base;

} // AE::RG

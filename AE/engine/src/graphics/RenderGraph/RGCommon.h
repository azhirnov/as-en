// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/GraphicsImpl.h"

namespace AE::App
{
	struct IOutputSurface_RenderTarget;
	class IOutputSurface;

	class IInputSurface;

} // AE::App


namespace AE::RG
{
namespace _hidden_
{
	using namespace AE::Graphics;

	#if defined(AE_ENABLE_VULKAN)
	using PrimaryCmdBufState_t	= VPrimaryCmdBufState;

	#elif defined(AE_ENABLE_METAL)
	using PrimaryCmdBufState_t	= MPrimaryCmdBufState;

	#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
	using PrimaryCmdBufState_t	= RPrimaryCmdBufState;

	#else
	#	error not implemented
	#endif

} // _hidden_

	using namespace AE::Base;

} // AE::RG

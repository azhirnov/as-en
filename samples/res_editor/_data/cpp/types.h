#pragma once

#include "graphics/Public/IDs.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4200)
#endif

namespace AE::ShaderTypes
{
	using namespace AE::Graphics;

#define AE_CANVAS_VERTEX_TYPES

#if defined(AE_ENABLE_VULKAN)
#	include "vk_types.h"
//#elif defined(AE_ENABLE_METAL)
//#	include "mac_types.h"
#endif

} // AE::ShaderTypes

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif

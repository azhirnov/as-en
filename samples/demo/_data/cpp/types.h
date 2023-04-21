#pragma once

#include "graphics/Public/IDs.h"

namespace AE::ShaderTypes
{
	using namespace AE::Graphics;

#define AE_CANVAS_VERTEX_TYPES

#if defined(AE_ENABLE_VULKAN)
#	include "vk_types.h"
#elif defined(AE_ENABLE_METAL)
#	include "mac_types.h"
#else
#	error not implemented
#endif

} // AE::ShaderTypes

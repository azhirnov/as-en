#pragma once

#include "graphics/Public/IDs.h"

namespace AE::ShaderTypes
{
	using namespace AE::Graphics;

	#define AE_CANVAS_VERTEX_TYPES

	#if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
	# include "vk_types.h"

	//#elif defined(AE_ENABLE_METAL)
	//# include "mac_types.h"

	#else
	# error not implemented
	#endif

} // AE::ShaderTypes

#include "res_editor/_ui_data/cpp/types.h"

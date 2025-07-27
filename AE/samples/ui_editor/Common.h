// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Platform.h"
#include "pch/Graphics.h"
#include "pch/Profiler.h"
#include "pch/VFS.h"
#include "pch/Scripting.h"

namespace AE::UIEditor
{
	using namespace AE::Graphics;
	using namespace AE::App;
	using namespace AE::ImportCoroutines;

	ImportBitOperators;

	using ActionQueueReader = IInputActions::ActionQueueReader;

	using AE::RG::DirectCtx;	// override Graphics::DirectCtx
	using AE::RG::IndirectCtx;	// override Graphics::IndirectCtx


	struct RTechInfo
	{
		GAutorelease<PipelinePackID>	packId;
		RenderTechPipelinesPtr			rtech;

		ND_ explicit operator bool ()	C_NE___	{ return packId and rtech; }
	};

} // AE::UIEditor

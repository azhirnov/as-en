// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "UIEditor.pch.h"

namespace AE::UIEditor
{
	using namespace AE::Graphics;
	using namespace AE::App;

	using AE::Threading::AsyncTask;
	using AE::Threading::CoroTask;

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

// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "demo/Common.h"
#include "demo/_data/cpp/types.h"

#include "base/Math/FPVCamera.h"
#include "base/Math/FlightCamera.h"

#include "graphics_hl/Resources/LoadableImage.h"
#include "graphics_hl/Resources/RasterFont.h"
#include "graphics_hl/Resources/StaticImageAtlas.h"

#include "geometry_tools/Cube/CubeRenderer.h"
#include "geometry_tools/SphericalCube/SphericalCubeRenderer.h"

#include "demo/Core/RenderGraph.h"

#include "profiler/ProfilerUI.h"

namespace AE::Samples::Demo
{

	//
	// Sample Base interface
	//

	class ISample : public EnableRC<ISample>
	{
	// types
	public:
		using ActionQueueReader = IInputActions::ActionQueueReader;


	// interface
	public:
		virtual ~ISample () {}

		ND_ virtual bool			Init (PipelinePackID pack)										= 0;

		ND_ virtual AsyncTask		Update (const ActionQueueReader &, ArrayView<AsyncTask> deps)	{ Unused( deps );  return null; }

		// deps - must contains task which returned by 'Update()' or task which depends on it.
		//
		ND_ virtual AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)				= 0;

		ND_ virtual InputModeName	GetInputMode ()													const = 0;
	};


} // AE::Samples::Demo

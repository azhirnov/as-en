// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// ImGui Sample
	//

	class ImGuiSample final : public ISample
	{
	// types
	private:
		class DrawTask;
		class ProcessInputTask;


	// variables
	public:
		ImGuiRenderer			imgui;
		Profiler::ProfilerUI	profiler;


	// methods
	public:
		ImGuiSample ()																		__NE___	: imgui{null} {}
		explicit ImGuiSample (ImGuiContext* ctx)											__NE___	: imgui{ctx} {}

		// ISample //
		bool			Init (PipelinePackID, IApplicationTS)								__NE_OV;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	__NE_OV;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					__NE_OV;
		InputModeName	GetInputMode ()														C_NE_OV;
	};


} // AE::Samples::Demo

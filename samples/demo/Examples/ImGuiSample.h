// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ImGuiRenderer.h"

namespace AE::Samples::Demo
{

	//
	// ImGui Sample
	//

	class ImGuiSample final : public ISample
	{
	// types
	private:
		class UploadTask;
		class DrawTask;
		class ProcessInputTask;


	// variables
	public:
		ImGuiRenderer			imgui;
		Profiler::ProfilerUI	profiler;


	// methods
	public:
		ImGuiSample () : imgui{null} {}
		explicit ImGuiSample (ImGuiContext* ctx) : imgui{ctx} {}

		// ISample //
		bool			Init (PipelinePackID pack)											override;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					override;
		InputModeName	GetInputMode ()														const override { return InputModeName{"imGUI"}; }
	};


} // AE::Samples::Demo

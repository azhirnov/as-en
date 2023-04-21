// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Utils/ImGuiRenderer.h"

namespace AE::Samples::Demo
{

	//
	// Hdr Test
	//

	class HdrTestSample final : public ISample
	{
	// types
	public:
		class UploadTask;
		class DrawTask;
		class ProcessInputTask;


	// variables
	public:
		Atomic<bool>			uploaded		{false};
		ImGuiRenderer			imgui;
		GraphicsPipelineID		hdrPpln;


	// methods
	public:
		HdrTestSample () : imgui{null} {}
		explicit HdrTestSample (ImGuiContext* ctx) : imgui{ctx} {}

		// ISample //
		bool			Init (PipelinePackID pack) override;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps) override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) override;
		InputModeName	GetInputMode () const override { return InputModeName{"imGUI"}; }
	};


} // AE::Samples::Demo

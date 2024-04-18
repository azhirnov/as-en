// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_IMGUI) and defined(AE_ENABLE_AUDIO)
# include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Audio Sample
	//

	class AudioSample final : public ISample
	{
	// types
	private:
		class DrawTask;
		class ProcessInputTask;
		class SaveSoundTask;


	// variables
	public:
		ImGuiRenderer				imgui;
		Profiler::ProfilerUI		profiler;

		RC<Audio::IAudioOutput>		sound;
		RC<Audio::IAudioInput>		recorder;
		RC<MemWStream>				inStream;


	// methods
	public:
		AudioSample ()																		__NE___	: imgui{null} {}
		explicit AudioSample (ImGuiContext* ctx)											__NE___	: imgui{ctx} {}

		// ISample //
		bool			Init (PipelinePackID, IApplicationTS)								__NE_OV;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	__NE_OV;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					__NE_OV;
		InputModeName	GetInputMode ()														C_NE_OV;
	};


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI and AE_ENABLE_AUDIO

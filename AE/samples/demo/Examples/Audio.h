// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_IMGUI) and defined(AE_ENABLE_AUDIO)
# include "Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Audio Sample
	//

	class AudioSample final : public ISample
	{
	// variables
	public:
		ImGuiRenderer				imgui;
		Profiler::ProfilerUI		profiler;

		RC<Audio::IAudioOutput>		sound;
		RC<Audio::IAudioInput>		recorder;
		RC<ArrayWStream>			inStream;


	// methods
	public:
		AudioSample ()																		__NE___	: imgui{null} {}
		explicit AudioSample (ImGuiContext* ctx)											__NE___	: imgui{ctx} {}

		// ISample //
		bool			Init (PipelinePackID, IApplicationTS)								__NE_OV;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	__NE_OV;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					__NE_OV;
		InputModeName	GetInputMode ()														C_NE_OV;

	private:
		static AsyncCoro   _SaveSoundTask (RC<ArrayWStream>, Audio::AudioDataDesc)			__NE___;
		static RenderCoro  _DrawTask (RC<AudioSample>, IOutputSurface &)					__NE___;
		static AsyncCoro   _ProcessInputTask (RC<AudioSample>, ActionQueueReader)			__NE___;
	};


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI and AE_ENABLE_AUDIO
